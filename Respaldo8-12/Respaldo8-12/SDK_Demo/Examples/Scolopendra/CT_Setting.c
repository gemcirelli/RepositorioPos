#include "CT_GlobalData.h"
#include "CT_Setting.h"
#include "CT_Input.h"
#include "CT_File.h"
#include "CT_Utils.h"


//Global Data Variable
extern COMM_DATA g_commData;
extern int16 user_type;
extern char scolo_ports[10][6];
extern int total_txn;
extern double total_amount;
extern int32 total_txn_to_confirm;
extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL]; 

//Local Variable
Item settings_choice_list[NUMBER_SETTING_LIST];
Item comm_list[NUMBER_COMM_LIST];
Item comm_choice_type[NUMBER_COMM_CHOISE_LIST];
Item comm_apn_list[NUMBER_COMM_APN_LIST];
Item comm_port_list[NUMBER_COMM_PORT_LIST];

void CT_Setting(){

	SETTINGS_STEP cStep;

	uint8 setting_index = 0;
	char userPass[20];
	char information[INF_LENGHT];
	uint8 cTemp;

	InitSettingList();

	if(CT_Setting_verify() == 0){

		cStep=SETTINGS_INIT;
		while(cStep!=SETTINGS_EXIT)
		{
			switch(cStep)
			{
			case SETTINGS_INIT:
				cTemp = CT_Menu(settings_choice_list, NUMBER_SETTING_LIST, &setting_index, TL_LIST, (uint8*) "Opcion");
				if( cTemp == 0x01 ){
					switch(setting_index)
					{
					case SETTINGS_COMM: cStep = SETTINGS_COMM;break;
					case SETTINGS_CREATE_LOG_FILE: cStep = SETTINGS_CREATE_LOG_FILE;break;
					case SETTINGS_READ_LOG_FILE: cStep = SETTINGS_READ_LOG_FILE;break;
					case SETTINGS_DELETE_LOG_FILE: cStep = SETTINGS_DELETE_LOG_FILE;break;
					}
				}else if( cStep == 0x00 ){
					cStep = SETTINGS_EXIT;
				}else{
					cStep = SETTINGS_INIT;
				}
				break;
			case SETTINGS_COMM:
				CT_Connectivity();
				cStep = SETTINGS_INIT;
				break;
			case SETTINGS_CREATE_LOG_FILE: //### managment files #### PLEASE: DONT REMOVE COMMENT LINES
				cTemp = CT_CreateLogFile();

				if( cTemp < 0x00 ){
					sprintf(information, "Error: %s", CT_MessageError(TP_GetLastError()) );
					CT_Notify(information, 0);
				}else{
					sprintf(information, "Arhivo log creado! %.2fKB", (float)cTemp/1000);
					CT_Notify(information, 0);
				}
				cStep = SETTINGS_INIT;
				break;
			case SETTINGS_READ_LOG_FILE:
				cTemp = CT_ReadLogFile();

				if( cTemp < 0x00 ){
					sprintf(information, "Error: %s", CT_MessageError(cTemp) );
					CT_Notify(information, 0);
				}else{
					sprintf(information, "Txn: %d\nAmount: %.02f\nS/C: %d", total_txn, total_amount, cTemp);
					CT_Notify(information, 0);
				}
				cStep = SETTINGS_INIT;
				break;
			case SETTINGS_DELETE_LOG_FILE:
				cTemp = CT_DeleteLogFile();

				if( cTemp != 0x00 ){
					sprintf(information, "Error: %s", CT_MessageError(cTemp) );
					CT_Notify(information, 0);
				}else{
					CT_DeleteLastTransFile();
					memset(serials_to_confirm, 0x00, LENGHT_CONFIRM_SERIAL);
					serials_to_confirm[0] = '\0';
					total_txn = 0;
					total_txn_to_confirm = 0;
					total_amount = 0;
					CT_Notify("Archivo Eliminado", 0);
				}
				cStep = SETTINGS_INIT;
				break;
			}
		}
	}
}


void CT_Setting_show_key(){
	char information[INF_LENGHT];
	char key[20];
	sprintf(key, "%u", KeyGen());
	sprintf(information, "Key: %s", key);
	CT_Notify(information,0);
}


uint8 CT_Setting_verify(){
	uint8 result = 0;
	char userPass[20];
	char information[INF_LENGHT];
	uint8 cTemp;
	char key[20];

	sprintf(key, "%u", KeyGen());
  //  result = CT_InputHidden(userPass,"Clave"); antigua llamada 
	result= CT_InputAnything(userPass,"Clave",CT_HIDDEN);
	if(result == 0xFB){
		if(strcmp(userPass, key) != 0){
			sprintf(information, "Error: Clave incorrecta");
			CT_Notify(information,0);
			result = 1;
		} else {
			result = 0;
		}
	}
	return result;
}

