#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPFonts.h>
#include <TPLCD.h>
#include <TP_Singleime.h>
#include <TP_Punctuationime.h>
#include <TPSTK.h>

#include "TP_MainMenu.h"

#include "TP_ListBox.h"

#include "TP_ResourceMM.h"

#include "TP_DisplayAPI.h"
#include "TP_DemoDefine.h"

#ifdef INCLUDE_MESSAGE_BOX
#include "TP_MSG_MenuID.h"
#include <TPSMS.h>
#include "TP_SmsStore.h"
#include "TP_MSGMenu.h"
#endif

#ifdef INCLUDE_CALL_RECORD
#include "TP_CallRecord_MenuID.h"
#include "TP_CallRecord.h"
#endif


#ifdef INCLUDE_PHONE_SETTING
#include "TP_PhoneSetting.h"
#endif
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
#include "Scolopendra\CT_Balance.h"
#include "Scolopendra\CT_Deposit.h"
#include "Scolopendra\CT_Recharge.h"
#include "Scolopendra\CT_File.h"
#include "Scolopendra\CT_Payment.h"
#include "Scolopendra\CT_Input.h"
#include "Scolopendra\CT_Print.h"
#include "Scolopendra\CT_utils.h"
#include "Scolopendra\CT_ClosePOS.h"
#include "Scolopendra\CT_Logout.h"

extern ST_FONT GoblaFont[32];

extern int total_txn;
extern double total_amount;
extern int32 total_txn_to_confirm;
extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL]; 

static TP_MenuThemeType menuThemeType = TP_MT_NONE;

#ifdef INCLUDE_MULTI_LANGUAGE
void TP_LanguageChooseDemo();
#endif

uint8 MenuItemPic[26] =
{
    0X0C,0x0C,0x00,0xf2,0x0c,0x04,0x02,0x02,0x02,0x02,0x04,0x0c,0xf2,0x00,0x00,0x04,0x03,0x02,0x04,0x04,0x04,0x04,0x02,0x03,0x04,0x00
};

///Spanish
static TP_KeyMaps SpanishKeyMap =
{
    12,

    {
        {TP_KEY_0,(uint8*)"\0"},
        {TP_KEY_1,(uint8*)"\0"},
        {TP_KEY_2,(uint8*)"\xC1\xCD"},
        {TP_KEY_3,(uint8*)"\xD3\xDA"},
        {TP_KEY_4,(uint8*)"\xE1\xED"},
        {TP_KEY_5,(uint8*)"\xF3\xFA"},
        {TP_KEY_6,(uint8*)"\0"},
        {TP_KEY_7,(uint8*)"\0"},
        {TP_KEY_8,(uint8*)"\0"},
        {TP_KEY_9,(uint8*)"\0"},
        {TP_KEY_STAR,(uint8*)"\0"},
        {TP_KEY_POUND,(uint8*)"\0"}
    }
};
static TP_KeyMaps SpanishUpCaseMap =
{
    12,

    {
        {TP_KEY_0,(uint8*)"\0"},
        {TP_KEY_1,(uint8*)"\xFD\xFF\xB0\xBF\xA8\xAA"},
        {TP_KEY_2,(uint8*)"\xC0\xC1\xC2\xC3"},
        {TP_KEY_3,(uint8*)"\xC8\xC9\xCA\xCB"},
        {TP_KEY_4,(uint8*)"\xCC\xCD\xCE\xCF"},
        {TP_KEY_5,(uint8*)"\0"},
        {TP_KEY_6,(uint8*)"\xD1\xD2\xD3\xD4\xD5"},
        {TP_KEY_7,(uint8*)"\0"},
        {TP_KEY_8,(uint8*)"\xD9\xDA\xDB\xDC"},
        {TP_KEY_9,(uint8*)"\0"},
        {TP_KEY_STAR,(uint8*)"\0"},
        {TP_KEY_POUND,(uint8*)"\0"}
    }
};
static TP_KeyMaps SpanishLowCaseMap =
{
    12,

    {
        {TP_KEY_0,(uint8*)"\0"},
        {TP_KEY_1,(uint8*)"\xFD\xFF\xB0\xBF\xA8\xAA"},
        {TP_KEY_2,(uint8*)"\xE0\xE1\xE2\xE3"},
        {TP_KEY_3,(uint8*)"\xE8\xE9\xEA\xEB"},
        {TP_KEY_4,(uint8*)"\xEC\xED\xEE\xEF"},
        {TP_KEY_5,(uint8*)"\0"},
        {TP_KEY_6,(uint8*)"\xF1\xF2\xF3\xF4\xF5"},
        {TP_KEY_7,(uint8*)"\0"},
        {TP_KEY_8,(uint8*)"\xF9\xFA\xFB\xFC"},
        {TP_KEY_9,(uint8*)"\0"},
        {TP_KEY_STAR,(uint8*)"\0"},
        {TP_KEY_POUND,(uint8*)"\0"}
    }
};
static TP_PUNC_INPUT SpanishPunc[] =
{
    {(uint8*)"\xFD", 0, (uint8*)"\xFD"},
    {(uint8*)"\xFF", 0, (uint8*)"\xFF"},
    {(uint8*)"\xB0", 0, (uint8*)"\xB0"},
    {(uint8*)"\xBF", 0, (uint8*)"\xBF"},
    {(uint8*)"\xA8", 0, (uint8*)"\xA8"},
    {(uint8*)"\xAA", 0, (uint8*)"\xAA"},
};

