/******************************************************************************
* Include Headers
*******************************************************************************/
#include <TPTypedef.h>
#include <TPBase.h>
#include <TPLCD.h>
#include <TPKeyPad.h>
#include <TPMobileCard.h>
#include <TP_Singleime.h>
#include <TPSerial.h>

#include "TP_ListBox.h"
#include "TP_PhoneSetting.h"
#include "TP_DisplayAPI.h"
#include "TP_ParamsManager.h"

#define min(x, y) ((x) < (y) ? (x) : (y))

//length of the security password
#define LENGTH_SECURITY_PASSWORD 4

//set the display position of time setting
#define SETDATETIME_START_POS_YEAR  3
#define SETDATETIME_START_POS_MONTH (SETDATETIME_START_POS_YEAR+5*6)
#define SETDATETIME_START_POS_DAY   (SETDATETIME_START_POS_MONTH+3*6)
#define SETDATETIME_START_POS_HOUR  (SETDATETIME_START_POS_DAY+3*6)
#define SETDATETIME_START_POS_MIN   (SETDATETIME_START_POS_HOUR+3*6)
#define SETDATETIME_START_POS_SEC   (SETDATETIME_START_POS_MIN+3*6)

#define SETDATETIME_END_POS_YEAR    (SETDATETIME_START_POS_YEAR+4*6)
#define SETDATETIME_END_POS_MONTH   (SETDATETIME_START_POS_MONTH+2*6)
#define SETDATETIME_END_POS_DAY     (SETDATETIME_START_POS_DAY+2*6)
#define SETDATETIME_END_POS_HOURS   (SETDATETIME_START_POS_HOUR+2*6)
#define SETDATETIME_END_POS_MIN     (SETDATETIME_START_POS_MIN+2*6)
#define SETDATETIME_END_POS_SEC     (SETDATETIME_START_POS_SEC+2*6)

#define SETDATETIME_TOP             28
#define SETDATETIME_BOTTOM          48


/***********************First level list********************************/
static const uint8 *SysSettingList[]=
{
    (uint8*)TEXT_SYSSETTING_VOICESETTING,
    (uint8*)TEXT_SYSSETTING_DISPLAYSETTING,
    (uint8*)TEXT_SYSSETTING_SECURITYSETTING,
    (uint8*)TEXT_SYSSETTING_PHONEINFO,
    (uint8*)TEXT_SYSSETTING_KEYPADLOCK
};
#define MENU_SYSSETTING_VOICESETTING        (1)
#define MENU_SYSSETTING_DISPLAYSETTING      (2)
#define MENU_SYSSETTING_SECURITYSETTING     (3)
#define MENU_SYSSETTING_PHONEINFO           (4)
#define MENU_SYSSETTING_KEYPADLOCK          (5)
/*************************************************************************/

/**************************Sublist of voice setting***********************/
static const uint8 *VoiceSettingList[]=
{
    (uint8*)TEXT_SYSSETTING_RING,
    (uint8*)TEXT_SYSSETTING_VOLUMN
};
#define MENU_SYSSETTING_RING                (1)
#define MENU_SYSSETTING_VOLUMN              (2)

static const uint8 *VolumnSettingList[]=
{
    (uint8*)TEXT_SYSSETTING_KEYPADVOLUMN,
    (uint8*)TEXT_SYSSETTING_RINGVOLUMN,
    (uint8*)TEXT_SYSSETTING_MICROPHONEVOLUMN,
    (uint8*)TEXT_SYSSETTING_SPEECHVOLUMN
};
#define MENU_SYSSETTING_KEYPADVOLUMN        (1)
#define MENU_SYSSETTING_RINGVOLUMN          (2)
#define MENU_SYSSETTING_MICROPHONEVOLUMN    (3)
#define MENU_SYSSETTING_SPEAKERVOLUMN        (4)

/*****************************************************************************/

/*************************Sublist of display setting**************************/
static const uint8 *DisplaySettingList[]=
{
    (uint8*)TEXT_SYSSETTING_BACKLIGHT,
    (uint8*)TEXT_SYSSETTING_CONTRAST,
    (uint8*)TEXT_SYSSETTING_TIME
};
#define MENU_SYSSETTING_BACKLIGHT           (1)
#define MENU_SYSSETTING_CONTRAST            (2)
#define MENU_SYSSETTING_TIME                (3)

/*******************************************************************************/

/*************************Sublist of security setting***************************/
static const uint8 *SecuritySettingList[]=
{
    (uint8*)TEXT_SYSSETTING_PIN,
    (uint8*)TEXT_SYSSETTING_CHANGEPASSWORD,
    (uint8*)TEXT_SYSSETTING_RESTORESETTING
};
#define MENU_SYSSETTING_PIN                  (1)
#define MENU_SYSSETTING_CHANGEPASSWORD       (2)
#define MENU_SYSSETTING_RESTORESETTING       (3)

static const uint8 *PinSettingList[]=
{
    (uint8*)TEXT_SYSSETTING_PINON,
    (uint8*)TEXT_SYSSETTING_PINOFF,
    (uint8*)TEXT_SYSSETTING_PINCHANGE
};
#define MENU_SYSSETTING_PINON               (1)
#define MENU_SYSSETTING_PINOFF              (2)
#define MENU_SYSSETTING_PINCHANGE           (3)
/*******************************************************************************/

/**************************Sublist of phone information**************************/

static const uint8 *PhoneInfoList[]=
{
    (uint8*)TEXT_SYSSETTING_PHONENO,
    (uint8*)TEXT_SYSSETTING_SOFTVERSION,
    (uint8*)TEXT_SYSTETTING_IMEI
};
#define MENU_SYSSETTING_PHONENO             (1)
#define MENU_SYSSETTING_SOFTVERSION         (2)
#define MENU_SYSSETTING_IMEI                (3)

/*********************************************************************************/

/**************************Sublist of keypad lock**************************/

static const uint8 *KeypadLockList[]=
{
    (uint8*)TEXT_SYSSETTING_ONOFF,
    (uint8*)TEXT_SYSSETTING_TIMEOUT
};
#define MENU_SYSSETTING_ONOFF               (1)
#define MENU_SYSSETTING_TIMEOUT             (2)

/*********************************************************************************/

int32 keypadLockTimeout = 0;

extern const char
SoftwareVersion[10];


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
static uint16 TP_HexToStr(uint8* Src,uint16 Size,uint8* des)
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

static uint16 TP_StrToHex(uint8* Src,uint16 Size,uint8* des)
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

/******************************************************************************
* Private Functions
*******************************************************************************/

static void ShowSetupResult(uint8 mode)
{
    if (mode == 0)
    {
        TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_SETUPFAIL,4000);
    }
    else if (mode == 1)
    {
        TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_SETUPSUCCESS,3000);
    }
    else if (mode == 2)
    {
        TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PARAMSABNORMAL,3000);
    }
    else if (mode == 3)
    {
        TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_OUTRANGE,3000);
    }
}


