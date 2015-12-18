
#include "Tptypedef.h"
#include "TPLcd.h"
#include "TPKeypad.h"
#include "TPFonts.h"
#include "TP_Singleime.h"

#include "TP_DispTest.h"
#include "TP_DisplayAPI.h"

#define FONT_WIDTH  6
#define FONT_HEIGHT 12


#define SECOND_LINE_FONT_TOP    (FONT_HEIGHT+2)
#define SECOND_LINE_FONT_BOT    (SECOND_LINE_FONT_TOP+FONT_HEIGHT)
#define THIRD_LINE_FONT_TOP     (SECOND_LINE_FONT_BOT+4)
#define THIRD_LINE_FONT_BOT     (THIRD_LINE_FONT_TOP+FONT_HEIGHT)

typedef struct __displaySetting
{
    Boolean inited;

    TP_CharSetNameE charSet;

    uint8 fontSize;
    uint8 attr;
    uint8 clsPrint;
    uint8 charSpace;
    uint8 lineSpace;

    uint8 dispLeft;
    uint8 dispRight;
    uint8 dispTop;
    uint8 dispBottom;

    uint8 drawLeft;
    uint8 drawRight;
    uint8 drawTop;
    uint8 drawBottom;

    uint8 fillLeft;
    uint8 fillRight;
    uint8 fillTop;
    uint8 fillBottom;

    uint8 gotoPixelX;
    uint8 gotoPixelY;
    //uint8 gotoX;
    //uint8 gotoY;

    uint8 textData[256];

}TP_DisplaySettingT;

TP_DisplaySettingT dispSetting = {0};

char *MainItem[] =
{
    "Init All",
    "CharSet Choose",
    "Font Set",
    "DisplayArea Set",
    "StartPos Set",
    "Space Set",
    "ClsPrint Set",
    "Attr Set",
    "DrawRect Set",
    "FillRect Set",
    "Disp Text Input",
    "Show Result"
};

typedef enum __MainItemValue
{
    MAINITEM_INIT = 1,
    MAINITEM_CHARSET,
    MAINITEM_FONT,
    MAINITEM_DISPLAY_AREA,
    MAINITEM_START_POS,
    MAINITEM_SPACE,
    MAINITEM_CLSPRINT,
    MAINITEM_ATTR,
    MAINITEM_DRAWRECT,
    MAINITEM_FILLRECT,
    MAINITEM_TEXT_INPUT,
    MAINITEM_SHOW_RESULT,

    MAINITEM_MAX
};

