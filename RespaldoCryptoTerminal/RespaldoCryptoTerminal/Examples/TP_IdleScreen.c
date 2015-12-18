#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeypad.h>
#include <TPFonts.h>
#include <TPLCD.h>
#include <TPSMS.h>
#include <TP_Singleime.h>
#include <TPSerial.h>
#include <TPPhone.h>
#include <TPMobileCard.h>

#include "TP_DemoDefine.h"
#include "TP_DemoMenu.h"
#include "TP_MainMenu.h"
#include "TP_IdleScreen.h"
#include "TP_ListBox.h"
#include "TP_DisplayAPI.h"
#include "Scolopendra\CT_GlobalData.h"

extern USER_DATA g_userData;

#ifdef INCLUDE_PARAM_MANAGER
#include "TP_ParamsManager.h"
#endif
#ifdef INCLUDE_PHONE_SETTING
#include "TP_PhoneSetting.h"
#include "TP_CallRecord.h"
#endif

#include "TP_Secretcode.h"

#include "TP_SmsStore.h"
#include "TP_MsgMenu.h"

#include "TP_Phone.h"



TP_DisplayInfo displayInfo=
                            {
                                0,
                                ALIGN_LEFT,
                                (uint8*)"Confirm",
                                (uint8*)"Cancel",
                                KEY_DEMO_CONFIRM,
                                KEY_DEMO_CANCEL,
                                TP_KEY_OK,
                                TP_KEY_NONE,
                                CFONT
                            };
TP_InputInfo inputInfo =
                            {
                                INPUTMODE_UPERCASE,
                                0xF4,
                                1,
                                3,
                                0,
                                TRUE,
                                TRUE
                            };


//ST_FONT  GoblaFont[32];
static uint8 currlevel= 0xFF;

/**
 * Function Description of TP_HexToStr():Change HEX to be String
 * @param Src HEX
 * @param Size Length of HEX
 * @param des output string after converted
 * @return output string after converted
 * @author  Zuo Jun
 * @date 20110712
 * @note Show both Icon ant Time
 */
uint16 TP_HexToStr(uint8* Src,uint16 Size,uint8* des)
{
    uint16 Index,Pos;
    Pos =0;
    for (Index=0;Index < Size;Index++)
    {
            if (((Src[Index] >> 4)>=0) && ((Src[Index] >> 4)<=9))
            {
                des[Pos++] = (Src[Index] >> 4) + 0x30;
            }
            else
            {
                des[Pos++] = (Src[Index] >> 4) + 0x37;
            }

            if (((Src[Index] & 0x0F )>=0) && ((Src[Index] & 0x0F)<=9))
            {
                des[Pos++] = (Src[Index] & 0x0F) + 0x30;
            }
            else
            {
                des[Pos++] = (Src[Index] & 0x0F) + 0x37;
            }

    }
    return Pos;
}

uint16 TP_StrToHex(uint8* Src,uint16 Size,uint8* des)
{
    uint16 Index,Pos;
    uint8 Byte;
    if (Size %2 >0)
    {
        return 0;
    }
    Pos =0;
    for (Index=0;Index < Size;Index++)
    {
        Byte =0;
        if ((Src[Index] >= 0x30) && (Src[Index] <= 0x39))
        {
            Byte= Src[Index] - 0x30;

        }
        else if((Src[Index] >= 0x41) && (Src[Index] <= 0x46))
        {
            Byte= Src[Index] - 0x37;
        }
        else if((Src[Index] >= 0x61) && (Src[Index] <= 0x66))
        {
            Byte= Src[Index] - 0x50;
        }

        Byte <<= 4;

        Index ++;

        if ((Src[Index] >= 0x30) && (Src[Index] <= 0x39))
        {
            Byte |= (Src[Index] - 0x30);

        }
        else if((Src[Index] >= 0x41) && (Src[Index] <= 0x46))
        {
            Byte |= (Src[Index] - 0x37);
        }
        else if((Src[Index] >= 0x61) && (Src[Index] <= 0x66))
        {
            Byte |= (Src[Index] - 0x50);
        }

        *des = Byte;
        des++;


    }
    return Size/2;
}