//French
static TP_KeyMaps FrenchUpCaseMap =
{
    12,

    {
        {TP_KEY_0,(uint8*)"\0"},
        {TP_KEY_1,(uint8*)"\xFF\xB0\xB2\xAA\xC7\xA7\xA8\xA3\xB5"},
        {TP_KEY_2,(uint8*)"\xC1\xC2"},
        {TP_KEY_3,(uint8*)"\xCA\xCB"},
        {TP_KEY_4,(uint8*)"\xCE\xCF"},
        {TP_KEY_5,(uint8*)"\0"},
        {TP_KEY_6,(uint8*)"\xD4\xD5"},
        {TP_KEY_7,(uint8*)"\0"},
        {TP_KEY_8,(uint8*)"\xDB\xDC"},
        {TP_KEY_9,(uint8*)"\0"},
        {TP_KEY_STAR,(uint8*)"\0"},
        {TP_KEY_POUND,(uint8*)"\0"}
    }
};
static TP_KeyMaps FrenchLowCaseMap =
{
    12,

    {
        {TP_KEY_0,(uint8*)"\0"},
        {TP_KEY_1,(uint8*)"\xFF\xB0\xB2\xAA\xC7\xA7\xA8\xA3\xB5"},
        {TP_KEY_2,(uint8*)"\xE0\xE2\xE3"},
        {TP_KEY_3,(uint8*)"\xE8\xE9\xEA\xEB"},
        {TP_KEY_4,(uint8*)"\xEE\xEF"},
        {TP_KEY_5,(uint8*)"\0"},
        {TP_KEY_6,(uint8*)"\xF4\xF5"},
        {TP_KEY_7,(uint8*)"\0"},
        {TP_KEY_8,(uint8*)"\xF9\xFB\xFC"},
        {TP_KEY_9,(uint8*)"\0"},
        {TP_KEY_STAR,(uint8*)"\0"},
        {TP_KEY_POUND,(uint8*)"\0"}
    }
};
static TP_PUNC_INPUT FrenchPunc[] =
{
    {(uint8*)"\xFF", 0, (uint8*)"\xFF"},
    {(uint8*)"\xB0", 0, (uint8*)"\xB0"},
    {(uint8*)"\xB2", 0, (uint8*)"\xB2"},
    {(uint8*)"\xAA", 0, (uint8*)"\xAA"},
    {(uint8*)"\xC7", 0, (uint8*)"\xC7"},
    {(uint8*)"\xA7", 0, (uint8*)"\xA7"},
    {(uint8*)"\xA8", 0, (uint8*)"\xA8"},
    {(uint8*)"\xA3", 0, (uint8*)"\xA3"},
    {(uint8*)"\xB5", 0, (uint8*)"\xB5"},
};



///Denmark
static TP_KeyMaps DenmarkKeyMap =
{
    12,

    {
        {TP_KEY_0,(uint8*)"\0"},
        {TP_KEY_1,(uint8*)"\0"},
        {TP_KEY_2,(uint8*)"\xC4\xC5\xC6"},
        {TP_KEY_3,(uint8*)"\xD6\xD8"},
        {TP_KEY_4,(uint8*)"\xE4\xE5\xE6"},
        {TP_KEY_5,(uint8*)"\xF6\xF8"},
        {TP_KEY_6,(uint8*)"\0"},
        {TP_KEY_7,(uint8*)"\0"},
        {TP_KEY_8,(uint8*)"\0"},
        {TP_KEY_9,(uint8*)"\0"},
        {TP_KEY_STAR,(uint8*)"\0"},
        {TP_KEY_POUND,(uint8*)"\0"}
    }
};