/**************************************************************
*
***************************************************************/
void inputData_2(uint8 *title,
                uint8 *caption1,    uint8 *value1,
                uint8 *caption2,    uint8 *value2 )
{
    uint8 data1[4] = {0};
    uint8 data2[4] = {0};
    uint8 editIndex = 0;
    Boolean IsCancel = FALSE;
    uint8 *leftCaption = "Confirm";
    uint8 *rightCaption = "Cancel";

    TP_ScrRestore(0);

    TP_SetInputModePosition(127, 64);
    TP_ShowInputNum(0, 0, 0);

    sprintf(data1, "%d", *value1);
    sprintf(data2, "%d", *value2);

    while(editIndex < 2)
    {
        TP_ScrCls();
        TP_ScrFontSet(CFONT);
        TP_ScrAttrSet(0);
        TP_ScrSpaceSet(0, 0);
        TP_SetDisplayArea(0, 0, 127, 63);
        if(title != PNULL)
        {
            TP_ScrGotoxyEx(1, 0);
            TP_LcdPrintf("%s",title);
        }
        if(leftCaption != PNULL)
        {
            TP_ScrGotoxyEx(0, 63-FONT_HEIGHT);
            TP_LcdPrintf("%s", leftCaption);
        }
        if(rightCaption != PNULL)
        {
            TP_ScrGotoxyEx(127-FONT_WIDTH*strlen(rightCaption), 63-FONT_HEIGHT);
            TP_LcdPrintf("%s", rightCaption);
        }

        TP_ScrGotoxyEx(1, SECOND_LINE_FONT_TOP);
        TP_LcdPrintf(caption1);
        TP_ScrGotoxyEx(64, SECOND_LINE_FONT_TOP);
        TP_LcdPrintf(caption2);

        TP_ScrDrawRect(64-7*3-4, SECOND_LINE_FONT_TOP-1, 64-2, SECOND_LINE_FONT_BOT+1);
        TP_SetDisplayArea(64-7*3-3, SECOND_LINE_FONT_TOP, 64-3, SECOND_LINE_FONT_BOT);
        TP_ScrGotoxyEx(64-7*3-3, SECOND_LINE_FONT_TOP);
        TP_LcdPrintf(data1);

        TP_ScrDrawRect(127-7*3-2, SECOND_LINE_FONT_TOP-1, 127, SECOND_LINE_FONT_BOT+1);
        TP_SetDisplayArea(127-7*3-1, SECOND_LINE_FONT_TOP, 127-1, SECOND_LINE_FONT_BOT);
        TP_ScrGotoxyEx(127-7*3-1, SECOND_LINE_FONT_TOP);
        TP_LcdPrintf(data2);

        switch(editIndex)
        {
            case 0:
                TP_SetDisplayArea(64-7*3-3, SECOND_LINE_FONT_TOP, 64-3, SECOND_LINE_FONT_BOT);
                TP_ScrGotoxyEx(64-7*3-3, SECOND_LINE_FONT_TOP);
                if(0xFF == TP_GetString(data1, 0xF4, 1, 3))
                {
                    IsCancel = TRUE;
                }
                break;

            case 1:
                TP_SetDisplayArea(127-7*3-1, SECOND_LINE_FONT_TOP, 127-1, SECOND_LINE_FONT_BOT);
                TP_ScrGotoxyEx(127-7*3-1, SECOND_LINE_FONT_TOP);
                if(0xFF == TP_GetString(data2, 0xF4, 1, 3))
                {
                    IsCancel = TRUE;
                }
                break;

            default:
                IsCancel = TRUE;
                break;
        }
        if(IsCancel == TRUE)
        {
            break;
        }
        editIndex++;
    }

    if(IsCancel == FALSE)
    {
        //数据处纈
        *value1 = atoi(data1);
        *value2 = atoi(data2);
    }

    TP_ScrCls();
    TP_ScrRestore(1);
}

