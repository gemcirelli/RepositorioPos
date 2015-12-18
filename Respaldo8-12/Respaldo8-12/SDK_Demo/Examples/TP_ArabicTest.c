
#include "TPTypedef.h"
#include "TPKeypad.h"
#include "TPLcd.h"
#include "TPFonts.h"
#include "TP_Singleime.h"

#include "TP_ListBox.h"
#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"

void TP_Arabic_AsciiFontDisplay()
{
    TP_CharSetT currCharSet = {0};
    uint8 aData[1024*3]={0};
    uint8 dobule = 0;
    uint8 startValue = 0x20;
    uint8 keycode;
    TP_DisplayInfo displayInfo =
    {
        PNULL,
        ALIGN_RIGHT,
        PNULL,
        PNULL,
        TP_KEY_NONE,
        KEY_DEMO_CANCEL,
        TP_KEY_NONE,
        TP_KEY_NONE,
        ASCII

    };
    TP_ListBoxRect Rect = {1, 1, 127, 63-16};

    TP_GetCurrentCharSet(&currCharSet);
    TP_SetCurrentCharSet(CHARSET_ARABIC_CP1256);
    TP_ScrFontSet(CFONT);
    do
    {
        TP_ScrCls();
        TP_SetDisplayArea(1, 63-16, 127, 63);
        TP_ScrGotoxyEx(127, 63-16);
        TP_LcdPrintf("%s", "Cancel");

        startValue = 0x81 + dobule * 3;
        TP_SetDisplayArea(1, 1, 127, 63-16);
        TP_ScrGotoxyEx(127, 1);
        memset(aData, 0, sizeof(aData));
        sprintf(&aData[strlen((char*)aData)], "0x%x:", startValue);
        aData[strlen((char*)aData)] = startValue;
        TP_LcdPrintf("%s", aData);

        if(startValue < 0xFF)
        {
            startValue++;
            TP_ScrGotoxyEx(127, 1+16);
            memset(aData, 0, sizeof(aData));
            sprintf(&aData[strlen((char*)aData)], "0x%x:", startValue);
            aData[strlen((char*)aData)] = startValue;
            TP_LcdPrintf("%s", aData);
        }

        if(startValue < 0xFF)
        {
            startValue++;
            TP_ScrGotoxyEx(127, 1+16*2);
            memset(aData, 0, sizeof(aData));
            sprintf(&aData[strlen((char*)aData)], "0x%x:", startValue);
            aData[strlen((char*)aData)] = startValue;
            TP_LcdPrintf("%s", aData);
        }

        while(1)
        {
            if(TP_Kbhit())
            {
                keycode = TP_GetKey();
                if(keycode == KEY_DEMO_CANCEL)
                {
                    TP_SetCurrentCharSet(currCharSet.name);
                    return;
                }
                else if(keycode == TP_KEY_DOWN)
                {
                    if(dobule < 42)
                    {
                        dobule++;
                    }
                }
                else if(keycode == TP_KEY_UP)
                {
                    if(dobule > 0)
                    {
                        dobule--;
                    }
                }
                break;
            }
        }
    }while(1);

    TP_SetCurrentCharSet(currCharSet.name);
}