static void SetMenuTheme(uint16 ItemKey)
{
    switch (ItemKey)
    {
    case 27 : //MenuLoop Display ON
        switch (menuThemeType)
        {
        case TP_MT_SHOWFOOT:
            menuThemeType = TP_MT_SHOWLOOPFOOT;
            break;

        case TP_MT_SHOWTITLE:
            menuThemeType = TP_MT_SHOWLOOPTITLE;
            break;

        case TP_MT_SHOWTITLEFOOT:
            menuThemeType = TP_MT_ALL;
            break;

        case TP_MT_NONE:
        case TP_MT_SHOWONEITEM:
            menuThemeType = TP_MT_SHOWLOOP;
            break;

        default:
            return;
        }
        break;
    case 28 : //MenuLoop Display OFF
        switch (menuThemeType)
        {
        case TP_MT_ALL:
            menuThemeType = TP_MT_SHOWTITLEFOOT;
            break;

        case TP_MT_SHOWLOOP:
            menuThemeType = TP_MT_NONE;
            break;

        case TP_MT_SHOWLOOPTITLE:
            menuThemeType = TP_MT_SHOWTITLE;
            break;

        case TP_MT_SHOWLOOPFOOT:
            menuThemeType = TP_MT_SHOWFOOT;
            break;

        default:
            return;
        }
        break;
    case 29 : //Show Title
        switch (menuThemeType)
        {
        case TP_MT_SHOWFOOT:
            menuThemeType = TP_MT_SHOWTITLEFOOT;
            break;

        case TP_MT_SHOWLOOP:
            menuThemeType = TP_MT_SHOWLOOPTITLE;
            break;

        case TP_MT_SHOWLOOPFOOT:
            menuThemeType = TP_MT_ALL;
            break;

        case TP_MT_NONE:
        case TP_MT_SHOWONEITEM:
            menuThemeType = TP_MT_SHOWTITLE;
            break;

        default:
            return;
        }
        break;
    case 30 : //Not Show Title
        switch (menuThemeType)
        {
        case TP_MT_ALL:
            menuThemeType = TP_MT_SHOWLOOPFOOT;
            break;

        case TP_MT_SHOWTITLE:
            menuThemeType = TP_MT_NONE;
            break;

        case TP_MT_SHOWLOOPTITLE:
            menuThemeType = TP_MT_SHOWLOOP;
            break;

        case TP_MT_SHOWTITLEFOOT:
            menuThemeType = TP_MT_SHOWFOOT;
            break;

        default:
            return;
        }
        break;
    case 31 : //Menu Show Foot ON
        switch (menuThemeType)
        {
        case TP_MT_SHOWLOOP:
            menuThemeType = TP_MT_SHOWLOOPFOOT;
            break;

        case TP_MT_SHOWTITLE:
            menuThemeType = TP_MT_SHOWTITLEFOOT;
            break;

        case TP_MT_SHOWLOOPTITLE:
            menuThemeType = TP_MT_ALL;
            break;

        case TP_MT_NONE:
        case TP_MT_SHOWONEITEM:
            menuThemeType = TP_MT_SHOWFOOT;
            break;

        default:
            return;
        }
        break;
    case 32 : //Menu Show Foot OFF
        switch (menuThemeType)
        {
        case TP_MT_ALL:
            menuThemeType = TP_MT_SHOWLOOPTITLE;
            break;

        case TP_MT_SHOWFOOT:
            menuThemeType = TP_MT_NONE;
            break;

        case TP_MT_SHOWLOOPFOOT:
            menuThemeType = TP_MT_SHOWLOOP;
            break;

        case TP_MT_SHOWTITLEFOOT:
            menuThemeType = TP_MT_SHOWTITLE;
            break;

        default:
            return;
        }
        break;
    default:
        return;
    }
    TP_SetMenuTheme(menuThemeType);
}
static void SetupSuccess()
{
    TP_PopupMessageTimed((uint8*)"Setup success!",1500);
}
/**
* Demo entrence£¬Choose Demo to run accroding to ItemID
* @author He Wendong
* @version 1.0
* @param ItemKey ID of Item
* @return None
* @since 2011 Dec 31
*/
void ONMenuItemClick(uint8 ItemKey)
{
	char information[INF_LENGHT];
	int32 cTemp;
	SALE_INFO sale_info;
	char date[20];
	char pin[21];
	
    switch (ItemKey)
    {
    case 1: //Keypad test
		CT_Login();
        break;
	case 2:
		CT_Balance();
		break;
    case 3:///Constract test
        CT_Deposit();
        break;
	case 4:
		CT_Recharge();
		break;
	case 5:
		CT_Payment();
		break;
	case 6:
		if( test_session() )
			CT_PrintLastTransaction();
		else{
			CT_Notify("Debe iniciar sesion para continuar.",0);
		}			
		break;
	case 7:
		cTemp = CT_CreateLogFile();

		if( cTemp < 0x00 ){
			sprintf(information, "Error: %s", CT_MessageError(TP_GetLastError()) );
			CT_Notify(information, 0);
		}else{
			sprintf(information, "Arhivo log creado! %.2fKB", (float)cTemp/1000);
			CT_Notify(information, 0);
		}

		break;
	case 8:
		cTemp = CT_ReadLogFile();

		if( cTemp < 0x00 ){
			sprintf(information, "Error: %s", CT_MessageError(cTemp) );
			CT_Notify(information, 0);
		}else{
			sprintf(information, "Txn: %d\nAmount: %.02f\nS/C: %d", total_txn, total_amount, cTemp);
			CT_Notify(information, 0);
		}
		break;
	case 9:
		cTemp = CT_DeleteLogFile();

		if( cTemp < 0x00 ){
			sprintf(information, "Error: %s", CT_MessageError(cTemp) );
			CT_Notify(information, 0);
		}else{
			CT_DeleteLastTransFile();
			serials_to_confirm[0] = '\0';
			total_txn = 0;
			total_txn_to_confirm = 0;
			total_amount = 0;
			CT_Notify("Archivo Eliminado", 0);
		}
		break;

	case 10:
		CT_ClosePOS();
		break;

	case 11:
		CT_Logout();
		break;

	case 12:
		CT_InputInteger(pin, "Pin");
		CT_PrintCode39(pin);
		break;

    case 18://Input Number
        ShowNumberInput();
        break;
    case 19://Chinese input method
        ShowHzInput();
        break;
    #if 0
    case 201:
        TP_EncryptSMSSendTest(0,0,0);
        break;
    case 202:
        TP_EncryptSMSReceiveTest();
        break;
    #endif

    case 24 : //Backlight ON
        TP_kbLight(2);
        TP_ScrBackLight(2);
        break;
    case 25 : //Backlight ON for 1 minute
        TP_ScrBackLight(1);
        TP_kbLight(1);
        break;
    case 26 : //Backlight OFF
        TP_ScrBackLight(0);
        TP_kbLight(0);
        break;
    case 22 : //Popup Dialog
        TP_ShowPopupDialog(KEY_DEMO_CONFIRM,KEY_DEMO_CANCEL);
        break;
    case 23 : //Time Popup Dialog
        TP_ShowTimePopupDialog(5000);
        break;

    case 27 : //MenuLoop Display ON

    case 28 : //MenuLoop Display OFF

    case 29 : //Show Title

    case 30 : //Not Show Title

    case 31 : //Menu Show Foot ON

    case 32 : //Menu Show Foot OFF
        SetMenuTheme(ItemKey);
        SetupSuccess();
        TP_MenuGetParent();
        break;

   case 34 : ///Spanish
        TP_SetCurrentCharSet(CHARSET_WEST);
        TP_ConfigInputModeExtern(INPUTMODE_LOWCASE,  &SpanishLowCaseMap);
        TP_ConfigInputModeExtern(INPUTMODE_UPERCASE, &SpanishUpCaseMap);
        TP_SetExternPunctuation(INPUTMODE_NUMBER,  SpanishPunc, sizeof(SpanishPunc)/sizeof(TP_PUNC_INPUT), 0);
        TP_SetExternPunctuation(INPUTMODE_LOWCASE, SpanishPunc, sizeof(SpanishPunc)/sizeof(TP_PUNC_INPUT), 0);
        TP_SetExternPunctuation(INPUTMODE_UPERCASE, SpanishPunc, sizeof(SpanishPunc)/sizeof(TP_PUNC_INPUT), 0);
        SetupSuccess();
        TP_MenuGetParent();
        break;

    case 35 : ///Denmark
        //TP_ScrSelectFont(&GoblaFont[CHARSET_WEST], &GoblaFont[CHARSET_DIY]);
        TP_SetCurrentCharSet(CHARSET_WEST);
        TP_ConfigInputModeExtern(INPUTMODE_LOWCASE,  &DenmarkKeyMap);
        TP_ConfigInputModeExtern(INPUTMODE_UPERCASE, &DenmarkKeyMap);
        TP_SetExternPunctuation(INPUTMODE_NUMBER,   0, 0, 0);
        TP_SetExternPunctuation(INPUTMODE_LOWCASE,  0, 0, 0);
        TP_SetExternPunctuation(INPUTMODE_UPERCASE, 0, 0, 0);
        SetupSuccess();
        TP_MenuGetParent();
        break;

    case 36 : ///Chinese
        //TP_ScrSelectFont(&GoblaFont[CHARSET_WEST], &GoblaFont[CHARSET_GBK]);
        TP_SetCurrentCharSet(CHARSET_GBK);
        TP_ConfigInputModeExtern(INPUTMODE_LOWCASE,0);
        TP_ConfigInputModeExtern(INPUTMODE_UPERCASE,0);
        TP_SetExternPunctuation(INPUTMODE_NUMBER, 0, 0, 0);
        TP_SetExternPunctuation(INPUTMODE_LOWCASE, 0, 0, 0);
        TP_SetExternPunctuation(INPUTMODE_UPERCASE, 0, 0, 0);
        SetupSuccess();
        TP_MenuGetParent();
        break;
    case 38:///Set Date
        if (TP_SetDate() == TRUE)
        {
            SetupSuccess();
        }
        break;
    case 40: //List Method
        TP_ListBoxShow(TL_LIST);
        break;
    case 41 : //Radio Box Method
        TP_ListBoxShow(TL_RADIOBOX);
        break;
    case 42 : ///Check Box Method
        TP_ListBoxShow(TL_CHECKBOX);
        break;
    case 44 ://LCD Mirror
        TP_LCDMirror(0);
        break;
    case 45 ://LCD Mirror
        TP_LCDMirror(1);
        break;
    case 46 ://Auto Dial Test
        TP_AutoDialTest();
        break;
    case 47 ://RingTest
        TP_RingTest();
        break;
    case 48 : //IconList Test
        TP_ListBoxShow(TL_ICONLIST);
        break;

    case 49: //USSD Test
        TP_ShowUSSDDilog();
        break;
    case 50 :  ///IMSI Test
        TP_ShowIMSIDialDilog();
        break;

    case 51: ///IMEI  Test
        TP_ShowIMEIDialDilog();
        break;
    case 53:
        ShowPinOnDialog();
        break;
    case 54 :
        ShowPinOffDialog();
        break;
    case 55 :
        ShowPinChangeDialog();
        break;

    case 103:
    case 104:
    case 106:
    case 107:
    case 109:
    case 110:
    case 200:
    case 201:
        TP_FileTest(ItemKey);
        break;

    case 116:
    case 117:
        TP_SerialTest(ItemKey);
        break;

    case 133:
    case 134:
    case 137:
    case 138:
    case 141:
    case 142:
    case 143:
    case 144:
    case 145:
    case 154:
    case 155:
    case 156:
    case 157:
    case 158:
    case 203:
    case 212:
        TP_PrintTest(ItemKey);
        break;

    case 213:
        OperatorSelectTest();
        break;

    case 166:
    case 167:
    case 168:
    case 173:
    case 174:
    case 210:
        if (TP_TcpipTest(ItemKey) == TRUE)
        {
            SetupSuccess();
        }
        break;
    case 182://keypad volume
        if (TP_KeyPadVolumeSetTest() == TRUE)
        {
            SetupSuccess();
        }
        break;
    case 183://Beep test
        TP_BeepTest();
        break;
    case 184://password input
        ShowPasswordInput();
        break;

#ifdef INCLUDE_PHONE_SETTING
    case 187:
        TP_PhoneSetting();
        break;
#endif

    case 186:
        TP_UpgradeTest();
        break;
#ifdef INCLUDE_MESSAGE_BOX
    #include "TP_MSG_MenuItemClick.h"
#endif

#ifdef INCLUDE_CALL_RECORD
    #include "TP_CallRecord_MenuItemClick.h"
#endif
    /*
    case 189:
        TP_SysSleepTest();
        break;
    case 190:
        TP_SysWakeUpTest();
        break;
    case 191:
        TP_SysSettingTest();
        break;
    */
    case 192://French
        //TP_ScrSelectFont(&GoblaFont[CHARSET_FRENCH], &GoblaFont[CHARSET_DIY]);
        TP_SetCurrentCharSet(CHARSET_LATIN0);
        TP_ConfigInputModeExtern(INPUTMODE_LOWCASE,  &FrenchLowCaseMap);
        TP_ConfigInputModeExtern(INPUTMODE_UPERCASE, &FrenchUpCaseMap);
        TP_SetExternPunctuation(INPUTMODE_NUMBER, FrenchPunc, sizeof(FrenchPunc)/sizeof(TP_PUNC_INPUT), 0);
        TP_SetExternPunctuation(INPUTMODE_LOWCASE, FrenchPunc, sizeof(FrenchPunc)/sizeof(TP_PUNC_INPUT), 0);
        TP_SetExternPunctuation(INPUTMODE_UPERCASE, FrenchPunc, sizeof(FrenchPunc)/sizeof(TP_PUNC_INPUT), 0);
        SetupSuccess();
        TP_MenuGetParent();
        break;
#ifdef INCLUDE_MULTI_LANGUAGE
    case 193://Language demo
        TP_LanguageChooseDemo();
        break;
#endif


#if defined(CARD_READER_MODULE_ON)
    case 205:
        ICReaderTest();
        break;
    case 206:
        MStripReaderTest();
        break;
#endif

    case 207://STK¿¨o¦oÃ
        TB_STK_Enter();
        break;

    case 208:
        NetworkStatusTest();
        break;

    case 209:
        CLIRTest();
        break;

    case 211:
        TP_SetCurrentCharSet(CHARSET_ASCII);
        TP_ConfigInputModeExtern(INPUTMODE_LOWCASE,  PNULL);
        TP_ConfigInputModeExtern(INPUTMODE_UPERCASE, PNULL);
        TP_SetExternPunctuation(INPUTMODE_NUMBER,   PNULL,  0, 0);
        TP_SetExternPunctuation(INPUTMODE_LOWCASE,  PNULL,  0, 0);
        TP_SetExternPunctuation(INPUTMODE_UPERCASE, PNULL,  0, 0);
        SetupSuccess();
        TP_MenuGetParent();
        break;

    case 215:
        tp_demo_flashparam_read();
        break;

    case 216:
        tp_demo_flashparam_print();
        break;

    case 218:
        TP_Arabic_AsciiFontDisplay();
        break;

    case 219:
        TP_ArabicPrint();
        break;

    case 220:
        TP_ArabicInputTest();
        break;

    case 221:
        //TP_DisplayTest();
        break;

#if defined(CARD_READER_MODULE_ON)
#if defined(WITH_NFC_CARD_READER)
    case 222:
        TP_MifareOneTest();
        break;
#endif
#endif

    default:
        if ((ItemKey >= 123) && (ItemKey <= 131))
        {
            TP_SerialTest(ItemKey);
            SetupSuccess();
            TP_MenuGetParent();
        }
        else if ((ItemKey >= 146) && (ItemKey <= 153))
        {
            TP_PrintTest(ItemKey);
            SetupSuccess();
            TP_MenuGetParent();
        }
        else if ((ItemKey >= 163) && (ItemKey <= 180))
        {
            TP_TcpipTest(ItemKey);
        }
        break;
    }

    TP_ScrFontSet(CFONT);
}


