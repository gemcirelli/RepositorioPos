/******************************************************************************
* Include Headers
*******************************************************************************/
#include <TPTypedef.h>
#include <TPBase.h>
#include <TPLCD.h>
#include <TPKeypad.h>
#include <TPSerial.h>
#include <TPNetwork.h>
#include <TPMobileCard.h>

#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"

static void doOperatorSelectManual(void)
{
    const char** pList;
    int32 selectIndex;
    int32 num;
    uint8 result;

    TP_DisplayInfo displayInfo=
    {
        (uint8 *)"Select Operator",
        ALIGN_MIDDLE,
        (uint8 *)"Select",
        (uint8 *)"Cancel",
        KEY_DEMO_CONFIRM,
        KEY_DEMO_CANCEL,
        TP_KEY_NONE,
        TP_KEY_NONE,
        CFONT
    };

    TP_PopupMessage("Enquery operators...");
    pList = TP_NetworkCreateOperatorsList(&selectIndex);

    if (pList == PNULL)
    {
        TP_PopupMessageTimed("No PLMN", 3 * 1000);

        return;
    }

    num = 0;
    while (pList[num] != PNULL)
    {
        num++;
    }

    if (TP_CreateListBoxInterface(TL_RADIOBOX,
                                  &displayInfo,
                                  pList,
                                  num,
                                  selectIndex + 1,
                                  &selectIndex,
                                  PNULL) == TRUE)
    {
        TP_PopupMessage("Selecting...");

        TP_DbgSerialPrn("SelectIndex:[%d]", selectIndex);

        result = TP_NetworkSelectOperator(OP_SELECT_MANUL, pList, selectIndex - 1);
        if (result != 0x00)
        {
            TP_PopupMessageTimed("Operation failed", 3000);
        }
        else
        {
            TP_PopupMessageTimed("Operation succeed", 3000);
        }
    }

    TP_NetworkDestroyOperatorsList(pList);
}


void doGetLacCellID(void)
{
    uint32         lac, cellid;
    uint8 keyCode;

     TP_Kbflush();
    while (TRUE)
    {

        lac = TP_NetworkGetLAC();
        cellid = TP_NetworkGetCellID();

        TP_LcdFreeze();
        TP_ScrCls();
        TP_ScrGotoxyEx(1, 1);
        TP_LcdPrintf((uint8*)"LAC : %x", lac);
        TP_ScrGotoxyEx(1, 13);
        TP_LcdPrintf((uint8*)"Cell ID : %x", cellid);
            TP_ScrClrRect(1, 50, 127, 63);
        TP_ScrGotoxyEx(1, 50);
        TP_LcdPrintf((uint8*)"%21s", "Quit");

        TP_ScrUpdate();

        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                keyCode = TP_GetKey();
                if (keyCode == TP_KEY_CANCEL)
                {
                    TP_Kbflush();
                    return;
                }
            }
        }
    }
}

void doGetNetworkStatus(void)
{
    uint8 keyCode;
    TP_ServiceStatusE svc;
    Boolean        isRoam;

    TP_Kbflush();
    while (TRUE)
    {
        svc = TP_NetworkGetStatus();
        isRoam = TP_NetworkIsRoaming();

        TP_LcdFreeze();
        TP_ScrCls();

        TP_ScrGotoxyEx(1, 1);
        TP_LcdPrintf((uint8*)"Network Status :");
        TP_ScrGotoxyEx(1, 13);
        if (svc == SRVCSTATUS_NOT_REGISTER)
        {
            TP_LcdPrintf((uint8*)"%5s", "Not Register");
        }
        else if(svc == SRVCSTATUS_REGISTERED)
        {
            TP_LcdPrintf((uint8*)"%5s", "Registed");
        }
        else if(svc == SRVCSTATUS_REGISTERING)
        {
            TP_LcdPrintf((uint8*)"%5s", "Registering");
        }
        else if (svc == SRVCSTATUS_NETWORK_REJECTED)
        {
            TP_LcdPrintf((uint8*)"%5s", "Network Rejected");
        }
        else if (svc == SRVCSTATUS_UNKNOWN)
        {
            TP_LcdPrintf((uint8*)"%5s", "Unknow");
        }
        else
        {
            TP_LcdPrintf((uint8*)"%5s", "Error!!");
        }

        TP_ScrGotoxyEx(1, 25);
        if (isRoam)
        {
            TP_LcdPrintf((uint8*)"%5s", "Roaming");
        }
        else
        {
            TP_LcdPrintf((uint8*)"%5s", "Not Roaming");
        }
        TP_ScrClrRect(1, 50, 127, 63);
        TP_ScrGotoxyEx(1, 50);
        TP_LcdPrintf((uint8*)"%21s", "Quit");

        TP_ScrUpdate();

        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                keyCode = TP_GetKey();
                if (keyCode == TP_KEY_CANCEL)
                {
                    TP_Kbflush();
                    return;
                }
            }
        }
    }
}


