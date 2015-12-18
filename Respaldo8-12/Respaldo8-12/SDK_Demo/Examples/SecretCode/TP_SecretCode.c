#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeypad.h>
#include <TPLCD.h>
#include <TPMobileCard.h>

#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"
#include "TP_Secretcode.h"

static void showVersionInfo(void)
{
    TP_Date compileDate;
    char    versionInfo[256];

    TP_Kbflush();
    TP_ScrCls();
    TP_ScrFontSet(ASCII);
    TP_SetDisplayArea(0, 0, 127, 63);
    TP_ScrSpaceSet(1, 1);

    TP_ScrGotoxy(0, 0);
    TP_GetProductInfo(versionInfo);
    TP_LcdPrintf((uint8*)"Product:\n%s", versionInfo);

    TP_ScrGotoxy(0, 4);
    TP_GetSDKVersion(versionInfo);
    TP_LcdPrintf((uint8*)"SDK:\n%s", versionInfo);

    while (TRUE)
    {
        TP_NotifyCheck();
        if (TP_GetKey() != TP_KEY_NONE)
        {
            TP_Kbflush();
            return;
        }
    }

    TP_ScrFontSet(CFONT);
    TP_ScrSpaceSet(0, 0);
}


static void TP_ShowRSSI()
{
    uint8 RSSI = 0;
    uint8 displayBuf[16];
    TP_Kbflush();
    TP_ScrCls();
    while (TRUE)
    {
        RSSI = TP_GetRSSIValue();
        sprintf((char*)displayBuf,"RSSI:%d",RSSI);
        TP_PopupMessage((uchar*)displayBuf);
        TP_NotifyCheck();
        if (TP_Kbhit())
        {
            TP_Kbflush();
            return;
        }
    }
}

static void TP_ShowIMEI()
{
    uint8 IMEI[16];
    uint8 displayBuf[32];
    memset(IMEI,0x00,sizeof(IMEI));
    memset(displayBuf,0x00,sizeof(displayBuf));
    TP_ScrCls();
    if(TP_GetModalIMEI(IMEI,15) == 0)
    {
        TP_PopupMessageTimed((uchar*)TEXT_SECRETCODE_ERROR,3000);
        return;
    }
    sprintf((char*)displayBuf,"IMEI:%s",IMEI);
    TP_PopupMessage((uchar*)displayBuf);
    while (TRUE)
    {
        TP_NotifyCheck();
        if (TP_Kbhit())
        {
            TP_Kbflush();
            return;
        }
    }
}

static void TP_ShowIMSI()
{
    uint8 IMSI[16];
    uint8 displayBuf[32];
    memset(IMSI,0x00,sizeof(IMSI));
    memset(displayBuf,0x00,sizeof(displayBuf));
    TP_ScrCls();
    if(TP_GetMobileCardIMSI(IMSI,15) == 0)
    {
        TP_PopupMessageTimed((uchar*)TEXT_SECRETCODE_ERROR,3000);
        return;
    }
    sprintf((char*)displayBuf,"IMSI:%s",IMSI);
    TP_PopupMessage((uchar*)displayBuf);
    while (TRUE)
    {
        TP_NotifyCheck();
        if (TP_Kbhit())
        {
            TP_Kbflush();
            return;
        }
    }
}

static void TP_ShowTime()
{
    uint8 displayBuf[64];
    TP_DateTime dateTime;
    TP_ScrCls();
    memset(&dateTime,0x00,sizeof(TP_DateTime));
    memset(displayBuf,0x00,sizeof(displayBuf));

    while (TRUE)
    {
        TP_GetDateTime(&dateTime);
        sprintf((char*)displayBuf,"    %04d-%02d-%02d \n     %02d:%02d:%02d",
                dateTime.Date.Year,
                dateTime.Date.Month,
                dateTime.Date.Day,
                dateTime.Time.Hours,
                dateTime.Time.Minutes,
                dateTime.Time.Seconds);
        TP_PopupMessage((uchar*)displayBuf);
        TP_NotifyCheck();
        if (TP_Kbhit())
        {
            TP_Kbflush();
            return;
        }
    }
}