static Boolean checkSecurityPassword()
{
    uint8 pBuf[LENGTH_SECURITY_PASSWORD+1] = {0};
    uint8 securityPassword[LENGTH_SECURITY_PASSWORD+1] = {0};
    TP_DisplayInfo displayInfo =
    {
        (uint8*)TEXT_SYSSETTING_INPUTPASSWORD,
        ALIGN_MIDDLE,
        (uint8*)TEXT_SYSSETTING_CONFIRM,
        (uint8*)TEXT_SYSSETTING_RETURN,
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_CANCEL2,
        CFONT
    };
    TP_InputInfo inputInfo =
    {
        INPUTMODE_NUMBER,
        0xD401,
        LENGTH_SECURITY_PASSWORD,
        LENGTH_SECURITY_PASSWORD,
        0,
        TRUE,
        FALSE
    };

    if (FALSE == TP_GetParamItem(NAME_PARAMS(SecurityPassword),VAT_String,&securityPassword))
    {
        ShowSetupResult(2);
        return FALSE;
    }
    //TP_DbgSerialPrn("<%s>",securityPassword);

    //inputInfo.minLen = LENGTH_SECURITY_PASSWORD;
    //inputInfo.maxLen = LENGTH_SECURITY_PASSWORD;
    //inputInfo.showInputNumber = TRUE;
    if (0xFB != TP_PassWordInput(&displayInfo,&inputInfo,'*',pBuf))
    {
        return FALSE;
    }
    if (0 != strcmp((const char*)pBuf,(const char*)securityPassword))
    {
        //TP_DbgSerialPrn("Not Equal!");
        TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PWDERROR,2000);
        return FALSE;
    }
    return TRUE;
}
static void RingSetting()
{
    int32 ringType = 1;
    uint8 *ringList[] =
    {
        (uint8*)"Ring1",(uint8*)"Ring2",(uint8*)"Ring3",(uint8*)"Ring4",(uint8*)"Ring5",
        (uint8*)"Ring6",(uint8*)"Ring7",(uint8*)"Ring8",(uint8*)"Ring9",(uint8*)"Ring10"
    };
    TP_DisplayInfo displayInfo =
    {
        (uint8*)TEXT_SYSSETTING_RING,
        ALIGN_MIDDLE,
        (uint8*)TEXT_SYSSETTING_CONFIRM,
        (uint8*)TEXT_SYSSETTING_RETURN,
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_CANCEL2,
        CFONT
    };
    if (TP_GetParamItem(NAME_PARAMS(RingType),VAT_Int32,(void *)&ringType) == TRUE)
    {
        while (TRUE)
        {
            displayInfo.strTitle = (uint8*)TEXT_SYSSETTING_RING;
            if( TRUE == TP_CreateListBoxInterface(TL_RADIOBOX,
                                                  &displayInfo,
                                                  ringList,
                                                  10,
                                                  ringType,
                                                  &ringType,
                                                  NULL))
            {
                displayInfo.strTitle = ringList[ringType-1];

                TP_PlayRing(ringType);
                if (TP_PopupMessageWithButton(&displayInfo) == TRUE)
                {
                    if (TP_SetParamItem(NAME_PARAMS(RingType),VAT_Int32,(void *)&ringType) == TRUE)
                    {
                        TP_StopRing();
                        ShowSetupResult(1);
                    }
                    else
                    {
                        TP_StopRing();
                        ShowSetupResult(2);
                    }
                }
                else
                {
                    TP_StopRing();
                }

            }
            else
            {
                break;
            }
        }
    }
    else
    {
        ShowSetupResult(2);
    }
}
static void VolumnSetting(uint8 type)
{
    PARAMS_NAMES paramsId;
    uint8 volumnType = 0;
    int32 volumn = 0;
    uint8 pBuf[2] ={0};
    TP_DisplayInfo displayInfo=
    {
        (uint8*)"Input level:(0~7)",
        ALIGN_LEFT,
        (uint8*)TEXT_SYSSETTING_CONFIRM,
        (uint8*)TEXT_SYSSETTING_RETURN,
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_CANCEL2,
        CFONT
    };
    TP_InputInfo inputInfo = {0};

    switch (type)
    {
    case MENU_SYSSETTING_KEYPADVOLUMN:
        paramsId = NAME_PARAMS(KeypadVolumn);
        volumnType = KEYPAD_VOLUMN;
        break;
    case MENU_SYSSETTING_RINGVOLUMN:
        paramsId = NAME_PARAMS(RingVolumn);
        volumnType = RING_VOLUMN;
        break;
    case MENU_SYSSETTING_MICROPHONEVOLUMN:
        paramsId = NAME_PARAMS(MicrophoneVolumn);
        volumnType = MICROPHONE_VOLUMN;
        break;
    case MENU_SYSSETTING_SPEAKERVOLUMN:
        paramsId = NAME_PARAMS(SpeakerVolumn);
        volumnType = SPEECH_VOLUMN;
        break;
    }

    if (TP_GetParamItem(paramsId,VAT_Int32,(void *)&volumn) == TRUE)
    {
        sprintf((char *)pBuf,"%d",volumn);
        inputInfo.maxLen = 1;
        while (TRUE)
        {
            if(TP_NumberInput(&displayInfo,&inputInfo,pBuf) == 0xFB)
            {
                if (strlen((char *)pBuf)==1)
                {
                    volumn = atoi((const char*)pBuf);
                    if (volumn >= VOLUME_0 && volumn <= VOLUME_7)
                    {
                        //TP_DbgSerialPrn("<volumn is %d>",volumn);
                        if (TP_SetParamItem(paramsId,VAT_Int32,(void *)&volumn) == TRUE)
                        {
                            ShowSetupResult(1);
                            TP_SetVolume(volumnType, volumn);
                            break;
                        }
                        else
                        {
                            ShowSetupResult(0);
                            break;
                        }
                    }
                    else
                    {
                        ShowSetupResult(3);
                    }
                }
            }
            else
            {
                break;
            }
        }//end of whie(TRUE)
    }
    else
    {
        ShowSetupResult(2);
    }
}
static void BacklightSetting()
{
    int32 backlightMode = 0;
    uint8 *BacklightSettingList[]=
    {
        (uint8*)TEXT_SYSSETTING_OFF,
        (uint8*)TEXT_SYSSETTING_ONONEMIN,
        (uint8*)TEXT_SYSSETTING_ON
    };
    TP_DisplayInfo displayInfo =
    {
        (uint8*)TEXT_SYSSETTING_BACKLIGHT,
        ALIGN_MIDDLE,
        (uint8*)TEXT_SYSSETTING_CONFIRM,
        (uint8*)TEXT_SYSSETTING_RETURN,
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_CANCEL2,
        CFONT
    };
    if (TP_GetParamItem(NAME_PARAMS(Backlight),VAT_Int32,&backlightMode) == TRUE)
    {
        backlightMode += 1;
        while (TRUE)
        {
            if (TRUE == TP_CreateListBoxInterface(TL_RADIOBOX,
                                                  &displayInfo,
                                                  BacklightSettingList,
                                                  3,
                                                  backlightMode,
                                                  &backlightMode,
                                                  NULL))
            {
                backlightMode -= 1;
                if (TP_SetParamItem(NAME_PARAMS(Backlight),VAT_Int32,&backlightMode) == TRUE)
                {
                    ShowSetupResult(1);
                    TP_ScrBackLight(backlightMode);
                    TP_kbLight(backlightMode);
                    break;
                }
                else
                {
                    ShowSetupResult(0);
                    break;
                }
            }
            else
            {
                break;
            }
        }//end of while(TRUE)
    }
    else
    {
        ShowSetupResult(2);
    }
}
static void ContrastSetting()
{
    uint8 keyCode;
    int32 gray = 22;
    uint8 gray2;

    TP_Kbflush();
    TP_ScrCls();

    gray = TP_ScrGrayGet();
    gray2 = gray;

    TP_ShowTextSingleLine(14,24,126,47,CFONT,ALIGN_LEFT,(uint8*)"Gray: %d",gray);
    while(TRUE)
    {
        TP_NotifyCheck();
        if (TP_Kbhit())
        {
            keyCode = TP_GetKey();
            switch (keyCode)
            {
            case KEY_SYSSETTING_CANCEL1:
            case KEY_SYSSETTING_CANCEL2:
                TP_ScrGray(gray2);
                return;

            case KEY_SYSSETTING_CONFIRM1:
            case KEY_SYSSETTING_CONFIRM2:
                if (TP_SetParamItem(NAME_PARAMS(Contrast),VAT_Int32,&gray) == TRUE)
                {
                    ShowSetupResult(1);
                    TP_ScrGray(gray);
                }
                else
                {
                    ShowSetupResult(0);
                    TP_ScrGray(gray2);
                }
                return;
            case TP_KEY_UP :
                if (gray >0)
                {
                    gray--;
                    TP_ScrGray(gray);
                }
                TP_ScrClsDisplayArea();
                TP_LcdPrintf((uint8*)"Gray: %d",gray);
                break;
            case TP_KEY_DOWN:
                if (gray < 63)
                {
                    gray++;
                    TP_ScrGray(gray);
                }
                TP_ScrClsDisplayArea();
                TP_LcdPrintf((uint8*)"Gray: %d",gray);
                break;
            }
        }
    }

}
static void TimeSetting()
{
    TP_DateTime dateTime = {0};
    TP_DateTime dateTimeBack = {0};
    uint8 result;
    uint8 editindex = 0;//
    uint8 editlen = 0;
    uint8 editdata[5];
    uint16 editValue = 0;
    uint32 charspace = 0;
    uint32 linespace = 0;
    uint8 cancelKeyList[] =
    {
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CANCEL2
    };
    uint8 confirmKeyList[] =
    {
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_DOWN,
        KEY_SYSSETTING_UP
    };
    uint8 keycode;
    Boolean IsSetTime = FALSE;
    TP_CharSetT CurrCharSet = {0};

    TP_GetCurrentCharSet(&CurrCharSet);
    TP_SetCurrentCharSet(CHARSET_ASCII);
    TP_ScrSpaceGet(&charspace, &linespace);
    TP_ScrSpaceSet(0, 0);

    TP_GetDateTime(&dateTime);
    memcpy(&dateTimeBack, &dateTime, sizeof(TP_DateTime));

    TP_SetInputModeControl(TP_KEY_NONE,TP_KEY_NONE,KEY_SYSSETTING_IMCHANGEKEY);
    TP_SetInputModeControlList(confirmKeyList, sizeof(confirmKeyList),
                               cancelKeyList, sizeof(cancelKeyList));
    TP_SetInputModePosition(128, 64);
    TP_ShowInputNum(0, 0, 0);
    while(1)
    {
        IsSetTime = FALSE;
        TP_ScrCls();
        TP_ScrAttrSet(0);
        TP_ScrSpaceSet(0, 0);
        TP_ScrFontSet(CFONT);
        TP_ScrDrawRect(0,0,127,63);
        TP_SetDisplayArea(0,0,127,63);
        TP_ScrGotoxyEx(4,3);
        TP_LcdPrintf((uint8*)TEXT_SYSSETTING_SETDATETIME);
        TP_ScrGotoxyEx(4,49);
        TP_LcdPrintf((uint8*)TEXT_SYSSETTING_SELECTEXIT);

        TP_SetDisplayArea(SETDATETIME_START_POS_YEAR,SETDATETIME_TOP,SETDATETIME_END_POS_YEAR,SETDATETIME_BOTTOM);
        TP_ScrGotoxyEx(SETDATETIME_START_POS_YEAR,SETDATETIME_TOP);
        TP_LcdPrintf("%04d",dateTime.Date.Year);

        TP_SetDisplayArea(SETDATETIME_END_POS_YEAR,SETDATETIME_TOP,SETDATETIME_START_POS_MONTH,SETDATETIME_BOTTOM);
        TP_ScrGotoxyEx(SETDATETIME_END_POS_YEAR,SETDATETIME_TOP);
        TP_LcdPrintf((uint8*)"-");

        TP_SetDisplayArea(SETDATETIME_START_POS_MONTH,SETDATETIME_TOP,SETDATETIME_END_POS_MONTH,SETDATETIME_BOTTOM);
        TP_ScrGotoxyEx(SETDATETIME_START_POS_MONTH,SETDATETIME_TOP);
        TP_LcdPrintf("%02d",dateTime.Date.Month);

        TP_SetDisplayArea(SETDATETIME_END_POS_MONTH,SETDATETIME_TOP,SETDATETIME_START_POS_DAY,SETDATETIME_BOTTOM);
        TP_ScrGotoxyEx(SETDATETIME_END_POS_MONTH,SETDATETIME_TOP);
        TP_LcdPrintf((uint8*)"-");

        TP_SetDisplayArea(SETDATETIME_START_POS_DAY,SETDATETIME_TOP,SETDATETIME_END_POS_DAY,SETDATETIME_BOTTOM);
        TP_ScrGotoxyEx(SETDATETIME_START_POS_DAY,SETDATETIME_TOP);
        TP_LcdPrintf("%02d",dateTime.Date.Day);

        TP_SetDisplayArea(SETDATETIME_END_POS_DAY,SETDATETIME_TOP,SETDATETIME_START_POS_HOUR,SETDATETIME_BOTTOM);
        TP_ScrGotoxyEx(SETDATETIME_END_POS_DAY,SETDATETIME_TOP);
        TP_LcdPrintf((uint8*)" ");

        TP_SetDisplayArea(SETDATETIME_START_POS_HOUR,SETDATETIME_TOP,SETDATETIME_END_POS_HOURS,SETDATETIME_BOTTOM);
        TP_ScrGotoxyEx(SETDATETIME_START_POS_HOUR,SETDATETIME_TOP);
        TP_LcdPrintf("%02d",dateTime.Time.Hours);

        TP_SetDisplayArea(SETDATETIME_END_POS_HOURS,SETDATETIME_TOP,SETDATETIME_START_POS_MIN,SETDATETIME_BOTTOM);
        TP_ScrGotoxyEx(SETDATETIME_END_POS_HOURS,SETDATETIME_TOP);
        TP_LcdPrintf((uint8*)":");

        TP_SetDisplayArea(SETDATETIME_START_POS_MIN,SETDATETIME_TOP,SETDATETIME_END_POS_MIN,SETDATETIME_BOTTOM);
        TP_ScrGotoxyEx(SETDATETIME_START_POS_MIN,SETDATETIME_TOP);
        TP_LcdPrintf("%02d",dateTime.Time.Minutes);

        TP_SetDisplayArea(SETDATETIME_END_POS_MIN,SETDATETIME_TOP,SETDATETIME_START_POS_SEC,SETDATETIME_BOTTOM);
        TP_ScrGotoxyEx(SETDATETIME_END_POS_MIN,SETDATETIME_TOP);
        TP_LcdPrintf((uint8*)":");

        TP_SetDisplayArea(SETDATETIME_START_POS_SEC,SETDATETIME_TOP,SETDATETIME_END_POS_SEC,SETDATETIME_BOTTOM);
        TP_ScrGotoxyEx(SETDATETIME_START_POS_SEC,SETDATETIME_TOP);
        TP_LcdPrintf("%02d",dateTime.Time.Seconds);
        switch(editindex)
        {
            case 0:
                editValue = dateTime.Date.Year;
                TP_SetDisplayArea(SETDATETIME_START_POS_YEAR-1,SETDATETIME_TOP,SETDATETIME_END_POS_YEAR,SETDATETIME_BOTTOM);
                TP_ScrGotoxyEx(SETDATETIME_START_POS_YEAR-1,SETDATETIME_TOP);
                break;
            case 1:
                editValue = dateTime.Date.Month;
                TP_SetDisplayArea(SETDATETIME_START_POS_MONTH-1,SETDATETIME_TOP,SETDATETIME_END_POS_MONTH,SETDATETIME_BOTTOM);
                TP_ScrGotoxyEx(SETDATETIME_START_POS_MONTH-1,SETDATETIME_TOP);
                break;
            case 2:
                editValue = dateTime.Date.Day;
                TP_SetDisplayArea(SETDATETIME_START_POS_DAY-1,SETDATETIME_TOP,SETDATETIME_END_POS_DAY,SETDATETIME_BOTTOM);
                TP_ScrGotoxyEx(SETDATETIME_START_POS_DAY-1,SETDATETIME_TOP);
                break;
            case 3:
                editValue = dateTime.Time.Hours;
                TP_SetDisplayArea(SETDATETIME_START_POS_HOUR-1,SETDATETIME_TOP,SETDATETIME_END_POS_HOURS,SETDATETIME_BOTTOM);
                TP_ScrGotoxyEx(SETDATETIME_START_POS_HOUR-1,SETDATETIME_TOP);
                break;
            case 4:
                editValue = dateTime.Time.Minutes;
                TP_SetDisplayArea(SETDATETIME_START_POS_MIN-1,SETDATETIME_TOP,SETDATETIME_END_POS_MIN,SETDATETIME_BOTTOM);
                TP_ScrGotoxyEx(SETDATETIME_START_POS_MIN-1,SETDATETIME_TOP);
                break;
            case 5:
                editValue = dateTime.Time.Seconds;
                TP_SetDisplayArea(SETDATETIME_START_POS_SEC-1,SETDATETIME_TOP,SETDATETIME_END_POS_SEC,SETDATETIME_BOTTOM);
                TP_ScrGotoxyEx(SETDATETIME_START_POS_SEC-1,SETDATETIME_TOP);
                break;
        }
        if(editindex==0)
        {
            editlen = 4;
        }
        else
        {
            editlen = 2;
        }
        memset(editdata,0,sizeof(editdata));
        if(editindex == 0)
        {
            sprintf(editdata, "%04d", editValue);
        }
        else
        {
            sprintf(editdata, "%02d", editValue);
        }
        result = TP_GetString((char *)editdata,0x74,1,editlen);
        switch(editindex)
        {
            case 0:
                dateTime.Date.Year = atoi(editdata);
                break;
            case 1:
                dateTime.Date.Month = atoi(editdata);
                break;
            case 2:
                dateTime.Date.Day = atoi(editdata);
                break;
            case 3:
                dateTime.Time.Hours = atoi(editdata);
                break;
            case 4:
                dateTime.Time.Minutes = atoi(editdata);
                break;
            case 5:
                dateTime.Time.Seconds = atoi(editdata);
                break;
        }
        if(result == 0xfa || result == 0xFB)
        {
            if( result == 0xFB )
            {
                keycode = TP_GetInputModeControlListValue();
                if(keycode == KEY_SYSSETTING_CONFIRM1 ||
                   keycode == KEY_SYSSETTING_CONFIRM2)//confirm key
                {
                    IsSetTime = TRUE;
                }
                else if(keycode == KEY_SYSSETTING_UP)//up key
                {
                    if(editindex > 0)
                    {
                        editindex--;
                    }
                    else
                    {
                        editindex = 0;
                    }
                }
                else if(keycode == KEY_SYSSETTING_DOWN)//down key
                {
                    if(editindex < 5)
                    {
                        editindex++;
                    }
                    else
                    {
                        editindex = 5;
                    }
                }
            }
            else
            {
                if(editindex<5)
                {
                    editindex++;
                }
            }

        }
        else if(result == 0xff)
        {
            keycode = TP_GetInputModeControlListValue();
            //TP_DbgSerialPrn("\n keycode = %d",keycode);
            if(keycode == KEY_SYSSETTING_CANCEL1 ||
               keycode == KEY_SYSSETTING_CANCEL2)
            {
                break;
            }
        }
        if(IsSetTime == TRUE)
        {
            result = TP_SetDateTime(&dateTime);
            switch (result)
            {
                case 0:
                    break;
                case 1:
                    TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_YEARERROR,1000);
                    break;
                case 2:
                    TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_MONTHERROR,1000);
                    break;
                case 3:
                    TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_DATEERROR,1000);
                    break;
                case 4 :
                    TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_HOURERROR,1000);
                    break;
                case 5 :
                    TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_MINUTEERROR,1000);
                    break;
                case 6 :
                    TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_SECONDERROR,1000);
                    break;
                case 7 :
                    TP_PopupMessageTimed((uint8*)TEXT_SYSSETTING_INVALID_DATA,1000);
                    break;
            }
            if(result == 0)//sucess
            {
                break;
            }
            else
            {
                memcpy(&dateTime, &dateTimeBack, sizeof(TP_DateTime));
                editindex = 0;
            }
        }
    }
    TP_ClearInputModeControlList();
    TP_ScrAttrSet(0);
    TP_ScrSpaceSet(charspace, linespace);
    if (result == 0)
    {
        ShowSetupResult(1);
    }
    TP_SetCurrentCharSet(CurrCharSet.name);

}
static void PinSetting(uint8 mode)
{
    uint8 pin[10] = {0};
    uint8 newPin[10];
    uint8 newPincCfm[10];
    uint8 ret = 0;
    TP_DisplayInfo displayInfo =
    {
        (uint8*)TEXT_SYSSETTING_PINNUMBER,
        ALIGN_MIDDLE,
        (uint8*)TEXT_SYSSETTING_CONFIRM,
        (uint8*)TEXT_SYSSETTING_RETURN,
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_CANCEL2,
        CFONT
    };
    TP_InputInfo inputInfo ={0};
    inputInfo.minLen = 4;
    inputInfo.maxLen = 8;
    inputInfo.showInputNumber = TRUE;
    inputInfo.defaultMethod = INPUTMODE_NUMBER;
    inputInfo.inputMode = 0xD401;
    switch (mode)
    {
    case MENU_SYSSETTING_PINON:
        if (TP_IsMobileCardPinLock())
        {
            TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PINALREADYON,3000);
            return;
        }
        if (0xFB != TP_PassWordInput(&displayInfo,&inputInfo,'*',pin))
        {
            return;
        }
        ret = TP_MobileCardPinOnOff(1,pin);
        if (ret == 0xFF)
        {
            TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PINONSUCCESS,3000);
            return;
        }
        if (ret == 0xFE)
        {
            TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PINERROR,3000);
            return ;
        }
        TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PINONFAIL,3000);
        return;

    case MENU_SYSSETTING_PINOFF:
        if (TP_IsMobileCardPinLock() == FALSE)
        {
            TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PINALREADYOFF,3000);
            return;
        }
        memset(pin,0x00,sizeof(pin));
        if (0xFB != TP_PassWordInput(&displayInfo,&inputInfo,'*',pin))
        {
            return;
        }
        //TP_DbgSerialPrn("<pin is %s>",pin);
        ret = TP_MobileCardPinOnOff(0,pin);
        if (ret == 0xFF)
        {
            TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PINOFFSUCCESS,3000);
            return;
        }
        if (ret == 0xFE)
        {
            TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PINERROR,3000);
            return ;
        }
        TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PINOFFFAIL,3000);
        return;
    case MENU_SYSSETTING_PINCHANGE:
        if (TP_IsMobileCardPinLock() == FALSE)
        {
            TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PINALREADYOFF,3000);
            return;
        }
        displayInfo.strTitle = (uint8*)TEXT_SYSSETTING_OLDPINNUMMER;
        if (0xFB != TP_PassWordInput(&displayInfo,&inputInfo,'*',pin))
        {
            return;
        }
        while (TRUE)
        {
            memset(newPin, 0x00,sizeof(newPin));
            memset(newPincCfm,0x00,sizeof(newPincCfm));

            displayInfo.strTitle = (uint8*)TEXT_SYSSETTING_INPUTNEWPIN;
            if (0xFB != TP_PassWordInput(&displayInfo,&inputInfo,'*',newPin))
            {
                return;
            }
            if (0xFB != TP_PassWordInput(&displayInfo,&inputInfo,'*',newPincCfm))
            {
                return;
            }
            if (0 == strcmp((const char*)newPin,(const char*)newPincCfm))
            {
                break;
            }
            else
            {
                TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_INPUERROR,3000);
            }
        }

        if (0xFF == TP_MobileCardChangePin(pin,newPin))
        {
            TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PINCHANGESUCCESS,3000);
            return;
        }
        TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PINCHANGEFAIL,3000);
        return;
    default:
        break;
    }
}
static void ChangeSecurityPassword()
{
    uint8 securityPassword[LENGTH_SECURITY_PASSWORD+1] ={0};
    uint8 oldPassword[LENGTH_SECURITY_PASSWORD+1];
    uint8 newPassword[LENGTH_SECURITY_PASSWORD+1];
    uint8 confirmPassword[LENGTH_SECURITY_PASSWORD+1];

    //uint8 keyCode;
    TP_DisplayInfo displayInfo =
    {
        (uint8*)TEXT_SYSSETTING_INPUTOLDPASSWORD,
        ALIGN_MIDDLE,
        (uint8*)TEXT_SYSSETTING_CONFIRM,
        (uint8*)TEXT_SYSSETTING_RETURN,
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_CANCEL2,
        CFONT
    };
    TP_InputInfo inputInfo =
    {
        INPUTMODE_NUMBER,
        0xD401,
        LENGTH_SECURITY_PASSWORD,
        LENGTH_SECURITY_PASSWORD,
        0,
        TRUE,
        FALSE
    };
    //inputInfo.minLen = LENGTH_SECURITY_PASSWORD;
    //inputInfo.maxLen = LENGTH_SECURITY_PASSWORD;
    //inputInfo.showInputNumber = TRUE;
    if (FALSE == TP_GetParamItem(NAME_PARAMS(SecurityPassword),VAT_String,&securityPassword))
    {
        ShowSetupResult(2);
        return;
    }
    //TP_DbgSerialPrn("<%s>",securityPassword);
    while (TRUE)
    {
        memset(oldPassword,0x00,LENGTH_SECURITY_PASSWORD+1);
        memset(newPassword,0x00,LENGTH_SECURITY_PASSWORD+1);
        memset(confirmPassword,0x00,LENGTH_SECURITY_PASSWORD+1);

        displayInfo.strTitle = (uint8*)TEXT_SYSSETTING_INPUTOLDPASSWORD;
        if (0xFB != TP_PassWordInput(&displayInfo,&inputInfo,'*',oldPassword))
        {
            break;
        }
        if (0 != strcmp((const char *)oldPassword,(const char *)securityPassword) )
        {
            TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_OLDPWDERROR,2000);
            continue ;
        }
        displayInfo.strTitle = (uint8*)TEXT_SYSSETTING_INPUTNEWPASSWORD;
        if (0xFB != TP_PassWordInput(&displayInfo,&inputInfo,'*',newPassword))
        {
            break;
        }
        displayInfo.strTitle = (uint8*)TEXT_SYSSETTING_INPUTCFMPASSWORD;
        if (0xFB != TP_PassWordInput(&displayInfo,&inputInfo,'*',confirmPassword))
        {
            break;
        }
        if (0 != strcmp((const char*)newPassword,(const char*)confirmPassword))
        {
            TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_PWDNOTEQUAL,2000);
            continue;
        }
        if (TRUE == TP_SetParamItem(NAME_PARAMS(SecurityPassword),VAT_String,&newPassword))
        {
            ShowSetupResult(1);
            return;
        }
        else
        {
            ShowSetupResult(0);
            return;
        }

    }
}