void doNetworkStatusTest(uint8 keyCode)
{
    uint16         nInfo;

    uint8          data[50];
    uint16         len;
    char           LongName[50];
    char           ShortName[50];
    char           mcc[5], mnc[5];


    nInfo = 0;
    if (keyCode == TP_KEY_1)
    {
        doGetLacCellID();
        return;
    }
    else if (keyCode == TP_KEY_2)
    {
        memset(LongName, 0, sizeof(LongName));
        TP_NetworkGetOperatorName(FALSE, LongName, sizeof(LongName));

        memset(ShortName, 0, sizeof(ShortName));
        TP_NetworkGetOperatorName(TRUE, ShortName, sizeof(ShortName));

        memset(mcc, 0, sizeof(mcc));
        memset(mnc, 0, sizeof(mnc));
        {
            uint8 result;

            result = TP_NetworkGetOperator(mcc, sizeof(mcc), mnc, sizeof(mnc));
        }

        nInfo = 2;
    }
    else if (keyCode == TP_KEY_3)
    {
        doGetNetworkStatus();
        return;
    }
    else if (keyCode == TP_KEY_4)
    {
        memset(LongName, 0, sizeof(LongName));
        memset(mcc, 0, sizeof(mcc));
        memset(mnc, 0, sizeof(mnc));
        TP_GetMobileCardOperator(mcc, sizeof(mcc), mnc, sizeof(mnc));
        TP_GetMobileCardOperatorName(LongName, sizeof(LongName));

        nInfo = 4;
    }
    else
    {
        return;
    }

    TP_LcdFreeze();
    TP_ScrCls();

    // display
    switch(nInfo)
    {
    case 1:
        break;
    case 2:
        TP_ScrGotoxyEx(1, 1);
        TP_LcdPrintf((uint8*)"Long : %s", LongName);
        TP_ScrGotoxyEx(1, 25);
        TP_LcdPrintf((uint8*)"Short : %s", ShortName);
        TP_ScrGotoxyEx(1, 37);
        TP_LcdPrintf((uint8*)"MCC:%s", mcc);
        TP_ScrGotoxyEx(64, 37);
        TP_LcdPrintf((uint8*)"MNC : %s", mnc);
        break;
    case 3:
        break;
    case 4:
        TP_ScrGotoxyEx(1, 1);
        TP_LcdPrintf((uint8*)"%s", "Get Sim Operator");
        TP_ScrGotoxyEx(1, 25);
        TP_LcdPrintf((uint8*)"name : %s", LongName);
        TP_ScrGotoxyEx(1, 38);
        TP_LcdPrintf((uint8*)"MCC:%s", mcc);
        TP_ScrGotoxyEx(64, 38);
        TP_LcdPrintf((uint8*)"MNC : %s", mnc);
        break;

    default:
        break;
    }

    TP_ScrClrRect(1, 50, 127, 63);
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf((uint8*)"%21s", "Quit");

    TP_ScrUpdate();
    TP_Kbflush();
    while (TRUE)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            if (keyCode == TP_KEY_CANCEL)
            {
                TP_Kbflush();
                return;
            }
        }
    }

}

void NetworkStatusTest(void)
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
        TP_LcdPrintf((uint8*)"%s\x00", "1 Get LAC & Cell ID\x00");
        TP_ScrGotoxyEx(1, 13);
        TP_LcdPrintf((uint8*)"%s\x00", "2 Get Operator\x00");
        TP_ScrGotoxyEx(1, 25);
        TP_LcdPrintf((uint8*)"%s\x00", "3 Get Network status\x00");
        TP_ScrGotoxyEx(1, 38);
        TP_LcdPrintf((uint8*)"%s\x00", "4 Get Sim Operator\x00");

        TP_ScrGotoxyEx(1, 50);
        TP_LcdPrintf((uint8*)"%21s", "Quit");
        TP_ScrUpdate();

        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            if (keyCode == TP_KEY_CANCEL)
            {
                TP_Kbflush();
                return;
            }else{
                doNetworkStatusTest(keyCode);
                TP_Kbflush();
            }
        }

    }
}


void OperatorSelectTest(void)
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
        TP_LcdPrintf((uint8*)"%s\x00", "1 Auto\x00");
        TP_ScrGotoxyEx(1, 13);
        TP_LcdPrintf((uint8*)"%s\x00", "2 Manual\x00");

        TP_ScrGotoxyEx(1, 50);
        TP_LcdPrintf((uint8*)"%21s", "Quit");
        TP_ScrUpdate();

        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            if (keyCode == TP_KEY_CANCEL)
            {
                TP_Kbflush();
                return;
            }
            else if (keyCode == TP_KEY_1)
            {
                TP_PopupMessage("Auto Selecting...");
                if (TP_NetworkSelectOperator(OP_SELECT_AUTO, PNULL, 0) == 0x00)
                {
                    TP_PopupMessageTimed("Operation succeed", 3000);
                }
                else
                {
                    TP_PopupMessageTimed("Operation failed", 3000);
                }
                TP_Kbflush();
                return;
            }
            else if (keyCode == TP_KEY_2)
            {
                doOperatorSelectManual();
                TP_Kbflush();
                return;
            }
        }

    }
}

