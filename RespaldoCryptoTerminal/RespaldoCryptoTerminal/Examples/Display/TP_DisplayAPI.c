#include <TPTypedef.h>
#include <TPBase.h>
#include <TPFonts.h>
#include <TPLCD.h>
#include <TPKeypad.h>
#include <TP_Singleime.h>
#include "TP_ListBox.h"

#include "TP_DisplayAPI.h"

#define SHOWTEXT_MAX_LINE 2010

#define min(x, y) ((x) < (y) ? (x) : (y))

Boolean TP_IsGBK(uint8 WordCode);

uint8 AlertImage[5][74] =
{
    {   0x18,
        0x18,
        0x00,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xFE,0x7E,0x3C,0x38,0xF0,0xE0,0xC0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x87,0x0F,0x1F,
        0x3F,0x7F,0x3F,0x1F,0x0F,0x87,0xC3,0xE1,0xF0,0xF8,0xFC,0xFE,0xFF,0xFF,0xFF,0xFF,
        0x00,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0x7E,0xFC,0xF8,0xFC,0xFE,0xFF,0xFF,0xFF,0xFF,
        0x7F,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x00
    },
    {   0x18,
        0x18,
        0x00,0xC0,0xE0,0xF0,0xF8,0xFC,0x3E,0x3E,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0x7E,0x3E,0x3C,0xF8,0xF0,0xE0,0xC0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,
        0x7C,0x38,0x01,0x83,0xC7,0x83,0x01,0x38,0x7C,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0x00,0x03,0x07,0x0F,0x1F,0x3F,0x79,0x78,0xFC,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,
        0x7C,0x78,0x39,0x1F,0x0F,0x07,0x03,0x00
    },
    {
        0x18,
        0x18,
        0x00,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFE,0xFF,0x03,0x03,0x03,0x03,0x03,0x03,0xFF,
        0xFE,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0x00,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0x7F,0xFF,0xC6,0x82,0x82,0x82,0x82,0xC6,0xFF,
        0x7F,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x00
    },
    {
        0x18,
        0x18,
        0x00,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFE,0xFF,0xCF,0x87,0x87,0x87,0x87,0xCF,0xFF,
        0xFE,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xFC,0xFC,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0x00,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0x7F,0xE7,0xE7,0xE0,0xE0,0xE0,0xE0,0xE0,0xE7,
        0x67,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x00
    },
    {
        0x18,
        0x18,
        0x00,0xC0,0xE0,0xF0,0xF8,0xFC,0x7E,0x3E,0x1F,0x8F,0xC7,0xC7,0xC7,0xC7,0x8F,0x0F,
        0x1E,0x3E,0x7C,0xF8,0xF0,0xE0,0xC0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFE,
        0xFE,0xFF,0xFF,0x3F,0x1F,0x0F,0x87,0xE3,0xF0,0xF0,0xF8,0xFF,0xFF,0xFF,0xFF,0xFF,
        0x00,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0x7F,0xFF,0xFF,0xFF,0xC2,0xC2,0xC2,0xFF,0xFF,
        0x7F,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x00
    }
};

static void diplayApi_ScrGotoxyEx(uint8 textDirection,
                                  uint8 left,
                                  uint8 right,
                                  uint8 top)
{
    if(textDirection == 0)
    {
        TP_ScrGotoxyEx(left, top);
    }
    else
    {
        TP_ScrGotoxyEx(right, top);
    }
}


void TP_DrawFootMenu( uint8 left,
                      uint8 top,
                      uint8 right,
                      uint8 bottom,
                      uint8 fontSize,
                      uint8 *leftCaption,
                      uint8 *rightCaption )
{
    TP_CharSetT CurrCharSet = {0};
    int32 leftWidth,rightWidth;

    if(leftCaption == PNULL
       && rightCaption == PNULL)
    {
        return;
    }

    TP_GetCurrentCharSet(&CurrCharSet);
    TP_ScrAttrSet(0);
    TP_ScrFontSet(fontSize);
    TP_SetDisplayArea(left, top, right, bottom);
    TP_ScrClsDisplayArea();

    if(leftCaption != PNULL)
    {
        leftWidth = TP_ScrDisplayWidthGet(leftCaption,
                                          strlen((char*)leftCaption));
        diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                              left,
                              left+leftWidth,
                              top);
        TP_LcdPrintf("%s", leftCaption);
    }
    if(rightCaption != PNULL)
    {
        rightWidth = TP_ScrDisplayWidthGet(rightCaption,
                                           strlen((char*)rightCaption));
        if(CurrCharSet.textDirection == 0)
        {
            if(right>=rightWidth)
            {
                TP_ScrGotoxyEx(right-rightWidth, top);
            }
            else
            {
                TP_ScrGotoxyEx(0, top);
            }
        }
        else
        {
            TP_ScrGotoxyEx(right, top);
        }
        TP_LcdPrintf("%s", rightCaption);
    }

}

void TP_GetFontInfo( ST_FONT *AsciiFont,ST_FONT *CfontFont)
{
    uint8 tempi;
    uint8 FontCnt;
    TP_CharSetT CurrCharSet;
    ST_FONT FontBuf[NUM_OF_CHARSET*6+1];
    memset(FontBuf, 0, sizeof(FontBuf));
    FontCnt = TP_ListFont(FontBuf, NUM_OF_CHARSET*6);
    TP_GetCurrentCharSet(&CurrCharSet);
    for(tempi=0; tempi<FontCnt; tempi++)
    {
        if(FontBuf[tempi].Type == 0 && FontBuf[tempi].CharSet == CurrCharSet.name)
        {
            if(FontBuf[tempi].Width != 0 && FontBuf[tempi].Height != 0)//±£Ö¤Êý¾ÝoÐÐ§ÐÔ
            {
                if(FontBuf[tempi].Size == ASCII)
                {
                    memcpy(AsciiFont, &FontBuf[tempi], sizeof(ST_FONT));
                }
                else if(FontBuf[tempi].Size == CFONT)
                {
                    memcpy(CfontFont, &FontBuf[tempi], sizeof(ST_FONT));
                }
            }
        }
    }

    AsciiFont->CharSet     = CurrCharSet.name;
    AsciiFont->Type        = 0;
    AsciiFont->Size        = 0;
    AsciiFont->Width       = 5;
    AsciiFont->Height      = 7;
    AsciiFont->IsBold      = 0;
    AsciiFont->IsItalic    = 0;
    AsciiFont->IsUnderline = 0;

    if (CurrCharSet.category == SINGLE_BYTE_CHARSET)
    {
        CfontFont->Width *= 2;
    }

}

/*************************************************************************************/
/*************************************Ë½oÐº¯Êý*****************************************/

uint8 TP_GetFontHeight()
{
    uint8 fontSize = 0;
    uint8 fontHeight = 0;
    ST_FONT asciiFont = {0}, cfontFont = {0};

    TP_ScrFontGet(&fontSize);
    TP_GetFontInfo(&asciiFont, &cfontFont);
    if(fontSize == ASCII)
    {
        fontHeight = asciiFont.Height;
        if(fontHeight == 0)
        {
            fontHeight = 7;
        }
    }
    else
    {
        fontHeight = cfontFont.Height;
        if(fontHeight == 0)
        {
            fontHeight = 12;
        }
    }
    return fontHeight;

}

static void SetScrOption()
{
    TP_ScrSpaceSet(0, 0);
    TP_ScrClsPrint(0);
    TP_ScrAttrSet(0);
}

static void DrawText(uint8 left,
                     uint8 top,
                     uint8 right,
                     uint8 bottom,
                     TP_DisplayAlign align,
                     uint8 *text)
{
    int32 textWidth = 0;
    uint8 startPosX = 0;
    uint8 offset = 0;
    TP_CharSetT CurrCharSet = {0};

    if(text == PNULL)
    {
        return;
    }
    textWidth = TP_ScrDisplayWidthGet(text, strlen(text));

    TP_GetCurrentCharSet(&CurrCharSet);
    TP_SetDisplayArea(left, top, right, bottom);
    TP_ScrClsDisplayArea();
    if(textWidth > (right-left+1))
    {
        if(CurrCharSet.textDirection == 0)
        {
            startPosX = left;
        }
        else
        {
            startPosX = right;
        }
    }
    else
    {
        //Ò»ÐÐÄÜÈÝÏÂ£¬¶ÔÆë·½Ê½²ÅoÐÐ§
        if(align == ALIGN_LEFT)
        {
            if(CurrCharSet.textDirection == 0)
            {
                startPosX = left;
            }
            else
            {
                startPosX = left + textWidth;
                if(startPosX > right)
                {
                    startPosX = right;
                }
            }
        }
        else if(align == ALIGN_MIDDLE)
        {
            if( textWidth < (right-left+1))
            {
                offset = (right - left + 1 - textWidth)/2;
            }
            else
            {
                offset = 0;
            }
            if(CurrCharSet.textDirection == 0)
            {
                startPosX = left + offset;
            }
            else
            {
                startPosX = right - offset;
            }
        }
        else
        {
            if(CurrCharSet.textDirection == 0)
            {
                if(right > textWidth)
                {
                    startPosX = right - textWidth;
                }
                else
                {
                    startPosX = 0;
                }
                if(startPosX < left)
                {
                    startPosX = left;
                }
            }
            else
            {
                startPosX = right;
            }
        }
    }
    TP_ScrGotoxyEx(startPosX, top);
    TP_LcdPrintf("%s", text);

}


//Êý×ÖÏÔÊ¾³¤¶È±ÈÈç£º50ÏÔÊ¾³¤¶ÈÎª2£¬100ÏÔÊ¾³¤¶ÈÎª3
static uint8 NumberPrintLen(uint16 Number )
{
    uint8 Len=1;
    uint16 Temp = Number;
    while ( (Temp/=10) >0 )
    {
        ++Len;
    }
    return Len;
}

#if 1

//ÎÄ±¾»»ÐÐ±ê¼Çi³¼Æ±£´æ
int32 SplitTextToBuff(uint8 * Text, char* (*strAddrBuf)[], int32 MaxLineCount )
{
    return TP_ScrLinesGet(Text,
                          strlen((char*)Text),
                          strAddrBuf,
                          PNULL,
                          MaxLineCount);
}


#else
//ÎÄ±¾»»ÐÐ±ê¼Çi³¼Æ±£´æ
int32 SplitTextToBuff(uint8 * Text, int32 MaxCharInRow,
                      char* (*strAddrBuf)[], int32 MaxLineCount )
{
    uint8 *DisplayData = Text;
    uint8 *StartPoint = DisplayData;
    uint16 Index = 0;
    uint16 LineLen = 0;
    uint16 LineCount = 0;

    if(Text == PNULL)
    {
        return;
    }

    while (Index < strlen((char *)DisplayData))
    {
        if(LineCount >= MaxLineCount)
        {
            break;
        }
        if (DisplayData[Index] == '\n')
        {
            (*strAddrBuf)[ LineCount ] = (char*)StartPoint;
            LineCount++;
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
            (*strAddrBuf)[ LineCount ] = (char*)StartPoint;
            LineCount++;
            //Èç¹ûÏÂÒ»¸öÊÇ»»ÐÐ·û£¬Ìø¹ý
            if (DisplayData[Index] == '\n')
            {
                Index++;
            }
            StartPoint  = &DisplayData[Index];
            LineLen =0;
        }

        else if (LineLen > MaxCharInRow) //¸ÃÇe¿öÖ»³öÏÖouÏÔÊ¾µÄ×îºoÒ»¸öÊÇÕ¼2¸ö×Ö·û¿i¶ÈµÄÇe¿ö
        {
            (*strAddrBuf)[ LineCount ] = (char*)StartPoint;
            LineCount++;
            StartPoint  = &DisplayData[Index-2];
            LineLen =2;
        }


    }
    if(LineCount < MaxLineCount)
    {
        if (LineLen >0)
        {
            (*strAddrBuf)[ LineCount ] = (char*)StartPoint;
            LineCount++;
        }
    }
    return LineCount;
}
#endif
//»æÖÆ¹ö¶¯Ìõ
void DrawScrBar( TP_ListBoxRect *ScrRect, float BlockHight, int32 StartLine )
{
    TP_SetDisplayArea( ScrRect->left, ScrRect->top, ScrRect->right,ScrRect->bottom );
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect( ScrRect->left, ScrRect->top, ScrRect->right,ScrRect->bottom);

    if ( BlockHight < 1 )
    {
        TP_ScrFillRect( ScrRect->left,
            //(int)(ScrRect->top + StartLine * BlockHight),
            (ScrRect->top + (float)StartLine * BlockHight),
            ScrRect->right,
            //(int)(ScrRect->top + StartLine * BlockHight + 1));
            (ScrRect->top + (float)StartLine * BlockHight) +1);
    }
    else
    {
        TP_ScrFillRect( ScrRect->left,
            (int)(ScrRect->top + StartLine * BlockHight),
            ScrRect->right,
            (int)(ScrRect->top + (StartLine+1) * BlockHight));
    }
}