static void RestoreFactorySetting()
{
    TP_DisplayInfo displayInfo =
    {
        (uint8*)TEXT_SYSSETTING_FACTORYSETTINGNOTE,
        ALIGN_LEFT,
        (uint8*)TEXT_SYSSETTING_YES,
        (uint8*)TEXT_SYSSETTING_NO,
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_CANCEL2,
        CFONT
    };
    TP_ScrCls();
    if(TP_PopupMessageWithButton(&displayInfo) == FALSE)
    {
        return;
    }
    if (checkSecurityPassword() == FALSE)
    {
        return;
    }
    TP_ScrCls();
    TP_PopupMessage((uchar*)TEXT_SYSSETTING_PLEASEWAIT);

    if (0 == TP_Set_AllParamsItem_Default())
    {
        TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_RESTORESUCCESS,3000);
    }
    else
    {
        TP_PopupMessageTimed((uchar*)TEXT_SYSSETTING_RESTOREFAIL,3000);
    }

    TP_PhoneSettingInitForPowerOn();
}


static void OwnNumberSetting()
{
    uint8 ownNumber[21] = {0};
    TP_DisplayInfo displayInfo =
    {
        (uint8*)TEXT_SYSSETTING_SETOWNNUMBER,
        ALIGN_LEFT,
        (uint8*)TEXT_SYSSETTING_CONFIRM,
        (uint8*)TEXT_SYSSETTING_RETURN,
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_CANCEL2,
        CFONT
    };
    TP_InputInfo inputInfo = {0};
    inputInfo.minLen = 1;
    inputInfo.maxLen = 20;
    inputInfo.showInputNumber = TRUE;
    if(FALSE == TP_GetParamItem(NAME_PARAMS(OwnNumber),VAT_String,ownNumber))
    {
        ShowSetupResult(2);
        return;
    }
    if (0xFB != TP_NumberInput(&displayInfo,&inputInfo,ownNumber))
    {
        return;
    }
    if (FALSE == TP_SetParamItem(NAME_PARAMS(OwnNumber),VAT_String,ownNumber))
    {
        ShowSetupResult(0);
        return;
    }
    ShowSetupResult(1);
}
static void ShowSoftwareVersion()
{
    char sversion[100];

    TP_DisplayInfo displayInfo =
    {
        (uint8*)TEXT_SYSSETTING_SOFTWAREVERSION,
        ALIGN_LEFT,
        (uint8*)TEXT_SYSSETTING_CONFIRM,
        (uint8*)TEXT_SYSSETTING_RETURN,
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_CANCEL2,
        CFONT
    };

    _snprintf(sversion, sizeof(sversion),
              "%s.%s.%s",
              TP_Data_APPInfo.productName,
              TP_Data_APPInfo.productTerminal,
              TP_Data_APPInfo.productVersion);

    TP_ShowTextFullScr(&displayInfo, "%s", (uint8*)sversion);
}

