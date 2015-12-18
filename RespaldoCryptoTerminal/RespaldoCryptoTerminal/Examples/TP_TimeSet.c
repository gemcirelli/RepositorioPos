#include <TPTypedef.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TPBASE.h>

#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"
#include "TP_IdleScreen.h"

/*
void TP_SetDate()
{
    uint8 str[100];
    uint8 Tmp[10];
    uint8 ResultCode;
    TP_ScrCls();
    TP_ScrDrawRect(0, 0, 127, 63);//Draw the window's border

    TP_ScrDrawRect(3, 3, 125, 48);//Draw the input content's window
    TP_ScrAttrSet(0);
    TP_ScrGotoxyEx(3,49);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(1,48,126,63);
    TP_LcdPrintf("Confirm      Cancel");

    TP_SetInputModeControl(TP_KEY_CONFIRM,TP_KEY_CANCEL,TP_KEY_POUND);
    //TP_ScrSpaceSet(1,1);
    TP_ScrGotoxyEx(4,4);
    TP_SetDisplayArea(4,4,124,47);
    TP_SetInputModePosition(59,52);
    TP_ShowInputNum(0, 1, 1);

    memset(str,0,sizeof(str));
    memset(Tmp,0,sizeof(Tmp));
    TP_GetTime(Tmp);
    TP_HexToStr(Tmp,6,&str[2]);
    str[0] = '2';
    str[1] = '0';
    // mode Mode the seventh bit press the Enter Key to return, and input the values to str.     The sixth, Large Font    The fifth, Allows the digital input.  The fourth, Color Inverse   The third, password input mode   The second, Left or Right Justified    The first, Underscore    The zero, No display
    TP_GetString(str,0xF4,14,14);
    TP_StrToHex(str,14,Tmp);


    ResultCode = TP_SetTime(&Tmp[1]);
    memset(str,0,sizeof(str));
    switch (ResultCode)
    {
        case 1:
            strcat(str,"year format error");
            break;
        case 2:
            strcat(str,"month format error");
            break;
        case 3:
            strcat(str,"day format error");
            break;
        case 4 :
            strcat(str,"hour format error");
            break;
        case 5 :
            strcat(str,"min format error");
            break;
        case 6 :
            strcat(str,"Second format error");
            break;
    }

    if (strlen((char *)str) >0)
    {
        TP_Kbflush();
        TP_ScrAttrSet(0);
        TP_ScrFontSet(CFONT);
        TP_SetDisplayArea(10,10,117,53);
        TP_ScrClsDisplayArea();
        TP_ScrDrawRect(11,11,116,52);


        TP_SetDisplayArea(12,12,115,51);

        TP_ScrGotoxyEx(12,12);


        TP_LcdPrintf(str);
        TP_TimerSet(0, 3000);
        while(1)
        {


            if(TP_TimerCheck(0) == 0)
            {
                return;
            }
            if (TP_Kbhit())
            {
                ///Any Key to Exit
                TP_Kbflush(); ///Need to clear the key assignments in keys
                return;

            }
        }
    }

}
*/

#define Y_START  (26)
#define Y_PIXELS (2 + 12 + 2)
#define Y_END    (Y_START + Y_PIXELS - 1)

#define YEAR_X_START   (0)
#define YEAR_X_PIXELS  (4 * 6 + 2)
#define YEAR_X_END     (YEAR_X_START + YEAR_X_PIXELS)

#define MONTH_X_START  (YEAR_X_END + 6)
#define MONTH_X_PIXELS (2 * 6 + 2)
#define MONTH_X_END    (MONTH_X_START + MONTH_X_PIXELS)

#define DAY_X_START    (MONTH_X_END + 6)
#define DAY_X_END      (DAY_X_START + MONTH_X_PIXELS )

#define HOUR_X_START   (DAY_X_END + 6)
#define HOUR_X_END     (HOUR_X_START + MONTH_X_PIXELS)

#define MINUTE_X_START (HOUR_X_END + 6)
#define MINUTE_X_END   (MINUTE_X_START + MONTH_X_PIXELS)