typedef struct
{
    uint8 KeyCode;
    uint8 KeyDisplay[20];
    uint8 KeyBlank[20];
    uint8 IsPress;
}TP_KeyPadItem;

#if   defined (TPS320)
static TP_KeyPadItem KeyPadList[] =
{
    {TP_KEY_F1,     "F1 ","   ",0},
    {TP_KEY_PRINT,  "PRINT ","      ",0},
    {TP_KEY_FEED,   "FEED ","     ",0},
    {TP_KEY_CONFIRM,"CONFIRM ","        ",0},
    {TP_KEY_CANCEL, "CANCEL ","       ",0},
    {TP_KEY_1,      "1  ","   ",0},
    {TP_KEY_2,      "2  ","   ",0},
    {TP_KEY_3,      "3  ","   ",0},
    {TP_KEY_4,      "4  ","   ",0},
    {TP_KEY_5,      "5  ","   ",0},

    {TP_KEY_6,      "6  ","   ",0},
    {TP_KEY_7,      "7  ","   ",0},
    {TP_KEY_8,      "8  ","   ",0},
    {TP_KEY_9,      "9  ","   ",0},
    {TP_KEY_0,      "0  ","   ",0},
    {TP_KEY_STAR,   "*  ","   ",0},
    {TP_KEY_POUND,  "#  ","   ",0},
    {TP_KEY_UP,     "UP ","   ",0},
    {TP_KEY_DOWN,   "DOWN ","     ",0},
    {TP_KEY_LEFT,   "LEFT ","     ",0},

    {TP_KEY_RIGHT,  "RIGHT ","      ",0},
    {TP_KEY_CLEAR,  "CLEAR ","      ",0},
    {TP_KEY_OK,     "OK ","   ",0},
    {TP_KEY_HANDFREE,     "HANDLEFREE","          ",0},

};

#endif

#ifdef TPS300
static TP_KeyPadItem KeyPadList[] =
{
    {TP_KEY_POWER,  "POWER ",  "      ",  0},
    {TP_KEY_FUNC,   "FUNC ",   "     ",   0},
    {TP_KEY_FEED,   "FEED ",   "     ",   0},
    {TP_KEY_MENU,   "MENU ",   "   ",     0},
    {TP_KEY_CANCEL, "CANCEL ", "       ", 0},

    {TP_KEY_1,      "1  ","   ",0},
    {TP_KEY_2,      "2  ","   ",0},
    {TP_KEY_3,      "3  ","   ",0},
    {TP_KEY_4,      "4  ","   ",0},
    {TP_KEY_5,      "5  ","   ",0},
    {TP_KEY_6,      "6  ","   ",0},
    {TP_KEY_7,      "7  ","   ",0},
    {TP_KEY_8,      "8  ","   ",0},
    {TP_KEY_9,      "9  ","   ",0},
    {TP_KEY_0,      "0  ","   ",0},
    {TP_KEY_STAR,   "*  ","   ",0},

    {TP_KEY_UP,     "UP ","   ",0},
    {TP_KEY_DOWN,   "DOWN ","     ",0},

    {TP_KEY_CLEAR,  "CLEAR ","      ",0},
    {TP_KEY_OK,     "OK ","   ",0},
};

#endif