static void TP_SetDefaultParam()
{
    TP_SetVolume(KEYPAD_VOLUMN, VOLUME_0);
    TP_SetVolume(RING_VOLUMN, VOLUME_3);
    TP_SetVolume(SPEECH_VOLUMN, VOLUME_3);
    TP_SetVolume(MICROPHONE_VOLUMN, VOLUME_3);
    TP_ScrBackLight(1);
    TP_kbLight(1);
}

static void ConvertKeyCodeToStr(uint8 keyCode,uint8* keyCodeStr)
{
    switch (keyCode)
    {
    case TP_KEY_0:
        strcpy((char*)keyCodeStr,"0");
        break;
    case TP_KEY_1:
        strcpy((char*)keyCodeStr,"1");
        break;
    case TP_KEY_2:
        strcpy((char*)keyCodeStr,"2");
        break;
    case TP_KEY_3:
        strcpy((char*)keyCodeStr,"3");
        break;
    case TP_KEY_4:
        strcpy((char*)keyCodeStr,"4");
        break;
    case TP_KEY_5:
        strcpy((char*)keyCodeStr,"5");
        break;
    case TP_KEY_6:
        strcpy((char*)keyCodeStr,"6");
        break;
    case TP_KEY_7:
        strcpy((char*)keyCodeStr,"7");
        break;
    case TP_KEY_8:
        strcpy((char*)keyCodeStr,"8");
        break;
    case TP_KEY_9:
        strcpy((char*)keyCodeStr,"9");
        break;
    case TP_KEY_POUND:
        strcpy((char*)keyCodeStr,"#");
        break;
    case TP_KEY_STAR:
        strcpy((char*)keyCodeStr,"*");
        break;
    default:
        break;
    }
}
void IdleKeyProcess(uint8 firstKeyCode)
{
    uint8 userKey ;
    uint8 str[20] ={0};
    uint8 cancellist[]={TP_KEY_ONHOOK};

    #if 0
    TP_DisplayInfo displayInfo =
    {
        "Please Input Phone:",
        ALIGN_MIDDLE,
        "Comfirm",
        "Cancel",
        TP_KEY_CONFIRM,
        TP_KEY_CANCEL,
        #if defined (TPS320)
        TP_KEY_HANDFREE,
        #else
        TP_KEY_OK,
        #endif
        0,
        CFONT
    };
    TP_InputInfo inputInfo =
    {
        0,0,1,sizeof(str)-1,5000,FALSE,FALSE
    };
    TP_Kbflush();
    TP_SetKey(firstKeyCode);
    memset(str, 0, sizeof(str));
    userKey = TP_NumberInput(&displayInfo,&inputInfo,str);
    #endif
    TP_Kbflush();
    TP_ScrCls();

    TP_ScrGotoxyEx(3,3);
    TP_SetDisplayArea(3,3,126,15);
    TP_LcdPrintf((uint8*)"Please Input Phone:");

    TP_ScrDrawRect(3, 17, 125, 48);//Draw the input content's window
    TP_ScrAttrSet(0);
    TP_ScrGotoxyEx(3,49);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(1,48,126,63);
    TP_LcdPrintf((uint8*)"Confirm       Cancel");

    TP_ScrGotoxyEx(4,19);
    TP_SetDisplayArea(4,19,124,47);
    TP_SetInputModePosition(128, 64);
    TP_ShowInputNum(0, 0, 0);


    memset(str,0x00,sizeof(str));
    //ConvertKeyCodeToStr(firstKeyCode,str);
    TP_SetKey(firstKeyCode);

    TP_SetInputModeControlEx(
                             KEY_DEMO_CONFIRM,
                             #if defined (TPS320)
                             TP_KEY_HANDFREE,
                             #else
                             TP_KEY_OK,
                             #endif
                             KEY_DEMO_CANCEL,
                             TP_KEY_CLEAR,
                             TP_KEY_NONE);
    TP_SetInputModeControlList(0, 0, cancellist, sizeof(cancellist));
    while(1)
    {
        TP_BanIncomingCall(TRUE);
        userKey = TP_GetStringTimedEx((char*)str,0xF4,1,(sizeof(str)-1),3000,NULL);
        TP_BanIncomingCall(FALSE);
        if(userKey == 0xFF)
        {
           break;
        }
        else if (userKey == 0xFB ||
                 userKey == 0xF9)
        {
            if (strlen((char*)str) > 0)
            {
#ifdef INCLUDE_SECRET_CODE
                if(TRUE == TP_SecretCodeParse((char*)str))
                {
                    break;
                }
                if (strcmp((char*)str, "*66#0") == 0)
                {
                    TP_DbgSerialPrnLevel(5);
                    break;
                }
                else if (strcmp((char*)str, "*66#1") == 0)
                {
                    TP_DbgSerialPrnLevel(0);
                    break;
                }

                TP_HookUp();
                TP_ShowDialDilog(str);
                break;
#else
                if (memcmp(str,"*#55#",5)== 0)
                {
                    TP_SystemTest();
                    return;
                }
                if (strcmp(str, "*66#0") == 0)
                {
                    TP_DbgSerialPrnLevel(5);
                }
                else if (strcmp(str, "*66#1") == 0)
                {
                    TP_DbgSerialPrnLevel(0);
                }
                else
                {
                    TP_HookUp();
                    TP_ShowDialDilog(str);
                }
#endif

            }
        }
    }
    TP_ClearInputModeControlList();
    TP_Kbflush();
}


