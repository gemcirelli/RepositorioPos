#include <TPTypedef.h>
#include <TPBase.h>
#include <TPUSSD.h>
#include <TPPhone.h>
#include <TPLCD.h>
#include <TPKEYPAD.h>
#include "TP_DemoDefine.h"
#include  "TP_ListBox.h"

Boolean TP_IsGBK(uint8 WordCode);

void ShowPopupDialogError(uint32 Ms)
{
    uint8 TmpBuffer[256];
    uint8 Canreturn;
    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(10,10,117,53);
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect(11,11,116,52);


    TP_SetDisplayArea(12,12,115,51);

    TP_ScrGotoxyEx(12,12);

    memset(TmpBuffer,0,sizeof(TmpBuffer));
    sprintf((char*)TmpBuffer,"USSD Call Error");
    TP_LcdPrintf(TmpBuffer);
    TP_TimerSet(0, Ms);
    while(1)
    {

        if(TP_TimerCheck(0) == 0)
        {
            return;
        }
        if (TP_Kbhit())
        {
            ///Press Any Key to Quit
            TP_Kbflush(); ///Key Value of The Key To Be Flushed
            return;

        }
    }
}

typedef struct
{

    uint8* LineNo;
    uint8 LineLen;
    uint8 Key[5];
}TP_ShowUssdTextLine;

typedef struct
{
    uint8 LineCount;
    TP_ShowUssdTextLine Lines[100];
}TP_ShowUssdTextLines;

static TP_ShowUssdTextLines TShowTextLines;