/**
* Perform of Menu footer
* @author He Wendong
* @version 1.0
* @param ItemKey ID of Item
* @param Value Storage Place of Display Characters of Menu footer
* @return None
* @since 2011 Dec 31
*/
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
	TP_AddMenuItem(0,2,(uint8*)"Consulta de saldo",MenuItemPic);
    //TP_AddMenuItem(0,2,(uint8*)"Backlight Demo",MenuItemPic);
    /*TP_AddMenuItem(2,24,(uint8*)"Backlight On",MenuItemPic);
    TP_AddMenuItem(2,25,(uint8*)"Backlight On 1min",MenuItemPic);
    TP_AddMenuItem(2,26,(uint8*)"Backlight Off",MenuItemPic);
	*/
    TP_AddMenuItem(0,3,(uint8*)"Agregar Deposito",MenuItemPic);
	TP_AddMenuItem(0,4,(uint8*)"Recarga electronica",MenuItemPic);
	TP_AddMenuItem(0,5,(uint8*)"Pago de factura",MenuItemPic);
	TP_AddMenuItem(0,6,(uint8*)"Ultima transaccion",MenuItemPic);
	//TP_AddMenuItem(0,7,(uint8*)"Crear Log File",MenuItemPic);
	TP_AddMenuItem(0,8,(uint8*)"Leer Log File",MenuItemPic);
	TP_AddMenuItem(0,9,(uint8*)"Eliminar Log File",MenuItemPic);
	TP_AddMenuItem(0,10,(uint8*)"Cierre de POS",MenuItemPic);
	TP_AddMenuItem(0,11,(uint8*)"Cerrar Sesion",MenuItemPic);
	TP_AddMenuItem(0,12,(uint8*)"Imprimir PIN",MenuItemPic);
    #if 0
    TP_AddMenuItem(7,201,"Encrypt SMS Send", MenuItemPic);
    TP_AddMenuItem(7,202,"Encrypt SMS Receive", MenuItemPic);
    #endif

   // TP_AddMenuItem(0,8,"Calling Demo",0);
    //TP_AddMenuItem(8,13,"Call Out",0);
    //TP_AddMenuItem(8,14,"Incoming Call",0);
    //TP_AddMenuItem(0,9,(uint8*)"Menu Attribute",MenuItemPic);


   // TP_AddMenuItem(9,15,(uint8*)"Loop Display",MenuItemPic);
    //TP_AddMenuItem(15,27,(uint8*)"Enable",MenuItemPic);
    //TP_AddMenuItem(15,28,(uint8*)"Disable",MenuItemPic);


    //TP_AddMenuItem(9,16,(uint8*)"Show Title",MenuItemPic);
    //TP_AddMenuItem(16,29,(uint8*)"Enable",MenuItemPic);
    //TP_AddMenuItem(16,30,(uint8*)"Disable",MenuItemPic);


    //TP_AddMenuItem(9,17,(uint8*)"Show Foot",MenuItemPic);
    //TP_AddMenuItem(17,31,(uint8*)"Enable",MenuItemPic);
    //TP_AddMenuItem(17,32,(uint8*)"Disable",MenuItemPic);

    //TP_AddMenuItem(9,33,(uint8*)"Loop display a long menu item",MenuItemPic);


    //TP_AddMenuItem(0,10,(uint8*)"Input Method Demo",MenuItemPic);
    //TP_AddMenuItem(10,18,(uint8*)"Number Input",MenuItemPic);
    //TP_AddMenuItem(10,19,(uint8*)"Chinese Input",MenuItemPic);
    //TP_AddMenuItem(10,20,(uint8*)"Charset",MenuItemPic);
   /* if (TP_SetCurrentCharSet(CHARSET_WEST) == TRUE)
    {
        TP_AddMenuItem(20,34,(uint8*)"Spanish",MenuItemPic);
        TP_AddMenuItem(20,35,(uint8*)"Denmark",MenuItemPic);
        TP_SetCurrentCharSet(curCharSet.name);
    }

    if (TP_SetCurrentCharSet(CHARSET_GBK) == TRUE)
    {
        TP_AddMenuItem(20,36,(uint8*)"Chinese",MenuItemPic);
        TP_SetCurrentCharSet(curCharSet.name);
    }*/

    //TP_AddMenuItem(0,21,(uint8*)"Window Demo",MenuItemPic);
    //TP_AddMenuItem(21,22,(uint8*)"Popup",MenuItemPic);
    //TP_AddMenuItem(21,23,(uint8*)"Popup with timing",MenuItemPic);

    //TP_AddMenuItem(0,37,(uint8*)"DateTime",MenuItemPic);
    //TP_AddMenuItem(37,38,(uint8*)"SetDate",MenuItemPic);

    //TP_AddMenuItem(0,39,(uint8*)"ListBox Demo",MenuItemPic);
    //TP_AddMenuItem(39,40,(uint8*)"List",MenuItemPic);
    //TP_AddMenuItem(39,41,(uint8*)"RadioBox",MenuItemPic);
    //TP_AddMenuItem(39,42,(uint8*)"CheckBox",MenuItemPic);

    //TP_AddMenuItem(0,43,(uint8*)"LCD Mirror",MenuItemPic);
    //TP_AddMenuItem(43,44,(uint8*)"LCD Normal",MenuItemPic);
    //TP_AddMenuItem(43,45,(uint8*)"LCD Mirror",MenuItemPic);
    //TP_AddMenuItem(0,46,(uint8*)"Phone Auto Test",MenuItemPic);
    //TP_AddMenuItem(0,47,(uint8*)"Ring Test",MenuItemPic);
    //TP_AddMenuItem(0,48,(uint8*)"IconList Test",MenuItemPic);
    //TP_AddMenuItem(0,49,(uint8*)"USSD Test",MenuItemPic);
    //TP_AddMenuItem(0,50,(uint8*)"IMSI Test",MenuItemPic);
    //TP_AddMenuItem(0,51,(uint8*)"IMEI Test",MenuItemPic);

    //TP_AddMenuItem(0,52,(uint8*)"SimCard",MenuItemPic);
    //TP_AddMenuItem(52,53,(uint8*)"Pin On",MenuItemPic);
    //TP_AddMenuItem(52,54,(uint8*)"Pin Off",MenuItemPic);
    //TP_AddMenuItem(52,55,(uint8*)"Pin Change",MenuItemPic);

    //TP_AddMenuItem(0,   100, (uint8*)"Storage Demo",   MenuItemPic);
    //TP_AddMenuItem(100, 101, (uint8*)"File System",    MenuItemPic);
    //TP_AddMenuItem(100, 102, (uint8*)"Parameter File", MenuItemPic);
    //TP_AddMenuItem(101, 103, (uint8*)"FS Free Size",   MenuItemPic);
    //TP_AddMenuItem(101, 104, (uint8*)"Format FS",      MenuItemPic);
    //TP_AddMenuItem(101, 105, (uint8*)"Read and Write", MenuItemPic);
    //TP_AddMenuItem(102, 106, (uint8*)"Read Items",     MenuItemPic);
    //TP_AddMenuItem(102, 107, (uint8*)"Write Items",    MenuItemPic);
    //TP_AddMenuItem(105, 109, (uint8*)"Create File",    MenuItemPic);
    //TP_AddMenuItem(105, 110, (uint8*)"Write File",     MenuItemPic);

    //TP_AddMenuItem(0,   115, (uint8*)"Serial Port Demo",     MenuItemPic);
    //TP_AddMenuItem(115, 116, (uint8*)"Sending Demo", MenuItemPic);
    //TP_AddMenuItem(115, 117, (uint8*)"Receiving Demo", MenuItemPic);
    //TP_AddMenuItem(115, 118, (uint8*)"Port Attribute", MenuItemPic);
    //TP_AddMenuItem(118, 119, (uint8*)"Port Rate",       MenuItemPic);
    //TP_AddMenuItem(118, 120, (uint8*)"Data Bits",       MenuItemPic);
    //TP_AddMenuItem(118, 121, (uint8*)"Parity",     MenuItemPic);
    //TP_AddMenuItem(118, 122, (uint8*)"Stop Bits",       MenuItemPic);
    //TP_AddMenuItem(119, 123, (uint8*)"9600",         MenuItemPic);
    //TP_AddMenuItem(119, 124, (uint8*)"115200",       MenuItemPic);
    //TP_AddMenuItem(120, 125, (uint8*)"7",            MenuItemPic);
    //TP_AddMenuItem(120, 126, (uint8*)"8",            MenuItemPic);
    //TP_AddMenuItem(121, 127, (uint8*)"None",           MenuItemPic);
    //TP_AddMenuItem(121, 128, (uint8*)"Odd",       MenuItemPic);
    //TP_AddMenuItem(121, 129, (uint8*)"Even",       MenuItemPic);
    //TP_AddMenuItem(122, 130, (uint8*)"1",            MenuItemPic);
    //TP_AddMenuItem(122, 131, (uint8*)"2",            MenuItemPic);

    //TP_AddMenuItem(0,   132, (uint8*)"Print Demo",      MenuItemPic);
    //TP_AddMenuItem(132, 133, (uint8*)"Printer Version", MenuItemPic);
    //TP_AddMenuItem(132, 134, (uint8*)"Left Indent",     MenuItemPic);
    //TP_AddMenuItem(132, 135, (uint8*)"Space",           MenuItemPic);
    //TP_AddMenuItem(132, 138, (uint8*)"Gray Level",      MenuItemPic);
    //TP_AddMenuItem(132, 141, (uint8*)"Add Content",     MenuItemPic);
    //TP_AddMenuItem(132, 142, (uint8*)"Clear Content",   MenuItemPic);
    //TP_AddMenuItem(132, 143, (uint8*)"Print",           MenuItemPic);
    //TP_AddMenuItem(135, 144, (uint8*)"Char Space",      MenuItemPic);
    //TP_AddMenuItem(135, 145, (uint8*)"Line Space",      MenuItemPic);
    //TP_AddMenuItem(143, 154, (uint8*)"Print Logo",      MenuItemPic);
    //TP_AddMenuItem(143, 155, (uint8*)"Walk Paper",      MenuItemPic);
    //TP_AddMenuItem(143, 156, (uint8*)"Print Content",   MenuItemPic);
    //TP_AddMenuItem(143, 157, (uint8*)"Session Mode",    MenuItemPic);
    //TP_AddMenuItem(143, 158, (uint8*)"Extreme Test",   MenuItemPic);

    //TP_AddMenuItem(0,   159, (uint8*)"Network Demo",    MenuItemPic);
    //TP_AddMenuItem(159, 160, (uint8*)"GPRS",     MenuItemPic);
    //TP_AddMenuItem(159, 161, (uint8*)"TCP",      MenuItemPic);
    //TP_AddMenuItem(159, 162, (uint8*)"UDP",      MenuItemPic);
    //TP_AddMenuItem(159, 163, (uint8*)"DNS Resovle",      MenuItemPic);
    ////TP_AddMenuItem(159, 164, "Ping",         MenuItemPic);
    //TP_AddMenuItem(159, 165, (uint8*)"Pressure Test",      MenuItemPic);
    //TP_AddMenuItem(160, 166, (uint8*)"APN",          MenuItemPic);
    //TP_AddMenuItem(160, 167, (uint8*)"User Name",       MenuItemPic);
    //TP_AddMenuItem(160, 168, (uint8*)"Password",         MenuItemPic);
    //TP_AddMenuItem(160, 169, (uint8*)"Network Status", MenuItemPic);
    //TP_AddMenuItem(160, 170, (uint8*)"Signal Level",     MenuItemPic);
    //TP_AddMenuItem(160, 171, (uint8*)"Dial",     MenuItemPic);
    //TP_AddMenuItem(160, 172, (uint8*)"Disconnect",     MenuItemPic);
    //TP_AddMenuItem(161, 173, (uint8*)"Server IP",     MenuItemPic);
    //TP_AddMenuItem(161, 174, (uint8*)"Server Port",       MenuItemPic);
    //TP_AddMenuItem(161, 175, (uint8*)"Socket Connect",   MenuItemPic);
    //TP_AddMenuItem(161, 176, (uint8*)"Close Socket",   MenuItemPic);
    //TP_AddMenuItem(161, 177, (uint8*)"Send Data",     MenuItemPic);
    //TP_AddMenuItem(161, 178, (uint8*)"Receive Data",     MenuItemPic);;
    //TP_AddMenuItem(162, 179, (uint8*)"Send Data",     MenuItemPic);
    //TP_AddMenuItem(162, 180, (uint8*)"Receive Data",     MenuItemPic);

    //TP_AddMenuItem(0, 182, (uint8*)"Keypad Volume Set", MenuItemPic);