#define SECOND_X_START (MINUTE_X_END + 6)
#define SECOND_X_END   (SECOND_X_START + MONTH_X_PIXELS)


Boolean TP_SetDate()
{
    uint8  str[50];
    uint8  strback[50];
    uint8  Tmp[10];
    uint8  editdata[5];
    uint8* edit_p;

    uint8 Year[5];
    uint8 Month[3];
    uint8 Day[3];
    uint8 Hour[3];
    uint8 Minute[3];
    uint8 Second[3];
    uint8 inputback;
    uint8 editindex = 0;
    uint8 editlen = 0;

    uint32 charspace = 0;
    uint32 linespace = 0;

    uint8 keycode;

    Boolean IsSetTime = FALSE;

    TP_ScrSpaceGet(&charspace, &linespace);
    TP_ScrSpaceSet(0, 0);

    memset(str,0,sizeof(str));
    memset(strback,0,sizeof(strback));
    memset(Tmp,0,sizeof(Tmp));
    memset(editdata, 0x00, sizeof(editdata));
    memset(Year,0,sizeof(Year));
    memset(Month,0,sizeof(Month));
    memset(Day,0,sizeof(Day));
    memset(Hour,0,sizeof(Hour));
    memset(Minute,0,sizeof(Minute));
    memset(Second,0,sizeof(Second));

    /* Get current date and time */
    TP_GetTime(Tmp);
    TP_HexToStr(Tmp, 6, &str[2]);
    str[0] = '2';
    str[1] = '0';
    memcpy(strback, str, strlen((char *)str));

    memcpy(Year,   &(str[0]),  4);
    memcpy(Month,  &(str[4]),  2);
    memcpy(Day,    &(str[6]),  2);
    memcpy(Hour,   &(str[8]),  2);
    memcpy(Minute, &(str[10]), 2);
    memcpy(Second, &(str[12]), 2);

    /* Control the inputing behaviors */
    TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, TP_KEY_NONE);
    TP_ClearInputModeControlList();
    /* Do not show input mode icon */
    TP_SetInputModePosition(128, 64);
    /* Do not show inputing numbers */
    TP_ShowInputNum(0, 0, 0);

    while (TRUE)
    {
        IsSetTime = FALSE;

        TP_ScrCls();
        TP_ScrAttrSet(0);
        TP_ScrSpaceSet(0, 0);
        TP_ScrFontSet(CFONT);
        TP_SetDisplayArea(0, 0, 127, 63);
        TP_ScrGotoxyEx(0, 0);
    #ifdef CHINESE
        TP_LcdPrintf((uint8*)"e柚萌掌u时间");

        TP_ScrDrawRect(0, 51, 127, 63);

        TP_ScrAttrSet(1);

        TP_ScrGotoxyEx(1, 51);
        TP_LcdPrintf((uint8*)"编辑");
        TP_ScrGotoxyEx(52, 51);
        TP_LcdPrintf("保存");
        TP_ScrGotoxyEx(103, 51);
        TP_LcdPrintf("取消");
    #else
        TP_LcdPrintf((uint8*)"Set Date Time");

        TP_ScrDrawRect(0, 51, 127, 63);

        TP_ScrAttrSet(1);

        TP_ScrGotoxyEx(1, 51);
        TP_LcdPrintf((uint8*)"Edit");
        TP_ScrGotoxyEx(52, 51);
        TP_LcdPrintf("Save");
        TP_ScrGotoxyEx(91, 51);
        TP_LcdPrintf("Cancel");
    #endif

        TP_ScrAttrSet(0);

        TP_SetDisplayArea(0, Y_START, 127, Y_END);

        /* Refresh the value of date and time */
        if (editindex == 0)
        {
            TP_ScrDrawRect(YEAR_X_START, Y_START, YEAR_X_END, Y_END);
        }
        TP_ScrGotoxyEx(YEAR_X_START + 1, Y_START + 2);
        TP_LcdPrintf(Year);

        TP_ScrGotoxyEx(YEAR_X_END + 1, Y_START + 2);
        TP_LcdPrintf((uint8*)"-");

        if (editindex == 1)
        {
            TP_ScrDrawRect(MONTH_X_START, Y_START,
                           MONTH_X_END, Y_END);
        }
        TP_ScrGotoxyEx(MONTH_X_START + 1, Y_START + 2);
        TP_LcdPrintf(Month);

        TP_ScrGotoxyEx(MONTH_X_END + 1, Y_START + 2);
        TP_LcdPrintf((uint8*)"-");

        if (editindex == 2)
        {
            TP_ScrDrawRect(DAY_X_START, Y_START,
                           DAY_X_END, Y_END);
        }
        TP_ScrGotoxyEx(DAY_X_START + 1, Y_START + 2);
        TP_LcdPrintf(Day);

        TP_ScrGotoxyEx(DAY_X_END + 1, Y_START + 2);
        TP_LcdPrintf((uint8*)" ");

        if (editindex == 3)
        {
            TP_ScrDrawRect(HOUR_X_START, Y_START,
                           HOUR_X_END,   Y_END);
        }
        TP_ScrGotoxyEx(HOUR_X_START + 1, Y_START + 2);
        TP_LcdPrintf(Hour);

        TP_ScrGotoxyEx(HOUR_X_END + 1, Y_START + 2);
        TP_LcdPrintf((uint8*)":");

        if (editindex == 4)
        {
            TP_ScrDrawRect(MINUTE_X_START, Y_START,
                           MINUTE_X_END,   Y_END);
        }
        TP_ScrGotoxyEx(MINUTE_X_START + 1, Y_START + 2);
        TP_LcdPrintf(Minute);

        TP_ScrGotoxyEx(MINUTE_X_END + 1, Y_START + 2);
        TP_LcdPrintf((uint8*)":");

        if (editindex == 5)
        {
            TP_ScrDrawRect(SECOND_X_START, Y_START,
                           SECOND_X_END,   Y_END);
        }
        TP_ScrGotoxyEx(SECOND_X_START + 1, Y_START + 2);
        TP_LcdPrintf(Second);

        if (TP_Kbhit() == 0xFF)
        {
            keycode = TP_GetKey();
            TP_Kbflush();

            if (keycode == TP_KEY_UP ||
                keycode == TP_KEY_LEFT)
            {
                if (editindex > 0)
                {
                    editindex--;
                }
                else
                {
                    editindex = 0;
                }
            }
            else if (keycode == TP_KEY_DOWN ||
                     keycode == TP_KEY_RIGHT)
            {
                if (editindex < 5)
                {
                    editindex++;
                }
                else
                {
                    editindex = 5;
                }
            }
            else if (keycode == KEY_DEMO_CANCEL)
            {
                break;
            }
            else if (keycode == KEY_DEMO_CONFIRM)
            {
                IsSetTime = TRUE;
            }
            else if (keycode == TP_KEY_OK)
            {
                memset(str, 0, sizeof(str));
                strcat((char *)str, (char *)Year);
                strcat((char *)str, (char *)Month);
                strcat((char *)str, (char *)Day);
                strcat((char *)str, (char *)Hour);
                strcat((char *)str, (char *)Minute);
                strcat((char *)str, (char *)Second);
                inputback = TP_SetDateTimeEx((char*)str);
                switch (inputback)
                {
             #ifdef CHINESE
                    case 0:
                        break;
                    case 1:
                        TP_PopupMessageTimed((uint8*)"年份出磇",1000);
                        break;
                    case 2:
                        TP_PopupMessageTimed((uint8*)"月份出磇",1000);
                        break;
                    case 3:
                        TP_PopupMessageTimed((uint8*)"日出磇",1000);
                        break;
                    case 4 :
                        TP_PopupMessageTimed((uint8*)"小时出磇",1000);
                        break;
                    case 5 :
                        TP_PopupMessageTimed((uint8*)"分謔出磇",1000);
                        break;
                    case 6 :
                        TP_PopupMessageTimed((uint8*)"秒出磇",1000);
                        break;
                    case 7 :
                        TP_PopupMessageTimed((uint8*)"无效数据",1000);
                        break;
             #else
                    case 0:
                        break;
                    case 1:
                        TP_PopupMessageTimed((uint8*)"year format error",1000);
                        break;
                    case 2:
                        TP_PopupMessageTimed((uint8*)"month format error",1000);
                        break;
                    case 3:
                        TP_PopupMessageTimed((uint8*)"day format error",1000);
                        break;
                    case 4 :
                        TP_PopupMessageTimed((uint8*)"hour format error",1000);
                        break;
                    case 5 :
                        TP_PopupMessageTimed((uint8*)"min format error",1000);
                        break;
                    case 6 :
                        TP_PopupMessageTimed((uint8*)"second format error",1000);
                        break;
                    case 7 :
                        TP_PopupMessageTimed((uint8*)"invalid data",1000);
                        break;
             #endif
                }
                if (inputback == 0)//sucess
                {
                    break;
                }
            }
        }

        if (IsSetTime == TRUE)
        {
            TP_SetDisplayArea(0, 0, 127, 63);
            TP_ScrClrRect(0, 51, 127, 63);
            TP_ScrDrawRect(0, 51, 127, 63);
            TP_ScrAttrSet(1);
        #ifdef CHINESE
            TP_ScrGotoxyEx(1, 51);
            TP_LcdPrintf((uint8*)"确认");
            TP_ScrGotoxyEx(103, 51);
            TP_LcdPrintf("取消");
        #else
            TP_ScrGotoxyEx(1, 51);
            TP_LcdPrintf((uint8*)"Confirm");
            TP_ScrGotoxyEx(91, 51);
            TP_LcdPrintf("Cancel");
        #endif

            TP_ScrAttrSet(0);

            switch(editindex)
            {
            case 0:
                edit_p = Year;
                TP_SetDisplayArea(YEAR_X_START + 1, Y_START + 2,
                                  YEAR_X_END - 1,   Y_END - 2);
                TP_ScrGotoxyEx(YEAR_X_START + 1, Y_START + 2);
                break;

            case 1:
                edit_p = Month;
                TP_SetDisplayArea(MONTH_X_START + 1, Y_START + 2,
                                  MONTH_X_END - 1,   Y_END - 2);
                TP_ScrGotoxyEx(MONTH_X_START + 1, Y_START + 2);
                break;

            case 2:
                edit_p = Day;
                TP_SetDisplayArea(DAY_X_START + 1, Y_START + 2,
                                  DAY_X_END - 1,   Y_END - 2);
                TP_ScrGotoxyEx(DAY_X_START + 1, Y_START + 2);
                break;

            case 3:
                edit_p = Hour;
                TP_SetDisplayArea(HOUR_X_START + 1, Y_START + 2,
                                  HOUR_X_END - 1,   Y_END - 2);
                TP_ScrGotoxyEx(HOUR_X_START + 1, Y_START + 2);
                break;

            case 4:
                edit_p = Minute;
                TP_SetDisplayArea(MINUTE_X_START + 1, Y_START + 2,
                                  MINUTE_X_END - 1,   Y_END - 2);
                TP_ScrGotoxyEx(MINUTE_X_START + 1, Y_START + 2);
                break;

            case 5:
                edit_p = Second;
                TP_SetDisplayArea(SECOND_X_START + 1, Y_START + 2,
                                  SECOND_X_END - 1,   Y_END - 2);
                TP_ScrGotoxyEx(SECOND_X_START + 1, Y_START + 2);
                break;
            }
            if (editindex == 0)
            {
                editlen = 4;
            }
            else
            {
                editlen = 2;
            }

            inputback = TP_GetString((char*)edit_p, 0xF4, editlen, editlen);
            TP_Kbflush();
            IsSetTime = FALSE;
        }
    }


    TP_ScrAttrSet(0);
    TP_ScrSpaceSet(charspace, linespace);
    if (inputback == 0)
    {
        return TRUE;
    }
    return FALSE;
}