void TP_Charging()
{
    if (currlevel == 0xFF)
    {
        currlevel = TP_GetBatterylevel();
    }
    else
    {
        currlevel++;
        if (currlevel > 4)
        {
            currlevel = TP_GetBatterylevel();
            if (currlevel == 4)
            {
                currlevel = 3;
            }
        }
    }

    TP_ScrSetIcon(ICON_BATTERY,currlevel);
}




/**
 * Function Description ShowIdleScreen():Display Main Program
 * @return None
 * @author  Zuo Jun
 * @date 20110712
 * @note Display both Icons and Time
 */
void ShowIdleScreen()
{
    uint8  keyCode;
    uint8  TmpBuffer[50];
    uint8  TimeBuffer[50];
    uint16 xPos;
    TP_ListBoxRect rect = {27,30,77,38};

    ST_FONT SingleCodeFont;
    ST_FONT MutiCodeFont;


    TP_Kbflush();

    TP_GetFontInfo(&SingleCodeFont,&MutiCodeFont);

	InitCompanyList();

    /*Clear Screen*/
    TP_ScrCls();
#ifdef INCLUDE_PHONE_SETTING
    if (TP_GetKeypadTimeout()!=0)
    {
        TP_TimerSet(7,TP_GetKeypadTimeout());
    }
#endif
    while(TRUE)
    {
        /*Init current screen*/
        TP_ScrFontSet(CFONT);
        TP_ScrSpaceSet(0, 0);
        TP_ScrAttrSet(0);

        /*Refresh Signal Level*/
        TP_ScrSetIcon(ICON_SIGNAL,TP_GetRSSILevel());

        #ifdef INCLUDE_MESSAGE_BOX
        /*Check for Unread SMS*/
        TP_IdleScreenShowMsgIcon();
        #endif

        /*Read Status of Power Charge*/
        if (TP_InCharge()==FALSE)
        {/*Powered by battery*/
            TP_ScrSetIcon(ICON_CHARGE,CLOSEICON);
            ///Refresh Battery Icon
            TP_ScrSetIcon(ICON_BATTERY,TP_GetBatterylevel());

            currlevel = 0xFF;
        }
        else
        {/*Charging*/
            TP_ScrSetIcon(ICON_CHARGE, OPENICON);

            #ifdef TPS300
            TP_ScrSetIcon(ICON_BATTERY, TP_GetBatterylevel());
            #else
            if (TP_IsCharging() == FALSE)
            {
                TP_ScrSetIcon(ICON_BATTERY, TP_GetBatterylevel());
            }
            else
            {
                /*Dynamic Refresh Battery Icon*/
                if (TP_TimerCheck(0) == 0)
                {
                    TP_Charging();
                    TP_TimerSet(0, 600);
                }
            }
            #endif
        }

        TP_LcdFreeze();
        TP_ScrFontSet(CFONT);
        TP_SetDisplayArea(0, 12, 127, 40);
        TP_ScrClsDisplayArea();
        /*Product Name or SIM Error Status*/
        switch(TP_GetMobileCardStatus())
        {
            case TP_MOBILECARDNONE:
                //TP_ScrClsDisplayArea();
                xPos = (128 - MutiCodeFont.Width/2* strlen((char *)"No SIM Card")) / 2;
                TP_ScrGotoxyEx(xPos, 13);
                TP_LcdPrintf((uint8*)"No SIM Card");
                xPos = (128 - MutiCodeFont.Width/2 * strlen((char *)"Insert After Poweroff")) / 2;
                TP_ScrGotoxyEx(xPos, 28);
                TP_LcdPrintf((uint8*)"Insert After PowerOff");
                break;

            case TP_MOBILECARDPUKLOCK:
                //TP_ScrClsDisplayArea();
                xPos = (128 - MutiCodeFont.Width/2 * strlen((char *)"SIM Card")) / 2;
                TP_ScrGotoxyEx(xPos, 13);
                TP_LcdPrintf((uint8*)"SIM Card");
                xPos = (128 - MutiCodeFont.Width/2 * strlen((char *)"PUK Locked")) / 2;
                TP_ScrGotoxyEx(xPos, 28);
                TP_LcdPrintf((uint8*)"PUK Locked");
                break;

            case TP_MOBILECARDINVALID:
                //TP_ScrClsDisplayArea();
                xPos = (128 - MutiCodeFont.Width/2 * strlen((char *)"SIM Card")) / 2;
                TP_ScrGotoxyEx(xPos, 13);
                TP_LcdPrintf((uint8*)"SIM Card");
                xPos = (128 - MutiCodeFont.Width/2 * strlen((char *)"Invalid")) / 2;
                TP_ScrGotoxyEx(xPos, 28);
                TP_LcdPrintf((uint8*)"Invalid");
                break;

            default:
                //TP_ScrClsDisplayArea();
                xPos = (128 - MutiCodeFont.Width/2 * strlen((char *)"GRE-5")) / 2;
                TP_ScrGotoxyEx(xPos, 20);
                TP_LcdPrintf((uint8*)"GRE-5");
                break;
        }

        /*Date & Time*/
		if( strlen(g_userData.securityToken) != 0 ){
			TP_ScrFontSet(ASCII);
			TP_SetDisplayArea(0, 41, 127, 63);
			TP_ScrClsDisplayArea();

			memset(TmpBuffer,0,sizeof(TmpBuffer));
			TP_GetTime(TmpBuffer);

			memset(TimeBuffer,0,sizeof(TimeBuffer));
			TP_HexToStr(TmpBuffer,7,TimeBuffer);
			memset(TmpBuffer,0x20,sizeof(TmpBuffer));

			memcpy(&TmpBuffer[0],&TimeBuffer[2],2);
			TmpBuffer[2] = '/';
			memcpy(&TmpBuffer[3],&TimeBuffer[4],2);
			TmpBuffer[5] = '/';
			memcpy(&TmpBuffer[6],"20",2);
			memcpy(&TmpBuffer[8],&TimeBuffer[0],2);
			TmpBuffer[10] = ' ';
			memcpy(&TmpBuffer[11],&TimeBuffer[6],2);
			TmpBuffer[13] = ':';
			memcpy(&TmpBuffer[14],&TimeBuffer[8],2);
			TmpBuffer[16] = ':';
			memcpy(&TmpBuffer[17],&TimeBuffer[10],2);
			TmpBuffer[19] =0;

			xPos = (128 - SingleCodeFont.Width* strlen((char *)TmpBuffer)) / 2;
			TP_ScrGotoxyEx(xPos, 41);
			TP_LcdPrintf(TmpBuffer);
		}

        /*Bottom Operation Tips*/
        TP_ScrFontSet(CFONT);
        TP_SetDisplayArea(0, 52, 127, 63);
        TP_ScrClsDisplayArea();
        TP_ScrGotoxyEx(0,52);
#ifdef INCLUDE_PHONE_SETTING
        if (TP_GetKeypadTimeout() != 0 &&
            TP_TimerCheck(7) == 0)
        {
            TP_ScrSetIcon(ICON_LOCK, OPENICON);
            TP_LcdPrintf((uint8*)"Locked");
        }
        else
        {
            TP_ScrSetIcon(ICON_LOCK, CLOSEICON);
            TP_LcdPrintf((uint8*)"Menu");
        }
#else
        TP_LcdPrintf((uint8*)"Menu");
#endif

#ifdef INCLUDE_CALL_RECORD
        TP_IdleScreenShowMissCallIcon();
#endif
        TP_ScrUpdate();

        /*Real Time Event Notify Check*/
        TP_NotifyCheck();

        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();

            #ifdef INCLUDE_PHONE_SETTING
            if (TP_TimerCheck(7) == 0 && TP_GetKeypadTimeout()!=0)
            {
                if(TP_UnlockKeypad() == TRUE)
                {
                    TP_ScrSetIcon(ICON_LOCK, CLOSEICON);
                    TP_TimerSet(7, TP_GetKeypadTimeout());
                }
                continue;
            }
            #endif

            //TP_DbgSerialPrn("\r\n keyCode:%d",keyCode);
            switch (keyCode)
            {

                case KEY_DEMO_MENU:
                    TP_MenuShow();
                    TP_ScrCls();
                    TP_GetFontInfo(&SingleCodeFont,&MutiCodeFont);
                    break;

            #ifdef INCLUDE_CALL_RECORD
                case KEY_DEMO_CANCEL:
                    if(GetMissCallCount()>0)
                    {
                        ShowCallRecordList(2);
                        TP_ScrCls();
                    }
                    break;
            #endif

                case TP_KEY_HANDFREE:
                case TP_KEY_OFFHOOK:
                case TP_KEY_OK:
                    keyCode = TP_GetCallStatus();
                    TP_DbgSerialPrn("\r\n CallStatus:%d ", keyCode);
                    //TP_PhoneHookUp();
                    if (keyCode == TP_HANGUP)
                    {
                        TP_PhoneHookUp();
                    }
                    else if (keyCode == TP_HOOKUP)
                    {
                        TP_PhoneHungUp();
                    }
                    else
                    {
                        TP_PhoneHungUp();
                        TP_PhoneHookUp();
                    }
                    TP_ScrCls();
                    break;
                case TP_KEY_POUND:
                case TP_KEY_STAR:
                case TP_KEY_0:
                case TP_KEY_1:
                case TP_KEY_2:
                case TP_KEY_3:
                case TP_KEY_4:
                case TP_KEY_5:
                case TP_KEY_6:
                case TP_KEY_7:
                case TP_KEY_8:
                case TP_KEY_9:
                   IdleKeyProcess(keyCode);
                   TP_ScrCls();
                   TP_ScrAttrSet(0);
                   break;
            }
        #ifdef INCLUDE_PHONE_SETTING
            if (TP_GetKeypadTimeout()!=0)
            {
                TP_TimerSet(7,TP_GetKeypadTimeout());
            }
        #endif
        }
    }
}



