/******************************************************************************
* Include Headers
*******************************************************************************/

#include <TPTypedef.h>
#include <TPBase.h>
#include <TPLCD.h>
#include <TPKeypad.h>
#include <TPSerial.h>
#include <TPNetwork.h>
#include <TPSS.h>
#include "TP_DemoDefine.h"

void get_current_clir_status(void)
{
    TP_CLIRStatusE status;
    uint8 keyCode;

    TP_LcdFreeze();
    TP_ScrCls();
    TP_ScrGotoxyEx(1, 10);
    TP_LcdPrintf((uint8*)"%s", "Getting CLIR Status");
    TP_ScrGotoxyEx(1, 30);
    TP_LcdPrintf((uint8*)"%s", "Please wait ......");
    TP_ScrUpdate();

    status = TP_SSCLIRGetStatus();

    TP_LcdFreeze();
    TP_ScrCls();
    TP_ScrGotoxyEx(1, 10);
    TP_LcdPrintf((uint8*)"%s", "Current CLIR Status :");

    TP_ScrGotoxyEx(1, 25);
    switch(status)
    {
    case CLIR_STATUS_NOT_PROVISION:
        TP_LcdPrintf((uint8*)"%5s", "NOT PROVISION");
        break;

    case CLIR_STATUS_PERMANENT:
        TP_LcdPrintf((uint8*)"%5s", "PERMANENT");
        break;

    case CLIR_STATUS_UNKNOW:
        TP_LcdPrintf((uint8*)"%5s", "UNKNOW");
        break;
    case CLIR_STATUS_TEMP_RESTRICT:
        TP_LcdPrintf((uint8*)"%5s", "TEMP RESTRICT");
        break;
    case CLIR_STATUS_TEMP_ALLOW:
        TP_LcdPrintf((uint8*)"%5s", "TEMP ALLOW");
        break;
    default:
        break;
    }

    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf((uint8*)"%21s", "Quit");

    TP_ScrUpdate();
    TP_Kbflush();
    while (TRUE)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                TP_Kbflush();
                return;
            }
        }
    }
}

void doCLIRTest(uint8 keyCode)
{
    TP_CLIRModeE mode;
    uint8           prompt[20];
    Boolean       ret;

    memset(prompt, 0, sizeof(prompt));

    if (keyCode == TP_KEY_1)
    {
        mode = CLIRMODE_AUTO;
        sprintf((char*)prompt, "%s", "Auto");
    }
    else if (keyCode == TP_KEY_2)
    {
        mode = CLIRMODE_INVOCATION;
        sprintf((char*)prompt, "%s", "Enable");
    }
    else if (keyCode == TP_KEY_3)
    {
        mode = CLIRMODE_SUPPRESSION;
        sprintf((char*)prompt, "%s", "Disable");
    }
    else if  (keyCode == TP_KEY_4)
    {
        get_current_clir_status();
        return;
    }
    else
    {
        return;
    }

    TP_LcdFreeze();
    TP_ScrCls();
    TP_ScrGotoxyEx(1, 10);
    TP_LcdPrintf((uint8*)"%s", "Setting CLIR Mode");
    TP_ScrGotoxyEx(1, 25);
    TP_LcdPrintf((uint8*)"%5s", prompt);
    TP_ScrGotoxyEx(1, 40);
    TP_LcdPrintf((uint8*)"%s", "Please wait ......");
    TP_ScrUpdate();

    TP_TimerSet(0, 2000);
    while(TP_TimerCheck(0)!=0);

    ret = TP_SSCLIRSetMode(mode);

    TP_LcdFreeze();
    TP_ScrCls();

    TP_ScrGotoxyEx(10, 25);
    switch(ret)
    {
    case 0x00:
        TP_LcdPrintf((uint8*)"%s", "Success!");
        break;
    case 0xFE:
        TP_LcdPrintf((uint8*)"%s", "Network Error!");
        break;
    case 0xFF:
        TP_LcdPrintf((uint8*)"%5s", "Param Error!");
        break;
    default:
        break;
    }

    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf((uint8*)"%21s", "Quit");

    TP_ScrUpdate();
    TP_Kbflush();
    while (TRUE)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                TP_Kbflush();
                return;
            }
        }
    }
}

void CLIRTest(void)
{
    uint8 keyCode;

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);

    TP_Kbflush();
    while (TRUE)
    {
        TP_LcdFreeze();
        TP_ScrCls();
        TP_ScrGotoxyEx(1, 1);
        TP_LcdPrintf((uint8*)"%s\x00", "1 Set CLIR Auto\x00");
        TP_ScrGotoxyEx(1, 13);
        TP_LcdPrintf((uint8*)"%s\x00", "2 Set CLIR Enable\x00");
        TP_ScrGotoxyEx(1, 25);
        TP_LcdPrintf((uint8*)"%s\x00", "3 Set CLIR Disable\x00");
        TP_ScrGotoxyEx(1, 38);
        TP_LcdPrintf((uint8*)"%s\x00", "4 Get Current Mode\x00");
        TP_ScrGotoxyEx(1, 50);
        TP_LcdPrintf((uint8*)"%21s", "Quit");
        TP_ScrUpdate();

        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                TP_Kbflush();
                return;
            }else{
                doCLIRTest(keyCode);
                TP_Kbflush();
            }
        }

    }
}