void TP_ShowUSSD(uint8 ConfirmKey, uint8 CancelKey,
                 uint8* fmt,uint8 SelectKey[5])
{
    uint8* DisplayData;
    uint8  DisplayOut[512];
    uint16 DisplayStartPos =0;
    uint8 MaxCharInRow = 0;
    uint8 userKey = 0;
    TP_ShowUssdTextLines *ShowTextLines;
    uint8 LineLen;
    uint16 Index;
    uint8* StartPoint;
    uint8 StartLine;
    uint8 MaxRow;
    uint8 retVal;

    TP_ListBox *listbox;
    TP_ListBoxRect rect;
    uint16 Strlen1;

    ShowTextLines = &TShowTextLines;

    //memcpy(DisplayData,fmt,strlen((char *)fmt));
    DisplayData = fmt;


    //sendTcpStringToSerialPort("TP_ShowUSSD", strlen((char *)"TP_ShowUSSD"), 0);
    MaxCharInRow = TP_GetMaxCharInRow();//the function TP_GetMaxCharInRow has added redundant one,so minus 1
    MaxRow = 3;//TP_GetMaxRowInScreen();
    //TP_AllocMemory(sizeof(TP_ShowUssdTextLines), (void**)&ShowTextLines);
    memset(ShowTextLines,0,sizeof(TP_ShowUssdTextLines));
    Index =0;
    LineLen =0;
    StartPoint = DisplayData;
    Strlen1 = strlen((char *)DisplayData);
    while (Index < Strlen1)
    {
        if (DisplayData[Index] == '\n')
        {
            ShowTextLines->Lines[ShowTextLines->LineCount].LineLen = LineLen;
            ShowTextLines->Lines[ShowTextLines->LineCount].LineNo = StartPoint;
            ShowTextLines->LineCount++;
            Index++;
            StartPoint  = &DisplayData[Index];
            LineLen =0;

            continue;
        }
        else if (TP_IsGBK(DisplayData[Index]) == TRUE)
        {
            Index++;
            LineLen++;
        }
        Index++;
        LineLen++;

        if (LineLen == MaxCharInRow)
        {
            ShowTextLines->Lines[ShowTextLines->LineCount].LineLen = LineLen;
            ShowTextLines->Lines[ShowTextLines->LineCount].LineNo = StartPoint;
            ShowTextLines->LineCount++;
            StartPoint  = &DisplayData[Index];
            LineLen =0;
        }
        else if (LineLen > MaxCharInRow) //this only appear in that the last character to diplay is double wide
        {
            ShowTextLines->Lines[ShowTextLines->LineCount].LineLen = LineLen-2;
            ShowTextLines->Lines[ShowTextLines->LineCount].LineNo = StartPoint;
            ShowTextLines->LineCount++;
            StartPoint  = &DisplayData[Index-2];
            LineLen =2;
        }


    }

    if (LineLen >0)
    {
        ShowTextLines->Lines[ShowTextLines->LineCount].LineLen = LineLen;
        ShowTextLines->Lines[ShowTextLines->LineCount].LineNo = StartPoint;

        ShowTextLines->LineCount++;
    }
    DisplayStartPos = 0;
    StartLine =0; //start to display from line 0
    memset(DisplayOut,0,sizeof(DisplayOut));
    Index=0;
    LineLen =0;
    while (Index<MaxRow)
    {
        memcpy(&DisplayOut[LineLen],ShowTextLines->Lines[StartLine+Index].LineNo,ShowTextLines->Lines[StartLine+Index].LineLen);
        LineLen += ShowTextLines->Lines[StartLine+Index].LineLen;
        if (ShowTextLines->Lines[StartLine+Index].LineLen == MaxCharInRow)
        {

        }
        else
        {
            DisplayOut[LineLen++] = '\n';
        }
        Index++;
        if (StartLine+Index > ShowTextLines->LineCount)
        {
            break;
        }
    }

    TP_LcdPrintf(DisplayOut);
    TP_Kbflush();

    while(1)
    {
        if(TP_Kbhit()!= 0)
        {
            userKey = TP_GetKey();
            if(userKey == ConfirmKey)
            {
                return;
            }
            else if(userKey == CancelKey)
            {
                return;
            }

            switch(userKey)
            {
            case TP_KEY_UP:
                if (StartLine == 0)
                {
                    break;
                }
                TP_ScrClsDisplayArea();
                StartLine--;
                memset(DisplayOut,0,sizeof(DisplayOut));
                Index=0;
                LineLen =0;
                while (Index<MaxRow)
                {
                    memcpy(&DisplayOut[LineLen],ShowTextLines->Lines[StartLine+Index].LineNo,
                           ShowTextLines->Lines[StartLine+Index].LineLen);
                    LineLen += ShowTextLines->Lines[StartLine+Index].LineLen;
                    if (ShowTextLines->Lines[StartLine+Index].LineLen == MaxCharInRow)
                    {

                    }
                    else
                    {
                        DisplayOut[LineLen++] = '\n';
                    }
                    Index++;
                    if (StartLine+Index > ShowTextLines->LineCount)
                    {
                        break;
                    }
                }
                TP_LcdPrintf(DisplayOut);
                break;

            case TP_KEY_DOWN:

                if (StartLine >= ShowTextLines->LineCount-1)
                {
                    break;
                }
                TP_ScrClsDisplayArea();
                StartLine++;

                memset(DisplayOut,0,sizeof(DisplayOut));
                Index=0;
                LineLen =0;
                while (Index<MaxRow)
                {
                    memcpy(&DisplayOut[LineLen],ShowTextLines->Lines[StartLine+Index].LineNo,ShowTextLines->Lines[StartLine+Index].LineLen);
                    LineLen += ShowTextLines->Lines[StartLine+Index].LineLen;
                    if (ShowTextLines->Lines[StartLine+Index].LineLen == MaxCharInRow)
                    {

                    }
                    else
                    {
                        DisplayOut[LineLen++] = '\n';
                    }
                    Index++;
                    if (StartLine+Index > ShowTextLines->LineCount)
                    {
                        break;
                    }
                }
                TP_LcdPrintf(DisplayOut);
                break;

            case TP_KEY_1:
            case TP_KEY_2:
            case TP_KEY_3:
            case TP_KEY_4:
            case TP_KEY_5:
            case TP_KEY_6:
            case TP_KEY_7:
            case TP_KEY_8:
            case TP_KEY_9:
            case TP_KEY_0:
            case TP_KEY_STAR:
            case TP_KEY_POUND:
                TP_ScrRestore(0);
                TP_SetDisplayArea(1,25,126,63);
                TP_ScrClsDisplayArea();

                TP_ScrDrawRect(3, 25, 125, 48);/*Draw Rectangle of Input Window */
                TP_ScrAttrSet(0);
                TP_ScrGotoxyEx(3,49);
                TP_ScrFontSet(CFONT);
                TP_SetDisplayArea(1,48,126,63);
                TP_LcdPrintf((uint8*)"Confirm       Cancel");

                TP_SetInputModeControl(KEY_DEMO_CONFIRM,KEY_DEMO_CANCEL,KEY_DEMO_POUND);
                /*TP_ShowInputNum(1,80,1);*/
                /*TP_ScrSpaceSet(1,1);*/
                TP_ScrGotoxyEx(6,32);
                TP_SetDisplayArea(6,32,124,47);
                TP_SetInputModePosition(59,52);
                memset(SelectKey,0,sizeof(SelectKey));
                TP_SetKey(userKey);
                retVal = TP_GetHzString((char*)SelectKey,0,20);
                if (retVal == 0XFB)
                {
                    TP_ScrRestore(1);
                    return;
                }
                else
                {
                    TP_ScrRestore(1);
                    break;
                }
            }
        }
    }
}

