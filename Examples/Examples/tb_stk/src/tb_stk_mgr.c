/******************************************************************************
* Include Headers
*******************************************************************************/
#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TP_SingleIME.h>
#include <TPStk.h>
#include <TPLCD.h>
#include <TPPhone.h>
#include <TPSerial.h>

#include "TP_ListBox.h"
#include "TP_DisplayAPI.h"
#include "tb_stk.h"

/******************************************************************************
* Manifest Constants
*******************************************************************************/
#define CHAR_WIDTH  (6)
#define CHAR_HEIGHT (12)

/******************************************************************************
* Macros
*******************************************************************************/
#define min(x, y) ((x) < (y) ? (x) : (y))

/******************************************************************************
* External Protected Functions
*******************************************************************************/
extern void ShowListSelectDialog(uint32 Ms,uint8* Buffer);

/******************************************************************************
* Private Variables
*******************************************************************************/
static TP_STKReqT TPstStkCmdReq1; //STK ¿¨ÏòoÃ»§ÇëÇo´¦ÀiÃüaî¡£
static TP_STKRspT TPstStkCmdRsp1; //oÃ»§´¦Àiiê±Ï£¬ÏoSTK¿¨o¦´ð

/******************************************************************************
* Private Function Prototypes
*******************************************************************************/
static uint8       stkInputData(const TP_STKGetInputReqT* pGetInputReq, TP_STKStringT* p);
static TP_ListBox* stkConstructMenu(TP_STKActionE ucRecvCmdP);

/******************************************************************************
* External Protected Variables
*******************************************************************************/
extern uint8
SpeechVolume[8][206];

/******************************************************************************
* Private Functions
*******************************************************************************/
static Boolean stkCallMake(void)
{
    uint8 str[256];
    uint8 CallTimes[10];
    uint8 KeyCode;
    uint8 volume;

    TP_ScrCls();
    TP_Kbflush();
    TP_ScrGotoxyEx(3,49);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(1,48,126,63);
    TP_LcdPrintf((uint8*)"              Cancel");

    memset(str,0,sizeof(str));
    TP_TimerSet(2, 0);
    while (TRUE)
    {
        switch (TP_GetCallStatus())
        {
        case TP_CALLOUT:
        case TP_CALLING://Dialing
            if (TP_TimerCheck(2) == 0)
            {
                TP_ScrCls();
                TP_ScrGotoxyEx(3,49);
                TP_ScrFontSet(CFONT);
                TP_SetDisplayArea(1,48,126,63);
                TP_LcdPrintf((uint8*)"              Cancel");
            }
            TP_ScrDrawRect(0, 0, 127, 63);//Draw the window's border
            TP_ScrGotoxyEx(3,8);
            TP_ScrFontSet(CFONT);
            TP_SetDisplayArea(3,8,126,20);
            TP_LcdPrintf((uint8*)"Dial ... ...");
            break;
        case TP_WAITING ://Wait for connecting
            if (TP_TimerCheck(2) == 0)
            {
                TP_ScrCls();

                TP_ScrGotoxyEx(3,49);
                TP_ScrFontSet(CFONT);
                TP_SetDisplayArea(1,48,126,63);
                TP_LcdPrintf((uint8*)"              Cancel");
            }
            TP_ScrDrawRect(0, 0, 127, 63);//Draw the window's border
            TP_ScrGotoxyEx(3,8);
            TP_ScrFontSet(CFONT);
            TP_SetDisplayArea(3,8,126,20);
            TP_ScrClsDisplayArea();
            TP_LcdPrintf((uint8*)"Waiting ... ...");
            break;
         case TP_CALLFAIL:
            TP_HangUpSTK();
            break;
        case TP_HANGUP:
            TP_ScrCls();//Clear the screen
            TP_ScrDrawRect(0, 0, 127, 63);//Draw the window's border
            TP_ScrGotoxyEx(3,8);
            TP_ScrFontSet(CFONT);
            TP_SetDisplayArea(3,8,126,20);
            TP_ScrClsDisplayArea();
            TP_LcdPrintf((uint8*)"Call End ... ...");
            TP_TimerSet(3, 1000);
            while (TP_TimerCheck(3) != 0);
            return FALSE;
            break;
        case TP_CALLCONNECTED:
            return TRUE;
        }

        ///Key Operation
        if (TP_Kbhit())
        {
            KeyCode = TP_GetKey();

            switch (KeyCode)
            {
            case TP_KEY_HANDFREE: //Under this situation, on-hook
            case TP_KEY_ONHOOK:
            case STK_KEY_CANCEL:
                TP_HangUpSTK();
                break;

            case TP_KEY_UP:
                volume = TP_GetVolume(3);
                if (volume < 7)
                {
                    TP_SetVolume(3, volume + 1);
                }

                TP_SetDisplayArea(30, 31, 97, 50);
                TP_ScrClsDisplayArea();
                TP_ScrGotoxyEx(30, 31);
                TP_ScrDrLogo(&SpeechVolume[TP_GetVolume(3)][0]);
                TP_TimerSet(2, 1000);
                break;

            case TP_KEY_DOWN:
                volume = TP_GetVolume(3);
                if (volume > 0)
                {
                    TP_SetVolume(3, volume - 1);
                }

                TP_SetDisplayArea(30, 31, 97, 50);
                TP_ScrClsDisplayArea();
                TP_ScrGotoxyEx(30, 31);
                TP_ScrDrLogo(&SpeechVolume[TP_GetVolume(3)][0]);
                TP_TimerSet(2, 1000);
                break;
            }
        }
    }
}


