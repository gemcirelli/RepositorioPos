#include "CT_Logout.h"
//#include "CT_GlobalData.h"
#include "CT_Input.h"

extern USER_DATA g_userData;


void CT_Logout(){
	if( g_userData.sessionState == ONLINE ){
		g_userData.securityToken[0] = '\0';
		g_userData.sessionState = OFFLINE;
		g_userData.balance = 0;
		CT_Notify("Sesion cerrada", 0);
	}
}