static void ShowIMEI()
{
    uint8 str[60];
    TP_DisplayInfo displayInfo =
    {
        (uint8*)TEXT_SYSSETTING_IMEINUMBER,
        ALIGN_MIDDLE,
        (uint8*)TEXT_SYSSETTING_CONFIRM,
        (uint8*)TEXT_SYSSETTING_RETURN,
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_CANCEL2,
        CFONT
    };

    TP_Kbflush();
    memset(str,0,sizeof(str));
    TP_GetModalIMEI(str, sizeof(str));
    TP_ShowTextFullScr(&displayInfo,str);
}

static void KeypadLockSwitch()
{
    int32 isLock = 0;
    uint8 *KeypadLockSwitchList[]=
    {
        (uint8*)TEXT_SYSSETTING_OFF,
        (uint8*)TEXT_SYSSETTING_ON
    };
    TP_DisplayInfo displayInfo =
    {
        (uint8*)TEXT_SYSSETTING_ONOFF,
        ALIGN_MIDDLE,
        (uint8*)TEXT_SYSSETTING_CONFIRM,
        (uint8*)TEXT_SYSSETTING_RETURN,
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_CANCEL2,
        CFONT
    };
    if (TP_GetParamItem(NAME_PARAMS(KeypadLock),VAT_Int32,&isLock) == TRUE)
    {
        isLock += 1;
        while (TRUE)
        {
            if (TRUE == TP_CreateListBoxInterface(TL_RADIOBOX,
                                                  &displayInfo,
                                                  KeypadLockSwitchList,
                                                  2,
                                                  isLock,
                                                  &isLock,
                                                  NULL))
            {
                isLock -= 1;
                if (TP_SetParamItem(NAME_PARAMS(KeypadLock),VAT_Int32,&isLock) == TRUE)
                {
                    ShowSetupResult(1);
                    if (isLock == 0)
                    {
                        keypadLockTimeout = 0;
                    }
                    else
                    {
                        if (TP_GetParamItem(NAME_PARAMS(KeypadLockTimeout),VAT_Int32,&keypadLockTimeout)==FALSE)
                        {
                            keypadLockTimeout = 0;
                        }
                    }
                    break;
                }
                else
                {
                    ShowSetupResult(0);
                    break;
                }
            }
            else
            {
                break;
            }
        }//end of while(TRUE)
    }
    else
    {
        ShowSetupResult(2);
    }
}