void inputData_4(uint8 *title,
                uint8 *caption1,    uint8 *value1,
                uint8 *caption2,    uint8 *value2,
                uint8 *caption3,    uint8 *value3,
                uint8 *caption4,    uint8 *value4 )
{
    uint8 data1[4] = {0};
    uint8 data2[4] = {0};
    uint8 data3[4] = {0};
    uint8 data4[4] = {0};
    uint8 editIndex = 0;
    Boolean IsCancel = FALSE;
    uint8 *leftCaption = "Confirm";
    uint8 *rightCaption = "Cancel";

    TP_ScrRestore(0);

    TP_SetInputModePosition(127, 64);
    TP_ShowInputNum(0, 0, 0);

    sprintf(data1, "%d", *value1);
    sprintf(data2, "%d", *value2);
    sprintf(data3, "%d", *value3);
    sprintf(data4, "%d", *value4);

    while(editIndex < 4)
    {
        TP_ScrCls();
        TP_ScrFontSet(CFONT);
        TP_ScrAttrSet(0);
        TP_ScrSpaceSet(0, 0);
        TP_SetDisplayArea(0, 0, 127, 63);
        if(title != PNULL)
        {
            TP_ScrGotoxyEx(1, 0);
            TP_LcdPrintf("%s",title);
        }
        if(leftCaption != PNULL)
        {
            TP_ScrGotoxyEx(0, 63-FONT_HEIGHT);
            TP_LcdPrintf("%s", leftCaption);
        }
        if(rightCaption != PNULL)
        {
            TP_ScrGotoxyEx(127-FONT_WIDTH*strlen(rightCaption), 63-FONT_HEIGHT);
            TP_LcdPrintf("%s", rightCaption);
        }

        TP_ScrGotoxyEx(1, SECOND_LINE_FONT_TOP);
        TP_LcdPrintf(caption1);
        TP_ScrGotoxyEx(64, SECOND_LINE_FONT_TOP);
        TP_LcdPrintf(caption2);

        TP_ScrGotoxyEx(1, THIRD_LINE_FONT_TOP);
        TP_LcdPrintf(caption3);
        TP_ScrGotoxyEx(64, THIRD_LINE_FONT_TOP);
        TP_LcdPrintf(caption4);

        TP_ScrDrawRect(64-7*3-4, SECOND_LINE_FONT_TOP-1, 64-2, SECOND_LINE_FONT_BOT+1);
        TP_SetDisplayArea(64-7*3-3, SECOND_LINE_FONT_TOP, 64-3, SECOND_LINE_FONT_BOT);
        TP_ScrGotoxyEx(64-7*3-3, SECOND_LINE_FONT_TOP);
        TP_LcdPrintf(data1);

        TP_ScrDrawRect(127-7*3-2, SECOND_LINE_FONT_TOP-1, 127, SECOND_LINE_FONT_BOT+1);
        TP_SetDisplayArea(127-7*3-1, SECOND_LINE_FONT_TOP, 127-1, SECOND_LINE_FONT_BOT);
        TP_ScrGotoxyEx(127-7*3-1, SECOND_LINE_FONT_TOP);
        TP_LcdPrintf(data2);

        TP_ScrDrawRect(64-7*3-4, THIRD_LINE_FONT_TOP-1, 64-2, THIRD_LINE_FONT_BOT+1);
        TP_SetDisplayArea(64-7*3-3, THIRD_LINE_FONT_TOP, 64-3, THIRD_LINE_FONT_BOT);
        TP_ScrGotoxyEx(64-7*3-3, THIRD_LINE_FONT_TOP);
        TP_LcdPrintf(data3);

        TP_ScrDrawRect(127-7*3-2, THIRD_LINE_FONT_TOP-1, 127, THIRD_LINE_FONT_BOT+1);
        TP_SetDisplayArea(127-7*3-1, THIRD_LINE_FONT_TOP, 127-1, THIRD_LINE_FONT_BOT);
        TP_ScrGotoxyEx(127-7*3-1, THIRD_LINE_FONT_TOP);
        TP_LcdPrintf(data4);

        switch(editIndex)
        {
            case 0:
                TP_SetDisplayArea(64-7*3-3, SECOND_LINE_FONT_TOP, 64-3, SECOND_LINE_FONT_BOT);
                TP_ScrGotoxyEx(64-7*3-3, SECOND_LINE_FONT_TOP);
                if(0xFF == TP_GetString(data1, 0xF4, 1, 3))
                {
                    IsCancel = TRUE;
                }
                break;

            case 1:
                TP_SetDisplayArea(127-7*3-1, SECOND_LINE_FONT_TOP, 127-1, SECOND_LINE_FONT_BOT);
                TP_ScrGotoxyEx(127-7*3-1, SECOND_LINE_FONT_TOP);
                if(0xFF == TP_GetString(data2, 0xF4, 1, 3))
                {
                    IsCancel = TRUE;
                }
                break;

            case 2:
                TP_SetDisplayArea(64-7*3-3, THIRD_LINE_FONT_TOP, 64-3, THIRD_LINE_FONT_BOT);
                TP_ScrGotoxyEx(64-7*3-3, THIRD_LINE_FONT_TOP);
                if(0xFF == TP_GetString(data3, 0xF4, 1, 3))
                {
                    IsCancel = TRUE;
                }
                break;

            case 3:
                TP_SetDisplayArea(127-7*3-1, THIRD_LINE_FONT_TOP, 127-1, THIRD_LINE_FONT_BOT);
                TP_ScrGotoxyEx(127-7*3-1, THIRD_LINE_FONT_TOP);
                if(0xFF == TP_GetString(data4, 0xF4, 1, 3))
                {
                    IsCancel = TRUE;
                }
                break;

            default:
                IsCancel = TRUE;
                break;
        }
        if(IsCancel == TRUE)
        {
            break;
        }
        editIndex++;
    }

    if(IsCancel == FALSE)
    {
        //数据处纈
        *value1 = atoi(data1);
        *value2 = atoi(data2);
        *value3 = atoi(data3);
        *value4 = atoi(data4);
    }

    TP_ScrCls();
    TP_ScrRestore(1);
}