#if defined(WITH_175X_ARM) && defined(WITH_SOUNDER_DRIVER)
    TP_AddMenuItem(0, 183, (uint8*)"Sound Beep Test", MenuItemPic);
#else
    //TP_AddMenuItem(0, 183, (uint8*)"Beep Test", MenuItemPic);
#endif
   // TP_AddMenuItem(10,184, (uint8*)"Password Input", MenuItemPic);

    //TP_AddMenuItem(0, 186, (uint8*)"SMS Upgrade Test", MenuItemPic);
#ifdef INCLUDE_PHONE_SETTING
   //TP_AddMenuItem(0,187,(uint8*)TEXT_SYSSETTING,MenuItemPic),
#endif

#ifdef INCLUDE_MESSAGE_BOX
  //  #include "TP_MSG_AddMenuItem.h"
#endif

#ifdef INCLUDE_CALL_RECORD
  //  #include "TP_CallRecord_AddMenuItem.h"
#endif

    /*
    TP_AddMenuItem(0, 188,   "Sleep and Wake up",   MenuItemPic);
    TP_AddMenuItem(188, 189, "Sleep",               MenuItemPic);
    TP_AddMenuItem(188, 190, "Wake up",             MenuItemPic);
    TP_AddMenuItem(188, 191, "Sleep Setting",       MenuItemPic);
    */
    if (TP_SetCurrentCharSet(CHARSET_LATIN0) == TRUE)
    {
        /*TP_AddMenuItem(20,192,(uint8*)"French",MenuItemPic);
        TP_SetCurrentCharSet(curCharSet.name);*/
    }

