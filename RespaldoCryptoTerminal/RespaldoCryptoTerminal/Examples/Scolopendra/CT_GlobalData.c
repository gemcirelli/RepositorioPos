#include "CT_GlobalData.h"
#include <time.h>

USER_DATA g_userData;
COMM_DATA g_commData;
int total_txn;

Item company_list_recharge[NUMBER_COMPANY_RECHARGE];
Item company_list_payment[NUMBER_COMPANY_PAYMENT];

char serials_to_confirm[LENGHT_CONFIRM_SERIAL];
char logs_to_confirm[LENGHT_CONFIRM_LOG];

//typedef int total_t;
double total_amount;
int32 total_txn_to_confirm;

void InitCommData(){
	//uint8 i;
	//char ports[10][7] = { "60000\0", "60001\0", "60002\0", "60003\0", "60004\0",
	//					  "60005\0", "60006\0", "60007\0", "60008\0", "60009\0" };
	//srand( time(0) );
	//i = rand()%10;
	sprintf(g_commData.apn,"internet.movistar.ve\x00");
	sprintf(g_commData.serverIp,"200.74.206.26\x00");
	sprintf(g_commData.serverPort,"60000\x00");
}

void InitCompanyList(){
	sprintf(company_list_recharge[0].name, "DirecTV");
	company_list_recharge[0].id = 1;

	sprintf(company_list_recharge[1].name, "Movilnet");
	company_list_recharge[1].id = 219;

	sprintf(company_list_payment[0].name, "Bigott");
	company_list_payment[0].id = 221;
}