static void disp_init(Boolean IsShowInterface)
{
    uint8 selectIndex = 0;
    uint8 chooseValue = 0;
    TP_DisplayInfo displayInfo =
    {
        "Init All",
        ALIGN_LEFT,
        "Confirm",
        "Cancel",
        KEY_DISP_CONFIRM1,
        KEY_DISP_CANCEL1,
        KEY_DISP_CONFIRM2,
        KEY_DISP_CANCEL2,
        CFONT
    };
    char *item[] =
    {
        "NO",
        "YES"
    };
    selectIndex = 1;
    if(IsShowInterface == TRUE)
    {
        if(FALSE == TP_CreateListBoxInterface(TL_RADIOBOX,
                                             &displayInfo,
                                             item,
                                             sizeof(item)/sizeof(char*),
                                             selectIndex,
                                             &chooseValue,
                                             PNULL) )
        {
            return;
        }
    }

    if(IsShowInterface == FALSE
       || chooseValue == 2)
    {
        memset(&dispSetting, 0, sizeof(TP_DisplaySettingT));
        dispSetting.charSet = CHARSET_ASCII;
        dispSetting.fontSize = CFONT;
        dispSetting.dispRight = 127;
        dispSetting.dispBottom = 63;
        sprintf(dispSetting.textData, "%s", "0123456789 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        dispSetting.inited = TRUE;
    }
}

static void disp_charSet()
{
    TP_CharSetNameE temp;
    TP_CharSetT backCharSet = {0};
    uint8 charSetArray[NUM_OF_CHARSET] = {0};
    uint8* charSetName[NUM_OF_CHARSET] = {0};
    uint8 charSetCount = 0;

    uint8 selectIndex = 1;
    uint8 chooseValue = 0;
    TP_DisplayInfo displayInfo =
    {
        "CharSet Choose",
        ALIGN_LEFT,
        "Confirm",
        "Cancel",
        KEY_DISP_CONFIRM1,
        KEY_DISP_CANCEL1,
        KEY_DISP_CONFIRM2,
        KEY_DISP_CANCEL2,
        CFONT
    };

    TP_GetCurrentCharSet(&backCharSet);

    for(temp=CHARSET_ASCII; temp<NUM_OF_CHARSET; temp++)
    {
        if(TP_SetCurrentCharSet(temp)==TRUE)
        {
            charSetArray[charSetCount] = temp;
            switch(temp)
            {
                case CHARSET_ASCII:
                    charSetName[charSetCount] = "ASCII";
                    break;
                case CHARSET_WEST:
                    charSetName[charSetCount] = "English";
                    break;
                case CHARSET_LATIN0:
                    charSetName[charSetCount] = "LATIN0";
                    break;
                case CHARSET_GBK:
                    charSetName[charSetCount] = "Chinese";
                    break;
                case CHARSET_ARABIC_CP1256:
                    charSetName[charSetCount] = "Arabic";
                    break;
                case CHARSET_THAI:
                    charSetName[charSetCount] = "Thai";
                    break;
                case CHARSET_DIY:
                    charSetName[charSetCount] = "DIY";
                    break;
                default:
                    charSetName[charSetCount] = "Unknow";
                    break;
            }
            charSetCount++;

            if(temp == dispSetting.charSet)
            {
                selectIndex = charSetCount;
            }
        }

    }

    TP_SetCurrentCharSet(backCharSet.name);

    if(TRUE == TP_CreateListBoxInterface(TL_RADIOBOX,
                                         &displayInfo,
                                         charSetName,
                                         charSetCount,
                                         selectIndex,
                                         &chooseValue,
                                         PNULL) )
    {
        dispSetting.charSet = charSetArray[chooseValue-1];
        TP_DbgSerialPrn("\r\n charset:%d",dispSetting.charSet);
    }

}

static void disp_font()
{
    uint8 selectIndex = 0;
    uint8 chooseValue = 0;
    TP_DisplayInfo displayInfo =
    {
        "Font Set",
        ALIGN_LEFT,
        "Confirm",
        "Cancel",
        KEY_DISP_CONFIRM1,
        KEY_DISP_CANCEL1,
        KEY_DISP_CONFIRM2,
        KEY_DISP_CANCEL2,
        CFONT
    };
    char *item[] =
    {
        "ASCII",
        "CFONT"
    };
    selectIndex = (dispSetting.fontSize==ASCII)?1:2;
    if(TRUE == TP_CreateListBoxInterface(TL_RADIOBOX,
                                         &displayInfo,
                                         item,
                                         sizeof(item)/sizeof(char*),
                                         selectIndex,
                                         &chooseValue,
                                         PNULL) )
    {
        dispSetting.fontSize = (chooseValue==1)?ASCII:CFONT;
    }
}

static void disp_displayArea()
{
    inputData_4("Display Area Set",
                "left:",    &dispSetting.dispLeft,
                "right:",   &dispSetting.dispRight,
                "top:",     &dispSetting.dispTop,
                "bottom:",  &dispSetting.dispBottom );
}

static void disp_startpos()
{
    inputData_2("Start Pos Set",
                "left:",    &dispSetting.gotoPixelX,
                "top:",     &dispSetting.gotoPixelY );
}

static void disp_space()
{
    inputData_2("Space Set",
                "char:",    &dispSetting.charSpace,
                "line:",     &dispSetting.lineSpace );
}

static void disp_clsprint()
{
    uint8 selectIndex = 0;
    uint8 chooseValue = 0;
    TP_DisplayInfo displayInfo =
    {
        "Clsprint Set",
        ALIGN_LEFT,
        "Confirm",
        "Cancel",
        KEY_DISP_CONFIRM1,
        KEY_DISP_CANCEL1,
        KEY_DISP_CONFIRM2,
        KEY_DISP_CANCEL2,
        CFONT
    };
    char *item[] =
    {
        "OFF",
        "ON"
    };
    selectIndex = (dispSetting.clsPrint==0)?1:2;
    if(TRUE == TP_CreateListBoxInterface(TL_RADIOBOX,
                                         &displayInfo,
                                         item,
                                         sizeof(item)/sizeof(char*),
                                         selectIndex,
                                         &chooseValue,
                                         PNULL) )
    {
        dispSetting.clsPrint = (chooseValue==1)?0:1;
    }
}

static void disp_attr()
{
    uint8 selectIndex = 0;
    uint8 chooseValue = 0;
    TP_DisplayInfo displayInfo =
    {
        "Attr Set",
        ALIGN_LEFT,
        "Confirm",
        "Cancel",
        KEY_DISP_CONFIRM1,
        KEY_DISP_CANCEL1,
        KEY_DISP_CONFIRM2,
        KEY_DISP_CANCEL2,
        CFONT
    };
    char *item[] =
    {
        "OFF",
        "ON"
    };
    selectIndex = (dispSetting.attr==0)?1:2;
    if(TRUE == TP_CreateListBoxInterface(TL_RADIOBOX,
                                         &displayInfo,
                                         item,
                                         sizeof(item)/sizeof(char*),
                                         selectIndex,
                                         &chooseValue,
                                         PNULL) )
    {
        dispSetting.attr = (chooseValue==1)?0:1;
    }
}

static void disp_drawRect()
{
    inputData_4("DrawRect Set",
                "left:",    &dispSetting.drawLeft,
                "right:",   &dispSetting.drawRight,
                "top:",     &dispSetting.drawTop,
                "bottom:",  &dispSetting.drawBottom );
}

static void disp_fillRect()
{
    inputData_4("FillRect Set",
                "left:",    &dispSetting.fillLeft,
                "right:",   &dispSetting.fillRight,
                "top:",     &dispSetting.fillTop,
                "bottom:",  &dispSetting.fillBottom );
}

static void disp_textInput()
{
    #if 1
    TP_CharSetT CurrCharSet = {0};
    TP_CharSetT CurrCharSet2 = {0};
    TP_ListBoxRect rect ={2,3+12,MAX_SCREEN_WIDTH-3,MAX_SCREEN_HEIGHT - 1 - 12};
    uint8 confirmKeyList[2] = {KEY_DISP_CONFIRM1,KEY_DISP_CONFIRM2};
    uint8 cancelKeyList[2] = {KEY_DISP_CANCEL1, KEY_DISP_CANCEL2};
    uint8 result = 0;

    TP_ScrCls();
    TP_ScrSpaceSet(0, 0);
    TP_ScrClsPrint(0);
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, MAX_SCREEN_WIDTH-1, MAX_SCREEN_HEIGHT-1);
    TP_ScrGotoxyEx(0, 0);
    TP_LcdPrintf("%s", "Display Text");
    TP_ScrGotoxyEx(0, MAX_SCREEN_HEIGHT-1-12);
    TP_LcdPrintf("%s", "Confirm");
    TP_ScrGotoxyEx(MAX_SCREEN_WIDTH-1-TP_ScrDisplayWidthGet("Cancel", strlen("Cancel")),
                   MAX_SCREEN_HEIGHT-1-12);
    TP_LcdPrintf("%s", "Cancel");


    TP_ScrDrawRect( rect.left, rect.top, rect.right, rect.bottom );
    {
        uint8 left = rect.right-40,top = rect.top-5;
        TP_ScrClrRect(left,top,left+3*8+10,top+8);
        TP_ScrDrawRect( left,top,left+3*8+10,top+8);
        TP_ShowInputNum( 1, left+1, top+2 );
    }
    TP_SetInputModePosition(58,55);

    TP_SetInputModeControlEx( KEY_DISP_CONFIRM1, KEY_DISP_CONFIRM2,
                              KEY_DISP_CANCEL1, TP_KEY_CLEAR, DISPLAY_KEY_POUND);
    TP_SetInputModeControlList(confirmKeyList, 2,cancelKeyList,2);

    TP_SetDisplayArea( rect.left+2,rect.top+4,rect.right-1,rect.bottom-1);

    TP_SetCurrentCharSet(dispSetting.charSet);
    TP_GetCurrentCharSet(&CurrCharSet2);
    if(CurrCharSet2.textDirection == 0)
    {
        TP_ScrGotoxyEx(rect.left+2,rect.top+4);
    }
    else
    {
        TP_ScrGotoxyEx(rect.right-1,rect.top+4);
    }
    do
    {
        result = TP_GetHzString((char*) dispSetting.textData, 0, 255 );
    }while(result == 0xFB && strlen((char *)dispSetting.textData)< 1);
    TP_SetCurrentCharSet(CurrCharSet.name);

    TP_ClearInputModeControlList();

    #else
    TP_DisplayInfo displayInfo =
    {
        "Display Text",
        ALIGN_LEFT,
        "Confirm",
        "Cancel",
        KEY_DISP_CONFIRM1,
        KEY_DISP_CANCEL1,
        KEY_DISP_CONFIRM2,
        KEY_DISP_CANCEL2,
        CFONT
    };
    TP_InputInfo inputInfo =
    {
        INPUTMODE_NUMBER,
        0,
        1,
        255,
        0,
        TRUE,
        TRUE
    };
    TP_CharSetT CurrCharSet = {0};

    TP_GetCurrentCharSet(&CurrCharSet);
    TP_SetCurrentCharSet(dispSetting.charSet);

    TP_TextInput(&displayInfo, &inputInfo, dispSetting.textData);

    TP_SetCurrentCharSet(CurrCharSet.name);

    #endif
}