static void KeypadTimeoutSetting()
{
    int32 isLock = 0;
    int32 timeout = 0;
    uint8 chooseValue = 0;
    uint8 *KeypadtimeoutList[]=
    {
        (uint8*)TEXT_SYSSETTING_ONEMIN,
        (uint8*)TEXT_SYSSETTING_FIVEMIN,
        (uint8*)TEXT_SYSSETTING_TENMIN
    };
    TP_DisplayInfo displayInfo =
    {
        (uint8*)TEXT_SYSSETTING_TIMEOUT,
        ALIGN_MIDDLE,
        (uint8*)TEXT_SYSSETTING_CONFIRM,
        (uint8*)TEXT_SYSSETTING_RETURN,
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_CANCEL2,
        CFONT
    };
    TP_GetParamItem(NAME_PARAMS(KeypadLock),VAT_Int32,&isLock);
    if (TP_GetParamItem(NAME_PARAMS(KeypadLockTimeout),VAT_Int32,&timeout) == TRUE)
    {
        switch (timeout)
        {
        case 300000:
            chooseValue = 2;
            break;
        case 600000:
            chooseValue = 3;
            break;
        case 60000:
        default:
            chooseValue = 1;
            break;
        }
        while (TRUE)
        {
            if (TRUE == TP_CreateListBoxInterface(TL_RADIOBOX,
                                                  &displayInfo,
                                                  KeypadtimeoutList,
                                                  3,
                                                  chooseValue,
                                                  &chooseValue,
                                                  NULL))
            {
                switch (chooseValue)
                {
                case 1:
                    timeout = 60000;
                    break;
                case 2:
                    timeout = 300000;
                    break;
                case 3:
                    timeout = 600000;
                default:
                    break;
                }
                keypadLockTimeout = isLock>0? timeout : 0;
                if (TP_SetParamItem(NAME_PARAMS(KeypadLockTimeout),VAT_Int32,&timeout) == TRUE)
                {
                    ShowSetupResult(1);
                    break;
                }
                else
                {
                    ShowSetupResult(0);
                    break;
                }
            }
            else
            {
                break;
            }
        }//end of while(TRUE)
    }
    else
    {
        ShowSetupResult(2);
    }
}
/*********************************************************************************
* Global Functions
**********************************************************************************/

