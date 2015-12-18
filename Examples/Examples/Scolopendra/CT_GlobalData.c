#include "CT_GlobalData.h"
#include <stdio.h>
#include <time.h>
#include <TPNet.h>
#include "CT_Input.h"
#include "CT_utils.h"
#include "CT_Setting.h"
#include "CT_File.h"

USER_DATA g_userData;
COMM_DATA g_commData;
COMPANIES_TO_PRINT_TICKET g_print_ticket_companies_DATA;
int total_txn;
int16 user_type;

Item company_list_recharge[NUMBER_COMPANY_RECHARGE];
Item company_list_payment[NUMBER_COMPANY_PAYMENT];

char serials_to_confirm[LENGHT_CONFIRM_SERIAL];
char log_owner_userName[USER_NAME_LENGTH];
char log_owner_usernumber[USER_NUMBER_LENGTH];
char logs_to_confirm[LENGHT_CONFIRM_LOG];
char response_message[MESSAGE_LENGHT];
//typedef int total_t;
double total_amount;
int32 total_txn_to_confirm;

char scolo_ports[10][6];

void InitData(){
	int i;
	uint32 sendRetrys;
	int32 result;
	uint8 imsi[60];
	SETTINGS_INFO recv_setting_info;

	char imei[20];
	uint8 imei2[20];
	char * str;
	memset(imei2,0,sizeof(imei2));
    TP_GetModalIMEI(imei2, sizeof(imei2));
	str = (char*) imei2;
	strncpy(imei, str+8, 15);

	g_userData.sessionState = OFFLINE;
	sprintf(g_userData.posId,imei);
	user_type = 0;

	CT_ReadCFGFile(&recv_setting_info);
	sprintf(g_commData.serverIp,recv_setting_info.serverIp);

	for(i = 0; i <= 10; i++){
		sprintf(scolo_ports[i],recv_setting_info.serverPort[i]);
	}

	TP_GetMobileCardIMSI(imsi, sizeof(imsi));

	if(imsi[4] == '2'){
		sprintf(g_commData.apn,"gprsweb.digitel.ve\x00");
		sprintf(recv_setting_info.apn,"gprsweb.digitel.ve\x00");
		CT_WriteCFGFile(recv_setting_info);
	}else if(imsi[4] == '4'){
		sprintf(g_commData.apn,"internet.movistar.ve\x00");
		sprintf(recv_setting_info.apn,"internet.movistar.ve\x00");
		CT_WriteCFGFile(recv_setting_info);
	}else if(imsi[4] == '6'){
		sprintf(g_commData.apn,"int.movilnet.com.ve\x00");
		sprintf(recv_setting_info.apn,"int.movilnet.com.ve\x00");
		CT_WriteCFGFile(recv_setting_info);
	}else{
		sprintf(g_commData.apn, recv_setting_info.apn);
	}

	sprintf(serials_to_confirm, "");
	sprintf(log_owner_userName, "");
	sprintf(log_owner_usernumber, "");

}

void change_port(){
	uint8 i;
	char * time;
	char * system_time;

	system_time = (char*) get_system_time();
	i = (system_time[13]-48);

	sprintf(g_commData.serverPort,"%s",scolo_ports[i]);
	system_time = PNULL;
}

void InitCompanyList(){
	sprintf(company_list_recharge[0].name, "DIRECTV");
	company_list_recharge[0].id = 1;

	sprintf(company_list_recharge[1].name, "Movilnet");
	company_list_recharge[1].id = 219;

	sprintf(company_list_payment[0].name, "Bigott");
	company_list_payment[0].id = 221;
}