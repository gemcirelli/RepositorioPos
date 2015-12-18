#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TP_SingleIME.h>

#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"
#include "TP_DemoMenu.h"
#include "CT_File.h"
#include "CT_Input.h"
#include "CT_Login.h"
#include "CT_CryptoManager.h"
#include "CT_Utils.h"
#include "CT_GlobalData.h"

extern USER_DATA g_userData;
extern char response_message[MESSAGE_LENGHT];
extern int16 user_type;
extern char log_owner_userName[USER_NAME_LENGTH];
extern char log_owner_usernumber[USER_NUMBER_LENGTH];

void CT_Login(){

	LOGIN_STEP cStep;
	char userName[20],userPass[20],message[128],balance[20];
	char information[INF_LENGHT];
	uint8 cTemp;
	int32 result;

	cStep=LOGIN_INIT;
	while(cStep!=LOGIN_EXIT)
	{	
		switch(cStep)
		{
			case LOGIN_INIT:
				cStep=LOGIN_ENTER_USERNAME;
				break;
				
			case LOGIN_ENTER_USERNAME:					
				cTemp=CT_InputString(userName,"Usuario");
				if(cTemp==0xFB && strlen(userName) > 0) {
					if ((strlen(log_owner_userName) == 0) || (strcmp(log_owner_userName, userName) == 0)) {
						cStep=LOGIN_ENTER_PASSWORD;
					} else {
						sprintf(information,"Debe realizar cierre del POS con el usuario \'%s\'",log_owner_userName);
						CT_Notify(information,0);
						cStep = LOGIN_EXIT;
					}
				} else if(cTemp==0xFF)
					cStep=LOGIN_EXIT;
				else
					cStep=LOGIN_FAIL;
				break;		
			
			case LOGIN_ENTER_PASSWORD:					
				cTemp=CT_InputHidden(userPass,"Clave");
				if(cTemp==0xFB)
					cStep=LOGIN_START_COMM;
				else if(cTemp==0xFF)
					cStep=LOGIN_ENTER_USERNAME;
				else
					cStep=LOGIN_FAIL;
				break;	
			
			case LOGIN_START_COMM:
				/*Close the previous sesion if one exists*/
				if( g_userData.sessionState == ONLINE){
					g_userData.securityToken[0] = '\0';
					g_userData.sessionState = OFFLINE;
					g_userData.balance = 0;
					memset(g_userData.userName, 0x00, sizeof(g_userData.userName));
					g_userData.proc_code[0] = '\0';
					user_type = 0;
		
				}
				result=CT_GetSecurityToken(userName,userPass);
				
				if(result==0x00){
					g_userData.sessionState = ONLINE;
					point_to_comma_decimal(g_userData.balance,balance);

					switch(user_type){
						case CC_SELL:
							sprintf(message,"Bienvenido %s",userName);
							break;
						case INDEP_SELL:
							sprintf(message,"Bienvenido %s\nSaldo: %s Bs",userName,balance);
							break;
						default:
							sprintf(message,"Bienvenido %s",userName);
							break;
					}

					CT_Notify(message,0);
					TP_LoadMenu();
					cStep=LOGIN_SUCCEED;
				}else if(result==0x01){
					g_userData.sessionState = OFFLINE;
					sprintf(information, "Error: %s", response_message);
					CT_Notify(information,0);
					cStep=LOGIN_FAIL;
				}else{
					g_userData.sessionState = OFFLINE;
					sprintf(information, "Error: %s", response_message);
					CT_Notify(information,0);
					cStep=LOGIN_ENTER_USERNAME;
					//CommError(1);
				}
				break;	
			
			case LOGIN_FAIL:
				
				cStep=LOGIN_INIT;
				break;

			
			case LOGIN_SUCCEED:
				//AppPrnTopup();
				//LoginSuccessful(1);
				
				cStep=LOGIN_EXIT;
				break;
		}
	}
}