#if defined(TP_PUBLIC_G)
static TP_KeyPadItem KeyPadList[] =
{
    {TP_KEY_POWER,  "POWER ",  "      ",  0},
    {TP_KEY_F1,     "F1 ","   ",0},
    {TP_KEY_F2,     "F2 ","   ",0},
    {TP_KEY_F3,     "F3 ","   ",0},
    {TP_KEY_F4,     "F4 ","   ",0},
    {TP_KEY_F5,     "F5 ","   ",0},
    {TP_KEY_CONFIRM,"CONFIRM ","        ",0},
    {TP_KEY_CANCEL, "CANCEL ","       ",0},
    {TP_KEY_1,      "1  ","   ",0},
    {TP_KEY_2,      "2  ","   ",0},
    {TP_KEY_3,      "3  ","   ",0},
    {TP_KEY_4,      "4  ","   ",0},
    {TP_KEY_5,      "5  ","   ",0},

    {TP_KEY_6,      "6  ","   ",0},
    {TP_KEY_7,      "7  ","   ",0},
    {TP_KEY_8,      "8  ","   ",0},
    {TP_KEY_9,      "9  ","   ",0},
    {TP_KEY_0,      "0  ","   ",0},
    {TP_KEY_STAR,   "*  ","   ",0},
    {TP_KEY_POUND,  "#  ","   ",0},
    {TP_KEY_UP,     "UP ","   ",0},
    {TP_KEY_DOWN,   "DOWN ","     ",0},
    {TP_KEY_LEFT,   "LEFT ","     ",0},
    {TP_KEY_RIGHT,  "RIGHT ","      ",0},
    {TP_KEY_HANDFREE,     "HANDFREE","          ",0}

};
#endif


static void TP_KeypadTest()
{
    uint8 Index;
    uint8 Lastpos;
    uint8 TmpBuffer[256];
    uint8 Canreturn;
    uint8 maxkeycount = 0;
    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(ASCII);
    TP_SetDisplayArea(0,0,127,63);
    TP_ScrGotoxyEx(0,0);
    TP_ScrClsDisplayArea();
    maxkeycount = sizeof(KeyPadList)/sizeof(TP_KeyPadItem);
    memset(TmpBuffer,0,sizeof(TmpBuffer));
    Lastpos=0;

    for (Index =0;Index< maxkeycount;Index++)
    {
        KeyPadList[Index].IsPress =0;
    }
    for (Index =0;Index< maxkeycount;Index++)
    {
        if ((strlen((char *)TmpBuffer) - Lastpos+strlen((char *)KeyPadList[Index].KeyBlank)) > TP_GetMaxCharInRow())
        {
            strcat((char *)TmpBuffer,"\n");
            Lastpos = strlen((char *)TmpBuffer);
        }
        if (KeyPadList[Index].IsPress ==0)
        {
            strcat((char *)TmpBuffer,(char *)KeyPadList[Index].KeyDisplay);
        }
        else
        {
            strcat((char *)TmpBuffer,(char *)KeyPadList[Index].KeyBlank);
        }
    }

    TP_LcdPrintf(TmpBuffer);
    while(1)
    {
        TP_NotifyCheck();
        if (TP_Kbhit())
        {
            uint8 KeyCode = TP_GetKey();


            TP_ScrClsDisplayArea();

            memset(TmpBuffer,0,sizeof(TmpBuffer));
            Lastpos=0;
            for (Index =0;Index< maxkeycount;Index++)
            {
                if (KeyCode == KeyPadList[Index].KeyCode)
                {
                    KeyPadList[Index].IsPress =1;
                }
                if ((strlen((char *)TmpBuffer) - Lastpos+strlen((char *)KeyPadList[Index].KeyBlank)) > TP_GetMaxCharInRow())
                {
                    strcat((char *)TmpBuffer,"\n");
                    Lastpos = strlen((char *)TmpBuffer);
                }
                if (KeyPadList[Index].IsPress ==0)
                {
                    strcat((char *)TmpBuffer,(char *)KeyPadList[Index].KeyDisplay);
                }
                else
                {
                    strcat((char *)TmpBuffer,(char *)KeyPadList[Index].KeyBlank);
                }
            }

            TP_LcdPrintf(TmpBuffer);
            Canreturn = 1;
            for (Index =0;Index< maxkeycount;Index++)
            {
                if (KeyPadList[Index].IsPress ==0)
                {
                    Canreturn =0;
                    break;
                }
            }

            if (Canreturn)
            {
                //TP_ScrBackLight(0);
                return;
            }

        }
    }
}

Boolean TP_SecretCodeParse(char* str)
{
    return TP_FactoryMode(str);
}