#ifdef INCLUDE_MULTI_LANGUAGE
    /*TP_AddMenuItem(0,193, (uint8*)"Language choose", MenuItemPic);
    TP_AddMenuItem(0,194, (uint8*)"Language Demo", MenuItemPic);

    TP_AddMenuItem(194, 195,(uint8*)TPRes_GetStringByID(STRINGID_CONFIRM), MenuItemPic);
    TP_AddMenuItem(194, 196,(uint8*)TPRes_GetStringByID(STRINGID_CANCEL), MenuItemPic);
    TP_AddMenuItem(194, 197,(uint8*)TPRes_GetStringByID(STRINGID_EXIT), MenuItemPic);
    TP_AddMenuItem(194, 198,(uint8*)TPRes_GetStringByID(STRINGID_BACK), MenuItemPic);
    TP_AddMenuItem(194, 199,(uint8*)TPRes_GetStringByID(STRINGID_MENU), MenuItemPic);*/
#endif

   /* TP_AddMenuItem(105, 200, (uint8*)"Read File",   MenuItemPic);
    TP_AddMenuItem(105, 201, (uint8*)"Delete File", MenuItemPic);

    TP_AddMenuItem(143, 202, (uint8*)"Barcode",         MenuItemPic);
    TP_AddMenuItem(202, 203, (uint8*)"Interleaved 2 of 5", MenuItemPic);
    TP_AddMenuItem(202, 212, (uint8*)"Code 39", MenuItemPic);*/

