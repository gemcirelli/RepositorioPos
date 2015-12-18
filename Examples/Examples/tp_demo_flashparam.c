#include <TPTypedef.h>
#include <TPBase.h>
#include <TPFlashParam.h>
#include <TPKeypad.h>
#include <TPLCD.h>
#include <TP_SingleIME.h>

#include "./TP_DemoDefine.h"
#include "./display/tp_displayapi.h"

void tp_demo_flashparam_read(void)
{
    char           itemName[513];
    char           itemValue[256];
    TP_DisplayInfo dispinfo;
    TP_ListBoxRect rect;
    int32          result;

    while (TRUE)
    {
        TP_ScrAttrSet(0);
        TP_ScrFontSet(ASCII);
        TP_ScrSpaceSet(1, 1);
        TP_ScrClsPrint(1);
        TP_SetDisplayArea(0, 0, 127, 63);

        TP_LcdFreeze();

        TP_ScrCls();

        TP_ShowInputNum(0, 128, 64);

        TP_ScrGotoxy(0, 0);
        TP_LcdPrintf("%18s", "Flash Param Read");
        TP_ScrGotoxy(0, 1);
        TP_LcdPrintf("Input item name:");
        TP_ScrGotoxy(0, 7);
        TP_LcdPrintf("%s%17s", "Read", "Exit");
        TP_ScrDrawRect(0, 16, 127, 54);
        TP_SetDisplayArea(2, 18, 125, 52);
        TP_ScrGotoxyEx(2, 18);
        TP_SetInputModePosition(57, 56);
        TP_SetInputMode(INPUTMODE_LOWCASE);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);

        TP_ScrUpdate();

        memset(itemName, 0x00, sizeof(itemName));
        result = TP_GetHzString(itemName, 0, 256);
        if (result != 0xFB)
        {
            break;
        }

        result = TP_FlashParam_Read(itemName, VAL_STRING, itemValue, sizeof(itemValue));
        if (result < 0)
        {
            sprintf(itemValue, "!Reading failed:\n%d", result);
            TP_PopupMessageTimed(itemValue, 3 * 1000);

            continue;
        }
        else if (result == 0)
        {
            sprintf(itemValue, "!Empty item");
            TP_PopupMessageTimed(itemValue, 3 * 1000);

            continue;
        }

        TP_ScrAttrSet(0);
        TP_ScrFontSet(ASCII);
        TP_ScrSpaceSet(1, 1);
        TP_ScrClsPrint(1);
        TP_SetDisplayArea(0, 0, 127, 63);

        TP_ScrCls();

        TP_ScrGotoxy(0, 0);
        TP_LcdPrintf("%18s", "Flash Param Read");
        TP_ScrGotoxy(0, 1);
        TP_LcdPrintf("Item value:");
        TP_ScrDrawRect(0, 16, 127, 54);
        TP_ScrGotoxy(0, 7);
        TP_LcdPrintf("%s%17s", "Back", "Exit");

        dispinfo.FONT        = ASCII;
        dispinfo.align       = ALIGN_LEFT;
        dispinfo.LKeyValue   = KEY_DEMO_CONFIRM;
        dispinfo.LF_KeyValue = TP_KEY_NONE;
        dispinfo.RKeyValue   = KEY_DEMO_CANCEL;
        dispinfo.RF_KeyValue = TP_KEY_NONE;
        dispinfo.strLButtonTitle = PNULL;
        dispinfo.strRButtonTitle = PNULL;
        dispinfo.strTitle        = PNULL;
        rect.left   = 2;
        rect.top    = 18;
        rect.right  = 125;
        rect.bottom = 52;
        result = TP_ShowText(&dispinfo, rect, itemValue);
        if (result == FALSE)
        {
            break;
        }
    }
}


void tp_demo_flashparam_print(void)
{
    TP_DbgSerialPrn("\r\nFlashParamData:[");
    TP_DbgSerialPrnHex((uint8*)(0x087D0000), 0x00008000);
    TP_DbgSerialPrn("]\r\n");
}