//»æÖÆÌaÊ¾oï
static void DrawFootMenu( TP_DisplayInfo *displayInfo, uint8 left,
                          uint8 top, uint8 right, uint8 bottom )
{
    if ( NULL == displayInfo )
    {
        return;
    }
    TP_DrawFootMenu(left,
                    top,
                    right,
                    bottom,
                    displayInfo->FONT,
                    displayInfo->strLButtonTitle,
                    displayInfo->strRButtonTitle);
}

static void DrawTitileMenu( TP_DisplayInfo *displayInfo, uint8 left,
                            uint8 top, uint8 right, uint8 bottom )
{
    if ( NULL == displayInfo || displayInfo->strTitle == PNULL )
    {
        return;
    }
    TP_ScrFontSet( displayInfo->FONT );
    TP_ScrAttrSet(0);
    DrawText(left,
             top,
             right,
             bottom,
             displayInfo->align,
             displayInfo->strTitle);
}


/**
 * Set the position of input number.
 *
 * @author linhx
 *
 * @param showInputNumber TRUE: show the input number<BR>
 *                          FALSE:do not show the input number
 * @param left
 * @param top
 * @param MaxLen
 *
 * @date 2012-09-07
 */
static void DrawInputNumRect(Boolean showInputNumber, uint8 left, uint8 top, uint16 MaxLen )
{
    if (showInputNumber == TRUE)
    {
        uint8 Len = NumberPrintLen( MaxLen );
        TP_ScrClrRect(left,top,left+Len*8+10,top+8);
        TP_ScrDrawRect( left,top,left+Len*8+10,top+8);
        TP_ShowInputNum( 1, left+1, top+2 );
    }
    else
    {
        TP_ShowInputNum(0,0,0);
    }
}

/**
 * Private function :Show the input method icon.
 *
 * @author linhx
 *
 * @param showIMIcon TRUE£ºshow icon <BR>
 *                   FALSE£ºdon't show the icon
 *
 * @param x Xaxis coordinate
 *
 * @param y Yaxis coordinate
 *
 * @date 2012-09-06
 *
 *
 */
static void DrawInputIcon(Boolean showIMIcon,uint8 x,uint8 y)
{
    if (showIMIcon == TRUE)
    {
        TP_SetInputModePosition(x,y);
    }
    else
    {
        TP_SetInputModePosition(MAX_SCREEN_WIDTH,MAX_SCREEN_HEIGHT);
    }
}

/**
 * Draw the input field.
 *
 * @author linhx
 *
 * @param displayInfo
 * @param inputInfo
 * @param rect
 *
 * @date 2012-09-07
 */
static void DrawInputArea(TP_DisplayInfo* displayInfo,
                          TP_InputInfo* inputInfo,
                          TP_ListBoxRect* rect)
{
    uint8 fontHeight = 0;

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet( displayInfo->FONT );
    fontHeight = TP_GetFontHeight();

    TP_ScrDrawRect( rect->left, rect->top, rect->right, rect->bottom );

    DrawFootMenu( displayInfo ,0,MAX_SCREEN_HEIGHT -fontHeight,
                  MAX_SCREEN_WIDTH-1, MAX_SCREEN_HEIGHT-1 );

    if (displayInfo->strTitle != PNULL)
    {
        TP_ShowTextSingleLine(0,0,MAX_SCREEN_WIDTH-1, fontHeight,displayInfo->FONT,
                              displayInfo->align,(uint8*)"%s",displayInfo->strTitle);
    }
    DrawInputNumRect (inputInfo->showInputNumber,
                      rect->right-40,rect->top-5,inputInfo->maxLen);

    DrawInputIcon(inputInfo->showIMIcon,58,MAX_SCREEN_HEIGHT - fontHeight+1);

}

/**
 * Set the input information.
 *
 * @author linhx
 *
 * @param displayInfo
 * @param inputInfo
 * @param rect
 *
 * @date 2012-09-10
 */
static void SettingInput(TP_DisplayInfo* displayInfo,TP_InputInfo* inputInfo,TP_ListBoxRect* rect)
{
    uint8 confirmKeyList[2];
    uint8 cancelKeyList[2];
    TP_CharSetT  CurrCharSet = {0};

    TP_GetCurrentCharSet(&CurrCharSet);
    confirmKeyList[0] = displayInfo->LKeyValue;
    confirmKeyList[1] = displayInfo->LF_KeyValue;
    cancelKeyList[0]  = displayInfo->RKeyValue;
    cancelKeyList[1]  = displayInfo->RF_KeyValue;

    if(inputInfo->defaultMethod < INPUTMODE_MAX)
    {
        TP_SetInputMode(inputInfo->defaultMethod);
    }
    TP_SetInputModeControlEx( displayInfo->LKeyValue, displayInfo->LF_KeyValue,
                              displayInfo->RKeyValue, TP_KEY_CLEAR, DISPLAY_KEY_POUND);
    TP_SetInputModeControlList(confirmKeyList, 2,cancelKeyList,2);
    TP_SetDisplayArea( rect->left+2,rect->top+4,rect->right-1,rect->bottom-1);
    diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                          rect->left+2,
                          rect->right-1,
                          rect->top+4);
}

static void ClearInputSetting()
{
    TP_ClearInputModeControlList();
}
/***********************************ÒÔeÏÎªË½oÐº¯Êý***************************************/
/*************************************************************************************/

//½«×ooÒ²Ëµ¥±êÌâ×eºÏ³eÒ»¸ö
void TP_MenuTitleCat( char *strbuf,uint16 bufLen, uint16 dispAreaWidth,
                      char *Lstr, char *Rstr, char Fillchar )
{
    #if 1
    uint16 strBufLen = 0;
    int16 leftWidth = 0,rightWidth = 0,spaceWidth = 0,areaWidth = 0;
    uint16 LstrLen = 0;
    uint16 RstrLen = 0;
    uint16 fillSpaceLen = 0;
    char* pLeftStr = Lstr;
    char* pRightStr = Rstr;
    uint32 charSpace = 0,lineSpace = 0;
    uint16 MaxLineCharLen = 0;

    if ( 0 == dispAreaWidth
         || NULL == strbuf
         || (pLeftStr == PNULL && pRightStr == PNULL) )
    {
        return;
    }
    if ( NULL == Lstr )
    {
        pLeftStr = " ";
    }
    if ( NULL == Rstr )
    {
        pRightStr = " ";
    }

    TP_ScrSpaceGet(&charSpace, &lineSpace);
    LstrLen = strlen((char *) pLeftStr );
    RstrLen = strlen((char *) pRightStr );
    leftWidth = TP_ScrDisplayWidthGet(pLeftStr, strlen(pLeftStr));
    rightWidth = TP_ScrDisplayWidthGet(pRightStr, strlen(pRightStr));
    spaceWidth = TP_ScrDisplayWidthGet(" ", strlen(" "));
    areaWidth = dispAreaWidth;

    //Ìî¿Õ¿Õ¸nµÄ¿i¶È
    areaWidth = areaWidth - leftWidth - rightWidth - charSpace;
    //Ìî³ä¿Õ¸n¸öÊý¼ÆËã
    if(spaceWidth > 0)
    {
        fillSpaceLen = areaWidth/(spaceWidth+charSpace);
        if( (areaWidth%(spaceWidth+charSpace) >= spaceWidth) )
        {
            fillSpaceLen++;
        }
    }
    else
    {
        fillSpaceLen = 0;
    }

    MaxLineCharLen = strlen(pLeftStr) + strlen(pRightStr) + fillSpaceLen;
    MaxLineCharLen = min(MaxLineCharLen, bufLen);

    memset( strbuf, 0, bufLen );
    strncat( strbuf, pLeftStr, min( LstrLen,MaxLineCharLen ));
    strBufLen = strlen((char *) strbuf );

    if ( strBufLen < MaxLineCharLen)
    {
        if ( strBufLen+RstrLen > MaxLineCharLen )
        {
            strncat( strbuf, pRightStr, min(RstrLen,  MaxLineCharLen-strBufLen));
        }
        else
        {
            while ( fillSpaceLen > 0 )
            {
                fillSpaceLen --;
                strbuf[ strBufLen+fillSpaceLen] = Fillchar;
            }
            strncat( strbuf, pRightStr, RstrLen);
        }
    }

    #else
    int strbuflen = 0;
    int LstrLen = strlen((char *) Lstr );
    int RstrLen = strlen((char *) Rstr );
    int FillcharLen = 0;

    if ( bufLen < MaxLen+1 || NULL == strbuf )
    {
        return;
    }
    if ( NULL == Lstr )
    {
        Lstr = " ";
    }
    if ( NULL == Rstr )
    {
        Rstr = " ";
    }
    memset( strbuf, 0, bufLen );
    strncat( strbuf, Lstr, min( LstrLen,bufLen-1 ));
    strbuflen = strlen((char *) strbuf );

    if ( strbuflen < MaxLen)
    {
        if ( strbuflen+RstrLen > MaxLen )
        {
            strncat( strbuf, Rstr, min(RstrLen,  MaxLen-strbuflen));
        }
        else
        {
            FillcharLen = MaxLen-strbuflen-RstrLen;
            while ( FillcharLen > 0 )
            {
                FillcharLen --;
                strbuf[ strbuflen+FillcharLen] = Fillchar;
            }
            strncat( strbuf, Rstr, RstrLen);
        }
    }
    #endif
}

/**
* Only Display a prompt
* @author He Wendong
* @version 1.0
* @param *msg Display the string
* @return
* @since January 3, 2012
*/
void TP_PopupMessage(uchar *msg)
{
    TP_CharSetT CurrCharSet = {0};
    TP_GetCurrentCharSet(&CurrCharSet);
    TP_Kbflush();
    SetScrOption();
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(10,10,117,52);
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect(11,11,116,51);
    TP_SetDisplayArea(12,12,115,50);
    diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                          12,
                          115,
                          12);
    //TP_ScrBackLight(1);
    TP_LcdPrintf("%s", msg);
}

/**
* Display a window, with time control
* @author He Wendong
* @version 1.0
* @param *msg Display the string
*        Ms Display time in milliseconds
* @return
* @since January 3, 2012
*/
void TP_PopupMessageTimed( uchar *msg,uint32 Ms )
{
    int16 timerid = TP_TimerAllocate();
    TP_CharSetT CurrCharSet = {0};

    TP_GetCurrentCharSet(&CurrCharSet);

    if(timerid < 0)
    {
        timerid = 1;
    }
    TP_ScrRestore(0);
    TP_Kbflush();
    SetScrOption();
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(10,10,117,52);
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect(11,11,116,51);
    TP_SetDisplayArea(12,12,115,50);
    diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                          12,
                          115,
                          12);

    TP_LcdPrintf("%s",msg);
    TP_TimerStart(timerid, Ms);
    while(1)
    {
        TP_NotifyCheck();
        if(TP_TimerGetRemain(timerid) == 0)
        {
            TP_ScrRestore(1);
            TP_TimerFree(timerid);
            return;
        }
        if (TP_Kbhit())
        {
            ///ÈÎÒâ¼üiË³ö
            TP_Kbflush(); ///ÐèÒªÇå¿Õ°´¼üÖÐµÄ¼üÖµ
            TP_ScrRestore(1);
            TP_TimerFree(timerid);
            return;

        }
    }
}