static void disp_showResult()
{
    TP_CharSetT CurrCharSet = {0};

    TP_GetCurrentCharSet(&CurrCharSet);
    TP_SetCurrentCharSet(dispSetting.charSet);

    TP_ScrCls();
    TP_ScrFontSet(dispSetting.fontSize);
    TP_SetDisplayArea(dispSetting.dispLeft,
                      dispSetting.dispTop,
                      dispSetting.dispRight,
                      dispSetting.dispBottom);
    TP_ScrGotoxyEx(dispSetting.gotoPixelX,
                   dispSetting.gotoPixelY);
    TP_ScrSpaceSet(dispSetting.charSpace,
                   dispSetting.lineSpace);
    TP_ScrClsPrint(dispSetting.clsPrint);
    TP_ScrAttrSet(dispSetting.attr);
    TP_ScrDrawRect(dispSetting.drawLeft,
                   dispSetting.drawTop,
                   dispSetting.drawRight,
                   dispSetting.drawBottom );
    TP_ScrFillRect(dispSetting.fillLeft,
                   dispSetting.fillTop,
                   dispSetting.fillRight,
                   dispSetting.fillBottom);
    TP_LcdPrintf("%s", dispSetting.textData);

    TP_Kbflush();
    while(1)
    {
        if(TP_Kbhit())
        {
            TP_Kbflush();
            break;
        }
    }

    TP_ScrSpaceSet(0, 0);
    TP_ScrAttrSet(0);
    TP_ScrClsPrint(0);
    TP_SetCurrentCharSet(CurrCharSet.name);

}

