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
extern Item company_list_recharge[NUMBER_COMPANY_RECHARGE];
extern int32 total_txn_to_confirm;
extern int total_txn;
extern char response_message[MESSAGE_LENGHT];

void CT_Payment(){
	PAYMENT_STEP cStep;
	char billNumber[20]; //Preguntar por el tamanio de esta cadena
	char billNumberConfirm[20];
	char confirmationPhone[TELEPHONE_LENGTH];
	char information[INF_LENGHT];
	char phone_information[INF_LENGHT];
	char serialNumber[SERIAL_LENGHT];
	char pin[PIN_LENGHT];
	char pin_printable[PIN_PRINTABLE_LENGHT];
	char paymentAmount[20];
	char serial_to_confirm_with_phone[SERIAL_TO_CONFIRM_UNIT];

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
	confirmationPhone[0] = '\0';
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
				//cTemp = CT_InputDecimal(paymentAmount, "Monto");
				cTemp = CT_InputAnything(paymentAmount, "Monto", 0);
				if( cTemp == 0xFB )
					if( strcmp(paymentAmount, "") != 0 && strcmp(paymentAmount, "0,00") != 0 ){
						cStep = PAYMENT_ADD_BILL_NUMBER;
					}else{
						CT_Notify("El monto debe ser\nmayor que 0,00",0);
					}
				else if( cTemp == 0xFF )
					cStep = PAYMENT_SELECT_COMPANY;
				else
					cStep = PAYMENT_FAIL;
				break;

			case PAYMENT_ADD_BILL_NUMBER:
				
				cTemp=CT_InputAnything(billNumber,"No. Factura",1);
				if( cTemp == 0xFB )
					cStep = PAYMENT_CONFIRM_BILL_NUMBER;
				else if( cTemp == 0xFF )
					cStep = PAYMENT_ADD_AMOUNT;
				else
					cStep = PAYMENT_FAIL;
				break;

			case PAYMENT_CONFIRM_BILL_NUMBER:
				
				cTemp=CT_InputAnything(billNumberConfirm, "No. Factura(Conf)",1);
				if( cTemp == 0xFB ){
					if ( strcmp(billNumber, billNumberConfirm) == 0 ){
						if(is_company_to_print_ticket(company_list_payment[company_index-1].id)==1){
							cStep = PAYMENT_CONFIRMATION;
						}else{
							cStep = PAYMENT_ADD_CONFIRMATION_PHONE;
						}
					} else{
						CT_Notify("Los numeros de facturas no coinciden",0);
						cStep = PAYMENT_ADD_BILL_NUMBER;
					}
				}else if( cTemp == 0xFF )
					cStep = PAYMENT_ADD_BILL_NUMBER;
				else
					cStep = PAYMENT_FAIL;
				break;

			case PAYMENT_ADD_CONFIRMATION_PHONE:
				
				cTemp= CT_InputAnything(confirmationPhone, "Telf. para envio SMS",1);
				if( cTemp == 0xFB ){
					/*Is verified that the telephone number meets the structure imposed*/
					if (strlen(confirmationPhone)+1 == TELEPHONE_LENGTH && confirmationPhone[0]=='0' && confirmationPhone[1]=='4') {
						cStep = PAYMENT_CONFIRMATION;
					}else{
						CT_Notify("El numero no cumple con el formato requerido",0);
					}
				}else if( cTemp == 0xFF ){
					cStep = PAYMENT_ADD_BILL_NUMBER;
					billNumber[0] = '\0';
					billNumberConfirm[0] = '\0';
					confirmationPhone[0] = '\0';
				}else{
					cStep = PAYMENT_FAIL;
				}
				break;

			case PAYMENT_CONFIRMATION:

				sprintf(information, "Compania: %s\nNo. Factura: %s\nMonto: %s Bs", company_list_payment[company_index-1].name, billNumber, paymentAmount);
				cTemp = CT_Confirm(information);
				if( cTemp == 0x00 ){
					if(strlen(confirmationPhone) > 0){
						sprintf(phone_information, "Telefono para\n envio de SMS de\n notificacion:\n %s", confirmationPhone);
						cTemp = CT_Confirm(phone_information);
						if( cTemp == 0x00 ){
							cStep = PAYMENT_START_COMM;
						}else{
							cStep = PAYMENT_INIT;
							billNumber[0] = '\0';
							billNumberConfirm[0] = '\0';
							confirmationPhone[0] = '\0';
						}
					}else{
						cStep = PAYMENT_START_COMM;
					}	
				}else{
					cStep = PAYMENT_INIT;
				}
				break;

			case PAYMENT_START_COMM:
				cTemp = CT_AddPayment(company_list_payment[company_index-1].id, billNumber, paymentAmount, serialNumber, pin, system_time, log, confirmationPhone);

				if( cTemp == 0x00 )
					cStep = PAYMENT_REGISTER;
				else
					cStep = PAYMENT_FAIL;
				break;

			case PAYMENT_REGISTER:
				/*If no one exists create a log file linked to the user*/
				CT_CreateLogFile();

				response = CT_RegisterInLastTransFile(company_list_payment[company_index-1].id, serialNumber, pin, billNumber, system_time, paymentAmount, serialNumber, pin, "0000000", log, 1, confirmationPhone);


				if (response == 0x00) {
					response = CT_RegisterInLogFile(company_list_payment[company_index-1].id, serialNumber, serialNumber, billNumber, system_time, paymentAmount, log, 1, strlen(confirmationPhone)>0?confirmationPhone:"00000000000");
				}

				if (response == 0x00) {
					cStep = PAYMENT_CONFIRM_TO_CRYPTO;
				} else {
					if( response == 0x01 ){
						sprintf(information, "Fallo en registro de transacción, el/los archivos no existen");
					} else {
						sprintf(information, "Fallo en registro de transacción, Error: %s", response_message);
					}
					CT_Notify(information, 0);
					cStep = PAYMENT_EXIT;
				} 
				break;

			case PAYMENT_CONFIRM_TO_CRYPTO:
				cTemp = CT_ConfirmOperation(company_list_payment[company_index-1].id, billNumber, paymentAmount, serialNumber, log, "", "", "", "");

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
				sprintf(serial_to_confirm_with_phone, serialNumber);
				//TODO enviar el numero de telefono para la confirmacion
				strcat(serial_to_confirm_with_phone,"00000000000");
				memcpy(&serials_to_confirm[(SERIAL_TO_CONFIRM_UNIT-1)*total_txn_to_confirm], serial_to_confirm_with_phone, SERIAL_TO_CONFIRM_UNIT-1);
				total_txn_to_confirm++;
				cStep = PAYMENT_PRINT;
				break;
			
			case PAYMENT_PRINT:
				if(is_company_to_print_ticket(company_list_payment[company_index-1].id)==1){
					cTemp = CT_PrintReceiptClientAndCompany(1, company_list_payment[company_index-1].name, company_list_payment[company_index-1].id, billNumber, paymentAmount, serialNumber, pin, system_time, NULL, NULL, NULL, "", "");
				}else{
					CT_Notify("Operacion Exitosa\nPago de factura realizado",0);
					CT_RegisterPrintSuccess();
				}
				cStep = PAYMENT_EXIT;
				break;

			case PAYMENT_FAIL:
				sprintf(information, "Error: %s", response_message);
				CT_Notify(information,0);
				cStep = PAYMENT_EXIT;
				break;
		}
	}
}