Boolean TP_GetParamItem(PARAMS_NAMES paramsId,ValueType valueType,void* value)
{
    Data data = {0};
    if (value == PNULL)
    {
        return FALSE;
    }
    if (TP_Get_ParamsItem_Val(paramsId,&data)<0)
    {
        return FALSE;
    }
    if (data.valueType != valueType)
    {
        return FALSE;
    }
    switch (data.valueType)
    {
    case VAT_Int32:
        *(int32 *)value = data.dBuffer.nVal;
        break;
    case VAT_String:
    case VAT_Stream:
        memcpy((uint8 *)value,data.dBuffer.cBuffer,strlen((char *)data.dBuffer.cBuffer));
        break;
    default:
        return FALSE;
    }
    return TRUE;
}


Boolean TP_SetParamItem(PARAMS_NAMES paramsId,ValueType valueType,void* value)
{
    Data data = {0};

    if (value == PNULL)
    {
        return FALSE;
    }
    switch (valueType)
    {
    case VAT_Int32 :
        if (TP_Format_Data(&data,VAT_Int32,sizeof(int32),value) < 0)
        {
            TP_DbgSerialPrn("<Fomat fail!>");
            return FALSE;
        }
        break;
    case VAT_String :
    case VAT_Stream :
        if (TP_Format_Data(&data,valueType,
                           min(sizeof(data.dBuffer.cBuffer)-1,strlen((char *)(uint8 *)value)),
                           value) <0)
        {
            TP_DbgSerialPrn("<Format fail2!>");
            return FALSE;
        }
        break;
    default:
        TP_DbgSerialPrn("<defaul fail>");
        return FALSE;
    }
    if (TP_Set_ParamsItem_Val(paramsId,&data)<0)
    {
        TP_DbgSerialPrn("<Set fail!>");
        return FALSE;
    }
    return TRUE;
}
uint32 TP_GetKeypadTimeout()
{
    return keypadLockTimeout;
}
Boolean TP_UnlockKeypad()
{
    uint8 result = 0;
    uint8 confirmKeyList[2];
    uint8 cancelKeyList[2];
    uint8 newPassword[LENGTH_SECURITY_PASSWORD+1] = {0};
    char buf[5] = {0};

    confirmKeyList[0] = KEY_SYSSETTING_CONFIRM1;
    confirmKeyList[1] = KEY_SYSSETTING_CONFIRM2;
    cancelKeyList[0]  = KEY_SYSSETTING_CANCEL1;
    cancelKeyList[1]  = KEY_SYSSETTING_CANCEL2;

    TP_GetParamItem(NAME_PARAMS(SecurityPassword),VAT_String,&newPassword);

    TP_ScrRestore(0);
    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(20,20,90,37);
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect(21,21,89,37);
    TP_SetDisplayArea(22,22,87,36);
    TP_ScrGotoxyEx(22,22);

    TP_ShowInputNum(0,0,0);
    TP_SetInputModePosition(128,64);
    TP_SetInputModeControlEx( KEY_SYSSETTING_CONFIRM1, KEY_SYSSETTING_CONFIRM2,
                              KEY_SYSSETTING_CANCEL1, TP_KEY_CLEAR, KEY_SYSSETTING_IMCHANGEKEY);
    TP_SetInputModeControlList(confirmKeyList, 2,cancelKeyList,2);
    result = TP_GetPasswordTimed(buf,4,4,0xD401,'*',5000);
    if (result == 0xF9 || result == 0xFB)
    {
        if (strlen((char*)newPassword) == strlen(buf)
            && strcmp((const char *)buf,newPassword) == 0 )
        {
            TP_ScrRestore(1);
            return TRUE;
        }
    }
    TP_ScrRestore(1);
    return FALSE;
}