#if defined(CARD_READER_MODULE_ON)
    TP_AddMenuItem(0, 204, (uint8*)"Card Reader Test", MenuItemPic);
#if defined(WITH_IC_CARD_READER)
    TP_AddMenuItem(204, 205, (uint8*)"IC Card Reader", MenuItemPic);
#endif

#if defined(WITH_MAGNETICSTRIP_CARD_READER)
    TP_AddMenuItem(204, 206, (uint8*)"Magnetic Strip Card Reader", MenuItemPic);
#endif

#if defined(WITH_NFC_CARD_READER)
    TP_AddMenuItem(0, 222, (uint8*)"Mifare One Test", MenuItemPic);
#endif
#endif

    memset(&stkAppName, 0x00, sizeof(stkAppName));
    TP_STKGetMenuTitle(STKACTION_SHOWMAINMENU, &stkAppName);
    if (strlen(stkAppName.textChars) > 0)
    {
        //TP_AddMenuItem(0, 207, stkAppName.textChars, MenuItemPic);
    }
    else
    {
        //TP_AddMenuItem(0, 207, (uint8*)"STK Application", MenuItemPic);
    }


    //TP_AddMenuItem(0, 208, (uint8*)"Network status", MenuItemPic);
    //TP_AddMenuItem(0, 209, (uint8*)"CLIR test", MenuItemPic);
    //TP_AddMenuItem(160, 210, (uint8*)"Local IP Address", MenuItemPic);

    //TP_AddMenuItem(20, 211, (uint8*)"English", MenuItemPic);

    //TP_AddMenuItem(0, 213, (uint8*)"Operator Select", MenuItemPic);

    //TP_AddMenuItem(0, 214, (uint8*)"Flash Param", MenuItemPic);
    //TP_AddMenuItem(214, 215, (uint8*)"Read Item", MenuItemPic);
    //TP_AddMenuItem(214, 216, (uint8*)"Output To Serial", MenuItemPic);

    TP_GetCurrentCharSet(&curCharSet);
    if (TP_SetCurrentCharSet(CHARSET_ARABIC_CP1256) == TRUE)
    {
        /*TP_AddMenuItem(0, 217, (uint8*)"Arabic Test", MenuItemPic);
        TP_AddMenuItem(217, 218, (uint8*)"ASCII Char Show", MenuItemPic);
        TP_AddMenuItem(217, 219, (uint8*)"Print", MenuItemPic);
        TP_AddMenuItem(217, 220, (uint8*)"Arabic Input", MenuItemPic);*/
    }
    TP_SetCurrentCharSet(curCharSet.name);

    //TP_AddMenuItem(0, 221, (uint8*)"Display Test", MenuItemPic);

    TP_SortMenuItem();

}