/**
 * Alert a window with special type icon with timeout control
 * @author linhx
 * @version 1.0
 *
 * @param alertType Special type of icon define in {@link
 *                  TP_AlertType}
 * @param msg Message to display.
 * @param Ms Display time in milliseconds
 * @date 2012-10-12
 */
void TP_PopupMessageWithIcon( TP_AlertType alertType,uchar *msg,uint32 Ms)
{
    int16 timerid = TP_TimerAllocate();
    TP_CharSetT CurrCharSet = {0};
    TP_GetCurrentCharSet(&CurrCharSet);

    if(timerid < 0)
    {
        timerid = 1;
    }

    TP_ScrRestore(0);
    TP_Kbflush();
    SetScrOption();
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(10,10,117,52);
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect(11,11,116,51);

    //TP_SetDisplayArea(12,12,115,50);

    TP_ScrGotoxyEx(12,12);
    if (alertType < ALERT_MAX)
    {
        TP_ScrDrLogo(AlertImage[alertType]);
    }
    TP_SetDisplayArea(36,12,115,50);
    diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                          36,
                          115,
                          12);

    TP_LcdPrintf("%s", msg);
    TP_TimerStart(timerid, Ms);
    while(1)
    {
        TP_NotifyCheck();
        if(TP_TimerGetRemain(timerid) == 0)
        {
            TP_ScrRestore(1);
            TP_TimerFree(timerid);
            return;
        }
        if (TP_Kbhit())
        {
            ///ÈÎÒâ¼üiË³ö
            TP_Kbflush(); ///ÐèÒªÇå¿Õ°´¼üÖÐµÄ¼üÖµ
            TP_ScrRestore(1);
            TP_TimerFree(timerid);
            return;

        }
    }
}

/*
º¯ÊýÃû³Æ£ºTP_PopupMesWithButton
oÃi¾£º    oÃou¾oÖÐÏÔÊ¾µ¯³ö½çÃæ£¬ÇÒÏìo¦×ooÒ°´¼ü·µ»Ø
×÷Õß£º    guangx
·µ»ØÖµËµÃ÷£º
    TRUE£º×o¼ü·µ»Ø
    FALSE£ºoÒ¼ü·µ»Ø»ò´iÎo·µ»Ø
ÊäÈë²ÎÊý£º
    displayInfo: ×ooÒ°´¼üÐÅÏ¢½a¹¹Ìå
*/
Boolean TP_PopupMessageWithButton(TP_DisplayInfo *displayInfo )
{
    uint8 fontHeight = 0;
    uint8 KeyCode;

    if ( NULL == displayInfo->strTitle)
    {
        return FALSE;
    }
    TP_Kbflush();
    TP_ScrRestore(0);
    SetScrOption();
    TP_ScrFontSet( displayInfo->FONT );
    fontHeight = TP_GetFontHeight();

    TP_SetDisplayArea( 10, 5, MAX_SCREEN_WIDTH-10, 59 );
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect( 10, 5, MAX_SCREEN_WIDTH-10, 59 );

    TP_ShowTextSingleLine( 11, 6, MAX_SCREEN_WIDTH-11, 58-fontHeight,
                           displayInfo->FONT,displayInfo->align,(uint8*)"%s",displayInfo->strTitle);
    DrawFootMenu( displayInfo, 13, 58-fontHeight, MAX_SCREEN_WIDTH-11, 58 );
    while (1)
    {
        TP_NotifyCheck();
        if ( TP_Kbhit() )
        {
            KeyCode = TP_GetKey();
            if ( KeyCode == displayInfo->LKeyValue || KeyCode == displayInfo->LF_KeyValue )
            {
                //TP_ScrSpaceSet(charspace,linespace);
                TP_ScrRestore(1);
                return TRUE;
            }
            else if( KeyCode == displayInfo->RKeyValue || KeyCode == displayInfo->RF_KeyValue )
            {
                //TP_ScrSpaceSet(charspace,linespace);
                TP_ScrRestore(1);
                return FALSE;
            }
        }
    }
}


static ShowTextKeyFunc ShowTextKeyDo = PNULL;
/**
* TP_ShowText()ÖÐ°´¼üÊÂ¼þ´¦Àiº¯Êý¶¨Òå
* @author ºÎÎÄ¶«
* @version 1.0
* @param func_p °´¼ü´¦Àiº¯ÊýÖ¸Õë<BR>
* @since 2012Äê5ÔÂ18ÈÕ
* @note
*/
void TP_ShowTextKeyFuncSet(ShowTextKeyFunc func_p)
{
    ShowTextKeyDo = func_p;
}

void TP_ShowTextSingleLine(uint8 left, uint8 top, uint8 right,uint8 bottom,
                           uint8 FONT,TP_DisplayAlign  align,uchar* fmt,...)
{
    char   LcdScreenBuffer[256];
    va_list  ap;

    memset(LcdScreenBuffer,0,sizeof(LcdScreenBuffer));
    if (PNULL == fmt )
    {
        return ;
    }
    if (strlen((char *)fmt) >= sizeof(LcdScreenBuffer))
    {
        return;
    }

    va_start(ap,fmt);

    _vsnprintf(LcdScreenBuffer, sizeof(LcdScreenBuffer),(char*)fmt, ap);

    va_end(ap);

    TP_ScrAttrSet(0);
    TP_ScrFontSet(FONT);

    DrawText(left, top, right, bottom, align, LcdScreenBuffer);
}



/**
 * ÔuÖ¸¶¨µÄaÐ±i¿òÇøoòRectÏÔÊ¾ÎÄ±¾£¬²»ÏÔÊ¾±êÌâ£¬Ïìo¦×ooÒ¼ü£¬Ïìo¦Òn×¢²aµÄº¯Êý
 * @author guangx
 * @param displayInfo
 * @param Rect
 * @param Text
 *
 * @return Boolean
 */
Boolean
TP_ShowText(TP_DisplayInfo* displayInfo,
            TP_ListBoxRect  Rect,
            uint8*          pScrText)
{
    TP_ListBoxRect TextRect = { 0 };
    TP_ListBoxRect ScrRect = { 0 };
    int32 StartLine = 0;
    Boolean ifDrawScrBar = FALSE;
    float BlockHight = 0;
    int32 LineCount = 0;
    int32 MaxRow = 0;
    uint32 iLine;
    uint32 displayBytes;

    uint8**  pLineStart;
    uint16*  nLineBytes;

    uint8 userKey = 0;
    TP_CharSetT CurrCharSet = {0};

    if (NULL == pScrText ||
        0    == pScrText[0])
    {
        return FALSE;
    }

    ////////////////////////////////
    TP_AllocMemory(sizeof(uint8*) * SHOWTEXT_MAX_LINE, (void**)&pLineStart);
    if (PNULL == pLineStart)
    {
        return FALSE;
    }
    memset(pLineStart, 0x00, sizeof(char*) * SHOWTEXT_MAX_LINE);

    TP_AllocMemory(sizeof(uint16) * SHOWTEXT_MAX_LINE, (void**)&nLineBytes);
    if (PNULL == nLineBytes)
    {
        return FALSE;
    }
    memset(nLineBytes, 0x00, sizeof(uint16) * SHOWTEXT_MAX_LINE);

    TP_GetCurrentCharSet(&CurrCharSet);
    TextRect.left   = Rect.left;
    TextRect.top    = Rect.top;
    TextRect.right  = Rect.right - 6;
    TextRect.bottom = Rect.bottom;

    TP_ScrFontSet(displayInfo->FONT);
    TP_SetDisplayArea(TextRect.left, TextRect.top, TextRect.right,TextRect.bottom );
    MaxRow = TP_GetMaxRowInScreen();


    LineCount = TP_ScrLinesGet(pScrText, strlen(pScrText),
                               pLineStart,
                               nLineBytes,
                               SHOWTEXT_MAX_LINE);
    if (LineCount == 0)
    {
        TP_PopupMessageTimed("System Error!", 3 * 1000);
    }

    if (LineCount > MaxRow)
    {
        ifDrawScrBar = TRUE;

        ScrRect.left   = Rect.right - 5;
        ScrRect.top    = Rect.top;
        ScrRect.right  = Rect.right;
        ScrRect.bottom = Rect.bottom - 2;
        //ºu¿e¸ß¶È
        BlockHight = (float)(ScrRect.bottom-ScrRect.top)/(float)( LineCount-MaxRow+1);
    }
    else
    {
        ifDrawScrBar = TRUE;

        ScrRect.left   = Rect.right - 5;
        ScrRect.top    = Rect.top;
        ScrRect.right  = Rect.right;
        ScrRect.bottom = Rect.bottom - 2;

        //ºu¿e¸ß¶È
        BlockHight = (float)(ScrRect.bottom - ScrRect.top + 1);
    }

    TP_Kbflush();
    StartLine = 0;
    while (TRUE)
    {
        displayBytes = 0;
        for (iLine = 0; iLine < MaxRow; iLine++)
        {
            displayBytes += nLineBytes[StartLine + iLine];
        }
        TP_SetDisplayArea(TextRect.left, TextRect.top,
                          TextRect.right,TextRect.bottom);
        TP_ScrClsDisplayArea();
        diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                              TextRect.left,
                              TextRect.right,
                              TextRect.top);
        TP_LcdPrintf("%.*s", displayBytes, pLineStart[StartLine]);


        //TP_ShowTextSingleLine(TextRect.left, TextRect.top, TextRect.right, TextRect.bottom,
        //                      displayInfo->FONT, ALIGN_LEFT, "%.*s", displayBytes, pLineStart[StartLine]);

        if (TRUE == ifDrawScrBar)
        {
            DrawScrBar(&ScrRect, BlockHight, StartLine);
        }

        while(1)
        {
            TP_NotifyCheck();

            if (0 != TP_Kbhit())
            {
                userKey = TP_GetKey();
                TP_Kbflush();

                /*oÃou´¦Ài×¢²aµÄ»Øµ÷º¯Êý*/
                if (ShowTextKeyDo != PNULL)
                {
                    ShowTextKeyDo(userKey, pScrText);
                }

                if ( userKey == displayInfo->LKeyValue ||
                     userKey == displayInfo->LF_KeyValue )
                {
                    TP_FreeMemory( (void**)&pLineStart);
                    TP_FreeMemory( (void**)&nLineBytes);

                    TP_ScrFontSet( CFONT );

                    /*Çå³ý×¢²aµÄ»Øµ÷º¯Êý*/
                    if (ShowTextKeyDo != PNULL)
                    {
                        ShowTextKeyDo = PNULL;
                    }

                    return TRUE;
                }
                else if( userKey == displayInfo->RKeyValue ||
                         userKey == displayInfo->RF_KeyValue )
                {
                    TP_FreeMemory( (void**)&pLineStart);
                    TP_FreeMemory( (void**)&nLineBytes);

                    TP_ScrFontSet( CFONT );

                    /*Çå³ý×¢²aµÄ»Øµ÷º¯Êý*/
                    if (ShowTextKeyDo!=PNULL)
                    {
                        ShowTextKeyDo = PNULL;
                    }

                    return FALSE;
                }
                else if (userKey == TP_KEY_DOWN)
                {
                    if (StartLine >= (LineCount - MaxRow))
                    {
                        if ( (LineCount - MaxRow) > 0 )
                        {
                            StartLine = 0;
                        }
                    }
                    else
                    {
                        ++StartLine;
                    }
                }
                else if (userKey == TP_KEY_UP)
                {
                    if (StartLine == 0 )
                    {
                        if ( (LineCount - MaxRow) > 0 )
                        {
                            StartLine = LineCount - MaxRow;
                        }
                    }
                    else
                    {
                        --StartLine;
                    }
                }

                break;
            }
        }
    }

    TP_FreeMemory( (void**)&pLineStart);
    TP_FreeMemory( (void**)&nLineBytes);

    /*Çå³ý×¢²aµÄ»Øµ÷º¯Êý*/
    if (ShowTextKeyDo!=PNULL)
    {
        ShowTextKeyDo = PNULL;
    }

    return FALSE;
}