static void stkCallActive(void)
{
    uint8 str[256];
    uint8 CallTimes[10];
    uint8 KeyCode;
    uint8 volume;

    TP_ScrCls();
    TP_Kbflush();
    TP_ScrAttrSet(0);

    TP_ScrGotoxyEx(3,49);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(1,48,126,63);
    TP_LcdPrintf((uint8*)"              Cancel");

    memset(str,0,sizeof(str));
    while (TRUE)
    {
        switch (TP_GetCallStatus())
        {
            case TP_CALLCONNECTED://Connected
                ///Once connected, needs to display on calling and timing
                if (TP_TimerCheck(2) == 0)
                {
                    TP_ScrCls();

                    TP_ScrGotoxyEx(3,49);
                    TP_ScrFontSet(CFONT);
                    TP_SetDisplayArea(1,48,126,63);
                    TP_LcdPrintf((uint8*)"              Cancel");
                }
                TP_ScrDrawRect(0, 0, 127, 63);//Draw the window's border
                TP_ScrGotoxyEx(3,8);
                TP_ScrFontSet(CFONT);
                TP_SetDisplayArea(3,8,126,20);
                TP_ScrClsDisplayArea();
                TP_LcdPrintf((uint8*)"Connected");

                TP_ScrGotoxyEx(3,21);
                TP_ScrFontSet(CFONT);
                TP_SetDisplayArea(3,21,126,33);
                TP_ScrClsDisplayArea();
                memset(CallTimes,0,sizeof(CallTimes));
                TP_GetFormatCallTime(CallTimes);
                TP_LcdPrintf(CallTimes);
                break;

            case TP_HANGUP:
                TP_ScrCls();//Clear the screen
                TP_ScrDrawRect(0, 0, 127, 63);//Draw the window's border
                TP_ScrGotoxyEx(3,8);
                TP_ScrFontSet(CFONT);
                TP_SetDisplayArea(3,8,126,20);
                TP_ScrClsDisplayArea();
                TP_LcdPrintf((uint8*)"Call End ... ...");
                TP_TimerSet(2, 1000);
                while (TP_TimerCheck(2) != 0);
                return;
                break;
        }

        if (strlen((char*)str) > 0)
        {
            TP_ScrGotoxyEx(3, 34);
            TP_ScrFontSet(CFONT);
            TP_SetDisplayArea(3, 34, 126, 46);
            TP_ScrClsDisplayArea();
            TP_LcdPrintf(str);
        }

        ///Key Operation
        if (TP_Kbhit())
        {
            KeyCode = TP_GetKey();

            switch (KeyCode)
            {
            case TP_KEY_HANDFREE: //Under this situation, on-hook
            case TP_KEY_ONHOOK:
            case STK_KEY_CANCEL:
                TP_HangUpSTK();
                break;

            case TP_KEY_1 :
                strcat((char*)str,"1");
                TP_SendDtmf(KeyCode);
                break;
            case TP_KEY_2  :
                strcat((char*)str,"2");
                TP_SendDtmf(KeyCode);
                break;
            case TP_KEY_3  :
                strcat((char*)str,"3");
                TP_SendDtmf(KeyCode);
                break;
            case TP_KEY_4  :
                strcat((char*)str,"4");
                TP_SendDtmf(KeyCode);
                break;
            case TP_KEY_5  :
                strcat((char*)str,"5");
                TP_SendDtmf(KeyCode);
                break;
            case TP_KEY_6  :
                strcat((char*)str,"6");
                TP_SendDtmf(KeyCode);
                break;
            case TP_KEY_7  :
                strcat((char*)str,"7");
                TP_SendDtmf(KeyCode);
                break;
            case TP_KEY_8  :
                strcat((char*)str,"8");
                TP_SendDtmf(KeyCode);
                break;
            case TP_KEY_9  :
                strcat((char*)str,"9");
                TP_SendDtmf(KeyCode);
                break;
            case TP_KEY_0  :
                strcat((char*)str,"0");
                TP_SendDtmf(KeyCode);
                break;
            case TP_KEY_STAR  :
                strcat((char*)str,"*");
                TP_SendDtmf(KeyCode);
                break;
            case TP_KEY_POUND :
                strcat((char*)str,"#");
                TP_SendDtmf(KeyCode);
                break;

            case TP_KEY_UP:
                volume = TP_GetVolume(3);
                if (volume < 7)
                {
                    TP_SetVolume(3, volume + 1);
                }

                TP_SetDisplayArea(30, 31, 97, 50);
                TP_ScrClsDisplayArea();
                TP_ScrGotoxyEx(30, 31);
                TP_ScrDrLogo(&SpeechVolume[TP_GetVolume(3)][0]);
                TP_TimerSet(2, 1000);
                break;

            case TP_KEY_DOWN:
                volume = TP_GetVolume(3);
                if (volume > 0)
                {
                    TP_SetVolume(3, volume - 1);
                }

                TP_SetDisplayArea(30, 31, 97, 50);
                TP_ScrClsDisplayArea();
                TP_ScrGotoxyEx(30, 31);
                TP_ScrDrLogo(&SpeechVolume[TP_GetVolume(3)][0]);
                TP_TimerSet(2, 1000);
                break;

            }
        }
    }
}


static TP_ListBox* stkConstructMenu(TP_STKActionE ucRecvCmdP)
{
    uint8   ucRet ,ucCur;
    TP_STKStringT   str1;
    TP_ListBox *  pListbox;
    TP_ListBoxRect rect = { 0 , 0, 127, 63};

    ucRet = TP_STKGetMenuItem(ucRecvCmdP, 0, (TP_STKStringT *)&str1);  //Ã»²Ëµ¥Ïî¿en¡µÄ¡£
    if(ucRet != 0)
        return   NULL;

    memset((void *)&str1, 0 ,sizeof(str1));
    ucRet = TP_STKGetMenuTitle(ucRecvCmdP, (TP_STKStringT *)&str1);

    if (str1.nTextChars != 0)
        pListbox = TP_NewListBox(TL_NO_EFFECT, rect, (uint8*)str1.textChars, NULL ,NULL); //TL_LIST
    else
        pListbox = TP_NewListBox(TL_NO_EFFECT, rect, (uint8*)NULL, NULL ,NULL); //TL_LIST

    if(pListbox == NULL)
        return   NULL;

    pListbox->ConfirmKey = STK_KEY_CONFIRM;
    pListbox->CancelKey  = STK_KEY_CANCEL;
    pListbox->SelectKey  = STK_KEY_OK;

    for(ucCur = 0; ucCur < 32 ;ucCur++)
    {
        ucRet = TP_STKGetMenuItem  (ucRecvCmdP, ucCur, (TP_STKStringT *)&str1);
        if(ucRet != 0)
            break;
        if(str1.nTextChars ==0)
            break;

        TP_ListBoxAddItem(pListbox, (uint8*)str1.textChars, str1.extParam, 0,NULL);
    }

    return  pListbox;
}



