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
//#include "CT_GlobalData.h"
#include "CT_Input.h"
#include "CT_Print.h"
#include "CT_utils.h"

extern USER_DATA g_userData;

typedef struct Bank_Data_Struct{
	uint16 id;
	char name[30];
}Bank_Data;

void CT_Deposit(){
	DEPOSIT_STEP cStep;
	char depositControlNumber[20], depositAmount[20];
	char information[INF_LENGHT];
	char * system_time;
	char time[15];
	char date[20];

	uint8 bank_index = 0;

	uint8 cTemp;

	Item bank_list[10];

	char * items[10] = {
		"Banesco",
		"Banco Mercantil",
		"Banco de Venezuela",
		"Corp. Banca",
		"Banco Bicentenario",
		"Banco del Tesoro",
		"Bancaribe",
		"Banplus",
		"B.O.D.",
		"Banco Provincial"
	};

	uint8 i;

	for(i=0;i<10;i++){
		sprintf(bank_list[i].name, items[i]);
		bank_list[i].id = i;
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
				cTemp = CT_Menu(bank_list, 10, &bank_index, TL_LIST, PNULL);
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
				if(cTemp==0xFB)
					cStep=DEPOSIT_CONFIRMATION;
				else if(cTemp==0xFF)
					cStep=DEPOSIT_CONTROL_NUMBER;
				else
					cStep=DEPOSIT_FAIL;
				break;

			case DEPOSIT_CONFIRMATION:
				sprintf(information, "Banco: %s\nNo. vaucher: %s\nMonto: %s Bs", bank_list[bank_index-1].name, depositControlNumber, depositAmount); 
				cTemp = CT_Confirm(information);

				if( cTemp == 0x00 ){
					cStep=DEPOSIT_START_COMM;
				}else{
					cStep=DEPOSIT_INIT;
				}
				break;
			
			case DEPOSIT_START_COMM:
				cTemp = CT_AddDeposit(bank_list[bank_index-1].id, depositControlNumber, depositAmount, time);
				
				if(cTemp==0x00){
					cStep = DEPOSIT_SUCCESS;
				}else if(cTemp==0x01){
					g_userData.sessionState = OFFLINE;
					sprintf(information,"Sesion Expirada. Ingrese Nuevamente");
					CT_Notify(information,0);
					cStep=DEPOSIT_FAIL;
				}else{
					sprintf(information,"Error de comunicacion. Intente nuevamente.");
					CT_Notify(information,0);
					cStep=DEPOSIT_FAIL;
				}
				break;	

			case DEPOSIT_SUCCESS:
				CT_Notify("Deposito Agregado\nExitosamente",0);
				cStep=DEPOSIT_PRINT_PROMPT;
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
				get_human_date(time, date);
				sprintf(information, "Fecha: %s\nBanco: %s\nNo. vaucher: %s\nMonto: %s Bs", date, bank_list[bank_index-1].name, depositControlNumber, depositAmount); 
				cTemp = CT_PrintReceipt("COMPROBANTE DE DEPOSITO", "", information, 0,0);

				cStep = DEPOSIT_EXIT;
				break;	

			case DEPOSIT_FAIL:
				cStep = DEPOSIT_EXIT;
				break;

		}
	}
}