/**
 * oÃouÈ«Æa¶àÐÐÎÄ±¾ÏÔÊ¾
 *
 * @author guangx
 *
 * @param displayInfo ÏÔÊ¾½a¹¹ÌåÐÅÏ¢
 * @param fmt: ¸nÊ½»¯Êä³ö×Ö·û´®
 *
 * @return TRUE:  ×o¼ü·µ»Ø<BR>
 *          FALSE: oÒ¼ü·µ»Ø»ò´iÎo·µ»Ø
 * @date 2012-08-02
 */
Boolean TP_ShowTextFullScr( TP_DisplayInfo *displayInfo,uchar* fmt,... )
{
    uint32 charspace =0 , linespace =0;
    char DisplayData[2*1024+1] = {0};
    va_list  ap;
    TP_ListBoxRect Rect = {0,0, MAX_SCREEN_WIDTH-1, MAX_SCREEN_HEIGHT-1 };
    Boolean Result;
    uint8 fontHeight = (displayInfo->FONT==ASCII)?7:12;

    if ( NULL == displayInfo  || NULL == fmt )
    {
        return FALSE;
    }
    TP_ScrFontSet( displayInfo->FONT );
    TP_ScrSpaceGet(&charspace,&linespace);
    SetScrOption();
    TP_ScrCls();
    fontHeight = TP_GetFontHeight();

    if ( NULL != displayInfo->strTitle )
    {
        TP_ShowTextSingleLine(0, 0, MAX_SCREEN_WIDTH-1, fontHeight,
                              displayInfo->FONT,displayInfo->align,(uint8*)"%s",displayInfo->strTitle);
        Rect.top = fontHeight;
    }
    DrawFootMenu( displayInfo, 0, MAX_SCREEN_HEIGHT-fontHeight,
                  MAX_SCREEN_WIDTH-1, MAX_SCREEN_HEIGHT-1 );
    if(displayInfo->strLButtonTitle != PNULL
       || displayInfo->strRButtonTitle != PNULL )
    {
        Rect.bottom = MAX_SCREEN_HEIGHT - fontHeight - 1;
    }
    if (strlen((char *)fmt) >= sizeof(DisplayData))
    {
        TP_ScrSpaceSet(charspace,linespace);
        return FALSE;
    }

    va_start(ap,fmt);
    _vsnprintf(DisplayData, sizeof(DisplayData)-1,(char*)fmt,ap);
    va_end(ap);

    //Result = TP_ShowText( displayInfo, Rect, FONT, DisplayData);
    Result = TP_ShowText(displayInfo,Rect,(uint8*)DisplayData);
    TP_ScrSpaceSet(charspace,linespace);
    return Result;
}

/**
 * Display a picture in special area
 *
 * @author linhx
 *
 * @param left Left margin
 * @param top Top margin
 * @param right  Right margin
 * @param bottom  Bottom margin
 * @param FontSize ASCII-singlebyte character small size
 *                       multibyte character do not display
 *                 CFONT-singlebyte character big size
 *                       multibyte character normal size
 * @param picture Picture to display
 *
 * @date 2012-09-11
 */
void TP_ShowPicture(uint8 left, uint8 top, uint8 right,
                   uint8 bottom,uint8 *picture)
{
    if (picture == PNULL)
    {
        return ;
    }
    TP_SetDisplayArea(left,top,right,bottom);
    TP_ScrClsDisplayArea();
    TP_ScrGotoxyEx(left,top);
    TP_ScrDrLogo(picture);
}


/**
 * Multi-line text string input
 *
 * @author Linhx
 * @version 1.0
 *
 * @param displayInfo Display information struct pointer
 * @param inputInfo Input information struct pointer
 * @param pBuf Output buffer pointer
 * @return 0xF9 Quit by timeout<BR>
 *         0xFB Quit by pressed the left key defined in {@link
 *              TP_DisplayInfo}<BR>
 *         0xFD Input control key has not been set<BR>
 *         0xFE Error parameter<BR>
 *         0xFF Quit by pressed the right key defined in {@link
 *              TP_DisplayInfo}
 *
 * @version 1.0
 *
 * @date 2012-08-02
 *
 * @note If the pointer of title is null, the title will not be
 *       display and the input area can display three lines text
 *       height,otherwise,the input area can only display two
 *       lines text height.
 */
uint8 TP_TextInput( TP_DisplayInfo *displayInfo,
                    TP_InputInfo* inputInfo, uchar* pBuf )
{
    uint8 result;
    uint8 fontHeight = 0;
    TP_ListBoxRect rect = {2,6,MAX_SCREEN_WIDTH-3,49};

    TP_ScrCls();
    SetScrOption();
    TP_ScrFontSet(displayInfo->FONT);
    fontHeight = TP_GetFontHeight();
    rect.bottom = MAX_SCREEN_HEIGHT - 1 - fontHeight ;

    if ( PNULL == displayInfo || PNULL == pBuf )
    {
        return 0xFE;
    }

    if (displayInfo->strTitle != PNULL)
    {
        rect.top += fontHeight;
    }

    DrawInputArea(displayInfo,inputInfo,&rect);

    SettingInput(displayInfo,inputInfo,&rect);

    do
    {
        result =  TP_GetHzString((char*) pBuf, inputInfo->timeoutInMS, inputInfo->maxLen );
    }while(result == 0xFB &&
           strlen((char *)pBuf)< inputInfo->minLen);

    ClearInputSetting();

    return result;
}


/**
 * Numeral string input with timeout controlled
 *
 * @author linhx
 *
 * @param displayInfo Display information struct pointer
 * @param inputInfo Input information struct pointer
 * @param pBuf Output buffer pointer
 *
 * @return 0xF9 Quit by timeout<BR>
 *         0xFA Quit by reach maxLen<BR>
 *         0xFB Quit by pressed the left key defined in {@link
 *              TP_DisplayInfo}<BR>
 *         0xFD Input control key has not been set<BR>
 *         0xFE Error parameter<BR>
 *         0xFF Quit by pressed the right key defined in {@link
 *              TP_DisplayInfo}
 *
 * @version 1.0
 *
 * @date 2012-09-10
 *
 * @note This function can be used to input float string by
 *       setting inputMode in {@link TP_InputInfo}. If the
 *       maxLen defined in {@link TP_InputInfo} is less than
 *       21,a single line input field will be
 *       create,otherwise,you will see a multi line input field.
 */
uint8 TP_NumberInput(TP_DisplayInfo *displayInfo,
                     TP_InputInfo * inputInfo,uchar *pBuf )
{
    uint8 result;
    uint8 fontHeight = 0;
    uint8 maxLineChar = 0;

    TP_ListBoxRect rect = {0,18,MAX_SCREEN_WIDTH-3,37};

    if ( displayInfo == NULL || pBuf == NULL )
    {
        return 0xFE;
    }

    TP_ScrCls();
    SetScrOption();
    TP_ScrFontSet(displayInfo->FONT);
    TP_SetDisplayArea(rect.left, rect.top, rect.right, rect.bottom);
    maxLineChar = TP_GetMaxCharInRow();
    fontHeight = TP_GetFontHeight();

    rect.top = fontHeight + 5;
    rect.bottom = rect.top + 5 + fontHeight;

    if (inputInfo->maxLen > maxLineChar )
    {
        rect.bottom += fontHeight;
    }
    if(rect.bottom > (MAX_SCREEN_HEIGHT - fontHeight - 1))
    {
        rect.bottom = MAX_SCREEN_HEIGHT - fontHeight - 1;
    }

    DrawInputArea(displayInfo,inputInfo,&rect);

    SettingInput(displayInfo,inputInfo,&rect);

    if ((inputInfo->inputMode & 0x00FF) == 0)
    {
        if(displayInfo->FONT == ASCII)
        {
            result = 0x94;
        }
        else
        {
            result = 0xD4;
        }
        result =  TP_GetStringTimed((char*) pBuf, result, inputInfo->minLen,
                                    inputInfo->maxLen,inputInfo->timeoutInMS);
    }
    else
    {
        result =  TP_GetStringTimed((char*) pBuf, inputInfo->inputMode, inputInfo->minLen,
                                    inputInfo->maxLen,inputInfo->timeoutInMS);
    }

    ClearInputSetting();

    return result;
}


/**
 * Password string input with timeout control
 *
 * @author linhx
 *
 * @param displayInfo Display information struct pointer
 * @param inputInfo Input information struct pointer
 * @param echo Echo display character to replace user input
 * @param pBuf Output buffer pointer
 *
 * @return 0xF9 Quit by timeout<BR>
 *         0xFA Quit by reach maxLen<BR>
 *         0xFB Quit by pressed the left key defined in {@link
 *              TP_DisplayInfo}<BR>
 *         0xFD Input control key has not been set<BR>
 *         0xFE Error parameter<BR>
 *         0xFF Quit by pressed the right key defined in {@link
 *              TP_DisplayInfo}
 *
 * @version 1.0
 *
 * @date  2012-09-10
 */
uint8 TP_PassWordInput( TP_DisplayInfo *displayInfo,
                          TP_InputInfo * inputInfo,uint8 echo, uchar *pBuf )
{
    uint8 result = 0;
    uint8 fontHeight = 0;
    uint8 maxLineChar = 0;
    TP_ListBoxRect rect = {0,18,MAX_SCREEN_WIDTH-3,37};
    uint16 mode = 0;

    if ( NULL == displayInfo || NULL ==pBuf )
    {
        return FALSE;
    }

    TP_ScrCls();
    SetScrOption();
    TP_ScrFontSet(displayInfo->FONT);
    TP_SetDisplayArea(rect.left, rect.top, rect.right, rect.bottom);
    maxLineChar = TP_GetMaxCharInRow();
    fontHeight = TP_GetFontHeight();

    rect.top = fontHeight + 5;
    rect.bottom = rect.top + 4 + fontHeight;

    if (inputInfo->maxLen > maxLineChar )
    {
        rect.bottom += fontHeight;
    }
    if(rect.bottom > (MAX_SCREEN_HEIGHT - fontHeight - 1))
    {
        rect.bottom = MAX_SCREEN_HEIGHT - fontHeight - 1;
    }

    DrawInputArea(displayInfo,inputInfo,&rect);

    SettingInput(displayInfo,inputInfo,&rect);

    if (inputInfo->inputMode == 0)
    {
        if(displayInfo->FONT == ASCII)
        {
            mode = 0x940F;
        }
        else
        {
            mode = 0xD40F;
        }
        result = TP_GetPasswordTimed((char*) pBuf, inputInfo->minLen,inputInfo->maxLen,
                                     mode, echo, inputInfo->timeoutInMS );
    }
    else
    {
        result = TP_GetPasswordTimed((char*) pBuf, inputInfo->minLen,inputInfo->maxLen,
                                     inputInfo->inputMode, echo, inputInfo->timeoutInMS );
    }

    ClearInputSetting();

    return result;
}


/**
 * º¯ Êý TP_CreateListBoxInterface() ¹¦ÄÜÃèÊö£º»æÖÆaÐ±i½çÃæ
 * @param ListBox  aÐ±i¶ÔÏo
 * @return ´oou0 ±iÊ¾ÊÇ°´È·ÈÏ¼ü·µ»ØµÄ£¬·µ»Ø0 ±iÊ¾ÊÇ°´È¡Ïû¼ü·µ»ØµÄ
 * @author ºÎÎÄ¶«
 * @version 1.1
 * @date 20120211
 * @note Ö»oÐµ±°´ÏÂÈ·ÈÏ¼ü»òÕßÈ¡Ïû¼üÊ±²Å·µ»Ø
 */
