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
#define NUMBER_AMOUNT 5

extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL];
extern int32 total_txn_to_confirm;
extern Item company_list_recharge[NUMBER_COMPANY_RECHARGE];
extern int total_txn;
extern char response_message[MESSAGE_LENGHT];

void CT_Recharge(){
	RECHARGE_STEP cStep;
	char suscriptorNumber[SUSC_LENGHT]; //Preguntar por el tamanio de esta cadena
	char suscriptorNumberConfirm[SUSC_LENGHT];
	char confirmationPhone[TELEPHONE_LENGTH];
	char information[INF_LENGHT];
	char phone_information[INF_LENGHT];
	char serialNumber[SERIAL_LENGHT];
	char pin[PIN_LENGHT];
	char service_serialNumber[SERIAL_LENGHT];
	char service_pin[PIN_LENGHT];
	char service_lot[MOVILNET_LOT_LENGHT];
	char pin_printable[PIN_PRINTABLE_LENGHT];
	char amount_recharge[20];
	char customerID[SUSC_LENGHT];
	char serial_to_confirm_with_phone[SERIAL_TO_CONFIRM_UNIT];

	char log[7];
	char program_days[5];
	char transaction_id[15];
	char desconection_date[15];

	SALE_INFO sale_info;

	uint8 cTemp;
	int32 response;
	uint8 size;
	uint8 fd; /*File descriptor to the*/
	Boolean valid;

	//Item company_list[NUMBER_COMPANY];
	Item amount_list[NUMBER_AMOUNT];

	/*---- For list of amounts -------*/ 
	char * amounts[NUMBER_AMOUNT] = {
		"30,00",
		"40,00",
		"60,00",
		"120,00",
		"180,00"
	};
	/*--------------------------------*/

	uint8 company_index = 0;
	uint8 amount_index = 0;

	uint8 i;

	char system_time[15];

	char date[20];

	transaction_id[0] = '\0';
	program_days[0] = '\0';
	desconection_date[0] = '\0';
	suscriptorNumber[0] = '\0';
	suscriptorNumberConfirm[0] = '\0';
	confirmationPhone[0] = '\0';
	memset(serialNumber, 0, SERIAL_LENGHT);
	for(i=0;i<NUMBER_AMOUNT;i++){
		sprintf(amount_list[i].name, amounts[i]);
		amount_list[i].id = i;
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
				if(company_list_recharge[company_index-1].id == 1){
					//cTemp = CT_InputDecimal(amount_recharge, "Monto");
					cTemp= CT_InputAnything(amount_recharge, "Monto",0);
					if( cTemp == 0xFB )
						if( strcmp(amount_recharge, "") != 0 && strcmp(amount_recharge, "0,00") != 0 ){
							cStep = RECHARGE_ADD_SUSC_NUMBER;
						}else{
							CT_Notify("El monto debe ser\nmayor que 0,00",0);
						}
					else if( cTemp == 0xFF)
						cStep = RECHARGE_SELECT_COMPANY;
					else
						cStep = RECHARGE_FAIL;
					break;
				}else{
					cTemp = CT_Menu(amount_list, NUMBER_AMOUNT, &amount_index, TL_LIST, "Monto (Bs)");
					sprintf(amount_recharge,"%s",amount_list[amount_index-1].name);
					if( cTemp == 0x01 )
						if(is_company_to_print_ticket(company_list_recharge[company_index-1].id)==1){
							cStep = RECHARGE_CONFIRMATION;
						}else{
							cStep = RECHARGE_ADD_CONFIRMATION_PHONE;
						}
					else if( cTemp == 0x00 )
						cStep = RECHARGE_SELECT_COMPANY;
					else
						cStep = RECHARGE_FAIL;
					break;
				}

			case RECHARGE_ADD_SUSC_NUMBER:
			
				cTemp= CT_InputAnything(suscriptorNumber, "Tarjeta de Acceso",1);
				if( cTemp == 0xFB ){
					/*Is verified that the suscriptor number meets the structure imposed*/
					if (strlen(suscriptorNumber) == 12) {
						if(is_company_to_print_ticket(company_list_recharge[company_index-1].id)==1){
							cStep = RECHARGE_CONFIRMATION;
						}else{
							cStep = RECHARGE_ADD_CONFIRMATION_PHONE;
						}
					}else{
						CT_Notify("El numero no cumple con el formato requerido",0);
					}
				}else if( cTemp == 0xFF ){
					cStep = RECHARGE_SELECT_AMOUNT;
					suscriptorNumber[0] = '\0';
					suscriptorNumberConfirm[0] = '\0';
				}else{
					cStep = RECHARGE_FAIL;
				}
				break;

			/*case RECHARGE_CONFIRM_SUSC_NUMBER:
				cTemp = CT_InputInteger(suscriptorNumberConfirm, "No. suscriptor(Conf)");
				if( cTemp == 0xFB ){
					if ( strcmp(suscriptorNumber, suscriptorNumberConfirm) == 0 )
						cStep = RECHARGE_CONFIRMATION;
					else{
						CT_Notify("El numero del suscriptor no coincide",0);
						cStep = RECHARGE_ADD_SUSC_NUMBER;
					}
				}else if( cTemp == 0xFF ){
					cStep = RECHARGE_ADD_SUSC_NUMBER;
					suscriptorNumber[0] = '\0';
					suscriptorNumberConfirm[0] = '\0';
				}else{
					cStep = RECHARGE_FAIL;
				}
				break;*/

			case RECHARGE_ADD_CONFIRMATION_PHONE:
			
				cTemp= CT_InputAnything(confirmationPhone, "Telf. para envio SMS",1);
				if( cTemp == 0xFB ){
					/*Is verified that the telephone number meets the structure imposed*/
					if (strlen(confirmationPhone)+1 == TELEPHONE_LENGTH && confirmationPhone[0]=='0' && confirmationPhone[1]=='4') {
						cStep = RECHARGE_CONFIRMATION;
					}else{
						CT_Notify("El numero no cumple con el formato requerido",0);
					}
				}else if( cTemp == 0xFF ){
					if(strlen(suscriptorNumber) > 0){
						cStep = RECHARGE_ADD_SUSC_NUMBER;
					} else {
						cStep = RECHARGE_SELECT_AMOUNT;
					}
					suscriptorNumber[0] = '\0';
					suscriptorNumberConfirm[0] = '\0';
					confirmationPhone[0] = '\0';
				}else{
					cStep = RECHARGE_FAIL;
				}
				break;

			case RECHARGE_CONFIRMATION:
				if(strlen(suscriptorNumber) > 0){
					sprintf(information, "Operadora: %s\n Tarjeta de Acceso:\n %s\n Monto: %s Bs", company_list_recharge[company_index-1].name, suscriptorNumber, amount_recharge);
				}else{
					sprintf(information, "Operadora: %s\n Monto: %s Bs", company_list_recharge[company_index-1].name, amount_recharge);
				}
				
				cTemp = CT_Confirm(information);
				if( cTemp == 0x00 ){
					if(strlen(confirmationPhone) > 0){
						sprintf(phone_information, "Telefono para\n envio de SMS de\n recibo:\n %s", confirmationPhone);
						cTemp = CT_Confirm(phone_information);
						if( cTemp == 0x00 ){
							cStep = RECHARGE_START_COMM;
						}else{
							cStep = RECHARGE_INIT;
							suscriptorNumber[0] = '\0';
							suscriptorNumberConfirm[0] = '\0';
							confirmationPhone[0] = '\0';
						}
					}else{
						cStep = RECHARGE_START_COMM;
					}
				}else{
					cStep = RECHARGE_INIT;
					suscriptorNumber[0] = '\0';
					suscriptorNumberConfirm[0] = '\0';
					confirmationPhone[0] = '\0';
				}
				break;

			case RECHARGE_START_COMM:
				if(company_list_recharge[company_index-1].id == 219)
					sprintf(suscriptorNumber, "04160000000"); //It is fixed, because Movilnet don't need suscriptor numbers for now
				cTemp = CT_AddRecharge(company_list_recharge[company_index-1].id, suscriptorNumber, amount_recharge, customerID, serialNumber, pin, service_serialNumber, service_pin, service_lot, system_time, log, confirmationPhone);

				if( cTemp == 0x00 ){
					cStep = RECHARGE_REGISTER;
				}else{
					cStep = RECHARGE_FAIL;
				}
				break;

			case RECHARGE_REGISTER:
				/*If no one exists create a log file linked to the user*/
				CT_CreateLogFile();
				
				response = CT_RegisterInLastTransFile(company_list_recharge[company_index-1].id, serialNumber, pin, suscriptorNumber, system_time, amount_recharge, service_serialNumber, service_pin, service_lot, log, 0, strlen(confirmationPhone)>0?confirmationPhone:"00000000000");

				if (response == 0x00) {
					response = CT_RegisterInLogFile(company_list_recharge[company_index-1].id, service_serialNumber, serialNumber, suscriptorNumber, system_time, amount_recharge, log, 0, strlen(confirmationPhone)>0?confirmationPhone:"00000000000");
				}

				if (response == 0x00) {
					cStep = RECHARGE_CONFIRM_TO_CRYPTO;
				} else {
					if( response == 0x01 ){
						sprintf(information, "Fallo en registro de transacción, el/los archivos no existen");
					} else {
						sprintf(information, "Fallo en registro de transacción, Error: %s", response_message);
					}
					CT_Notify(information, 0);
					cStep = RECHARGE_EXIT;
				} 
				break;																																	

			case RECHARGE_CONFIRM_TO_CRYPTO:
				cTemp = CT_ConfirmOperation(company_list_recharge[company_index-1].id, suscriptorNumber, amount_recharge, serialNumber, log, program_days, customerID, transaction_id, desconection_date);

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
				sprintf(serial_to_confirm_with_phone, serialNumber);
				strcat(serial_to_confirm_with_phone,strlen(confirmationPhone)>0?confirmationPhone:"00000000000");
				memcpy(&serials_to_confirm[(SERIAL_TO_CONFIRM_UNIT-1)*total_txn_to_confirm], serial_to_confirm_with_phone, SERIAL_TO_CONFIRM_UNIT-1);
				serial_to_confirm_with_phone[0] = '\0';
				total_txn_to_confirm++;
				cStep = RECHARGE_PRINT;
				break;


			case RECHARGE_PRINT:
				if(is_company_to_print_ticket(company_list_recharge[company_index-1].id)==1){
					cTemp = CT_PrintReceiptClientAndCompany(0, company_list_recharge[company_index-1].name, company_list_recharge[company_index-1].id, suscriptorNumber, amount_recharge, service_serialNumber, service_pin, system_time, program_days, service_lot, NULL, transaction_id, desconection_date);
				}else{
					CT_Notify("Operacion Exitosa\nRecarga realizada",0);
					CT_RegisterPrintSuccess();
				}
				cStep = RECHARGE_EXIT;
				break;

			case RECHARGE_FAIL:
				sprintf(information, "Error: %s", response_message);
				CT_Notify(information,0);
				cStep = RECHARGE_EXIT;
				suscriptorNumber[0] = '\0';
				suscriptorNumberConfirm[0] = '\0';
				confirmationPhone[0] = '\0';
				break;

		}
	}
}