void CT_Connectivity(){
	
	SETTINGS_INFO setting_info;
	COMM_STEP cStep;
	char information[INF_LENGHT];

	uint8 comm_index = 0;
	uint8 choice_index = 0;
	uint8 comm_port_index = 0;
	uint8 apn_index = 0;
	uint8 cTemp;

	CT_ReadCFGFile(&setting_info);

	InitCommPort(setting_info.serverPort);
	InitCommList();
	InitCommChoise();
	InitCommApnList();		

	cStep=COMM_INIT;
	while(cStep!=COMM_EXIT)
	{
		switch(cStep)
		{
		case COMM_INIT:
			cTemp = CT_Menu(comm_list, NUMBER_COMM_LIST, &comm_index, TL_LIST, (uint8*) "Opcion");

			if( cTemp == 0x01 ){
				switch(comm_index)
				{
					case COMM_SERVER: cStep = COMM_SERVER;break;
					case COMM_PORT: cStep = COMM_PORT;break;
					case COMM_APN: cStep = COMM_APN;break;
					case COMM_REST: cStep = COMM_REST;break;
					case COMM_USERNAME: cStep = COMM_USERNAME;break;
					case COMM_PASSWORD: cStep = COMM_PASSWORD;break;
				}
			}else if( cStep == 0x00 ){
				cStep = COMM_EXIT;
			}else{
				cStep = COMM_EXIT;
			}
			break;
		case COMM_SERVER:
			cTemp = CT_InputAnything(g_commData.serverIp,"IP Servidor", CT_IP);
			if(cTemp==0xFB && strlen(g_commData.serverIp) > 0){
				sprintf(setting_info.serverIp,g_commData.serverIp);
				CT_WriteCFGFile(setting_info);
				sprintf(information, "Success");
				CT_Notify(information,0);
				cStep=COMM_INIT;
			}else if(cTemp==0xFF){
				cStep=COMM_EXIT;
			}else{
				sprintf(information, "Fail");
				CT_Notify(information,0);
				cStep=COMM_FAIL;
			}
			break;
		case COMM_PORT:	
			cTemp = CT_Menu(comm_port_list, NUMBER_COMM_PORT_LIST, &comm_port_index, TL_LIST, (uint8*) "Opcion");
			if(cTemp == 0x01){
				//cTemp=CT_InputInteger(scolo_ports[comm_port_index-1],"Puerto");
				cTemp=CT_InputAnything(scolo_ports[comm_port_index-1],"Puerto",1);
				if(cTemp==0xFB && strlen(scolo_ports[comm_port_index-1]) > 0){
					sprintf(comm_port_list[comm_port_index-1].name, scolo_ports[comm_port_index-1]);//copy port in list
					sprintf(setting_info.serverPort[comm_port_index-1], scolo_ports[comm_port_index-1]);//copy port in the settings
					CT_WriteCFGFile(setting_info);//Write file setting
					sprintf(information, "success");
					CT_Notify(information,0);
					cStep=COMM_PORT;
				}else if(cTemp==0xFF){
					cStep=COMM_INIT;
				}else{
					cStep=COMM_FAIL;
				}
				break;
			}else if( cStep == 0x00 ){
				cStep = COMM_INIT;break;
			}else{
				cStep = COMM_EXIT;break;
			}
			sprintf(information, "Error: En desarrollo");
			CT_Notify(information,0);
			cStep=COMM_INIT;
			break;
		case COMM_APN:
			cTemp = CT_Menu(comm_choice_type, NUMBER_COMM_CHOISE_LIST, &choice_index, TL_LIST, (uint8*) "Opcion");
			if(cTemp == 0x01){
				if(choice_index == 1){
					//cTemp=CT_InputString(g_commData.apn,"APN");//antigua llamada
					cTemp=CT_InputAnything(g_commData.apn,"APN",2);
					if(cTemp==0xFB && strlen(g_commData.apn) > 0){
						sprintf(setting_info.apn, g_commData.apn);//copy APN in the settings
						CT_WriteCFGFile(setting_info);//Write file setting
						sprintf(information, "success");
						CT_Notify(information,0);
						cStep=COMM_INIT;
					}else if(cTemp==0xFF){
						cStep=COMM_EXIT;
					}else{
						cStep=COMM_FAIL;
					}
					break;
				}else{
					cTemp = CT_Menu(comm_apn_list, NUMBER_COMM_APN_LIST, &apn_index, TL_LIST, (uint8*) "Opcion");
					if( cTemp == 0x01 ){
						sprintf(g_commData.apn,comm_apn_list[apn_index-1].name);
						sprintf(setting_info.apn, g_commData.apn);//copy APN in the settings
						CT_WriteCFGFile(setting_info);//Write file setting
						sprintf(information, "success");
						CT_Notify(information,0);
						if(TP_WlPPPCheck() == 0){
							TP_WlPPPLogout(); 
						}
						cStep = COMM_INIT;
					}else if( cStep == 0x00 ){
						cStep = COMM_EXIT;
					}else{
						cStep = COMM_EXIT;
					}
					break;
				}
			}else if( cStep == 0x00 ){
				cStep = COMM_INIT;break;
			}else{
				cStep = COMM_EXIT;break;
			}
		case COMM_REST:
			cTemp = CT_Confirm("¿Desea restaurar los valores de conectividad?");
			if( cTemp == 0x00 ){
				CT_InitCFGFile();
				InitData();
				CT_ReadCFGFile(&setting_info);
				InitCommPort(setting_info.serverPort);
				cStep = COMM_INIT;
			}else{
				cStep = COMM_EXIT;
			}
			break;
		case COMM_USERNAME:
			//cTemp = CT_InputString(g_commData.userName,"Username");
			cTemp = CT_InputAnything(g_commData.userName,"Username",2);
			if(cTemp==0xFB){
				sprintf(information, "success");
				CT_Notify(information,0);
				cStep=COMM_INIT;
			}else if(cTemp==0xFF){
				cStep=COMM_EXIT;
			}else{
				cStep=COMM_FAIL;
			}
			break;
		case COMM_PASSWORD:
			//cTemp = CT_InputString(g_commData.password,"Password");
			cTemp = CT_InputAnything(g_commData.password,"Password",2);		
			if(cTemp==0xFB){
				sprintf(information, "success");
				CT_Notify(information,0);
				cStep=COMM_INIT;
			}else if(cTemp==0xFF){
				cStep=COMM_EXIT;
			}else{
				cStep=COMM_FAIL;
			}
			break;
		case COMM_FAIL:

			cStep=COMM_INIT;
			break;
		}
	}
}