Boolean TP_CreateListBoxInterface(TP_ListBoxStyle listboxStyle,
                                 TP_DisplayInfo *displayInfo,
                                 uint8 *item[],
                                 uint8 itemNum,
                                 uint8 selectIndex,
                                 uint8 *chooseValue,
                                 uint8 *icon)
{
    uint8 tempi;
    TP_ListBox *listBox;
    TP_ListBoxRect rect = {0,0,MAX_SCREEN_WIDTH -1,MAX_SCREEN_HEIGHT -1 };
    uint8 captionCount = 0;
    uint8 footStr[MAX_LR_BUTTON_TITLE_LEN*2+1]={0};

    if (listboxStyle != TL_NO_EFFECT &&
        listboxStyle != TL_LIST      &&
        listboxStyle != TL_RADIOBOX  &&
        listboxStyle != TL_ICONLIST  )
    {
        return FALSE;
    }
    if (NULL == displayInfo ||
        NULL == item )
    {
        return FALSE;
    }

    TP_ScrCls();
    SetScrOption();
    TP_ScrFontSet(CFONT);//listboxoÃµÄÊÇCFONT×ÖÌå
    TP_MenuTitleCat( (char*)footStr, MAX_LR_BUTTON_TITLE_LEN*2, rect.right-rect.left+1,
                     (char*)displayInfo->strLButtonTitle, (char*)displayInfo->strRButtonTitle, ' ');

    listBox = TP_NewListBox(listboxStyle, rect, displayInfo->strTitle, footStr,NULL);
    listBox->ConfirmKey  = displayInfo->LKeyValue;
    listBox->CancelKey   = displayInfo->RKeyValue;
    listBox->LF_KeyValue = displayInfo->LF_KeyValue;
    listBox->RF_KeyValue = displayInfo->RF_KeyValue;
    listBox->TitleAlign  = displayInfo->align;

    //aÐ±i±êÌâ´¦Ài
    for(tempi=0;tempi<itemNum;tempi++)
    {
        if(item[tempi] != NULL)
        {
            captionCount++;
            if (listboxStyle == TL_ICONLIST)
            {
                TP_ListBoxAddItem(listBox, item[tempi], captionCount, (selectIndex==captionCount?1:0),icon);
            }
            else
            {
                TP_ListBoxAddItem(listBox, item[tempi], captionCount, (selectIndex==captionCount?1:0),NULL);
            }
        }
    }

    //ÏÔÊ¾
    if (TP_ShowListBox(listBox))
    {
        *chooseValue = listBox->SelectItemValue[0];
        TP_ListBoxFree(listBox);
        return TRUE;
    }
    else
    {
        TP_ListBoxFree(listBox);
        return FALSE;
    }

}

/**
 * Create listbox with checkbox style.
 *
 * @author linhx
 *
 * @param displayInfo   Pointer of display information
 * @param listBoxInfo   Pointer of the listBoxInfo infomation
 *                      structure list
 * @param itemNum       Number of items,it must equal with the
 *                      number of the structure list which the
 *                      listBoxInfo point to.
 * @param selectKey     Key code to seleck checkbox
 *
 * @return TRUE: Quit by pressed the confirm key<BR>
 *          FALSE: Quit by pressed the cancel key or
 *                  error parameter.
 *
 * @version 1.0
 *
 * @date 2012-09-26
 */
Boolean TP_CreateCheckBoxInterface(TP_DisplayInfo *displayInfo,
                                   TP_ListBoxInfo listBoxInfo[],
                                   uint8 itemNum,uint8 selectKey)
{
    TP_ListBox *listBox = NULL;
    TP_ListBoxRect rect = {0,0,MAX_SCREEN_WIDTH -1,MAX_SCREEN_HEIGHT -1 };
    uint8 footStr[MAX_LR_BUTTON_TITLE_LEN*2+1]={0};
    uint8 iChar;

    if (NULL == displayInfo ||
        NULL == listBoxInfo)
    {
        return FALSE;
    }

    TP_ScrCls();
    SetScrOption();
    TP_ScrFontSet(CFONT);//listboxoÃµÄÊÇCFONT×ÖÌå
    TP_MenuTitleCat( (char*)footStr, MAX_LR_BUTTON_TITLE_LEN*2, rect.right-rect.left+1,
                     (char*)displayInfo->strLButtonTitle, (char*)displayInfo->strRButtonTitle, ' ');

    listBox = TP_NewListBox(TL_CHECKBOX, rect, displayInfo->strTitle, footStr,NULL);
    if (listBox == PNULL)
    {
        return FALSE;
    }

    listBox->ConfirmKey  = displayInfo->LKeyValue;
    listBox->CancelKey   = displayInfo->RKeyValue;
    listBox->LF_KeyValue = displayInfo->LF_KeyValue;
    listBox->RF_KeyValue = displayInfo->RF_KeyValue;
    listBox->SelectKey   = selectKey;
    listBox->TitleAlign  = displayInfo->align;

    for (iChar = 0; iChar <itemNum; iChar++)
    {
        TP_ListBoxAddItem(listBox, listBoxInfo[iChar].caption, listBoxInfo[iChar].value, (listBoxInfo[iChar].selected == TRUE)?1:0,NULL);
    }

    if (TP_ShowListBox(listBox))
    {
        for (iChar = 0; iChar<itemNum; iChar++)
        {
            listBoxInfo[iChar].selected = FALSE;
        }
        for (iChar = 0; iChar <listBox->SelectItemCount;iChar++)
        {
            listBoxInfo[listBox->SelectItems[iChar]].selected = TRUE;
        }
        TP_ListBoxFree(listBox);
        return TRUE;
    }
    else
    {
        TP_ListBoxFree(listBox);
        return FALSE;
    }
}

/**
 *
 *
 * @author linhx (2012/10/12)
 *
 * @param displayInfo
 * @param listboxStyle
 * @param listBoxInfo
 * @param itemNum
 * @param optionStyle
 * @param optionInfo
 * @param optionNum
 * @param optionSelectKey
 *
 * @return Boolean
 */
Boolean TP_CreateListBoxWithOp(TP_DisplayInfo *displayInfo,
                               TP_ListBoxStyle listboxStyle,
                               TP_ListBoxInfo listBoxInfo[],uint8 itemNum,
                               TP_ListBoxStyle optionStyle,
                               TP_ListBoxInfo optionInfo[],uint8 optionNum,
                               uint8 optionSelectKey)
{
    TP_ListBox *listBox = NULL;
    TP_ListBox *optListBox = NULL;
    TP_ListBoxRect rect = {0,0,MAX_SCREEN_WIDTH -1,MAX_SCREEN_HEIGHT -1 };
    uint8 footStr[MAX_LR_BUTTON_TITLE_LEN*2+1]={0};
    uint8 iChar;
    uint8 fontHeight = 0;

    if (displayInfo == NULL ||
        listBoxInfo == NULL ||
        optionInfo == NULL  ||
        itemNum == 0        ||
        optionNum == 0      ||
        listboxStyle == TL_CHECKBOX)
    {
        return FALSE;
    }

    TP_ScrCls();
    SetScrOption();
    TP_ScrFontSet(CFONT);//listboxoÃµÄÊÇCFONT×ÖÌå
    fontHeight = TP_GetFontHeight();
    TP_MenuTitleCat( (char*)footStr, MAX_LR_BUTTON_TITLE_LEN*2, rect.right-rect.left+1,
                     (char*)displayInfo->strLButtonTitle, (char*)displayInfo->strRButtonTitle, ' ');

    listBox = TP_NewListBox(listboxStyle, rect, displayInfo->strTitle, footStr,NULL);
    if (listBox == PNULL)
    {
        return FALSE;
    }
    listBox->ConfirmKey  = displayInfo->LKeyValue;
    listBox->CancelKey   = displayInfo->RKeyValue;
    listBox->LF_KeyValue = displayInfo->LF_KeyValue;
    listBox->RF_KeyValue = displayInfo->RF_KeyValue;
    listBox->TitleAlign  = displayInfo->align;
    for (iChar = 0; iChar <itemNum; iChar++)
    {

        TP_ListBoxAddItem(listBox,
                          listBoxInfo[iChar].caption,
                          listBoxInfo[iChar].value,
                          (listBoxInfo[iChar].selected == TRUE)?1:0,
                          listBoxInfo[iChar].icon);
    }
    while (TP_ShowListBox(listBox))
    {
        for (iChar = 0; iChar<itemNum; iChar++)
        {
            listBoxInfo[iChar].selected = FALSE;
        }
        for (iChar = 0; iChar <listBox->SelectItemCount;iChar++)
        {
            listBoxInfo[listBox->SelectItems[iChar]].selected = TRUE;
        }
        rect.left = 20;
        rect.top = MAX_SCREEN_HEIGHT - fontHeight*3 -1;
        rect.right = 85;
        rect.bottom = rect.top + fontHeight*2 + 1;
        TP_SetDisplayArea(rect.left-2,rect.top-2,rect.right+2,rect.bottom+2);
        TP_ScrClsDisplayArea();
        TP_ScrDrawRect(rect.left-2,rect.top-2,rect.right+2,rect.bottom+2);
        optListBox = TP_NewListBox(optionStyle,rect,NULL,NULL,NULL);
        if (optListBox == NULL)
        {
            TP_ListBoxFree(listBox);
            return FALSE;
        }

        optListBox->ConfirmKey  = displayInfo->LKeyValue;
        optListBox->CancelKey   = displayInfo->RKeyValue;
        optListBox->LF_KeyValue = displayInfo->LF_KeyValue;
        optListBox->RF_KeyValue = displayInfo->RF_KeyValue;
        optListBox->ifDrawScrBall = FALSE;
        if (optionStyle == TL_CHECKBOX)
        {
            optListBox->SelectKey = optionSelectKey;
        }

        for (iChar =0; iChar < optionNum; iChar++)
        {
            TP_ListBoxAddItem(optListBox,
                          optionInfo[iChar].caption,
                          optionInfo[iChar].value,
                          (optionInfo[iChar].selected == TRUE)?1:0,
                          optionInfo[iChar].icon);
        }

        if (TP_ShowListBox(optListBox))
        {
            for (iChar = 0; iChar<optionNum; iChar++)
            {
                optionInfo[iChar].selected = FALSE;
            }
            for (iChar = 0; iChar <optListBox->SelectItemCount;iChar++)
            {
                optionInfo[optListBox->SelectItems[iChar]].selected = TRUE;
            }
            TP_ListBoxFree(optListBox);
            TP_ListBoxFree(listBox);
            return TRUE;
        }
        else
        {
            TP_ListBoxFree(optListBox);
        }

    }//End of while (TP_ShowListBox(listBox))

     TP_ListBoxFree(listBox);
     return FALSE;
}

/**
 * To show the progress bar and show the percentage if possible.
 *
 * @author linhx
 *
 * @version 1.0
 *
 * @param note Display note of progress bar
 *
 * @param current Current process value
 *
 * @param total Total process value
 *
 * @date 2012-08-31
 *
 * @note If the right margin value in parameter rect is less
 *       than {@link #MAX_SCREEN_WIDTH}-33,the percentage will
 *       be display.
 *
 */
