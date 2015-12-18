#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TP_SingleIME.h>

#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"
#include "CT_Input.h"
#include "CT_Login.h"
#include "CT_CryptoManager.h"
#include "CT_Utils.h"
//#include "CT_GlobalData.h"

extern USER_DATA g_userData;

void CT_Login(){

	LOGIN_STEP cStep;
	char userName[20],userPass[20],message[128],balance[20];
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
				if(cTemp==0xFB)
					cStep=LOGIN_ENTER_PASSWORD;
				else if(cTemp==0xFF)
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
				result=CT_GetSecurityToken(userName,userPass);
				
				if(result==0x00){
					g_userData.sessionState = ONLINE;
					point_to_comma_decimal(g_userData.balance,balance);
					sprintf(message,"Bienvenido %s\nSaldo: %s Bs",userName,balance);
					CT_Notify(message,0);
					cStep=LOGIN_SUCCEED;
				}else if(result==0x01){
					g_userData.sessionState = OFFLINE;
					sprintf(message,"Error. Usuario o clave invalidos.");
					CT_Notify(message,0);
					cStep=LOGIN_FAIL;
				}else{
					g_userData.sessionState = OFFLINE;
					sprintf(message,"Error: %d", result);
					CT_Notify(message,0);
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