void TP_PhoneSetting()
{
    uint8 state = 0;
    uint8 captionNum = 0;
    uint8 chooseValue = MENU_SYSSETTING_VOICESETTING;
    TP_DisplayInfo displayInfo =
    {
        (uint8*)TEXT_SYSSETTING,
        ALIGN_MIDDLE,
        (uint8*)TEXT_SYSSETTING_CONFIRM,
        (uint8*)TEXT_SYSSETTING_RETURN,
        KEY_SYSSETTING_CONFIRM1,
        KEY_SYSSETTING_CANCEL1,
        KEY_SYSSETTING_CONFIRM2,
        KEY_SYSSETTING_CANCEL2,
        CFONT
    };
    while (state != 0xFF)
    {
        switch (state)
        {
        case 0://first level list
            displayInfo.strTitle = (uint8*)TEXT_SYSSETTING;
            captionNum = sizeof(SysSettingList)/sizeof(uint8*);
            if (TRUE == TP_CreateListBoxInterface(TL_LIST,
                                                  &displayInfo,
                                                  SysSettingList,
                                                  captionNum,
                                                  chooseValue,
                                                  &chooseValue,
                                                  NULL))
            {
                switch (chooseValue)
                {
                case MENU_SYSSETTING_VOICESETTING:
                    state = 1;
                    chooseValue = MENU_SYSSETTING_RING;
                    break;
                case MENU_SYSSETTING_DISPLAYSETTING:
                    state = 2;
                    chooseValue = MENU_SYSSETTING_BACKLIGHT;
                    break;
                case MENU_SYSSETTING_SECURITYSETTING:
                    if (checkSecurityPassword() == TRUE)
                    {
                        state = 3;
                        chooseValue = MENU_SYSSETTING_PIN;
                    }
                    break;
                case MENU_SYSSETTING_PHONEINFO:
                    state = 4;
                    chooseValue = MENU_SYSSETTING_PHONENO;
                    break;
                case MENU_SYSSETTING_KEYPADLOCK:
                    state = 6;
                    chooseValue = MENU_SYSSETTING_ONOFF;
                    break;
                }
            }
            else
            {
                state =  0xFF;
            }
            break;
        case 1://Second level list : Voice setting
            captionNum = sizeof(VoiceSettingList)/sizeof(uint8*);
            displayInfo.strTitle = (uint8*)TEXT_SYSSETTING_VOICESETTING;
            if (TRUE == TP_CreateListBoxInterface(TL_LIST,
                                                  &displayInfo,
                                                  VoiceSettingList,
                                                  captionNum,
                                                  chooseValue,
                                                  &chooseValue,
                                                  NULL))
            {
                switch (chooseValue)
                {
                case MENU_SYSSETTING_RING:
                    RingSetting();
                    break;
                case MENU_SYSSETTING_VOLUMN:
                    state = 5 ;
                    chooseValue = MENU_SYSSETTING_KEYPADVOLUMN;
                    break;
                }
            }
            else
            {
                state = 0;
                chooseValue = MENU_SYSSETTING_VOICESETTING;
            }
            break;
        case 2: //second level list: Display setting
            captionNum = sizeof(DisplaySettingList)/sizeof(uint8*);
            displayInfo.strTitle = (uint8*)TEXT_SYSSETTING_DISPLAYSETTING;
            if (TRUE == TP_CreateListBoxInterface(TL_LIST,
                                                  &displayInfo,
                                                  DisplaySettingList,
                                                  captionNum,
                                                  chooseValue,
                                                  &chooseValue,
                                                  NULL))
            {
                switch (chooseValue)
                {
                case MENU_SYSSETTING_BACKLIGHT:
                    BacklightSetting();
                    break;
                case MENU_SYSSETTING_CONTRAST:
                    ContrastSetting();
                    break;
                case MENU_SYSSETTING_TIME:
                    TimeSetting();
                    break;
                }
            }
            else
            {
                state = 0;
                chooseValue = MENU_SYSSETTING_DISPLAYSETTING;
            }
            break;
        case 3://second level list: Security setting
            captionNum = sizeof(SecuritySettingList)/sizeof(uint8*);
            displayInfo.strTitle = (uint8*)TEXT_SYSSETTING_SECURITYSETTING;
            if (TRUE == TP_CreateListBoxInterface(TL_LIST,
                                                  &displayInfo,
                                                  SecuritySettingList,
                                                  captionNum,
                                                  chooseValue,
                                                  &chooseValue,
                                                  NULL))
            {
                switch (chooseValue)
                {
                case MENU_SYSSETTING_PIN:
                    state = 7;
                    chooseValue = MENU_SYSSETTING_PINON;
                    break;
                case MENU_SYSSETTING_CHANGEPASSWORD:
                    ChangeSecurityPassword();
                    break;
                case MENU_SYSSETTING_RESTORESETTING:
                    RestoreFactorySetting();
                    break;
                }
            }
            else
            {
                state = 0;
                chooseValue = MENU_SYSSETTING_SECURITYSETTING;
            }
            break;
        case 4://second level list: Phone information
            captionNum = sizeof(PhoneInfoList)/sizeof(uint8*);
            displayInfo.strTitle = (uint8*)TEXT_SYSSETTING_PHONEINFO;
            if (TRUE == TP_CreateListBoxInterface(TL_LIST,
                                                  &displayInfo,
                                                  PhoneInfoList,
                                                  captionNum,
                                                  chooseValue,
                                                  &chooseValue,
                                                  NULL))
            {
                switch (chooseValue)
                {
                case MENU_SYSSETTING_PHONENO:
                    OwnNumberSetting();
                    break;
                case MENU_SYSSETTING_SOFTVERSION:
                    ShowSoftwareVersion();
                    break;
                case MENU_SYSSETTING_IMEI:
                    ShowIMEI();
                    break;
                }
            }
            else
            {
                state = 0;
                chooseValue = MENU_SYSSETTING_PHONEINFO;
            }
            break;
        case 6://second level list: Keypad lock
            captionNum = sizeof(KeypadLockList)/sizeof(uint8*);
            displayInfo.strTitle = (uint8*)TEXT_SYSSETTING_KEYPADLOCK;
            if (TRUE == TP_CreateListBoxInterface(TL_LIST,
                                      &displayInfo,
                                      KeypadLockList,
                                      captionNum,
                                      chooseValue,
                                      &chooseValue,
                                      NULL))
            {
                switch (chooseValue)
                {
                case MENU_SYSSETTING_ONOFF:
                    KeypadLockSwitch();
                    break;
                case MENU_SYSSETTING_TIMEOUT:
                    KeypadTimeoutSetting();
                    break;
                }
            }
            else
            {
                state = 0;
                chooseValue = chooseValue = MENU_SYSSETTING_KEYPADLOCK;
            }
            break;
        case 5://third level list :volumn setting
            captionNum = sizeof(VolumnSettingList)/sizeof(uint8*);
            displayInfo.strTitle = (uint8*)TEXT_SYSSETTING_VOLUMN;
            if ( TRUE == TP_CreateListBoxInterface(TL_LIST,
                                                   &displayInfo,
                                                   VolumnSettingList,
                                                   captionNum,
                                                   chooseValue,
                                                   &chooseValue,
                                                   NULL))
            {
                VolumnSetting(chooseValue);
            }
            else
            {
                state = 1;
                chooseValue = MENU_SYSSETTING_VOLUMN;
            }
            break;
        case 7: //third level list : PIN setting
            captionNum = sizeof(PinSettingList)/sizeof(uint8*);
            displayInfo.strTitle = (uint8*)TEXT_SYSSETTING_PIN;
            if (TRUE == TP_CreateListBoxInterface(TL_LIST,
                                                  &displayInfo,
                                                  PinSettingList,
                                                  captionNum,
                                                  chooseValue,
                                                  &chooseValue,
                                                  NULL))
            {
                PinSetting(chooseValue);
            }
            else
            {
                state = 3;
                chooseValue = MENU_SYSSETTING_PIN;
            }
        }// end switch (state)

    }// end while (state != 0xFF)
}