void InitSettingList(){
	sprintf(settings_choice_list[0].name, "Config. Conectividad");
	comm_list[0].id = 0;
	sprintf(settings_choice_list[1].name, "Leer Log File");
	comm_list[1].id = 1;
	sprintf(settings_choice_list[2].name, "Crear Log File");
	comm_list[2].id = 2;	
	sprintf(settings_choice_list[3].name, "Eliminar Log File");
	comm_list[3].id = 3;
}

void InitCommList(){
	sprintf(comm_list[0].name, "Server");
	comm_list[0].id = COMM_SERVER;

	sprintf(comm_list[1].name, "Puerto");
	comm_list[1].id = COMM_PORT;

	sprintf(comm_list[2].name, "APN");
	comm_list[2].id = COMM_APN;

	sprintf(comm_list[3].name, "Restaurar Conect.");
	comm_list[3].id = COMM_REST;

	//sprintf(comm_list[3].name, "Username"); //It is not included in this list, for inclusion in the list change NUMBER_COMM_LIST
	//comm_list[3].id = COMM_USERNAME;

	//sprintf(comm_list[4].name, "Password"); //It is not included in this list, for inclusion in the list change NUMBER_COMM_LIST
	//comm_list[4].id = COMM_PASSWORD;
}

void InitCommChoise(){
	sprintf(comm_choice_type[0].name, "Manual");
	comm_choice_type[0].id = 0;

	sprintf(comm_choice_type[1].name, "Lista");
	comm_choice_type[1].id = 1;

}

void InitCommApnList(){
	sprintf(comm_apn_list[0].name, "apn01.cwpanama.com.pa");
	comm_apn_list[0].id = 0;

	sprintf(comm_apn_list[1].name, "web.digicelpanama.com");
	comm_apn_list[1].id = 1;

	sprintf(comm_apn_list[2].name, "internet.movistar.pa");
	comm_apn_list[2].id = 2;

	sprintf(comm_apn_list[3].name, "web.claro.com.pa");
	comm_apn_list[3].id = 3;

	sprintf(comm_apn_list[4].name, "gprsweb.digitel.ve");
	comm_apn_list[4].id = 4;

	sprintf(comm_apn_list[5].name, "internet.movistar.ve");
	comm_apn_list[5].id = 5;

	sprintf(comm_apn_list[6].name, "int.movilnet.com.ve");
	comm_apn_list[6].id = 6;

	sprintf(comm_apn_list[7].name, "internet.ideasclaro.com.do");
	comm_apn_list[7].id = 7;
}

void InitCommPort(const char serverPort[NUMBER_COMM_PORT_LIST][6]){
	int i;
	for(i = 0; i < NUMBER_COMM_PORT_LIST; i++){
		sprintf(comm_port_list[i].name, serverPort[i]);
		comm_port_list[i].id = i;	
	}
}