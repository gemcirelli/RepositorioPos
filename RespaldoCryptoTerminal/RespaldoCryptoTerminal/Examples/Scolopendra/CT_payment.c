#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TP_SingleIME.h>


#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"

#include "CT_Payment.h"
#include "CT_Login.h"
#include "CT_CryptoManager.h"
//#include "CT_GlobalData.h"
#include "CT_Input.h"
#include "CT_Print.h"
#include "CT_utils.h"
#include "CT_File.h"

#define NUMBER_COMPANY 1

extern Item company_list_payment[NUMBER_COMPANY_PAYMENT];
extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL];
extern int32 total_txn_to_confirm;
extern int total_txn;

void CT_Payment(){
	PAYMENT_STEP cStep;
	char billNumber[20]; //Preguntar por el tamanio de esta cadena
	char billNumberConfirm[20];
	char information[INF_LENGHT];
	char serialNumber[SERIAL_LENGHT];
	char pin[PIN_LENGHT];
	char pin_printable[PIN_PRINTABLE_LENGHT];
	char paymentAmount[20];

	char log[7];

	char company_name_upper[30];

	uint8 cTemp;
	int32 response;

	//Item company_list[NUMBER_COMPANY];

	/*---- For list of companies -----*/
	//char * items[NUMBER_COMPANY] = {
	//	"Bigott"
	//};

	//uint16 ids[NUMBER_COMPANY] = {221};
	/*--------------------------------*/

	uint8 company_index = 0;

	uint8 i;

	char system_time[15];

	char date[20];

	//for(i=0;i<NUMBER_COMPANY;i++){
	//	sprintf(company_list[i].name, items[i]);
	//	company_list[i].id = ids[i];
	//}

	cStep = PAYMENT_INIT;

	while(cStep != PAYMENT_EXIT)
	{
		switch( cStep )
		{
			case PAYMENT_INIT:	
				if( test_session() ){
					if( total_txn < MAX_TXN ){
						cStep = PAYMENT_SELECT_COMPANY;
					}else{
						CT_Notify("Total de Txn excedida. Debe cerrar el POS",0);
						cStep = PAYMENT_EXIT;
					}				
				}else{
					CT_Notify("Debe iniciar sesion para continuar.",0);
					cStep = PAYMENT_EXIT;
				}
				break;

			case PAYMENT_SELECT_COMPANY:
				cTemp = CT_Menu(company_list_payment, NUMBER_COMPANY_PAYMENT, &company_index, TL_LIST, (uint8*) "Compania");
				if( cTemp )
					cStep = PAYMENT_ADD_AMOUNT;
				else 
					cStep = PAYMENT_EXIT;
				break;

			case PAYMENT_ADD_AMOUNT:
				cTemp = CT_InputDecimal(paymentAmount, "Monto");
				if( cTemp == 0xFB )
					cStep = PAYMENT_ADD_BILL_NUMBER;
				else if( cTemp == 0xFF )
					cStep = PAYMENT_ADD_AMOUNT;
				else
					cStep = PAYMENT_FAIL;
				break;

			case PAYMENT_ADD_BILL_NUMBER:
				cTemp = CT_InputInteger(billNumber, "No. Factura");
				if( cTemp == 0xFB )
					cStep = PAYMENT_CONFIRM_BILL_NUMBER;
				else if( cTemp == 0xFF )
					cStep = PAYMENT_ADD_AMOUNT;
				else
					cStep = PAYMENT_FAIL;
				break;

			case PAYMENT_CONFIRM_BILL_NUMBER:
				cTemp = CT_InputInteger(billNumberConfirm, "No. Factura(Conf)");
				if( cTemp == 0xFB ){
					if ( strcmp(billNumber, billNumberConfirm) == 0 )
						cStep = PAYMENT_CONFIRMATION;
					else{
						CT_Notify("Los numeros de facturas no coinciden",0);
						cStep = PAYMENT_ADD_BILL_NUMBER;
					}
				}else if( cTemp == 0xFF )
					cStep = PAYMENT_ADD_BILL_NUMBER;
				else
					cStep = PAYMENT_FAIL;
				break;

			case PAYMENT_CONFIRMATION:
				sprintf(information, "Compania: %s\nNo. Factura: %s\nMonto: %s Bs", company_list_payment[company_index-1].name, billNumber, paymentAmount);
				cTemp = CT_Confirm(information);
				if( cTemp == 0x00 )
					cStep = PAYMENT_START_COMM;
				else
					cStep = PAYMENT_INIT;
				break;

			case PAYMENT_START_COMM:
				cTemp = CT_AddPayment(company_list_payment[company_index-1].id, billNumber, paymentAmount, serialNumber, pin, system_time, log);

				if( cTemp == 0x00 )
					cStep = PAYMENT_REGISTER;
				else
					cStep = PAYMENT_FAIL;
				break;

			case PAYMENT_REGISTER:
				response = CT_RegisterInLastTransFile(company_list_payment[company_index-1].id, serialNumber, pin, billNumber, system_time, paymentAmount, log, 1);

				CT_RegisterInLogFile(company_list_payment[company_index-1].id, serialNumber, billNumber, system_time, paymentAmount, log, 1);

				if( response == 0x01 ){
					CT_Notify("Log File no existe", 0);
				}else{
					if( response < 0 ){
						sprintf(information, "Error: %d", response);
						CT_Notify(information, 0);
					}else{
						CT_Notify("Operacion Registrada", 2);
					}
				}

				cStep = PAYMENT_CONFIRM_TO_CRYPTO;
				break;

			case PAYMENT_CONFIRM_TO_CRYPTO:
				cTemp = CT_ConfirmOperation(company_list_payment[company_index-1].id, billNumber, paymentAmount, serialNumber, log);

				if( cTemp == 0x00 ){
					cStep = PAYMENT_SUCCESS;
				}else{
					cStep = PAYMENT_CONFIRM_FAIL;
				}
				break;

			case PAYMENT_SUCCESS:
				CT_RegisterConfirmAction();

				cStep = PAYMENT_PRINT;
				break;

			case PAYMENT_CONFIRM_FAIL:
				memcpy(&serials_to_confirm[(SERIAL_LENGHT-1)*total_txn_to_confirm], serialNumber, SERIAL_LENGHT-1);
				total_txn_to_confirm++;

				cStep = PAYMENT_PRINT;
				break;
			
			case PAYMENT_PRINT:
				cTemp = CT_PrintReceiptClientAndCompany(1, company_list_payment[company_index-1].name, company_list_payment[company_index-1].id, billNumber, paymentAmount, serialNumber, pin, system_time);
				cStep = PAYMENT_EXIT;
				break;

			case PAYMENT_FAIL:
				CT_Notify("Error",0);
				cStep = PAYMENT_EXIT;
				break;
		}
	}
}