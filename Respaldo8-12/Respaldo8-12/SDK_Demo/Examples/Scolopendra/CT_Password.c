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
#include "CT_Password.h"
#include "CT_CryptoManager.h"
#include "CT_Utils.h"
#include "CT_GlobalData.h"

extern USER_DATA g_userData;
extern char response_message[MESSAGE_LENGHT];
extern char log_owner_userName[USER_NAME_LENGTH];
extern char log_owner_usernumber[USER_NUMBER_LENGTH];

void CT_Password(){

	PASSWORD_STEP cStep;
	char userName[20],userPass[20],userNewPass[20],userConfirmNewPass[20],message[128],balance[20];
	char information[INF_LENGHT];
	uint8 cTemp;
	int32 result;

	cStep=PASSWORD_INIT;
	while(cStep!=PASSWORD_EXIT)
	{	
		switch(cStep)
		{
			case PASSWORD_INIT:
				cStep=PASSWORD_ENTER_USERNAME;
				break;
				
			case PASSWORD_ENTER_USERNAME:					
				cTemp=CT_InputString(userName,"Usuario");
				if(cTemp==0xFB && strlen(userName) > 0) {
					if ((strlen(log_owner_userName) == 0) || (strcmp(log_owner_userName, userName) == 0)) {
						cStep=PASSWORD_ENTER_CURRENT_PASSWORD;
					} else {
						sprintf(information,"Debe realizar cierre del POS con el usuario \'%s\'",log_owner_userName);
						CT_Notify(information,0);
						cStep = PASSWORD_EXIT;
					}
				} else if(cTemp==0xFF)
					cStep=PASSWORD_EXIT;
				else
					cStep=PASSWORD_FAIL;
				break;		
			
			case PASSWORD_ENTER_CURRENT_PASSWORD:					
				cTemp=CT_InputHidden(userPass,"Clave actual");
				if(cTemp==0xFB)
					cStep=PASSWORD_ENTER_NEW_PASSWORD;
				else if(cTemp==0xFF){
					cStep=PASSWORD_ENTER_USERNAME;
					userName[0] = '\0';
					userPass[0] = '\0';
					userNewPass[0] = '\0';
					userConfirmNewPass[0] = '\0';
				}else
					cStep=PASSWORD_FAIL;
				break;

			case PASSWORD_ENTER_NEW_PASSWORD:
				cTemp=CT_InputHidden(userNewPass,"Nueva clave");
				if(cTemp==0xFB)
					if(strlen(userNewPass)>6){
						cStep=PASSWORD_CONFIRM_NEW_PASSWORD;
					}else{
						CT_Notify("La clave debe contener al menos 7 digitos",0);
					}
				else if(cTemp==0xFF){
					cStep=PASSWORD_ENTER_CURRENT_PASSWORD;
					userPass[0] = '\0';
					userNewPass[0] = '\0';
					userConfirmNewPass[0] = '\0';
				} else
					cStep=PASSWORD_FAIL;
				break;

			case PASSWORD_CONFIRM_NEW_PASSWORD:
				cTemp=CT_InputHidden(userConfirmNewPass,"Confirme clave");
				if(cTemp==0xFB)
					if(strcmp(userNewPass,userConfirmNewPass)==0){
					cStep=PASSWORD_START_COMM;
					} else {
						CT_Notify("La nueva clave no coincide con la confirmacion",0); 
						cStep=PASSWORD_ENTER_NEW_PASSWORD;
					}
				else if(cTemp==0xFF){
					cStep=PASSWORD_ENTER_NEW_PASSWORD;
					userNewPass[0] = '\0';
					userConfirmNewPass[0] = '\0';
				} else
					cStep=PASSWORD_FAIL;
				break;	
			
			case PASSWORD_START_COMM:
				
				result=CT_ChangePassword(userName,userPass,userNewPass);
				
				if(result==0x00){
					CT_Notify(response_message,0);
					cStep=PASSWORD_SUCCEED;
				}else if(result==0x01){
					sprintf(information, "Error: %s", response_message);
					CT_Notify(information,0);
					cStep=PASSWORD_FAIL;
				}else{
					sprintf(information, "Error: %s", response_message);
					CT_Notify(information,0);
					cStep=PASSWORD_ENTER_USERNAME;
				}
				break;	
			
			case PASSWORD_FAIL:
				cStep=PASSWORD_INIT;
				break;

			
			case PASSWORD_SUCCEED:				
				cStep=PASSWORD_EXIT;
				break;
		}
	}
}