void TP_ShowProgressBar(uchar* note,uint32 current,uint32 total)
{
    uint8 progress=0;
    uint8 length = 0;
    TP_ListBoxRect bacRect;
    TP_ListBoxRect NoteRect = {14,22,127,39};
    TP_ListBoxRect rect = {27,40,77,49};
    TP_CharSetT CurrCharSet = {0};

    if (total == 0)
    {
        if (current == 0)
        {
            progress = 100;
        }
        else
        {
            return;
        }
    }
    else
    {
        progress = min(100*current/total,100);
    }

    length =  progress*(rect.right - rect.left)/100;

    SetScrOption();
    TP_ScrFontSet(CFONT);
    TP_GetDisplayArea(&bacRect.left,&bacRect.top,&bacRect.right,&bacRect.bottom);
    TP_GetCurrentCharSet(&CurrCharSet);

    if(note != PNULL)
    {
        TP_SetDisplayArea(NoteRect.left, NoteRect.top,NoteRect.right, NoteRect.bottom);
        TP_ScrClsDisplayArea();
        diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                              NoteRect.left,
                              NoteRect.right,
                              NoteRect.top);
        TP_LcdPrintf((uint8*)"%s", note);

    }

    TP_SetDisplayArea(rect.left, rect.top, rect.right+5, rect.bottom);
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect(rect.left, rect.top, rect.right, rect.bottom);
    TP_ScrFillRect(rect.left,rect.top,rect.left+length,rect.bottom);
    if ((MAX_SCREEN_WIDTH-1-rect.right)>32)
    {
        TP_SetDisplayArea(rect.right+1, rect.top, rect.right+32, rect.bottom);
        TP_ScrClsDisplayArea();
        diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                              rect.right+1,
                              rect.right+32,
                              rect.top);
        TP_LcdPrintf((uint8*)"%d%%",progress);
    }

    TP_SetDisplayArea(bacRect.left,bacRect.top,bacRect.right,bacRect.bottom);
}


Boolean TP_IsGBK(uint8 WordCode)
{
    ST_FONT  SingleFont;
    ST_FONT  MutlFont;


    TP_GetFontInfo(&SingleFont,&MutlFont);

    if (MutlFont.CharSet == CHARSET_DIY)
    {
        return FALSE;
    }

    if (WordCode > 0x80)
    {
        return TRUE;
    }
    return FALSE;
}


Boolean TP_DateInput( TP_DisplayInfo *displayInfo, TP_Date *pDate )
{
    char strYear[5] ={0};
    char strMonth[3] ={0};
    char strDay[3] ={0};
    uint8 KeyCode;
    Boolean ifExit = FALSE;
    Boolean ifConfirm = FALSE;
    uint8 editindex = 0;//
    uint8 editData[5] = {0};
    uint8 editlen = 2;
    uint8 confirmKeyList[4];
    uint8 cancelKeyList[2];
    TP_CharSetT CurrCharSet = {0};
    uint8 fontHeight = 0;
    uint32 charSpace = 0, lineSpace = 0;
    //Ê±·ÖÃëÊýÖµµÄÏÔÊ¾±ß½çÖµ
    uint8 drawTop = 0;
    uint8 drawBottom = 0;
    uint8 drawLeft_y = 0;
    uint8 drawRight_y = 0;
    uint8 drawLeft_m = 0;
    uint8 drawRight_m = 0;
    uint8 drawLeft_d = 0;
    uint8 drawRight_d = 0;
    //Ê±·ÖÃëÌaÊ¾oïµÄÏÔÊ¾±ß½çÖµ
    uint8 drawLeft_yc = 0;
    uint8 drawRight_yc = 0;
    uint8 drawLeft_mc = 0;
    uint8 drawRight_mc = 0;
    uint8 drawLeft_dc = 0;
    uint8 drawRight_dc = 0;

    uint8 oneCharWidth = 0;

    TP_Date tempDate = {0};
    uint8 result = 0;


    if ( NULL == displayInfo || NULL == pDate )
    {
        return FALSE;
    }

    TP_GetCurrentCharSet(&CurrCharSet);
    TP_ScrCls();
    SetScrOption();
    TP_ScrFontSet( displayInfo->FONT );
    fontHeight = TP_GetFontHeight();
    TP_ScrSpaceGet(&charSpace, &lineSpace);
    DrawTitileMenu(displayInfo ,0,0,MAX_SCREEN_WIDTH-1, fontHeight);
    DrawFootMenu( displayInfo ,0,MAX_SCREEN_HEIGHT-fontHeight,MAX_SCREEN_WIDTH-1, MAX_SCREEN_HEIGHT-1 );

    TP_ShowInputNum( 0,0,0 );
    DrawInputIcon(FALSE,0,0);
    TP_SetInputModeControl(TP_KEY_NONE,TP_KEY_NONE,DISPLAY_KEY_POUND);
    memset(confirmKeyList, TP_KEY_NONE, sizeof(confirmKeyList));
    memset(cancelKeyList, TP_KEY_NONE, sizeof(cancelKeyList));
    #ifdef TPS300
    confirmKeyList[0] = TP_KEY_UP;
    confirmKeyList[1] = TP_KEY_DOWN;
    #else
    confirmKeyList[0] = TP_KEY_LEFT;
    confirmKeyList[1] = TP_KEY_RIGHT;
    #endif
    confirmKeyList[2] = displayInfo->LF_KeyValue;
    confirmKeyList[3] = displayInfo->LKeyValue;
    cancelKeyList[0] = displayInfo->RF_KeyValue;
    cancelKeyList[1] = displayInfo->RKeyValue;
    TP_SetInputModeControlList(confirmKeyList, 4, cancelKeyList, 2);

    strcat(strYear,"-");
    strcat(strMonth,"-");
    strcat(strDay,"-");

    /**************************************************************
    *ÏÔÊ¾nùÊ½
    * Hour HCaption Minute MCaption Sec SCaption
    ***************************************************************/
    /**************************************************************
    * ¼ÆËãÏÔÊ¾ÇøoòµÄ±ß½çÖµ
    ***************************************************************/
    {
        //ÈÃÆäÔËÐÐa½´ÎÊÇÎªaË¾oÖÐÏÔÊ¾
        uint8 count = 0;
        oneCharWidth = TP_ScrDisplayWidthGet("8", 1) + charSpace;
        drawTop =fontHeight + (MAX_SCREEN_HEIGHT - fontHeight*3)/2 ;//¾oÖÐÏÔÊ¾
        drawBottom  = drawTop + fontHeight +1;
        drawLeft_y  = 1;

        while(count < 2)
        {
            drawRight_y = drawLeft_y + oneCharWidth*4 +2;
            drawLeft_m  = drawRight_y + TP_ScrDisplayWidthGet(strYear,strlen(strYear)) +6+3;
            drawRight_m = drawLeft_m + oneCharWidth*2 +2;
            drawLeft_d  = drawRight_m + TP_ScrDisplayWidthGet(strMonth,strlen(strMonth)) +6+3;
            drawRight_d = drawLeft_d + oneCharWidth*2 +2;

            drawLeft_yc  = drawRight_y +2;
            drawRight_yc = drawLeft_m -2;
            drawLeft_mc  = drawRight_m +2;
            drawRight_mc = drawLeft_d -2;
            drawLeft_dc  = drawRight_d +2;
            drawRight_dc = drawLeft_dc + TP_ScrDisplayWidthGet(strDay,strlen(strDay))+2;

            if(count == 0)
            {
                drawLeft_y = (MAX_SCREEN_WIDTH - (drawRight_d - drawLeft_y +1))/2 +2;
            }

            count++;
        }
    }

    TP_ScrDrawRect( drawLeft_y-1 ,
                    drawTop-1,
                    drawRight_y+1,
                    drawBottom+1);
    DrawText(drawLeft_yc,
             drawTop,
             drawRight_yc,
             drawBottom,
             ALIGN_MIDDLE,
             strYear);

    TP_ScrDrawRect( drawLeft_m-1 ,
                    drawTop-1,
                    drawRight_m+1,
                    drawBottom+1);
    DrawText(drawLeft_mc,
             drawTop,
             drawRight_mc,
             drawBottom,
             ALIGN_MIDDLE,
             strMonth);

    TP_ScrDrawRect( drawLeft_d-1 ,
                    drawTop-1,
                    drawRight_d+1,
                    drawBottom+1);
    /*
    DrawText(drawLeft_sc,
             drawTop,
             drawRight_mc,
             drawBottom,
             ALIGN_MIDDLE,
             strHour);
    */

    memcpy(&tempDate, pDate, sizeof(TP_Date));

    memset(strYear, 0, sizeof(strYear));
    sprintf( strYear, "%04d", tempDate.Year);
    memset(strMonth, 0, sizeof(strMonth));
    sprintf( strMonth, "%02d", tempDate.Month);
    memset(strDay, 0, sizeof(strDay));
    sprintf( strDay, "%02d", tempDate.Day);

    while ( 1 )
    {
        TP_NotifyCheck();
        //TP_ScrClrRect(0,MAX_SCREEN_HEIGHT-14, MAX_SCREEN_WIDTH-1, MAX_SCREEN_HEIGHT-1);

        TP_SetDisplayArea( drawLeft_y,
                           drawTop,
                           drawRight_y,
                           drawBottom);
        TP_ScrClsDisplayArea();
        diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                              drawLeft_y,
                              drawRight_y,
                              drawTop);
        TP_LcdPrintf( (uint8 *)strYear);

        TP_SetDisplayArea( drawLeft_m,
                           drawTop,
                           drawRight_m,
                           drawBottom);
        TP_ScrClsDisplayArea();
        diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                              drawLeft_m,
                              drawRight_m,
                              drawTop);
        TP_LcdPrintf( (uint8 *)strMonth);

        TP_SetDisplayArea( drawLeft_d,
                           drawTop,
                           drawRight_d,
                           drawBottom);
        TP_ScrClsDisplayArea();
        diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                              drawLeft_d,
                              drawRight_d,
                              drawTop);
        TP_LcdPrintf( (uint8 *)strDay);

        memset(editData, 0, sizeof(editData));
        switch(editindex)
        {
            case 0:
                TP_SetDisplayArea( drawLeft_y,
                                   drawTop,
                                   drawRight_y,
                                   drawBottom);
                diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                                      drawLeft_y,
                                      drawRight_y,
                                      drawTop);
                memcpy(editData, strYear, strlen(strYear));
                break;
            case 1:
                TP_SetDisplayArea( drawLeft_m,
                                   drawTop,
                                   drawRight_m,
                                   drawBottom);
                diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                                      drawLeft_m,
                                      drawRight_m,
                                      drawTop);
                memcpy(editData, strMonth, strlen(strMonth));
                break;
            case 2:
                TP_SetDisplayArea( drawLeft_d,
                                   drawTop,
                                   drawRight_d,
                                   drawBottom);
                diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                                      drawLeft_d,
                                      drawRight_d,
                                      drawTop);
                memcpy(editData, strDay, strlen(strDay));
                break;
        }
        if(editindex == 0)
        {
            editlen = 4;
        }
        else
        {
            editlen = 2;
        }
        if(displayInfo->FONT == ASCII)
        {
            result = 0x34;
        }
        else
        {
            result = 0x74;
        }
        result = TP_GetString((char*)editData, result, 1, editlen);
        switch(editindex)
        {
            case 0:
                memset(strYear, 0, sizeof(strYear));
                tempDate.Year = atoi(editData);
                sprintf(strYear, "%04d", tempDate.Year);
                break;
            case 1:
                memset(strMonth, 0, sizeof(strMonth));
                tempDate.Month = atoi(editData);
                sprintf(strMonth, "%02d", tempDate.Month);
                break;
            case 2:
                memset(strDay, 0, sizeof(strDay));
                tempDate.Day = atoi(editData);
                sprintf(strDay, "%02d", tempDate.Day);
                break;
        }
        if( result == 0xFA)
        {
            if(editindex < 2)
            {
                editindex++;
            }
            else
            {
                editindex = 2;
            }
        }
        else
        {
            KeyCode = TP_GetInputModeControlListValue();
            if(KeyCode == confirmKeyList[0])
            {
                if(editindex > 0 )
                {
                    editindex--;
                }
                else
                {
                    editindex = 0;
                }
            }
            else if(KeyCode == confirmKeyList[1])
            {
                if(editindex < 2 )
                {
                    editindex++;
                }
                else
                {
                    editindex = 2;
                }
            }
            else if( KeyCode == displayInfo->RF_KeyValue
                    || KeyCode == displayInfo->RKeyValue )
            {
                ifConfirm = FALSE;
                break;
            }
            else if( KeyCode == displayInfo->LF_KeyValue
                    || KeyCode == displayInfo->LKeyValue )
            {
                ifConfirm = TRUE;
                memcpy(pDate, &tempDate, sizeof(TP_Date));
                break;
            }
        }


    }
    TP_ClearInputModeControlList();
    return ifConfirm;
}

