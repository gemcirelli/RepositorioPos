#include "CT_Logout.h"
#include "TP_DemoMenu.h"
//#include "CT_GlobalData.h"
#include "CT_Input.h"

extern USER_DATA g_userData;
extern COMPANIES_TO_PRINT_TICKET g_print_ticket_companies_DATA;
extern int16 user_type;

void CT_Logout(){
	uint8 i;
	if( g_userData.sessionState == ONLINE){
		g_userData.securityToken[0] = '\0';
		g_userData.sessionState = OFFLINE;
		g_userData.balance = 0;
		g_userData.proc_code[0] = '\0';
		for(i=0; i<SERVICE_COMPANIES_TOTAL; i++){
			g_print_ticket_companies_DATA.print_ticket_companies[i]=0;
		}
		memset(g_userData.userName, 0x00, sizeof(g_userData.userName));
		user_type = 0;
		CT_Notify("Sesion cerrada", 0);
		TP_LoadMenu();
	}
}