void TP_DisplayTest()
{
    uint8 selectIndex = 0;
    uint8 chooseValue = 0;
    TP_DisplayInfo displayInfo =
    {
        "Display Test",
        ALIGN_LEFT,
        "Confirm",
        "Cancel",
        KEY_DISP_CONFIRM1,
        KEY_DISP_CANCEL1,
        KEY_DISP_CONFIRM2,
        KEY_DISP_CANCEL2,
        CFONT
    };
    TP_CharSetT CurrCharSet = {0};

    TP_GetCurrentCharSet(&CurrCharSet);
    TP_SetCurrentCharSet(CHARSET_ASCII);

    TP_ScrSpaceSet(0, 0);
    TP_ScrAttrSet(0);
    TP_ScrClsPrint(0);

    if(dispSetting.inited == FALSE)
    {
        disp_init(FALSE);
    }

    while(1)
    {
        if(TRUE == TP_CreateListBoxInterface(TL_LIST,
                                             &displayInfo,
                                             MainItem,
                                             sizeof(MainItem)/sizeof(char*),
                                             selectIndex,
                                             &chooseValue,
                                             PNULL) )
        {
            switch(chooseValue)
            {
                case MAINITEM_INIT:
                    disp_init(TRUE);
                    break;
                case MAINITEM_CHARSET:
                    disp_charSet();
                    break;
                case MAINITEM_FONT:
                    disp_font();
                    break;
                case MAINITEM_DISPLAY_AREA:
                    disp_displayArea();
                    break;
                case MAINITEM_START_POS:
                    disp_startpos();
                    break;
                case MAINITEM_SPACE:
                    disp_space();
                    break;
                case MAINITEM_CLSPRINT:
                    disp_clsprint();
                    break;
                case MAINITEM_ATTR:
                    disp_attr();
                    break;
                case MAINITEM_DRAWRECT:
                    disp_drawRect();
                    break;
                case MAINITEM_FILLRECT:
                    disp_fillRect();
                    break;
                case MAINITEM_TEXT_INPUT:
                    disp_textInput();
                    break;
                case MAINITEM_SHOW_RESULT:
                    disp_showResult();
                    break;
                default:
                    break;
            }
            selectIndex = chooseValue;
        }
        else
        {
            break;
        }
    }

    TP_SetCurrentCharSet(CurrCharSet.name);
    TP_ScrCls();
}