Boolean TP_TimeInput( TP_DisplayInfo *displayInfo, TP_Time *pTime )
{
    char strHour[3] ={0};
    char strMin[3] ={0};
    char strSec[3] ={0};
    uint8 KeyCode;
    Boolean ifExit = FALSE;
    Boolean ifConfirm = FALSE;
    uint8 editindex = 0;//
    uint8 editData[3] = {0};
    uint8 editlen = 2;
    uint8 confirmKeyList[4];
    uint8 cancelKeyList[2];
    TP_CharSetT CurrCharSet = {0};
    uint8 fontHeight = 0;
    uint32 charSpace = 0, lineSpace = 0;
    //Ê±·ÖÃëÊýÖµµÄÏÔÊ¾±ß½çÖµ
    uint8 drawTop = 0;
    uint8 drawBottom = 0;
    uint8 drawLeft_h = 0;
    uint8 drawRight_h = 0;
    uint8 drawLeft_m = 0;
    uint8 drawRight_m = 0;
    uint8 drawLeft_s = 0;
    uint8 drawRight_s = 0;
    //Ê±·ÖÃëÌaÊ¾oïµÄÏÔÊ¾±ß½çÖµ
    uint8 drawLeft_hc = 0;
    uint8 drawRight_hc = 0;
    uint8 drawLeft_mc = 0;
    uint8 drawRight_mc = 0;
    uint8 drawLeft_sc = 0;
    uint8 drawRight_sc = 0;

    uint8 oneCharWidth = 0;

    TP_Time tempTime = {0};
    uint8 result = 0;


    if ( NULL == displayInfo || NULL == pTime )
    {
        return FALSE;
    }

    TP_GetCurrentCharSet(&CurrCharSet);
    TP_ScrCls();
    SetScrOption();
    TP_ScrFontSet( displayInfo->FONT );
    fontHeight = TP_GetFontHeight();
    TP_ScrSpaceGet(&charSpace, &lineSpace);
    DrawTitileMenu(displayInfo ,0,0,MAX_SCREEN_WIDTH-1, fontHeight);
    DrawFootMenu( displayInfo ,0,MAX_SCREEN_HEIGHT-fontHeight,MAX_SCREEN_WIDTH-1, MAX_SCREEN_HEIGHT-1 );

    TP_ShowInputNum( 0,0,0 );
    DrawInputIcon(FALSE,0,0);
    TP_SetInputModeControl(TP_KEY_NONE,TP_KEY_NONE,DISPLAY_KEY_POUND);
    memset(confirmKeyList, TP_KEY_NONE, sizeof(confirmKeyList));
    memset(cancelKeyList, TP_KEY_NONE, sizeof(cancelKeyList));
    #ifdef TPS300
    confirmKeyList[0] = TP_KEY_UP;
    confirmKeyList[1] = TP_KEY_DOWN;
    #else
    confirmKeyList[0] = TP_KEY_LEFT;
    confirmKeyList[1] = TP_KEY_RIGHT;
    #endif
    confirmKeyList[2] = displayInfo->LF_KeyValue;
    confirmKeyList[3] = displayInfo->LKeyValue;
    cancelKeyList[0] = displayInfo->RF_KeyValue;
    cancelKeyList[1] = displayInfo->RKeyValue;
    TP_SetInputModeControlList(confirmKeyList, 4, cancelKeyList, 2);

    strcat(strHour,":");
    strcat(strMin,":");
    strcat(strSec,":");

    /**************************************************************
    *ÏÔÊ¾nùÊ½
    * Hour HCaption Minute MCaption Sec SCaption
    ***************************************************************/
    /**************************************************************
    * ¼ÆËãÏÔÊ¾ÇøoòµÄ±ß½çÖµ
    ***************************************************************/
    {
        //ÈÃÆäÔËÐÐa½´ÎÊÇÎªaË¾oÖÐÏÔÊ¾
        uint8 count = 0;
        oneCharWidth = TP_ScrDisplayWidthGet("8", 1) + charSpace;
        drawTop =fontHeight + (MAX_SCREEN_HEIGHT - fontHeight*3)/2 ;//¾oÖÐÏÔÊ¾
        drawBottom  = drawTop + fontHeight +1;
        drawLeft_h  = 1;

        while(count < 2)
        {
            drawRight_h = drawLeft_h + oneCharWidth*2 +2;
            drawLeft_m  = drawRight_h + TP_ScrDisplayWidthGet(strHour,strlen(strHour)) +6+3;
            drawRight_m = drawLeft_m + oneCharWidth*2 +2;
            drawLeft_s  = drawRight_m + TP_ScrDisplayWidthGet(strMin,strlen(strMin)) +6+3;
            drawRight_s = drawLeft_s + oneCharWidth*2 +2;

            drawLeft_hc  = drawRight_h +2;
            drawRight_hc = drawLeft_m -2;
            drawLeft_mc  = drawRight_m +2;
            drawRight_mc = drawLeft_s -2;
            drawLeft_sc  = drawRight_s +2;
            drawRight_sc = drawLeft_sc + TP_ScrDisplayWidthGet(strSec,strlen(strSec))+2;

            if(count == 0)
            {
                drawLeft_h = (MAX_SCREEN_WIDTH - (drawRight_s - drawLeft_h +1))/2 +2;
            }

            count++;
        }
    }

    TP_ScrDrawRect( drawLeft_h-1 ,
                    drawTop-1,
                    drawRight_h+1,
                    drawBottom+1);
    DrawText(drawLeft_hc,
             drawTop,
             drawRight_hc,
             drawBottom,
             ALIGN_MIDDLE,
             strHour);

    TP_ScrDrawRect( drawLeft_m-1 ,
                    drawTop-1,
                    drawRight_m+1,
                    drawBottom+1);
    DrawText(drawLeft_mc,
             drawTop,
             drawRight_mc,
             drawBottom,
             ALIGN_MIDDLE,
             strMin);

    TP_ScrDrawRect( drawLeft_s-1 ,
                    drawTop-1,
                    drawRight_s+1,
                    drawBottom+1);
    /*
    DrawText(drawLeft_sc,
             drawTop,
             drawRight_mc,
             drawBottom,
             ALIGN_MIDDLE,
             strHour);
    */

    memcpy(&tempTime, pTime, sizeof(TP_Time));

    memset(strHour, 0, sizeof(strHour));
    sprintf( strHour, "%02d", tempTime.Hours);
    memset(strMin, 0, sizeof(strMin));
    sprintf( strMin, "%02d", tempTime.Minutes);
    memset(strSec, 0, sizeof(strSec));
    sprintf( strSec, "%02d", tempTime.Seconds);

    while ( 1 )
    {
        TP_NotifyCheck();
        //TP_ScrClrRect(0,MAX_SCREEN_HEIGHT-14, MAX_SCREEN_WIDTH-1, MAX_SCREEN_HEIGHT-1);


        TP_SetDisplayArea( drawLeft_h,
                           drawTop,
                           drawRight_h,
                           drawBottom);
        TP_ScrClsDisplayArea();
        diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                              drawLeft_h,
                              drawRight_h,
                              drawTop);
        TP_LcdPrintf( (uint8 *)strHour);

        TP_SetDisplayArea( drawLeft_m,
                           drawTop,
                           drawRight_m,
                           drawBottom);
        TP_ScrClsDisplayArea();
        diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                              drawLeft_m,
                              drawRight_m,
                              drawTop);
        TP_LcdPrintf( (uint8 *)strMin);

        TP_SetDisplayArea( drawLeft_s,
                           drawTop,
                           drawRight_s,
                           drawBottom);
        TP_ScrClsDisplayArea();
        diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                              drawLeft_s,
                              drawRight_s,
                              drawTop);
        TP_LcdPrintf( (uint8 *)strSec);

        memset(editData, 0, sizeof(editData));
        switch(editindex)
        {
            case 0:
                TP_SetDisplayArea( drawLeft_h,
                                   drawTop,
                                   drawRight_h,
                                   drawBottom);
                diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                                      drawLeft_h,
                                      drawRight_h,
                                      drawTop);
                memcpy(editData, strHour, strlen(strHour));
                break;
            case 1:
                TP_SetDisplayArea( drawLeft_m,
                                   drawTop,
                                   drawRight_m,
                                   drawBottom);
                diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                                      drawLeft_m,
                                      drawRight_m,
                                      drawTop);
                memcpy(editData, strMin, strlen(strMin));
                break;
            case 2:
                TP_SetDisplayArea( drawLeft_s,
                                   drawTop,
                                   drawRight_s,
                                   drawBottom);
                diplayApi_ScrGotoxyEx(CurrCharSet.textDirection,
                                      drawLeft_s,
                                      drawRight_s,
                                      drawTop);
                memcpy(editData, strSec, strlen(strSec));
                break;
        }
        if(displayInfo->FONT == ASCII)
        {
            result = 0x34;
        }
        else
        {
            result = 0x74;
        }
        result = TP_GetString((char*)editData, result, 1, editlen);
        switch(editindex)
        {
            case 0:
                memset(strHour, 0, sizeof(strHour));
                tempTime.Hours = atoi(editData);
                sprintf(strHour, "%02d", tempTime.Hours);
                break;
            case 1:
                memset(strMin, 0, sizeof(strMin));
                tempTime.Minutes = atoi(editData);
                sprintf(strMin, "%02d", tempTime.Minutes);
                break;
            case 2:
                memset(strSec, 0, sizeof(strSec));
                tempTime.Seconds = atoi(editData);
                sprintf(strSec, "%02d", tempTime.Seconds);
                break;
        }
        if( result == 0xFA)
        {
            if(editindex < 2)
            {
                editindex++;
            }
            else
            {
                editindex = 2;
            }
        }
        else
        {
            KeyCode = TP_GetInputModeControlListValue();
            if(KeyCode == confirmKeyList[0])
            {
                if(editindex > 0 )
                {
                    editindex--;
                }
                else
                {
                    editindex = 0;
                }
            }
            else if(KeyCode == confirmKeyList[1])
            {
                if(editindex < 2 )
                {
                    editindex++;
                }
                else
                {
                    editindex = 2;
                }
            }
            else if( KeyCode == displayInfo->RF_KeyValue
                    || KeyCode == displayInfo->RKeyValue )
            {
                ifConfirm = FALSE;
                break;
            }
            else if( KeyCode == displayInfo->LF_KeyValue
                    || KeyCode == displayInfo->LKeyValue )
            {
                ifConfirm = TRUE;
                memcpy(pTime, &tempTime, sizeof(TP_Time));
                break;
            }
        }


    }
    TP_ClearInputModeControlList();
    return ifConfirm;
}