//oÃouÔu¿ª»uoÃ»§³õÊ¼»¯ÖÐ£¬eèÖÃoÃ»§eè¶¨µÄ²ÎÊý
void TP_PhoneSettingInitForPowerOn()
{
    int32 value = 0;
    if (TP_GetParamItem(NAME_PARAMS(KeypadVolumn),VAT_Int32,&value) == TRUE)
    {
        TP_SetVolume(KEYPAD_VOLUMN, value);
    }
    else
    {
        TP_SetVolume(KEYPAD_VOLUMN, VOLUME_0);
    }
    if (TP_GetParamItem(NAME_PARAMS(RingVolumn),VAT_Int32,&value) == TRUE)
    {
        TP_SetVolume(RING_VOLUMN, value);
    }
    else
    {
        TP_SetVolume(RING_VOLUMN, VOLUME_3);
    }
    if (TP_GetParamItem(NAME_PARAMS(SpeakerVolumn),VAT_Int32,&value) == TRUE)
    {
        TP_SetVolume(SPEECH_VOLUMN, value);
    }
    else
    {
        TP_SetVolume(SPEECH_VOLUMN, VOLUME_3);
    }
    if (TP_GetParamItem(NAME_PARAMS(MicrophoneVolumn),VAT_Int32,&value) == TRUE)
    {
        TP_SetVolume(MICROPHONE_VOLUMN, value);
    }
    else
    {
        TP_SetVolume(MICROPHONE_VOLUMN, VOLUME_3);
    }
    if (TP_GetParamItem(NAME_PARAMS(Backlight),VAT_Int32,&value) == TRUE)
    {
        TP_ScrBackLight(value);
        TP_kbLight(value);
    }
    else
    {
        TP_ScrBackLight(1);
        TP_kbLight(1);
    }
    if (TP_GetParamItem(NAME_PARAMS(Contrast),VAT_Int32,&value) == TRUE)
    {
        TP_ScrGray(value);
    }

    keypadLockTimeout = 0;
    if (TP_GetParamItem(NAME_PARAMS(KeypadLock),VAT_Int32,&value)== TRUE)
    {
        if (value == 1)
        {
            if (TP_GetParamItem(NAME_PARAMS(KeypadLockTimeout),VAT_Int32,&keypadLockTimeout)==FALSE)
            {
                keypadLockTimeout = 0;
            }
        }
    }
}