void TP_ShowUSSDDilog()
{
    uint8 str[256];
    uint8 CallTimes[10];
    uint8 KeyCode;

    uint16 len;
    uint8* PBuffer;
    uint32 OperateCode;
    uint8 KeyArr[21];
    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrDrawRect(0, 0, 127, 63);//Draw the window's border

    TP_ScrGotoxyEx(3,3);
    TP_SetDisplayArea(3,3,126,15);
    TP_LcdPrintf((uint8*)"Input USSD Number:");

    TP_ScrDrawRect(3, 17, 125, 48);//Draw the input content's window
    TP_ScrAttrSet(0);
    TP_ScrGotoxyEx(3,49);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(1,48,126,63);
    TP_LcdPrintf((uint8*)"Confirm       Cancel");

    TP_SetInputModeControl(KEY_DEMO_CONFIRM,KEY_DEMO_CANCEL,0xFF);
    TP_ShowInputNum(0, 1, 1);
    TP_SetInputModePosition(128, 64);
    //TP_ScrSpaceSet(1,1);
    TP_ScrGotoxyEx(4,19);
    TP_SetDisplayArea(4,19,124,47);
    //TP_SetInputModePosition(59,52);

    memset(str,0,sizeof(str));
    // mode Mode the seventh bit press the Enter Key to return, and input the values to str.     The sixth, Large Font    The fifth, Allows
    TP_GetStringEx((char*)str,0xC4,1,19,NULL);
    TP_ScrCls();
    if (strlen((char *)str) ==0)
    {

        return;
    }

    TP_TimerSet(1, 30000);

    if (TP_USSDCallOut((char*)str) == 0xFF)
    {
        ShowPopupDialogError(3000);

        return;
    }

    TP_Kbflush();
    memset(str,0,sizeof(str));
    while (1)
    {
        switch (TP_GetUSSDStatus())
        {
            case US_ERROR :   ///Occur the Except
                ShowPopupDialogError(3000);
                return;
                break;
            case US_Connecting :  /// Connet to Server progress
                //TP_ScrDrawRect(0, 0, 127, 63);//Draw the window's border
                TP_ScrCls();
                TP_ScrGotoxyEx(3,8);
                TP_ScrFontSet(CFONT);
                TP_SetDisplayArea(3,8,126,20);
                TP_ScrClsDisplayArea();
                TP_LcdPrintf((uint8*)"Connecting ... ...");
                if (TP_TimerCheck(1) == 0)
                {
                    TP_ScrCls();
                    TP_ScrGotoxyEx(3,8);
                    TP_ScrFontSet(CFONT);
                    TP_SetDisplayArea(3,8,126,20);
                    TP_ScrClsDisplayArea();
                    TP_LcdPrintf((uint8*)"No Response");
                    TP_Kbflush();
                    while (TP_Kbhit() != 0xFF);
                    TP_Kbflush();
                    TP_EndUSSDCall();
                    return;
                }
                break;
            case US_SendMessage :
                //TP_ScrDrawRect(0, 0, 127, 63);//Draw the window's border
                TP_ScrCls();
                TP_ScrGotoxyEx(3,8);
                TP_ScrFontSet(CFONT);
                TP_SetDisplayArea(3,8,126,20);
                TP_ScrClsDisplayArea();
                if (TP_TimerCheck(1) == 0)
                {
                    TP_LcdPrintf((uint8*)"No Response");
                    TP_Kbflush();
                    while (TP_Kbhit() != 0xFF);
                    TP_Kbflush();
                    TP_EndUSSDCall();
                    return;
                }
                else
                {
                    TP_LcdPrintf((uint8*)"Sending ... ...");
                }
                break;
            case US_ReceiveMessage : ///receive Message from Server

                len = TP_GetUSSDMessageLen();
                len++;
                len = 512;
                TP_ScrClsDisplayArea();
                TP_AllocMemory(len, (void**)&PBuffer);
                TP_ScrGotoxyEx(4,9);
                TP_SetDisplayArea(3,8,126,50);
                if (PBuffer == NULL)
                {
                    return;
                }


                memset(PBuffer,0,len);
                len = TP_GetUSSDMessage(PBuffer, len,&OperateCode);
                PBuffer[len] =0;
                //printf(PBuffer);

                memset(KeyArr,0,sizeof(KeyArr));
                TP_ShowUSSD(KEY_DEMO_CONFIRM,KEY_DEMO_CANCEL,PBuffer,KeyArr);
                if ( strlen((char *)KeyArr) == 0)
                {
                    TP_FreeMemory((void**)&PBuffer);
                    TP_EndUSSDCall();
                    return;
                }
                TP_FreeMemory((void**)&PBuffer);
                ///»√o√ªß ‰»Îo¶¥ƒu»›
                TP_SendUSSDMessage(KeyArr,  strlen((char*)KeyArr) );
                TP_TimerSet(1, 30000);
                //return;
                break;

        }


    }

}


