#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TP_SingleIME.h>

#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"
#include "CT_Input.h"
#include "CT_Balance.h"
#include "CT_CryptoManager.h"
//#include "CT_GlobalData.h"
#include "CT_Print.h"
#include "CT_Utils.h"

extern USER_DATA g_userData;

void CT_Balance(){


	BALANCE_STEP cStep;
	char message[128];
	uint8 cTemp;
	char date[20], balance[20], pendingBalance[20];
	char * system_time;
	char time[15];


	cStep=BALANCE_INIT;
	while(cStep!=BALANCE_EXIT)
	{	
		switch(cStep)
		{
			case BALANCE_INIT:
				if( test_session() )
					cStep=BALANCE_START_COMM;
				else{
					sprintf(message,"Debe iniciar sesion para continuar.");
					CT_Notify(message,0);
					cStep = BALANCE_FAIL;
				}
				break;
			
			case BALANCE_START_COMM:
				cTemp=CT_GetBalance();
				if(cTemp==0x00){
					point_to_comma_decimal(g_userData.balance,balance);
					point_to_comma_decimal(g_userData.pendingBalance,pendingBalance);
					sprintf(message,"\t\t\t\t\t\t\tSaldo\n Act: %s Bs\n Dif: %s Bs",balance,pendingBalance);
					CT_Notify(message,0);
					cStep=BALANCE_EXIT;
				}else if(cTemp==0x01){
					g_userData.sessionState = EXPIRED;
					sprintf(message,"Sesion expirada. Ingrese nuevamente.");
					CT_Notify(message,0);
					cStep=BALANCE_FAIL;
				}else{
					sprintf(message,"Error de comunicacion. Intente nuevamente.");
					CT_Notify(message,0);
					cStep=BALANCE_FAIL;
					//CommError(1);
				}
				break;	
			
/*			case BALANCE_PRINT_PROMPT:
				sprintf(message,"¿Imprimir recibo?");
				cTemp = CT_Confirm(message);
				if(cTemp==0x00){
					cStep=BALANCE_PRINT;
				}else{
					cStep=BALANCE_EXIT;
				}
				break;

			case BALANCE_PRINT:
				system_time = (char *)get_system_time();
				memcpy(time, system_time, 15);
				get_human_date(time, date);

				point_to_comma_decimal(g_userData.balance,balance);
				point_to_comma_decimal(g_userData.pendingBalance,pendingBalance);
				sprintf(message,"Fecha: %s\nSaldo actual: %s Bs\nSaldo diferido: %s Bs\n",date, balance, pendingBalance);
				CT_PrintReceipt("CONSULTA DE SALDO","",message,0,1,0);
				cStep=BALANCE_EXIT;
				break;
*/
			case BALANCE_FAIL:
				cStep=BALANCE_EXIT;
				break;
		}
	}
}