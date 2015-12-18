#include <TPTypedef.h>
#include <TPPrint.h>
#include <TP_BarCode.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPFonts.h>
#include <TPLCD.h>
#include <TP_Singleime.h>
#include <TP_Punctuationime.h>
#include <TPSTK.h>

#include "TP_MainMenu.h"
#include "TP_ListBox.h"

#include "TP_DemoListBox.h"
#include "TP_Constract.h"
#include "TP_DialogTest.h"
#include "TP_FileTest.h"
#include "TP_UpgradeTest.h"
#include "TP_InputTest.h"
#include "TP_KeypadTest.h"
#include "TP_Demomenu.h"
#include "TP_Phone.h"
#include "TP_PrintTest.h"
#include "TP_RingTest.h"
#include "TP_SerialTest.h"
#include "TP_SimCard.h"
#include "TP_TcpipTest.h"
#include "TP_TimeSet.h"
#include "TP_USSD.h"
#include "tb_stk.h"
#include "TP_NetworkStatus.h"
#include "TP_SSTest.h"
#include "TP_ArabicTest.h"
#include "TP_MifareOneTest.h"

#include "Scolopendra\CT_Login.h"
#include "Scolopendra\CT_Logout.h"
#include "Scolopendra\CT_Password.h"
#include "Scolopendra\CT_Balance.h"
#include "Scolopendra\CT_Deposit.h"
#include "Scolopendra\CT_Recharge.h"
#include "Scolopendra\CT_File.h"
#include "Scolopendra\CT_Payment.h"
#include "Scolopendra\CT_Wallet.h"
#include "Scolopendra\CT_Input.h"
#include "Scolopendra\CT_Print.h"
#include "Scolopendra\CT_utils.h"
#include "Scolopendra\CT_ClosePOS.h"
#include "Scolopendra\CT_Logout.h"
#include "Scolopendra\CT_GlobalData.h"
#include "Scolopendra\CT_Setting.h"

//Global Data Variable
extern int16 user_type;
static TP_MenuThemeType menuThemeType = TP_MT_NONE;

uint8 MenuItemPic[26] =
{
	0X0C,0x0C,0x00,0xf2,0x0c,0x04,0x02,0x02,0x02,0x02,0x04,0x0c,0xf2,0x00,0x00,0x04,0x03,0x02,0x04,0x04,0x04,0x04,0x02,0x03,0x04,0x00
};
void ONMenuItemClick(uint8 ItemKey)
{
	char information[INF_LENGHT];
	int32 cTemp;
	SALE_INFO sale_info;
	char date[20];
	char pin[21];

	switch (ItemKey)
	{
	case 1: 
		CT_Login();
		break;
	case 2:
		CT_Balance();
		break;
	case 3:
		CT_Deposit();
		break;
	case 4:
		CT_Recharge();
		break;
	case 5:
		CT_Wallet();
		break;
	case 6:
		CT_Payment();  
		break;
	case 7:
		if( test_session() )
			CT_PrintLastTransaction();
		else{
			CT_Notify("Debe iniciar sesion para continuar.",0);
		}			
		break;
	case 31:
		CT_Password();
		break;
	case 10:
		CT_ClosePOS();
		break;
	case 11:
		CT_Logout();
		break;
	case 12:
		CT_InputInteger(pin, "Pin");
		break;
	case 21:
		CT_Setting();
		break;
	case 24:
		CT_Setting_show_key();
		break;
	case 186:
		TP_UpgradeTest();
		break;
	}
}
void ON_MenuItemFoot(uint8 ItemKey,uint8* Value)
{
	sprintf((char *)Value,"%d",ItemKey);}

void TP_LoadMenu()
{
	TP_CharSetT curCharSet;
	TP_STKStringT stkAppName;
	TP_GetCurrentCharSet(&curCharSet);
	TP_CreateMenu(menuThemeType, ONMenuItemClick, ON_MenuItemFoot);
	TP_SetMenuIconShowMode(TP_MENU_SHOWICON);
	TP_AddMenuItem(0,1,(uint8*)"Iniciar sesion",MenuItemPic);
	if(user_type == INDEP_SELL){
		TP_AddMenuItem(0,2,(uint8*)"Consulta de saldo",MenuItemPic);
		TP_AddMenuItem(0,3,(uint8*)"Agregar Deposito",MenuItemPic);
		//TP_AddMenuItem(0,6,(uint8*)"Pago de factura",MenuItemPic);
	}
	TP_AddMenuItem(0,20,(uint8*)"Configuraciones",MenuItemPic);
	if(user_type > 0 && user_type){
		TP_AddMenuItem(0,4,(uint8*)"Recarga electronica",MenuItemPic);
		//TP_AddMenuItem(0,5,(uint8*)"Pago electronico",MenuItemPic);
		TP_AddMenuItem(0,7,(uint8*)"Ultima transaccion",MenuItemPic);		
		TP_AddMenuItem(0,10,(uint8*)"Cierre de POS",MenuItemPic);
		TP_AddMenuItem(0,11,(uint8*)"Cerrar Sesion",MenuItemPic);
		TP_AddMenuItem(0,31,(uint8*)"Cambio de clave",MenuItemPic);
	}
	TP_AddMenuItem(20,21,(uint8*)"Avanzado", MenuItemPic);
	TP_AddMenuItem(20,186,(uint8*)"Actualizacion", MenuItemPic);
	TP_AddMenuItem(20,24,(uint8*)"keyGen", MenuItemPic);
	TP_SortMenuItem(); //teclas(flechas) para desplazarse a lo largo de los items mostrados en el menu
} 
void TP_MenuShow()
{
	TP_LoadMenu();
	TP_ShowMenu(0,1);
}
