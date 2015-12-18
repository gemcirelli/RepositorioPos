#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TP_SingleIME.h>


#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"

#include "CT_Recharge.h"
#include "CT_Login.h"
#include "CT_CryptoManager.h"
#include "CT_GlobalData.h"
#include "CT_Input.h"
#include "CT_Print.h"
#include "CT_utils.h"
#include "CT_File.h"

#define NUMBER_COMPANY 2
#define NUMBER_AMOUNT 4

extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL];
extern int32 total_txn_to_confirm;
extern Item company_list_recharge[NUMBER_COMPANY_RECHARGE];
extern int total_txn;

void CT_Recharge(){
	RECHARGE_STEP cStep;
	char suscriptorNumber[SUSC_LENGHT]; //Preguntar por el tamanio de esta cadena
	char suscriptorNumberConfirm[SUSC_LENGHT];
	char information[INF_LENGHT];
	char serialNumber[SERIAL_LENGHT];
	char pin[PIN_LENGHT];
	char pin_printable[PIN_PRINTABLE_LENGHT];

	char log[7];

	SALE_INFO sale_info;

	uint8 cTemp;
	int32 response;

	//Item company_list[NUMBER_COMPANY];
	Item amount_list[NUMBER_AMOUNT];

	/*---- For list of companies -----*/
	//char * items[NUMBER_COMPANY] = {
	//	"DirecTV",
	//	"Movilnet"
	//};

	//uint16 ids[NUMBER_COMPANY] = {1, 219};
	/*--------------------------------*/

	/*---- For list of amounts -------*/ 
	char * amounts[NUMBER_AMOUNT] = {
		"20,00",
		"50,00",
		"100,00",
		"250,00"
	};
	/*--------------------------------*/

	uint8 company_index = 0;
	uint8 amount_index = 0;

	uint8 i;

	char system_time[15];

	char date[20];

	//for(i=0;i<NUMBER_COMPANY;i++){
	//	sprintf(company_list[i].name, items[i]);
	//	company_list[i].id = ids[i];
	//}

	for(i=0;i<NUMBER_AMOUNT;i++){
		sprintf(amount_list[i].name, amounts[i]);
	}

	cStep = RECHARGE_INIT;

	while(cStep != RECHARGE_EXIT)
	{
		switch( cStep )
		{
			case RECHARGE_INIT:	
				if( test_session() ){
					if( total_txn < MAX_TXN ){
						cStep = RECHARGE_SELECT_COMPANY;
					}else{
						CT_Notify("Total de Txn excedida. Debe cerrar el POS",0);
						cStep = RECHARGE_EXIT;
					}				
				}else{
					CT_Notify("Debe iniciar sesion para continuar.",0);
					cStep = RECHARGE_EXIT;
				}
				break;

			case RECHARGE_SELECT_COMPANY:
				cTemp = CT_Menu(company_list_recharge, NUMBER_COMPANY_RECHARGE, &company_index, TL_LIST, (uint8*) "Compania");
				if( cTemp == 0x01 )
					cStep = RECHARGE_SELECT_AMOUNT;
				else if( cStep == 0x00 )
					cStep = RECHARGE_EXIT;
				else
					cStep = RECHARGE_EXIT;
				break;

			case RECHARGE_SELECT_AMOUNT:
				cTemp = CT_Menu(amount_list, NUMBER_AMOUNT, &amount_index, TL_LIST, "Monto (Bs)");
				if( cTemp == 0x01 )
					cStep = RECHARGE_ADD_SUSC_NUMBER;
				else if( cTemp == 0x00 )
					cStep = RECHARGE_SELECT_COMPANY;
				else
					cStep = RECHARGE_FAIL;
				break;

			case RECHARGE_ADD_SUSC_NUMBER:
				cTemp = CT_InputInteger(suscriptorNumber, "No. suscriptor");
				if( cTemp == 0xFB )
					cStep = RECHARGE_CONFIRM_SUSC_NUMBER;
				else if( cTemp == 0xFF )
					cStep = RECHARGE_SELECT_AMOUNT;
				else
					cStep = RECHARGE_FAIL;
				break;

			case RECHARGE_CONFIRM_SUSC_NUMBER:
				cTemp = CT_InputInteger(suscriptorNumberConfirm, "No. suscriptor(Conf)");
				if( cTemp == 0xFB ){
					if ( strcmp(suscriptorNumber, suscriptorNumberConfirm) == 0 )
						cStep = RECHARGE_CONFIRMATION;
					else{
						CT_Notify("El numero del suscriptor no coincide",0);
						cStep = RECHARGE_ADD_SUSC_NUMBER;
					}
				}else if( cTemp == 0xFF )
					cStep = RECHARGE_ADD_SUSC_NUMBER;
				else
					cStep = RECHARGE_FAIL;
				break;

			case RECHARGE_CONFIRMATION:
				sprintf(information, "Operadora: %s\nNumero: %s\nMonto: %s Bs", company_list_recharge[company_index-1].name, suscriptorNumber, amount_list[amount_index-1].name);
				cTemp = CT_Confirm(information);
				if( cTemp == 0x00 )
					cStep = RECHARGE_START_COMM;
				else
					cStep = RECHARGE_INIT;
				break;

			case RECHARGE_START_COMM:
				cTemp = CT_AddRecharge(company_list_recharge[company_index-1].id, suscriptorNumber, amount_list[amount_index-1].name, serialNumber, pin, system_time, log);

				if( cTemp == 0x00 )
					cStep = RECHARGE_REGISTER;
				else
					cStep = RECHARGE_FAIL;
				break;

			case RECHARGE_REGISTER:				
				response = CT_RegisterInLastTransFile(company_list_recharge[company_index-1].id, serialNumber, pin, suscriptorNumber, system_time, amount_list[amount_index-1].name, log, 0);

				CT_RegisterInLogFile(company_list_recharge[company_index-1].id, serialNumber, suscriptorNumber, system_time, amount_list[amount_index-1].name, log, 0);

				if( response == 0x01 ){
					CT_Notify("File don't exist", 0);
				}else{
					if( response < 0 ){
						sprintf(information, "Error: %d", response);
						CT_Notify(information, 0);
					}else{
						CT_Notify("Operacion Registrada", 2);
					}
				}

				cStep = RECHARGE_CONFIRM_TO_CRYPTO;
				break;

			case RECHARGE_CONFIRM_TO_CRYPTO:
				cTemp = CT_ConfirmOperation(company_list_recharge[company_index-1].id, suscriptorNumber, amount_list[amount_index-1].name, serialNumber, log);

				if( cTemp == 0x00 ){
					cStep = RECHARGE_SUCCESS;
				}else{
					cStep = RECHARGE_CONFIRM_FAIL;
				}
				break;

			case RECHARGE_SUCCESS:
				CT_RegisterConfirmAction();

				cStep = RECHARGE_PRINT;
				break;

			case RECHARGE_CONFIRM_FAIL:
				memcpy(&serials_to_confirm[(SERIAL_LENGHT-1)*total_txn_to_confirm], serialNumber, SERIAL_LENGHT-1);
				total_txn_to_confirm++;

				cStep = RECHARGE_PRINT;
				break;


			case RECHARGE_PRINT:
				cTemp = CT_PrintReceiptClientAndCompany(0, company_list_recharge[company_index-1].name, company_list_recharge[company_index-1].id, suscriptorNumber, amount_list[amount_index-1].name, serialNumber, pin, system_time);

				cStep = RECHARGE_EXIT;
				break;

			case RECHARGE_FAIL:
				CT_Notify("Error",0);
				cStep = RECHARGE_EXIT;
				break;

		}
	}
}