const uint8 PixelTelpoLogo2[]={
    64,128,
    /*--  WidthxHeight=128x64  --*/
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xF0,0xF8,0xFC,
    0xFC,0x7E,0x3E,0x3F,0x1F,0x0F,0x0F,0x07,0x07,0x07,0x07,0x03,0x03,0x03,0x03,0x03,
    0x07,0x07,0x07,0x07,0x0E,0x1E,0x1C,0x3C,0x78,0x70,0xE0,0xC0,0x80,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0x10,0xF0,
    0xF0,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,0x80,0x80,0x08,0x38,0x30,0xC0,0xC0,
    0x40,0x40,0xF8,0xF8,0x40,0x40,0xC0,0xC0,0x40,0x00,0x00,0x00,0x80,0xE0,0xF8,0x78,
    0x50,0x40,0x48,0x78,0x70,0x50,0x40,0x40,0x40,0x40,0x00,0x00,0x00,0x00,0xE0,0xE0,
    0xA0,0xB0,0xB8,0xA8,0xA0,0xA0,0xE0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0xFC,0xFE,0xFF,0xFF,0x7F,0x0F,0x03,0x01,
    0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,
    0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE1,0xE3,0xE3,0xE0,0xE0,
    0xE0,0x60,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x82,0xC2,0x72,0x3F,
    0x1F,0x32,0x62,0xC2,0x82,0x82,0x02,0x02,0x00,0x20,0x23,0xFB,0xFF,0x87,0xFF,0x7F,
    0x0C,0x3C,0xF7,0xC7,0xF4,0x3C,0x0C,0x00,0x00,0x00,0x04,0x06,0x03,0xFF,0xFF,0x00,
    0xF5,0xF5,0x15,0x15,0x15,0x15,0xF5,0xF5,0x00,0x00,0x00,0x00,0x00,0xC0,0xCF,0xEF,
    0xEA,0x0A,0x2A,0xEA,0xCA,0x0A,0x8F,0x8F,0x40,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x20,0xFF,0xFF,0xFF,0xFF,0xFF,0x03,0x00,0x40,0x60,0x70,
    0x38,0x1C,0x8E,0xC7,0xF3,0xF9,0xFC,0xFC,0x1C,0x1C,0x0C,0x0C,0x0C,0x0C,0x0C,0x1C,
    0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0x7C,0x7C,
    0x3C,0x1C,0x1C,0x0C,0x04,0x00,0x00,0x00,0x80,0x84,0x86,0x83,0x81,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x81,0x83,0x83,0x81,0x80,0x80,0x80,0x87,0x87,0x83,0x85,0x84,
    0x86,0x83,0x81,0x80,0x81,0x83,0x87,0x86,0x82,0x80,0x80,0x80,0x80,0x83,0x83,0x80,
    0x83,0x83,0x81,0x81,0x81,0x81,0x83,0x83,0x80,0x80,0x80,0x80,0x81,0x81,0x81,0x81,
    0x83,0x82,0x82,0x82,0x82,0x82,0x83,0x83,0x80,0x81,0x81,0x80,0x80,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x1F,0x7F,0xFF,0xFF,0xFF,0x78,0x00,0x80,0xE0,
    0xF8,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8,0xF0,0xF0,0xE0,0xE0,0xF0,0x70,0x78,
    0x3F,0x3F,0x3F,0x1F,0x1F,0x0F,0x0F,0x07,0x07,0x03,0x01,0x01,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x81,0x81,0x81,0x81,
    0x81,0x01,0x81,0x81,0x81,0x81,0x81,0x81,0x01,0x81,0x81,0x81,0x81,0x01,0x01,0x01,
    0x81,0x81,0x81,0x81,0x81,0x81,0x01,0x81,0x81,0x81,0x81,0x81,0x81,0x01,0x01,0x81,
    0x81,0x81,0x81,0x01,0x01,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,
    0x81,0x81,0x01,0x81,0x81,0x81,0x81,0x81,0x81,0x01,0x01,0x01,0x01,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x81,0xE1,0xF8,0xFE,0xFF,0x7F,0x3F,
    0x3F,0x9F,0xDF,0xCF,0xCF,0x87,0x87,0x87,0x83,0x03,0x81,0x01,0x80,0x00,0x80,0x00,
    0x80,0x80,0x80,0xC0,0xC0,0xC0,0xE0,0x70,0x70,0x38,0x38,0x1C,0x0E,0x0E,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x21,0x3D,0x3F,0x03,0x01,
    0x21,0x20,0x3C,0x3F,0x37,0x34,0x01,0x21,0x20,0x3C,0x3F,0x33,0x30,0x00,0x20,0x20,
    0x3C,0x3F,0x37,0x34,0x01,0x21,0x20,0x3C,0x3F,0x07,0x06,0x07,0x01,0x3C,0x3F,0x23,
    0x30,0x1F,0x0F,0x00,0x30,0x3F,0x1F,0x3F,0x3F,0x1E,0x27,0x21,0x3C,0x3F,0x37,0x34,
    0x01,0x21,0x20,0x3C,0x3F,0x0F,0x3E,0x33,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x01,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
    0x03,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
static waitforkey()
{
    TP_Kbflush();
    while(1)
    {
        if(TP_Kbhit())
        {
            TP_Kbflush();
            break;
        }
    }
}
void TP_DisplayAPITest()
{
    static uint8 fontSize = 0;
    uint8 tempBuf[256] = {0};
    TP_ListBoxRect rect =
    {
        0, 0, 127, 63
    };
    TP_DisplayInfo displayInfo =
    {
        "Data input:",
        ALIGN_LEFT,
        "Confirm",
        "Cancel",
        TP_KEY_CONFIRM,
        TP_KEY_CANCEL,
        TP_KEY_NONE,
        TP_KEY_NONE,
        ASCII
    };
    TP_InputInfo inputInfo =
    {
        INPUTMODE_NUMBER,
        0xF4,
        1,
        128,
        0,
        TRUE,
        TRUE
    };

    uint8 *item[10] =
    {
        "listbox 1",
        "listbox 2",
        "listbox 3",
        "listbox 4",
        "listbox 5",
        "listbox 6",
        "listbox 7",
        "listbox 8",
        "listbox 9"
    };
    TP_ListBoxInfo listBoxInfo[] =
    {
        {"choose 1", 1, PNULL, FALSE},
        {"choose 2", 2, PNULL, FALSE},
        {"choose 3", 3, PNULL, TRUE},
        {"choose 4", 4, PNULL, FALSE},
        {"choose 5", 5, PNULL, FALSE},
    };
    TP_ListBoxInfo optionInfo[] =
    {
        {"Option 1", 1, PNULL, FALSE},
        {"Option 2", 2, PNULL, FALSE},
        {"Option 3", 3, PNULL, TRUE},
        {"Option 4", 4, PNULL, FALSE},
        {"Option 5", 5, PNULL, FALSE},
    };
    uint8 chooseValue = 0;
    TP_DateTime dateTime = {0};
    uint8 result = 0;
    uint8 tempi = 0;

    if(fontSize == 0)
    {
        fontSize = 1;
        displayInfo.FONT = CFONT;
    }
    else
    {
        fontSize = 0;
        displayInfo.FONT = ASCII;
    }
    sprintf(tempBuf, "%s", "01234567890 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz");
    TP_ScrCls();
    TP_DrawFootMenu(0, 10, 127, 10+17, 0,"Confirm","Cancel");
    waitforkey();

    TP_PopupMessage("this is a popupMessage!");
    waitforkey();

    TP_PopupMessageTimed("PopupMessage!!!!", 3000);

    TP_PopupMessageWithIcon(ALERT_OK, "ICON OK", 3000);
    TP_PopupMessageWithIcon(ALERT_ERROR, "ICON error", 3000);
    TP_PopupMessageWithIcon(ALERT_WARNING, "ICON warning", 3000);
    TP_PopupMessageWithIcon(ALERT_INFO, "ICON info", 3000);
    TP_PopupMessageWithIcon(ALERT_QUESTION, "ICON question", 3000);

    displayInfo.strTitle = "popup message with button";
    result = TP_PopupMessageWithButton(&displayInfo);
    TP_DbgSerialPrn("\r\n TP_PopupMessageWithButton :%d", result);

    displayInfo.strTitle = "show text";
    result = TP_ShowText(&displayInfo, rect, tempBuf);
    TP_DbgSerialPrn("\r\n TP_ShowText:%d", result);

    displayInfo.strTitle = "show text full scr";
    result = TP_ShowTextFullScr(&displayInfo, "%s", tempBuf);
    TP_DbgSerialPrn("\r\n TP_ShowTextFullScr:%d", result);

    TP_ShowPicture(0, 0, 127, 63, PixelTelpoLogo2);
    waitforkey();

    displayInfo.strTitle = "text input";
    result = TP_TextInput(&displayInfo, &inputInfo, tempBuf);
    TP_DbgSerialPrn("\r\n TP_TextInput:0x%02X, %s", result, tempBuf);

    memset(tempBuf,0,sizeof(tempBuf));
    displayInfo.strTitle = "number input";
    if(displayInfo.FONT == ASCII)
    {
        inputInfo.inputMode = 0x94;
    }
    else
    {
        inputInfo.inputMode = 0xD4;
    }
    result = TP_NumberInput(&displayInfo, &inputInfo, tempBuf);
    TP_DbgSerialPrn("\r\n TP_NumberInput:0x%02X, %s", result, tempBuf);

    displayInfo.strTitle = "password input";
    if(displayInfo.FONT == ASCII)
    {
        inputInfo.inputMode = 0x940F;
    }
    else
    {
        inputInfo.inputMode = 0xD40F;
    }
    result = TP_PassWordInput(&displayInfo, &inputInfo, '*', tempBuf);
    TP_DbgSerialPrn("\r\n TP_PassWordInput:0x%02X, %s", result, tempBuf);

    TP_ScrCls();
    TP_ShowProgressBar("progress,waiting.....", 25, 75);
    waitforkey();

    displayInfo.strTitle = "listbox show ";
    result = TP_CreateListBoxInterface(TL_LIST,
                                      &displayInfo,
                                      item,
                                      8,
                                      2,
                                      &chooseValue,
                                      PNULL);
    TP_DbgSerialPrn("\r\n TP_CreateListBoxInterface:%d, chooseValue = %d", result, chooseValue);

    displayInfo.strTitle = "checkbox show 3";
    result = TP_CreateCheckBoxInterface(&displayInfo,
                                       listBoxInfo,
                                       3,
                                       TP_KEY_OK);
    TP_DbgSerialPrn("\r\n TP_CreateCheckBoxInterface:%d,", result);
    for(tempi=0; tempi<sizeof(listBoxInfo)/sizeof(TP_ListBoxInfo); tempi++)
    {
        if(listBoxInfo[tempi].selected == TRUE)
        {
            TP_DbgSerialPrn("\r\n %s is selected,%d", listBoxInfo[tempi].caption, listBoxInfo[tempi].value);
        }
    }


    displayInfo.strTitle = "listbox with op show ";
    result = TP_CreateListBoxWithOp(&displayInfo,
                                   TL_LIST,
                                   listBoxInfo,
                                   5,
                                   TL_CHECKBOX,
                                   optionInfo,
                                   3,
                                   TP_KEY_OK);
    TP_DbgSerialPrn("\r\n TP_CreateListBoxWithOp:%d", result);
    for(tempi=0; tempi<sizeof(listBoxInfo)/sizeof(TP_ListBoxInfo); tempi++)
    {
        if(listBoxInfo[tempi].selected == TRUE)
        {
            TP_DbgSerialPrn("\r\n %s is selected,%d", listBoxInfo[tempi].caption, listBoxInfo[tempi].value);
        }
    }
    for(tempi=0; tempi<sizeof(optionInfo)/sizeof(TP_ListBoxInfo); tempi++)
    {
        if(optionInfo[tempi].selected == TRUE)
        {
            TP_DbgSerialPrn("\r\n %s is selected,%d", optionInfo[tempi].caption, optionInfo[tempi].value);
        }
    }

    TP_GetDateTime(&dateTime);
    displayInfo.strTitle = "date input";
    result = TP_DateInput(&displayInfo, &dateTime.Date);
    TP_DbgSerialPrn("\r\n TP_DateInput:%d, %d-%d-%d",
                    result,
                    dateTime.Date.Year,
                    dateTime.Date.Month,
                    dateTime.Date.Day );
    displayInfo.strTitle = "time input";
    TP_TimeInput(&displayInfo, &dateTime.Time);
    TP_DbgSerialPrn("\r\n TP_TimeInput:%d, %d-%d-%d",
                    result,
                    dateTime.Time.Hours,
                    dateTime.Time.Minutes,
                    dateTime.Time.Seconds );
    TP_SetDateTime(&dateTime);


    TP_ScrCls();


}

