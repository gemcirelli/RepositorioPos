#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TP_SingleIME.h>


#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"

#include "CT_Wallet.h"
#include "CT_CryptoManager.h"
#include "CT_Input.h"
#include "CT_Print.h"
#include "CT_utils.h"
#include "CT_File.h"

#define NUMBER_COMPANY 1

extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL];
extern int32 total_txn_to_confirm;
extern int total_txn;
extern char response_message[MESSAGE_LENGHT];

void CT_Wallet(){
	WALLET_STEP cStep;
	char suscriptorNumber[SUSC_LENGHT]; //Preguntar por el tamanio de esta cadena
	char pin[PIN_LENGHT];
	char information[INF_LENGHT];
	char amount_recharge[20];
	char id_e_customer[11];

	char log[7];
	char balance[13];

	uint8 cTemp;
	int32 response;
	uint8 i;
	uint8 walletRetry;

	char system_time[15];

	char date[20];

	cStep = WALLET_INIT;
	walletRetry = 0;
	while(cStep != WALLET_EXIT)
	{
		switch( cStep )
		{
		case WALLET_INIT:	
			if( test_session() ){
				if( total_txn < MAX_TXN ){
					cStep = WALLET_ADD_SUSC_NUMBER;
				}else{
					CT_Notify("Total de Txn excedida. Debe cerrar el POS",0);
					cStep = WALLET_EXIT;
				}				
			}else{
				CT_Notify("Debe iniciar sesion para continuar.",0);
				cStep = WALLET_EXIT;
			}
			break;

		case WALLET_ADD_SUSC_NUMBER:
			cTemp = CT_InputInteger(suscriptorNumber, "No. cuenta");
			if( cTemp == 0xFB ){
				cStep = WALLET_ADD_AMOUNT;
			}else{
				cStep = WALLET_EXIT;
				suscriptorNumber[0] = '\0';
			}
			break;

		case WALLET_ADD_AMOUNT:
			cTemp = CT_InputDecimal(amount_recharge, "Monto");
			if( cTemp == 0xFB )
				if( strcmp(amount_recharge, "") != 0 && strcmp(amount_recharge, "0,00") != 0 ){
					cStep = WALLET_START_COMM;
				}else{
					CT_Notify("El monto debe ser\nmayor que 0,00",0);
				}
			else if( cTemp == 0xFF)
				cStep = WALLET_ADD_SUSC_NUMBER;
			else
				cStep = WALLET_FAIL;
			break;

		case WALLET_START_COMM:
			// TO DO HARCODED EL COMPANY, PRODUCT TYPE, CODE, ITEM.
			cTemp = CT_EWalletPayment(234, suscriptorNumber, "1", "1", "1", amount_recharge, id_e_customer, system_time, log); 

			if( cTemp == 0x00 )
				cStep = WALLET_ADD_PIN;
			else
				cStep = WALLET_FAIL;
			break;

		case WALLET_REGISTER:

			/*response = CT_RegisterInLastTransFile(company_list_payment[company_index-1].id, serialNumber, pin, billNumber, system_time, paymentAmount, log, 1);

			CT_RegisterInLogFile(company_list_payment[company_index-1].id, serialNumber, billNumber, system_time, paymentAmount, log, 1);

			if( response == 0x01 ){
			CT_Notify("Log File no existe", 0);
			}else{
			if( response < 0 ){
			sprintf(information, "Error: %s", response_message);
			CT_Notify(information, 0);
			}else{
			CT_Notify("Operacion Registrada", 2);
			}
			}

			cStep = PAYMENT_CONFIRM_TO_CRYPTO;
			break;*/

		case WALLET_ADD_PIN:
			cTemp = CT_InputInteger(pin, "PIN del cliente");
			if( cTemp == 0xFB ){
				cStep = WALLET_CONFIRM_TO_CRYPTO;
			}else{
				cStep = WALLET_EXIT;
				pin[0] = '\0';
			}
			break;

		case WALLET_CONFIRM_TO_CRYPTO:
			// TO DO HARCODED EL COMPANY, PRODUCT TYPE, CODE, ITEM.
			cTemp = CT_EWalletPaymentConfirm(234, log, suscriptorNumber, "1", "1", "1", amount_recharge, &system_time[4], pin, balance); 

			if( cTemp == 0x00 ){
				cStep = WALLET_SUCCESS;
			}else{
				cStep = WALLET_CONFIRM_FAIL;
			}
			break;

		case WALLET_SUCCESS:
			CT_RegisterConfirmAction();

			cStep = WALLET_PRINT;
			break;

		case WALLET_CONFIRM_FAIL:
			/*memcpy(&serials_to_confirm[(SERIAL_LENGHT-1)*total_txn_to_confirm], serialNumber, SERIAL_LENGHT-1);
			total_txn_to_confirm++;

			cStep = WALLET_PRINT;
			break;*/
			walletRetry++;
			if( walletRetry < 2 ){
				sprintf(information, "Error: %s ¿Desea continuar?", response_message);
				cTemp = CT_Confirm(information);
				if( cTemp == 0x00 ){
					cStep = WALLET_ADD_PIN;
				}else{
					cStep = WALLET_EXIT;
				}
			}else{
				cStep = WALLET_FAIL;
			}
			break;
		case WALLET_PRINT:
			//TODO Harcoded
			cTemp = CT_PrintReceiptClientAndCompany(2, "GRE5", CT_GRE5, suscriptorNumber, amount_recharge, NULL, pin, system_time, NULL, NULL, balance, "", "");

			cStep = WALLET_EXIT;
			break;

		case WALLET_FAIL:
			sprintf(information, "Error: %s", response_message);
			CT_Notify(information,0);
			cStep = WALLET_EXIT;
			break;
		}
	}
}