//·µ»ØÖµ;ÊäÈëÊý¾ÝµÄ³¤¶È¡£0£­Ã»ÊäÈë
static uint8 stkInputData(const TP_STKGetInputReqT* pGetInputReq, TP_STKStringT* pInputString)
{
    TP_CharSetT currentCharSet;
    uint16      ucs2Buf[256];
    uint8       ucLen1;
    uint8       result;

    TP_ScrFontSet(CFONT);
    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangle of Window

    TP_SetDisplayArea(2,2,125,15);
    TP_ScrGotoxyEx(3,3);
    TP_LcdPrintf((uint8*)pGetInputReq->dispMsg.textChars);

    TP_ScrDrawRect(2, 16, 125, 44);//ÊäÈë¿ò£¬   //TP_ScrDrawRect(3, 3, 125, 48);//Êý×ÖÊäÈë¡£

    TP_SetDisplayArea(2,46,126,63);
    TP_ScrGotoxyEx(3,49);
    TP_LcdPrintf((uint8*)"Confirm       Cancel");

    TP_ShowInputNum(1,56,47);   //ÏÔÊ¾Òn¾­ÊäÈëµÄ¸öÊýºi×ÜµÄ¸öÊý // 80,1
    TP_SetInputModePosition(59,55);

    TP_SetDisplayArea(4,17,124,43);
    TP_ScrGotoxyEx(4,17); //

    memset((void*)pInputString, 0 ,sizeof(TP_STKStringT));

    TP_Kbflush();

    if (pGetInputReq->inputType == STK_INPUTTYPE_DIGIT)
    {
        TP_SetInputModeControl(STK_KEY_CONFIRM, STK_KEY_CANCEL, TP_KEY_NONE);

        if (pGetInputReq->isPwd == FALSE)
        {
            result = TP_GetString((char*)(pInputString->textChars),
                                  0xF4, pGetInputReq->minChars, pGetInputReq->maxChars);
        }
        else
        {
            result = TP_GetPasswordTimed((char*)(pInputString->textChars),
                                         pGetInputReq->minChars, pGetInputReq->maxChars,
                                         0xF401, '*',
                                         0);
        }

        TP_Kbflush();

        if (result == 0xFF)
        {
            return 0;
        }

         pInputString->extParam   = STK_INPUTTYPE_DIGIT;
         pInputString->nTextChars = strlen((char*)pInputString->textChars);

        if (pGetInputReq->inputEncoding == 0)
        {
            pInputString->textEncoding = 0;
        }
        else
        {
            pInputString->textEncoding = 1;
            pInputString->nTextChars = TP_ScrToUnicode((uint8*)pInputString->textChars,
                                                       pInputString->textChars,
                                                       pInputString->nTextChars);
        }
    }
    else if (pGetInputReq->inputType == STK_INPUTTYPE_ALPHABET)
    {
        TP_GetCurrentCharSet(&currentCharSet);

        if (currentCharSet.category == SINGLE_BYTE_CHARSET)
        {
            TP_SetInputModeControl(STK_KEY_CONFIRM, STK_KEY_CANCEL, STK_KEY_POUND);
            while (TRUE)
            {
                result = TP_GetHzString((char*)pInputString->textChars,
                                        0,
                                        pGetInputReq->maxChars);
                if (result == 0xFB)
                {
                    if (strlen((char*)pInputString->textChars) >= pGetInputReq->minChars)
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            if (result == 0xFF)
            {
                return 0;
            }

            ucLen1 = strlen((char*)pInputString->textChars);

            if (pGetInputReq->inputEncoding == 1)
            {
                ucLen1 = TP_ScrToUnicode((uint8*)pInputString->textChars, pInputString->textChars, ucLen1);
                pInputString->textEncoding = 1;
            }
            else
            {
                pInputString->textEncoding = 0;
            }
            pInputString->nTextChars   = ucLen1;
            pInputString->extParam     = STK_INPUTTYPE_ALPHABET;
        }
        else
        {
            if (pGetInputReq->inputEncoding == 0)
            {
                TP_SetInputModeControl(STK_KEY_CONFIRM, STK_KEY_CANCEL, TP_KEY_NONE);
                TP_SetInputMode(INPUTMODE_LOWCASE);
                while (TRUE)
                {
                    result = TP_GetHzString((char*)pInputString->textChars,
                                            0,
                                            pGetInputReq->maxChars);
                    if (result == 0xFB)
                    {
                        if (strlen((char*)pInputString->textChars) >= pGetInputReq->minChars)
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                if (result == 0xFF)
                {
                    return 0;
                }

                ucLen1 = strlen((char*)pInputString->textChars);

                pInputString->textEncoding = 0;
                pInputString->nTextChars   = ucLen1;
                pInputString->extParam     = STK_INPUTTYPE_ALPHABET;
            }
            else
            {
                TP_SetInputModeControl(STK_KEY_CONFIRM, STK_KEY_CANCEL, STK_KEY_POUND);
                while (TRUE)
                {
                    result = TP_GetHzString((char*)pInputString->textChars,
                                            0,
                                            pGetInputReq->maxChars);
                    if (result == 0xFB)
                    {
                        memset(ucs2Buf, 0x00, sizeof(ucs2Buf));
                        TP_ScrToUnicode((uint8*)pInputString->textChars, ucs2Buf, 255);
                        if (STRLEN(ucs2Buf) >= pGetInputReq->minChars)
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                if (result == 0xFF)
                {
                    return 0;
                }

                TP_ScrToUnicode((uint8*)pInputString->textChars, pInputString->textChars, 255);

                ucLen1 = STRLEN(pInputString->textChars);
                pInputString->textEncoding = 1;
                pInputString->nTextChars   = ucLen1;
                pInputString->extParam     = STK_INPUTTYPE_ALPHABET;
            }
        }
    }

    TP_Kbflush();

    return  pInputString->nTextChars;
}


static uint8 stkInputKey(const TP_STKGetInkeyReqT* pGetInkeyReq, TP_STKStringT* pInputString)
{
    TP_CharSetT currentCharSet;
    uint16      ucs2Buf[256];
    uint8       ucLen1;
    uint8       result;

    TP_ScrFontSet(CFONT);
    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangle of Window

    TP_SetDisplayArea(2,2,125,15);
    TP_ScrGotoxyEx(3,3);
    TP_LcdPrintf((uint8*)pGetInkeyReq->dispMsg.textChars);

    TP_ScrDrawRect(2, 16, 125, 44);//ÊäÈë¿ò£¬   //TP_ScrDrawRect(3, 3, 125, 48);//Êý×ÖÊäÈë¡£

    TP_SetDisplayArea(2,46,126,63);
    TP_ScrGotoxyEx(3,49);
    TP_LcdPrintf((uint8*)"Confirm       Cancel");

    TP_ShowInputNum(1,56,47);   //ÏÔÊ¾Òn¾­ÊäÈëµÄ¸öÊýºi×ÜµÄ¸öÊý // 80,1
    TP_SetInputModePosition(59,55);

    TP_SetDisplayArea(4,17,124,43);
    TP_ScrGotoxyEx(4,17); //

    memset((void*)pInputString, 0 ,sizeof(TP_STKStringT));

    TP_Kbflush();

    if (pGetInkeyReq->inputType == STK_INPUTTYPE_DIGIT)
    {
        TP_SetInputModeControl(STK_KEY_CONFIRM, STK_KEY_CANCEL, TP_KEY_NONE);

        result = TP_GetString((char*)(pInputString->textChars),
                              0xF4, 1, 1);

        TP_Kbflush();

        if (result == 0xFF)
        {
            return 0;
        }

        pInputString->extParam   = STK_INPUTTYPE_DIGIT;
        pInputString->nTextChars = strlen((char*)pInputString->textChars);

        if (pGetInkeyReq->inputEncoding == 0)
        {
            pInputString->textEncoding = 0;
        }
        else
        {
            pInputString->textEncoding = 1;
            TP_ScrToUnicode((uint8*)pInputString->textChars,
                            pInputString->textChars,
                            pInputString->nTextChars);
        }
    }
    else if (pGetInkeyReq->inputType == STK_INPUTTYPE_ALPHABET)
    {
        TP_GetCurrentCharSet(&currentCharSet);

        if (currentCharSet.category == SINGLE_BYTE_CHARSET)
        {
            TP_SetInputModeControl(STK_KEY_CONFIRM, STK_KEY_CANCEL, STK_KEY_POUND);
            while (TRUE)
            {
                result = TP_GetHzString((char*)pInputString->textChars,
                                        0,
                                        1);
                if (result == 0xFB)
                {
                    if (strlen((char*)pInputString->textChars) >= 1)
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            if (result == 0xFF)
            {
                return 0;
            }

            ucLen1 = strlen((char*)pInputString->textChars);

            if (pGetInkeyReq->inputEncoding == 1)
            {
                TP_ScrToUnicode((uint8*)pInputString->textChars, pInputString->textChars, ucLen1);
                pInputString->textEncoding = 1;
            }
            else
            {
                pInputString->textEncoding = 0;
            }
            pInputString->nTextChars   = ucLen1;
            pInputString->extParam     = STK_INPUTTYPE_ALPHABET;
        }
        else
        {
            if (pGetInkeyReq->inputEncoding == 0)
            {
                TP_SetInputModeControl(STK_KEY_CONFIRM, STK_KEY_CANCEL, TP_KEY_NONE);
                TP_SetInputMode(INPUTMODE_LOWCASE);
                while (TRUE)
                {
                    result = TP_GetHzString((char*)pInputString->textChars,
                                            0,
                                            1);
                    if (result == 0xFB)
                    {
                        if (strlen((char*)pInputString->textChars) >= 1)
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                if (result == 0xFF)
                {
                    return 0;
                }

                ucLen1 = strlen((char*)pInputString->textChars);

                pInputString->textEncoding = 0;
                pInputString->nTextChars   = ucLen1;
                pInputString->extParam     = STK_INPUTTYPE_ALPHABET;
            }
            else
            {
                TP_SetInputModeControl(STK_KEY_CONFIRM, STK_KEY_CANCEL, STK_KEY_POUND);
                while (TRUE)
                {
                    result = TP_GetHzString((char*)pInputString->textChars,
                                            0,
                                            1);
                    if (result == 0xFB)
                    {
                        memset(ucs2Buf, 0x00, sizeof(ucs2Buf));
                        TP_ScrToUnicode((uint8*)pInputString->textChars, ucs2Buf, 255);
                        if (STRLEN(ucs2Buf) >= 1)
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                if (result == 0xFF)
                {
                    return 0;
                }

                TP_ScrToUnicode((uint8*)pInputString->textChars, pInputString->textChars, 255);
                ucLen1 = STRLEN(pInputString->textChars);

                pInputString->textEncoding = 1;
                pInputString->nTextChars   = ucLen1;
                pInputString->extParam     = STK_INPUTTYPE_ALPHABET;
            }
        }
    }

    TP_Kbflush();

    return  pInputString->nTextChars;
}

static void displayLine(const char* pLine, uint8 style, uint8 yStart)
{
    uint16 lineWidth;
    uint16 areaWidth;
    uint8  xPos;

    lineWidth = strlen(pLine) * CHAR_WIDTH;
    areaWidth = 114 - 13 + 1;
    xPos      = 13;
    if (style == ALIGN_MIDDLE)
    {
        xPos += ((areaWidth - lineWidth) / 2);
    }
    else if (style == ALIGN_RIGHT)
    {
        xPos += (areaWidth - lineWidth);
    }

    TP_ScrGotoxyEx(xPos, yStart);
    TP_LcdPrintf((uint8*)"%s\x0", pLine);
}

static void initDisplayPage(void)
{
    TP_ScrAttrSet(0);
    TP_ScrSpaceSet(0, 0);
    TP_SetDisplayArea(0, 0, 127, 63);

    TP_LcdFreeze();
    TP_ScrDrawRect(0, 0, 127, 63);
    TP_ScrClrRect (1, 1, 126, 62);
}

static void displayButton(void)
{
    TP_ScrGotoxyEx(3, 50);
    TP_LcdPrintf((uint8*)"Confirm");
    TP_ScrGotoxyEx(86, 50);
    TP_LcdPrintf((uint8*)"Cancel");
}


//»æÖÆ¹ö¶¯Ìõ
static void drawScrBar(TP_ListBoxRect* ScrRect, int32 StartLine, float BlockHight)
{
    TP_SetDisplayArea( ScrRect->left, ScrRect->top, ScrRect->right,ScrRect->bottom );
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect( ScrRect->left, ScrRect->top, ScrRect->right,ScrRect->bottom);

    if ( BlockHight < 1 )
    {
        TP_ScrFillRect(ScrRect->left,
                       (uint32)ScrRect->top +
                       (uint32)((float)StartLine * BlockHight),
                       ScrRect->right,
                       (uint32)ScrRect->top +
                       (uint32)((float)StartLine * BlockHight) +1);
    }
    else
    {
        TP_ScrFillRect(ScrRect->left,
                       (uint32)ScrRect->top +
                       (uint32)((float)StartLine * BlockHight),
                       ScrRect->right,
                       (uint32)ScrRect->top +
                       (uint32)((float)(StartLine + 1) * BlockHight));
    }
}

static uint8 StkShowQueryDialog(const char* pQueryStr, uint32 timeout)
{
    TP_ListBoxRect borderRect;
    uint8 yPos = 1; //
    uint8 userKey;
    int32 StartLine = 0;
    Boolean ifDrawScrBar = FALSE;
    float BlockHight = 0;
    int32 LineCount = 0;
    int32 MaxRow = 0;
    int32 MaxCharInRow = 0;
    int32 CopyLen = 0;
    char* (*strAddrBuf)[] = PNULL;
    char* temp = PNULL;

    TP_AllocMemory( sizeof(char*) * 10, (void**)&strAddrBuf );
    if (PNULL == strAddrBuf )
    {
        return 0xFB;
    }
    memset( strAddrBuf, 0, sizeof(char*) * 10);

    TP_AllocMemory(256, (void**)&temp);
    if (PNULL == temp )
    {
        TP_FreeMemory((void**)&strAddrBuf );
        return 0xFB;
    }
    memset(temp, 0, 256);

    initDisplayPage();

    if (timeout == 0)
    {
        displayButton();
    }

    //yPos += (CHAR_HEIGHT + 1);
    //yPos += 1;

    //TP_ScrDrawRect(10, yPos, 117, 47);
    TP_ScrDrawRect(0, 0, 127, 48);

    borderRect.top    = 0;
    borderRect.bottom = 48;
    borderRect.left   = 123;
    borderRect.right  = 127;
    TP_SetDisplayArea(3, yPos + 2, 120, 45);

    MaxRow       = TP_GetMaxRowInScreen();
    MaxCharInRow = TP_GetMaxCharInRow();

    LineCount = SplitTextToBuff((uint8*)pQueryStr, strAddrBuf, 10);
    LineCount = min(LineCount, 10);
    if (LineCount > MaxRow)
    {
        ifDrawScrBar = TRUE;
        BlockHight   = (float)(47 - yPos) / (float)( LineCount - MaxRow + 1);
    }

    CopyLen = min(strlen((char*)(*strAddrBuf)[StartLine]), MaxRow * (MaxCharInRow + 1));
    memcpy(temp, (*strAddrBuf)[StartLine], CopyLen);
    temp[CopyLen] = 0;

    TP_ShowTextSingleLine(3, yPos + 2, 120, 45, CFONT, ALIGN_LEFT, (uchar*)"%s", (uchar*)temp);
    if (TRUE == ifDrawScrBar)
    {
        drawScrBar(&borderRect, StartLine, BlockHight);
    }

    TP_ScrUpdate();

    if (timeout > 0)
    {
        TP_TimerSet(0, timeout);
    }

    TP_Kbflush();
    while (TRUE)
    {
        if (TP_Kbhit() == 0xFF)
        {
            timeout = 0;

            userKey = TP_GetKey();
            TP_Kbflush();
            if (userKey == STK_KEY_CONFIRM)
            {
                TP_FreeMemory((void**)&strAddrBuf);
                TP_FreeMemory((void**)&temp);
                return 0x00;
            }
            else
            if (userKey == STK_KEY_CANCEL)
            {
                TP_FreeMemory((void**)&strAddrBuf);
                TP_FreeMemory((void**)&temp);
                TP_ScrFontSet( CFONT );
                return 0x01;
            }
            else
            if (userKey == TP_KEY_UP || userKey == TP_KEY_DOWN )
            {
                if (userKey == TP_KEY_DOWN)
                {
                    if (StartLine < (LineCount - MaxRow))
                    {
                        ++StartLine;
                    }
                }
                else
                {
                    if (StartLine > 0 )
                    {
                        --StartLine;
                    }
                }

                CopyLen = min( strlen((char*)(*strAddrBuf)[StartLine]), MaxRow * (MaxCharInRow + 1));
                memcpy(temp, (*strAddrBuf)[StartLine], CopyLen);
                temp[CopyLen] = 0;

                TP_LcdFreeze();

                TP_ShowTextSingleLine(3, yPos + 2, 120, 45,
                                      CFONT, ALIGN_LEFT,(uchar*)"%s", (uchar*)temp);

                if (TRUE == ifDrawScrBar)
                {
                    drawScrBar(&borderRect, StartLine, BlockHight);
                }
                TP_ScrUpdate();
            }
        }

        if (timeout > 0)
        {
            if (TP_TimerCheck(0) == 0)
            {
                TP_FreeMemory((void**)&strAddrBuf);
                TP_FreeMemory((void**)&temp);

                return 0x00;
            }
        }
    }
}


/******************************************************************************
* Public Functions
*******************************************************************************/
void TB_STK_Enter(void)
{
    TP_ListBox*   pListMenu;
    Boolean       bRetUsr1;
    Boolean       bRet1;
    uint8         ucaData1[50];
    TP_STKActionE ucRecvCmdCode;
    uint8         ucStkRet;
    uint16        uiLen1;
    uint8         KeyCode;
    uint8         ucRet1 ;

    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    pListMenu  = PNULL;

STKAPPSTART1:
    ucRet1 = TP_STKStart(&TPstStkCmdReq1);
    if(ucRet1 != 0)
    {
        ShowListSelectDialog(3000, (uint8*)"STK Not Initialized");
        return;
    }

    for(;;)
    {
        ucRecvCmdCode = TPstStkCmdReq1.reqAction;

        memset((void *)&TPstStkCmdRsp1, 0 ,sizeof(TPstStkCmdRsp1));
        TPstStkCmdRsp1.rspAction = ucRecvCmdCode;

        switch(ucRecvCmdCode)
        {
    //´´½¨Ö÷²Ëµ¥»òÕß ×o²Ëµ¥£¬È»ºoÏÔÊ¾¡£
        case STKACTION_SHOWMAINMENU:  //Ö÷²Ëµ¥
        case STKACTION_SELECTITEM: //×o²Ëµ¥
            {
                TP_STKSelectItemRspT* pSelItemRsp;

                pSelItemRsp = (TP_STKSelectItemRspT *)&TPstStkCmdRsp1.rspData.SelectItem;

                if(pListMenu != PNULL)
                {
                    TP_ListBoxFree(pListMenu);
                }
                pListMenu = stkConstructMenu(ucRecvCmdCode);
                if(pListMenu == PNULL)
                {
                    goto  STKAPPEXIT1;
                }

                ucRet1 = TP_ShowListBox(pListMenu);
                if (ucRet1 == FALSE)
                {
                    if (TP_STKIsStarted() == FALSE)
                    {
                        goto  STKAPPEXIT1;
                    }

                    pSelItemRsp->result = STKRSLT_BackwardMove;  // STKRSLT_TerminatedByUser=oÃ»§ÖÕÖ¹¡£
                }
                else
                {
                    pSelItemRsp->result = STKRSLT_OK;  //·µ»Ø
                    pSelItemRsp->selectedItem = (uint8)pListMenu->SelectItemValue[0];
                }
            }
            break;

        case STKACTION_GETINPUT:
            {
                TP_STKGetInputReqT* pGetInputReq;
                TP_STKGetInputRspT* pGetInputRsp;

                pGetInputReq = (TP_STKGetInputReqT*)&TPstStkCmdReq1.reqData.GetInput;
                pGetInputRsp = (TP_STKGetInputRspT*)&TPstStkCmdRsp1.rspData.GetInput;

                ucRet1 = stkInputData(pGetInputReq, (TP_STKStringT*)&pGetInputRsp->inputData);

                if (ucRet1 == 0)
                {
                    pGetInputRsp->result = STKRSLT_BackwardMove;  //·µ»Ø
                }
                else
                {
                    pGetInputRsp->result = STKRSLT_OK;
                }
            }
            break;


        case STKACTION_GETAKEY:
            {
                TP_STKGetInkeyReqT* pGetAKeyReq;
                TP_STKGetInkeyRspT* pGetAKeyRsp;

                pGetAKeyReq = (TP_STKGetInkeyReqT*)&TPstStkCmdReq1.reqData.GetInkey;
                pGetAKeyRsp = (TP_STKGetInkeyRspT*)&TPstStkCmdRsp1.rspData.GetInkey;

                if (pGetAKeyReq->dispMsg.nTextChars == 0)
                {
                    pGetAKeyReq->dispMsg.textEncoding = 0;
                    strcpy((char*)pGetAKeyReq->dispMsg.textChars , (char *)"Press A Key");
                    pGetAKeyReq->dispMsg.nTextChars = strlen((char*)pGetAKeyReq->dispMsg.textChars);
                }

                if(pGetAKeyReq->isYesNo == TRUE)  //ES NO
                {
                    TP_Kbflush();   //ÏÈÇåÆa
                    TP_ScrAttrSet(0);
                    TP_ScrFontSet(CFONT);
                    TP_SetDisplayArea(0,0,127,63);
                    TP_ScrClsDisplayArea();

                    TP_ScrDrawRect(1,1,126, 48); //ÏÔÊ¾ÄuÈÝ
                    TP_SetDisplayArea(2,2,125,48);
                    TP_ScrGotoxyEx(2,2);
                    TP_LcdPrintf((uint8*)pGetAKeyReq->dispMsg.textChars);

                    TP_ScrDrawRect(1,50,126, 63); //ÏÔÊ¾ n¡Ôn²Ëµ¥
                    TP_SetDisplayArea(2,51,124,62);
                    TP_ScrGotoxyEx(3,51);
                    strcpy((char *)ucaData1, (char *)"Confirm       Cancel");
                    TP_LcdPrintf(ucaData1);

                    KeyCode = 0;
                    TP_Kbflush();
                    while (TRUE)
                    {
                        if (TP_Kbhit())
                        {
                            KeyCode = TP_GetKey();
                            TP_Kbflush();
                            if (KeyCode == STK_KEY_CONFIRM)
                            {
                                pGetAKeyRsp->result = STKRSLT_OK;
                                pGetAKeyRsp->inputData.textChars[0] = 0x01;

                                break;
                            }
                            else if (KeyCode == STK_KEY_CANCEL)
                            {
                                pGetAKeyRsp->result = STKRSLT_OK;
                                pGetAKeyRsp->inputData.textChars[0] = 0x00;

                                break;
                            }
                        }
                    }
                    pGetAKeyRsp->inputData.nTextChars   = 1;
                    pGetAKeyRsp->inputData.textEncoding = 0;
                }
                else
                {
                    ucRet1 = stkInputKey(pGetAKeyReq, &pGetAKeyRsp->inputData);
                    if(ucRet1 == 0)
                    {
                        pGetAKeyRsp->result = STKRSLT_BackwardMove;  //·µ»Ø
                    }
                    else
                    {
                        pGetAKeyRsp->result = STKRSLT_OK;
                    }
                }
            }
            break;


        case  STKACTION_DISPLAYTEXT:
            {
                Boolean   ucMode1;
                TP_STKDisplayTextReqT* pDispTextReq;
                TP_STKDisplayTextRspT* pDispTextRsp;

                pDispTextReq = (TP_STKDisplayTextReqT*)&TPstStkCmdReq1.reqData.DisplayText;
                pDispTextRsp = (TP_STKDisplayTextRspT*)&TPstStkCmdRsp1.rspData.DisplayText;
                ucMode1= pDispTextReq->needUserRsp;

                if (ucMode1 == FALSE)
                {
                    StkShowQueryDialog((char*)pDispTextReq->dispMsg.textChars, 3 * 1000);
                    KeyCode = 0;
                }
                else
                {
                    KeyCode = StkShowQueryDialog((char*)pDispTextReq->dispMsg.textChars, 0);
                }

                if (KeyCode == 0) //È·ÈÏ
                {
                    pDispTextRsp->userRsp = STKRSLT_OK;
                }
                else
                {
                    pDispTextRsp->userRsp = STKRSLT_BackwardMove;
                }
            }
            break;

        case  STKACTION_SENDSMS:
            {
                TP_STKSendSMSReqT* pSendSmsReq;
                TP_STKSendSMSRspT* pSendSmsRsp;

                pSendSmsReq = (TP_STKSendSMSReqT *)&TPstStkCmdReq1.reqData.SendSMS;
                pSendSmsRsp = (TP_STKSendSMSRspT *)&TPstStkCmdRsp1.rspData.SendSMS;

                if(pSendSmsReq->dispMsg.nTextChars == 0)
                {
                    pSendSmsReq->dispMsg.textEncoding = 0;
                    strcpy((char *)pSendSmsReq->dispMsg.textChars, (char*)"Sms Sending...");
                    pSendSmsReq->dispMsg.nTextChars = strlen((char *)pSendSmsReq->dispMsg.textChars);
                }

                TP_Kbflush();
                TP_ScrAttrSet(0);
                TP_ScrFontSet(CFONT);
                TP_SetDisplayArea(0,0,127,63);
                TP_ScrClsDisplayArea();
                TP_ScrDrawRect(1,1,126, 48); //ÏÔÊ¾ÄuÈÝ
                TP_SetDisplayArea(2,2,125,48);
                TP_ScrGotoxyEx(2,2);
                TP_LcdPrintf((uint8*)pSendSmsReq->dispMsg.textChars);

                TP_ScrDrawRect(1,50,126, 63); //ÏÔÊ¾ n¡Ôn²Ëµ¥
                TP_SetDisplayArea(2,51,124,62);
                TP_ScrGotoxyEx(3,51);
                strcpy((char *)ucaData1, (char *)"Confirm       Cancel");
                TP_LcdPrintf(ucaData1);

                KeyCode = 0;
                TP_TimerSet(0, 3000);
                TP_Kbflush();
                while(1)
                {
                    if (TP_Kbhit())
                    {
                        KeyCode = TP_GetKey();
                        if ((KeyCode == STK_KEY_CONFIRM) || (KeyCode == STK_KEY_CANCEL))
                            break;
                    }

                    if (TP_TimerCheck(0) == 0)
                    {
                        KeyCode = STK_KEY_CONFIRM;

                        break;
                    }
                }

                if(KeyCode == STK_KEY_CONFIRM)
                {
                    pSendSmsRsp->result = STKRSLT_OK;
                    //TP_DbgSerialPrn((char *)"++++STKCMD_SendSms  TP_KEY_CONFIRM\xd\xa");
                }
                else
                {
                    pSendSmsRsp->result = STKRSLT_BackwardMove;
                    //TP_DbgSerialPrn((char *)"++++STKCMD_SendSms  TP_KEY_CANCEL\xd\xa");
                }
            }
            break;

        case  STKACTION_SENDUSSD:
            {
                TP_STKUSSDCallReqT* pUssdCallReq;
                TP_STKUSSDCallRspT* pUssdCallRsp;

                pUssdCallReq = (TP_STKUSSDCallReqT *)&TPstStkCmdReq1.reqData.USSDCall;
                pUssdCallRsp = (TP_STKUSSDCallRspT *)&TPstStkCmdRsp1.rspData.USSDCall;

                if(pUssdCallReq->dispMsg.nTextChars == 0)
                {
                    pUssdCallReq->dispMsg.textEncoding = 0;
                    strcpy((char *)pUssdCallReq->dispMsg.textChars, (char*)"USSD Sending...");
                    pUssdCallReq->dispMsg.nTextChars = strlen((char *)pUssdCallReq->dispMsg.textChars);
                }

                TP_Kbflush();
                TP_ScrAttrSet(0);
                TP_ScrFontSet(CFONT);
                TP_SetDisplayArea(0,0,127,63);
                TP_ScrClsDisplayArea();
                TP_ScrDrawRect(1,1,126, 48); //ÏÔÊ¾ÄuÈÝ
                TP_SetDisplayArea(2,2,125,48);
                TP_ScrGotoxyEx(2,2);
                TP_LcdPrintf((uint8*)pUssdCallReq->dispMsg.textChars);

                TP_ScrDrawRect(1,50,126, 63); //ÏÔÊ¾ n¡Ôn²Ëµ¥
                TP_SetDisplayArea(2,51,124,62);
                TP_ScrGotoxyEx(3,51);
                strcpy((char *)ucaData1, (char *)"Confirm       Cancel");
                TP_LcdPrintf(ucaData1);

                KeyCode = 0;
                TP_TimerSet(0, 3000);

                TP_Kbflush();
                while(1)
                {
                    if (TP_Kbhit())
                    {
                        KeyCode = TP_GetKey();
                        TP_Kbflush();
                        if ((KeyCode == STK_KEY_CONFIRM) || (KeyCode == STK_KEY_CANCEL))
                            break;
                    }

                    if (TP_TimerCheck(0) == 0)
                    {
                        KeyCode = STK_KEY_CONFIRM;

                        break;
                    }
                }

                if (KeyCode == STK_KEY_CONFIRM)
                {
                    pUssdCallRsp->result = STKRSLT_OK;
                }
                else
                {
                    pUssdCallRsp->result = STKRSLT_BackwardMove;
                }
            }
            break;

        case  STKACTION_SENDSS:
            {
                TP_STKSSOperateReqT* pSSOprReq;
                TP_STKSSOperateRspT* pSSOprRsp;

                pSSOprReq = (TP_STKSSOperateReqT *)&TPstStkCmdReq1.reqData.SSOperate;
                pSSOprRsp = (TP_STKSSOperateRspT *)&TPstStkCmdRsp1.rspData.SSOperate;

                if (pSSOprReq->dispMsg.nTextChars == 0)
                {
                    pSSOprReq->dispMsg.textEncoding = 0;
                    strcpy((char *)pSSOprReq->dispMsg.textChars, (char*)"SS Sending...");
                    pSSOprReq->dispMsg.nTextChars = strlen((char*)pSSOprReq->dispMsg.textChars);
                }

                TP_Kbflush();
                TP_ScrAttrSet(0);
                TP_ScrFontSet(CFONT);
                TP_SetDisplayArea(0,0,127,63);
                TP_ScrClsDisplayArea();
                TP_ScrDrawRect(1,1,126, 48); //ÏÔÊ¾ÄuÈÝ
                TP_SetDisplayArea(2,2,125,48);
                TP_ScrGotoxyEx(2,2);
                TP_LcdPrintf((uint8*)pSSOprReq->dispMsg.textChars);

                TP_ScrDrawRect(1,50,126, 63); //ÏÔÊ¾ n¡Ôn²Ëµ¥
                TP_SetDisplayArea(2,51,124,62);
                TP_ScrGotoxyEx(3,51);
                strcpy((char *)ucaData1, (char *)"Confirm       Cancel");
                TP_LcdPrintf(ucaData1);

                KeyCode = 0;
                TP_TimerSet(0, 3000);

                TP_Kbflush();
                while (TRUE)
                {
                    if (TP_Kbhit())
                    {
                        KeyCode = TP_GetKey();
                        TP_Kbflush();
                        if ((KeyCode == STK_KEY_CONFIRM) || (KeyCode == STK_KEY_CANCEL))
                            break;
                    }

                    if (TP_TimerCheck(0) == 0)
                    {
                        KeyCode = STK_KEY_CONFIRM;
                        break;
                    }
                }
                if(KeyCode == STK_KEY_CONFIRM)
                {
                    pSSOprRsp->result = STKRSLT_OK;
                }
                else
                {
                    pSSOprRsp->result = STKRSLT_BackwardMove;
                }
            }
            break;

        case  STKACTION_SETUPCALL:
            {
                TP_STKMakeCallReqT* pMakeCallReq;
                TP_STKMakeCallRspT* pMakeCallRsp;

                pMakeCallReq = (TP_STKMakeCallReqT *)&TPstStkCmdReq1.reqData.MakeCall;
                pMakeCallRsp = (TP_STKMakeCallRspT *)&TPstStkCmdRsp1.rspData.MakeCall;
                if(pMakeCallReq->step == 0)
                {
                    if(pMakeCallReq->dispMsg.nTextChars == 0)
                    {
                        pMakeCallReq->dispMsg.textEncoding = 0;
                        strcpy((char *)pMakeCallReq->dispMsg.textChars , (char *)"Steup Call ...");
                        pMakeCallReq->dispMsg.nTextChars = strlen((char*)pMakeCallReq->dispMsg.textChars);
                    }

                    TP_Kbflush();   //ÏÈÇåÆa
                    TP_ScrAttrSet(0);
                    TP_ScrFontSet(CFONT);
                    TP_SetDisplayArea(0,0,127,63);
                    TP_ScrClsDisplayArea();

                    TP_ScrDrawRect(1,1,126, 48); //ÏÔÊ¾ÄuÈÝ
                    TP_SetDisplayArea(2,2,125,48);
                    TP_ScrGotoxyEx(2,2);
                    TP_LcdPrintf((uint8*)pMakeCallReq->dispMsg.textChars);

                    TP_ScrDrawRect(1,50,126, 63); //ÏÔÊ¾ n¡Ôn²Ëµ¥
                    TP_SetDisplayArea(2,51,124,62);
                    TP_ScrGotoxyEx(3,51);
                    strcpy((char *)ucaData1, (char *)"Confirm       Cancel");
                    TP_LcdPrintf(ucaData1);

                    KeyCode = 0;
                    TP_TimerSet(0, 3000);
                    TP_Kbflush();
                    while(1)
                    {
                        if (TP_Kbhit())
                        {
                            KeyCode = TP_GetKey();
                            if ((KeyCode == STK_KEY_CONFIRM) || (KeyCode == STK_KEY_CANCEL))
                            {
                                break;
                            }
                        }

                        if (TP_TimerCheck(0) == 0)
                        {
                            KeyCode = STK_KEY_CONFIRM;

                            break;
                        }
                    }
                    pMakeCallRsp->step = 1;
                    if(KeyCode == STK_KEY_CONFIRM)
                    {
                        pMakeCallRsp->result = STKRSLT_OK;
                    }
                    else
                    {
                        pMakeCallRsp->result = STKRSLT_BackwardMove;
                    }
                }
                else
                {
                    TP_STKMakeCallRspT* pMakeCallRsp;

                    pMakeCallRsp = (TP_STKMakeCallRspT *)&TPstStkCmdRsp1.rspData.MakeCall;
                    if (stkCallMake() == TRUE)
                    {
                        stkCallActive();
                    }
                    pMakeCallRsp->step = 1; //¿ªÊ¼ºô½ÐaË¡£Ôò²»ÔÙÏÂ·¢ STK Ãüaî£¬¶øµÈ´ý STK µÄ½aÊøÃüaî¡£
                    pMakeCallRsp->result = STKRSLT_BackwardMove;

                }
            }
            break;

        default:
            goto   STKAPPEXIT1;
        }

        ucStkRet = TP_STKRespond((const TP_STKRspT*)&TPstStkCmdRsp1 , (TP_STKReqT*)&TPstStkCmdReq1 );

      //TP_DbgSerialPrn((char *)"TP_STKRespond Done=%02X \xd\xa" , ucStkRet);
        if(ucStkRet == 1) //´¦Ài½aÊøaË¡£
        {
            goto   STKAPPSTART1;
        }
        if(ucStkRet == 2) //¿¨Æ¬´iÎo¡£
        {
            //ShowListSelectDialog(3000,(uint8*)"Stk Card Err!");
            break;
        }
        if(ucStkRet == 3) //²»ÄÜ´¦ÀiµÄÃüaî¡£
        {
            //sprintf((char *)ucaData1, (char *)"²»ÄÜ´¦ÀiµÄÃüaî:%02X", ucRecvCmdCode);
            //ShowListSelectDialog(3000,ucaData1);
            break;
        }
        continue;
    }

  STKAPPEXIT1:
    TP_STKEnd();
    if(pListMenu != NULL)
        TP_ListBoxFree(pListMenu);
    //TP_ScrBackLight(1);
    return;
}


