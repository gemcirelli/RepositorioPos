#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TP_SingleIME.h>


#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"

#include "CT_Deposit.h"
#include "CT_Login.h"
#include "CT_CryptoManager.h"
#include "CT_Input.h"
#include "CT_Print.h"
#include "CT_Utils.h"

extern USER_DATA g_userData;
extern char response_message[MESSAGE_LENGHT];

typedef struct Bank_Data_Struct{
	uint16 id;
	char name[30];
}Bank_Data;

void CT_Deposit(){
	DEPOSIT_STEP cStep;
	char depositControlNumber[30], depositAmount[20];
	char information[INF_LENGHT];
	char * system_time;
	char time[15];
	char dateReg[20];
	char dateDep[20];
	char format_date[20];

	uint8 bank_index = 0;

	uint8 cTemp;

	Item bank_list[11];

	char * items[11] = {
		"Banesco",
		"Banco de Venezuela",
		"Banco Provincial",
		"Banco Mercantil",
		"B.O.D.",
		"Bancaribe",
		"Banco Bicentenario",		
		"Banco Fondo Comun",		
		"Banco Nacional de Credito",
		"Banplus",
		"Sofitasa"
	};

	uint8 i;

	for(i=0;i<11;i++){
		sprintf(bank_list[i].name, items[i]);
		bank_list[i].id = i+1;
	}

	cStep=DEPOSIT_INIT;

	while(cStep!=DEPOSIT_EXIT)
	{	
		switch(cStep)
		{
			case DEPOSIT_INIT:
				if( test_session() )
					cStep = DEPOSIT_SELECT_BANK;
				else{
					CT_Notify("Debe iniciar sesion para continuar.",0);
					cStep = DEPOSIT_EXIT;
				}
				break;
				
			case DEPOSIT_SELECT_BANK:	
				cTemp = CT_Menu(bank_list, 11, &bank_index, TL_LIST, PNULL);
				if(cTemp==0x01)
					cStep = DEPOSIT_CONTROL_NUMBER;
				else if( cTemp == 0xFF )
					cStep = DEPOSIT_EXIT;
				else
					cStep = DEPOSIT_FAIL;
				break;		
			
			case DEPOSIT_CONTROL_NUMBER:					
				cTemp=CT_InputInteger(depositControlNumber,"Numero de Deposito");
				if(cTemp==0xFB)
					cStep=DEPOSIT_ADD_AMOUNT;
				else if(cTemp==0xFF)
					cStep=DEPOSIT_SELECT_BANK;
				else
					cStep=DEPOSIT_FAIL;
				break;	

			case DEPOSIT_ADD_AMOUNT:					
				cTemp=CT_InputDecimal(depositAmount,"Monto de Deposito");
				if(cTemp==0xFB){
					if( strcmp(depositAmount, "") != 0 && strcmp(depositAmount, "0,00") != 0 ){
						cStep = DEPOSIT_ADD_DATE;
					}else{
						CT_Notify("El monto debe ser\nmayor que 0,00",0);
					}
				}else if(cTemp==0xFF)
					cStep=DEPOSIT_CONTROL_NUMBER;
				else
					cStep=DEPOSIT_FAIL;
				break;

			case DEPOSIT_ADD_DATE:					
				cTemp=CT_InputDate(dateDep,"Fecha del deposito");
				if(cTemp==0xFB)
					cStep=DEPOSIT_CONFIRMATION;
				else if(cTemp==0xFF)
					cStep=DEPOSIT_ADD_AMOUNT;
				else
					cStep=DEPOSIT_FAIL;
				break;

			case DEPOSIT_CONFIRMATION:
				get_date_from_number(dateDep, format_date);
				format_date[10] = '\0';
				sprintf(information, "%s\nNo.: %s\nMonto: %s Bs\nFecha: %s", bank_list[bank_index-1].name, depositControlNumber, depositAmount, format_date); 
				cTemp = CT_Confirm(information);

				if( cTemp == 0x00 ){
					cStep=DEPOSIT_START_COMM;
				}else{
					cStep=DEPOSIT_INIT;
				}
				break;
			
			case DEPOSIT_START_COMM:
				cTemp = CT_AddDeposit(bank_list[bank_index-1].id, depositControlNumber, depositAmount, dateDep);
				
				if(cTemp==0x00){
					cStep = DEPOSIT_SUCCESS;
				}else if(cTemp==0x01){
					g_userData.sessionState = OFFLINE;
					sprintf(information, "Error: %s", response_message);
					CT_Notify(information,0);
					cStep=DEPOSIT_FAIL;
				}else{
					sprintf(information, "Error: %s", response_message);
					CT_Notify(information,0);
					cStep=DEPOSIT_FAIL;
				}
				break;	

			case DEPOSIT_SUCCESS:
				CT_Notify("Deposito Agregado\nExitosamente\n",0);
				if(is_company_to_print_ticket(atoi(g_userData.userCompany))==1){
					cStep=DEPOSIT_PRINT_PROMPT;
				}else{
					CT_Notify("Recibira un correo\nelectronico con\nla informacion",0);
					cStep=DEPOSIT_EXIT;
				}
				break;

			case DEPOSIT_PRINT_PROMPT:
				cTemp = CT_Confirm("¿Imprimir recibo?");

				if( cTemp == 0x00 ){
					cStep = DEPOSIT_PRINT;
				}else{
					cStep = DEPOSIT_EXIT;
				}
				break;

			case DEPOSIT_PRINT:
				get_date_from_number(dateDep, format_date);
				format_date[10] = '\0';
				system_time = (char*) get_system_time();
				memcpy(time, system_time, 15);
				get_human_date(time, dateReg, 0);
				sprintf(information, "Registrado: %s\nBanco: %s\nNo. voucher: %s\nMonto: %s Bs\nFecha Deposito: %s", dateReg, bank_list[bank_index-1].name, depositControlNumber, depositAmount, format_date); 
				cTemp = CT_PrintReceipt(CT_GRE5,0,"","COMPROBANTE DE DEPOSITO", "", information, "", "");

				cStep = DEPOSIT_EXIT;
				break;	

			case DEPOSIT_FAIL:
				cStep = DEPOSIT_EXIT;
				break;

		}
	}
}