void TP_ArabicInputTest()
{
    uint8 str[1024];
    uint8 result = 0;
    TP_CharSetT currCharSet = {0};
    TP_GetCurrentCharSet(&currCharSet);
    TP_SetCurrentCharSet(CHARSET_ARABIC_CP1256);

    TP_ScrCls();
    TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangle of Window

    TP_ScrDrawRect(3, 8, 124, 63-16-1);//Draw Rectangle of Input Window
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(1,63-16,126,63);
    TP_ScrGotoxyEx(3+8*5,63-16);
    TP_LcdPrintf((uint8*)"Print");
    TP_ScrGotoxyEx(125,63-16);
    TP_LcdPrintf((uint8*)"Cancel");
    TP_SetInputModeControl(KEY_DEMO_CONFIRM,KEY_DEMO_CANCEL,KEY_DEMO_POUND);
    TP_ShowInputNum(1,80,1);
    //TP_ScrSpaceSet(1,1);
    TP_SetDisplayArea(4,9,123,63-16-2);
    TP_ScrGotoxyEx(123,4);
    TP_SetInputModePosition(59,52);
    TP_SetInputMode(INPUTMODE_NUMBER);
    memset(str,0,sizeof(str));

    //strcat(str,"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    // mode Bit 7 To press Confirm key to return, and to populate the value of interface to str    Bit 6 Large-font display    Bit 5 To display input length    Bit 4 Reserved    Bit 3 Password type input    Bit 2 Left-aligned or right-aligned    Bit 1 Underline    Bit 0 Not displayed
    while(1)
    {
        result = TP_GetHzString((char *)str,0,500);
        //result = TP_GetString((char *)str,0xB0,1,500);
        TP_DbgSerialPrn("\r\n String:%s",str);

        if(result == 0xFB)
        {
            if(strlen((char*)str) > 0 )
            {
                TP_PrnInit();
                TP_PrnFontSet(3);
                TP_PrnGraySet(3);
                TP_PrnClearString();
                TP_PrnSessionStart();
                TP_PrnAddString((uint8*)"%s",str);
                result = TP_PrnString();
                TP_DbgSerialPrn("\r\n Print reuslt:0x%02x", result);
                TP_PrnClearString();
                if(result == 0)
                {
                    result = TP_PrnStep(8);
                }
                result = TP_PrnSessionEnd();
                TP_DbgSerialPrn("\r\n Print reuslt:0x%02x", result);
                if(result != 0x00)
                {
                    //int16 timerId = TP_TimerAllocate();
                    TP_ScrRestore(0);
                    TP_SetCurrentCharSet(CHARSET_ASCII);
                    TP_ScrCls();
                    TP_ScrFontSet(CFONT);
                    TP_SetDisplayArea(0, 0, 127, 63);
                    TP_ScrGotoxyEx(1, 1);
                    TP_LcdPrintf("Print Fail:0x%02X", result);
                    TP_ScrGotoxyEx(1,  63-16);
                    TP_LcdPrintf("Press Any Key To Exit");
                    //TP_TimerStart(timerId, 3*1000);
                    TP_ScrUpdate();
                    TP_Kbflush();
                    while(1)
                    {
                        if(TP_Kbhit())
                        {
                            TP_GetKey();
                            break;
                        }
                        //if(TP_TimerGetRemain(timerId) == 0)
                        //{
                        //    TP_TimerFree(timerId);
                        //    break;
                        //}
                    }
                    TP_SetCurrentCharSet(CHARSET_ARABIC_CP1256);
                    TP_ScrRestore(1);
                }
            }
            else
            {
                //break;
            }
        }
        else
        {
            break;
        }
    }

    TP_SetCurrentCharSet(currCharSet.name);
}

void TP_ArabicPrint()
{
    uint8 data[256] = {0};
    uint8 tempi;
    uint8 result;

    TP_CharSetT currCharSet = {0};

    TP_ScrCls();
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(1, 1, 127, 63);
    TP_ScrGotoxyEx(1, 1);
    TP_LcdPrintf(" Printing......");

    TP_GetCurrentCharSet(&currCharSet);
    TP_SetCurrentCharSet(CHARSET_ARABIC_CP1256);

    for(tempi=0x20; tempi<0xFF; tempi++)
    {
        data[tempi-0x20] = tempi;
    }


    TP_PrnInit();
    TP_PrnGraySet(3);
    TP_PrnSessionStart();
    TP_PrnClearString();
    TP_PrnAddString((uint8*)"%s",data);
    result = TP_PrnString();
    TP_DbgSerialPrn("\r\n Print reuslt:0x%02x", result);
    TP_PrnClearString();
    if(result == 0)
    {
        result = TP_PrnStep(8);
    }
    result = TP_PrnSessionEnd();
    TP_SetCurrentCharSet(currCharSet.name);
    TP_DbgSerialPrn("\r\n Print reuslt:0x%02x", result);


    if(result != 0)
    {
        TP_ScrClsDisplayArea();
        TP_LcdPrintf(" Print Fail:0x%02X", result);
        TP_ScrGotoxyEx(1,  63-16);
        TP_LcdPrintf("Press Any Key To Exit");
        while(1)
        {
            if(TP_Kbhit())
            {
                TP_GetKey();
                break;
            }
        }
    }
}