/**
 * Function Description of TP_MenuShow()£º Show Menu
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note
 */
void TP_MenuShow()
{
    TP_LoadMenu();
    TP_ShowMenu(0,1);
}

#ifdef INCLUDE_MULTI_LANGUAGE
static uint8 languagechoose = 0;
void TP_LanguageChooseDemo()
{
    uint8 ChooseValue = 0;
    uint8 *caption[] =
    {
        (uint8*)"English",
        (uint8*)"Chinese"
    };
    TP_DisplayInfo displayInfo;
    displayInfo.align = ALIGN_MIDDLE;
    displayInfo.FONT = CFONT;
    displayInfo.LF_KeyValue = KEY_DEMO_CONFIRM;
    displayInfo.LKeyValue = TP_KEY_NONE;
    displayInfo.RF_KeyValue = KEY_DEMO_CANCEL;
    displayInfo.RKeyValue = TP_KEY_NONE;
    displayInfo.strLButtonTitle = (uint8*)TPRes_GetStringByID(STRINGID_CONFIRM);
    displayInfo.strRButtonTitle = (uint8*)TPRes_GetStringByID(STRINGID_CANCEL);
    displayInfo.strTitle = (uint8*)TPRes_GetStringByID(STRINGID_LANGUAGE);
    if( TP_CreateListBoxInterface(TL_RADIOBOX,
                                  &displayInfo,
                                  caption,
                                  sizeof(caption)/sizeof(uint8*),
                                  TP_GetAppLanguage()==LANGUAGE_CHINESE?2:1,
                                  &ChooseValue,
                                  0) )
    {
        if(ChooseValue == 2)
        {
            TP_SetAppLanguage(LANGUAGE_CHINESE);
        }
        else
        {
            TP_SetAppLanguage(LANGUAGE_ENGLISH);
        }
        TP_LoadMenu();
        TP_SetJumpMenuID(0,194);
    }
}
#endif