#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TP_SingleIME.h>

#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"

///Input Number
void ShowNumberInput()
{
    uint8 str[256];
    TP_ScrCls();
    TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangle of Window

    TP_ScrDrawRect(3, 3, 125, 48);//Draw Rectangle of Input Window
    TP_ScrAttrSet(0);
    TP_ScrGotoxyEx(3,49);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(1,48,126,63);
    TP_LcdPrintf((uint8*)"Confirm       Cancel");

    TP_SetInputModeControl(KEY_DEMO_CONFIRM,KEY_DEMO_CANCEL,KEY_DEMO_POUND);

    TP_ShowInputNum(0,1,1);
    //TP_ScrSpaceSet(1,1);
    TP_ScrGotoxyEx(4,4);
    TP_SetDisplayArea(4,4,124,47);
    TP_SetInputModePosition(59,52);

    memset(str,0,sizeof(str));
    // mode Bit 7 To press Confirm key to return, and to populate the value of interface to str    Bit 6 Large-font display    Bit 5 To display input length    Bit 4 Reserved    Bit 3 Password type input    Bit 2 Left-aligned or right-aligned    Bit 1 Underline    Bit 0 Not displayed
    TP_GetString((char *)str,0x90,1,255);
}


///Input Chinese
void ShowHzInput()
{
    TP_DisplayInfo displayInfo;
    TP_ListBoxRect rect;

    uint8   str[1025];
    uint8   result;
    Boolean userRet;

    TP_ScrCls();
    TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangle of Window

    TP_ScrDrawRect(3, 8, 125, 48);//Draw Rectangle of Input Window
    TP_ScrAttrSet(0);
    TP_ScrGotoxyEx(3,49);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(1,48,126,63);
    TP_LcdPrintf((uint8*)"Confirm       Cancel");
    TP_SetInputModeControl(KEY_DEMO_CONFIRM,KEY_DEMO_CANCEL,KEY_DEMO_POUND);
    TP_ShowInputNum(1,80,1);
    //TP_ScrSpaceSet(1,1);
    TP_ScrGotoxyEx(4,4);
    TP_SetDisplayArea(4,9,124,47);
    TP_SetInputModePosition(59,52);
    TP_SetInputMode(INPUTMODE_LOWCASE);
    memset(str,0,sizeof(str));

    result = TP_GetHzString((char *)str, 0, 500);
    if (result == 0xFF)
    {
        return;
    }

    TP_DbgSerialPrn("%s", str);

    displayInfo.align    = ALIGN_LEFT;
    displayInfo.FONT     = CFONT;
    displayInfo.LKeyValue   = KEY_DEMO_CONFIRM;
    displayInfo.LF_KeyValue = TP_KEY_NONE;
    displayInfo.RKeyValue   = KEY_DEMO_CANCEL;
    displayInfo.RF_KeyValue = TP_KEY_NONE;

    TP_ScrCls();
    TP_SetDisplayArea(0, 0, 127, 63);

    TP_ScrDrawRect(0, 50, 127, 63);
    TP_ScrGotoxyEx(2, 52);
    TP_LcdPrintf("%s%17s", "Next", "Exit");

    rect.left   = 2;
    rect.top    = 2;
    rect.right  = 125;
    rect.bottom = 48;

    TP_ScrDrawRect(rect.left - 2, rect.top - 2, rect.right + 2, rect.bottom + 2);

    while (TRUE)
    {
        userRet = TP_ShowText(&displayInfo, rect, str);

        if (userRet == TRUE)
        {
            rect.left  += 6;
            rect.right -= 6;

            if ((rect.right - rect.left) <= 24)
            {
                displayInfo.LKeyValue   = TP_KEY_NONE;
                displayInfo.LF_KeyValue = TP_KEY_NONE;

                TP_ScrCls();
                TP_SetDisplayArea(0, 0, 127, 63);

                TP_ScrDrawRect(rect.left - 2, rect.top - 2, rect.right + 2, rect.bottom + 2);
                TP_ScrDrawRect(0, 50, 127, 63);
                TP_ScrGotoxyEx(2, 52);
                TP_LcdPrintf("%21s", "Exit");
            }
            else
            {
                TP_ScrCls();
                TP_SetDisplayArea(0, 0, 127, 63);

                TP_ScrDrawRect(rect.left - 2, rect.top - 2, rect.right + 2, rect.bottom + 2);
                TP_ScrDrawRect(0, 50, 127, 63);
                TP_ScrGotoxyEx(2, 52);
                TP_LcdPrintf("%s%17s", "Next", "Exit");
            }
        }
        else
        {
            return;
        }
    }
}


void ShowPasswordInput()
{
    uint8 str[1024];
    uint8 result;

    TP_ScrCls();
    TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangle of Window

    TP_ScrDrawRect(3, 8, 125, 48);//Draw Rectangle of Input Window
    TP_ScrAttrSet(0);
    TP_ScrGotoxyEx(3,49);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(1,48,126,63);
    TP_LcdPrintf((uint8*)"Confirm       Cancel");

    TP_SetInputModeControl(KEY_DEMO_CONFIRM,KEY_DEMO_CANCEL,KEY_DEMO_POUND);

    TP_ShowInputNum(1,80,1);
    //TP_ScrSpaceSet(1,1);
    TP_ScrGotoxyEx(4,9);
    TP_SetDisplayArea(4,9,124,47);
    TP_SetInputModePosition(59,52);

    memset(str,0,sizeof(str));

    result = TP_GetPasswordTimed((char *)str, 1, 8, 0xD40F, '*', 0);

    if (result == 0xF9 ||
        result == 0xFA ||
        result == 0xFB)
    {
        if (strlen((char *)str) > 0)
        {
            TP_ScrCls();
            TP_ScrDrawRect(3, 8, 125, 48);
            TP_ScrGotoxyEx(5, 10);
            TP_SetDisplayArea(5, 10, 123, 46);
            TP_LcdPrintf((uint8*)"Password:\n%s", str);
            TP_ScrGotoxyEx(3,49);
            TP_SetDisplayArea(1,48,126,63);
            TP_LcdPrintf((uint8*)"Confirm       Cancel");

            TP_Kbflush();
            while (TRUE)
            {
                if (TP_Kbhit() == 0xFF)
                {
                    result = TP_GetKey();
                    if (result == KEY_DEMO_CONFIRM ||
                        result == KEY_DEMO_CANCEL)
                    {
                        break;
                    }
                }
            }
        }
    }

    TP_Kbflush();

    //TP_DbgSerialPrn("\n Password:%s==",str);
}

