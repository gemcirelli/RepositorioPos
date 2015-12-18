#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeypad.h>
#include <TPFonts.h>
#include <TPLCD.h>

#include "TP_ListBox.h"
#include "TP_DisplayAPI.h"

#define min(x, y) ((x) < (y) ? (x) : (y))

//Declare Icons for Radio or Check Box
///Icon for Un-Selected in Radio Box
uint8 TP_ListBoxRadioUncheck[] =
{
    0x0C,0x0C,0x00,0x00,0xF0,0x08,0x04,0x04,0x04,0x04,0x08,0xE0,0x00,0x00,
0x00,0x00,0x00,0x01,0x02,0x02,0x02,0x02,0x01,0x00,0x00,0x00
};
///Icon for Selected in Radio Box
uint8 TP_ListBoxRadiocheck[] =
{
    0x0C,0x0C,0x00,0x00,0xF0,0x08,0x64,0xF4,0xF4,0x64,0x08,0xE0,0x00,0x00,
0x00,0x00,0x00,0x01,0x02,0x02,0x02,0x02,0x01,0x00,0x00,0x00
};
///Icon for Un-Selected in Check Box
uint8 TP_ListBoxCheckBoxUncheck[] =
{
    0x0C,0x0C,0x00,0x00,0xF8,0x08,0x08,0x08,0x08,0x08,0x08,0xF8,0x00,0x00,
0x00,0x00,0x07,0x04,0x04,0x04,0x04,0x04,0x04,0x07,0x00,0x00
};
///Icon for Selected in Check Box
uint8 TP_ListBoxCheckBoxcheck[] =
{
    0x0C,0x0C,0x00,0x00,0xF8,0x48,0x88,0x88,0x68,0x18,0x0E,0xF9,0x00,0x00,
0x00,0x00,0x07,0x04,0x04,0x05,0x04,0x04,0x04,0x07,0x00,0x00
};

#if 0
static
uint16 TP_ScrDisplayWidthGet(const uint8* pText,
                             uint16 nTextChars  )
{
    TP_DotsInfoT DotsInfo = {0};
    uint16 dealedChars = 0;
    uint16 tempChars = 0;
    uint32 charSpace = 0,lineSpace = 0;
    uint16 width = 0;

    TP_ScrSpaceGet(&charSpace, &lineSpace);
    while(dealedChars < nTextChars)
    {
        memset(&DotsInfo, 0, sizeof(TP_DotsInfoT));
        tempChars = TP_ScrFontInfoGet(&pText[dealedChars],
                                      nTextChars - dealedChars,
                                      &DotsInfo);
        if(tempChars == 0)
        {
            break;
        }
        width += DotsInfo.Width;
        if(dealedChars < (nTextChars-1))
        {
            width += charSpace;
        }
        dealedChars += tempChars;

    }
    return width;
}
#endif

void DrawListBox(TP_ListBox* ListBox,uint8 UpdateAll);

/**
 * Function description of TP_NewListBox() £º initialization for list box
 * @param listboxstyle  style of list box
 * @param rect  location of list box
 * @param Title
 * @param Button  prompt of button
 * @return list item
 * @author  Zuo Jun
 * @version 1.0
 * @date 20120205
 * @note for list items, check box can select multiple items, radio box and list box can select only one item.
 */
TP_ListBox* TP_NewListBox(TP_ListBoxStyle listboxstyle,TP_ListBoxRect rect,uint8* Title,uint8* Button,ListBoxFunc listboxfunc_p)
{
    TP_ListBox* listbox = PNULL;
    TP_AllocMemory(sizeof(TP_ListBox), (void**)&listbox);
    if (listbox == PNULL)
    {
        return PNULL;
    }
    memset(listbox,0,sizeof(TP_ListBox));
    if (listboxstyle >= TL_MAX)
    {
        listboxstyle = TL_NO_EFFECT;
    }
    listbox->listboxstyle = listboxstyle;
    //listbox->Title = Title;
    //listbox->Button= Button;
    if(Title != NULL)
    {
        memcpy(listbox->Title,Title,min(strlen((char *)Title),LISTBOX_TITIL_LENGTH-1));
    }
    if(Button != NULL)
    {
        memcpy(listbox->Button,Button,min(strlen((char *)Button),LISTBOX_BUTTON_STRING_LENGTH-1));
    }
    listbox->StartPos = 0xFFFF;
    listbox->CurrPage = 0xFF;

    listbox->ifDrawScrBall = TRUE;
    listbox->TitleAlign = ALIGN_MIDDLE;
    //listbox->ifNumSelect = TRUE;

    listbox->ConfirmKey = TP_KEY_NONE;
    listbox->CancelKey = TP_KEY_NONE;
    listbox->SelectKey = TP_KEY_NONE;
    listbox->LF_KeyValue = TP_KEY_NONE;
    listbox->RF_KeyValue = TP_KEY_NONE;

    memcpy(&listbox->DisplayRect,&rect,sizeof(TP_ListBoxRect));
    listbox->listboxfunc = listboxfunc_p;
    return listbox;
}

/**
 * º¯ Êý TP_ListBoxDeleteItem() ¹¦ÄÜÃèÊö£ºe¾³ýÒ»¸öaÐ±iÏî
 * @param ListBox  aÐ±i¶ÔÏo
 * @param Index  aÐ±iÏîË÷Òý
 * @return ÎÞ
 * @author  ×o¿¡
 * @version 1.0
 * @date 20120205
 * @note
 */

void TP_ListBoxDeleteItem(TP_ListBox* ListBox,uint8 Index)
{
    uint8 LIndex;
    if (ListBox == NULL)
    {
        return;
    }

    if (ListBox->ItemCount <= Index)
    {
        return;
    }


    if (ListBox->ItemList[Index] != PNULL)
    {
        TP_FreeMemory((void**)&ListBox->ItemList[Index]);
    }
    ListBox->ItemList[Index]      = PNULL;
    if(Index == ListBox->ItemCount-1)
    {
        ListBox->ItemValueList[Index] = 0;
        ListBox->ItemIconList[Index]  = PNULL;
    }
    else
    {
        for (LIndex = Index;LIndex <ListBox->ItemCount-1;LIndex++ )
        {
            ListBox->ItemList[LIndex] = ListBox->ItemList[LIndex+1];
            ListBox->ItemValueList[LIndex] = ListBox->ItemValueList[LIndex+1];
            ListBox->ItemIconList[LIndex] = ListBox->ItemIconList[LIndex+1];
        }
        ListBox->ItemList[ListBox->ItemCount-1]      = PNULL;
        ListBox->ItemValueList[ListBox->ItemCount-1] = 0;
        ListBox->ItemIconList[ListBox->ItemCount-1]  = PNULL;
    }

    //if (ListBox->SelectItemIndex == ListBox->ItemCount)
    {
        if (ListBox->SelectItemIndex >0 && ListBox->SelectItemIndex >= Index )
        {
            ListBox->SelectItemIndex --;
        }
    }
    if(ListBox->ItemCount>0)
    {
        ListBox->ItemCount--;
    }

    if(ListBox->ItemCount>0)
    {
        DrawListBox(ListBox,1);
    }
}


/**
 * Function Description of TP_ListBoxAddItem()£ºinitialization for list box
 * @param ListBox  items
 * @param Caption  description
 * @param Value   value
 * @param IsSelected selected
 * @param Icon point, only when parameter is TP_ListBoxStyle, it will be set to be TL_ICONLIST
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20120205
 * @note
 */
void TP_ListBoxAddItem(TP_ListBox* ListBox,
                              uint8* Caption,
                              uint32 Value,
                              uint8 IsSelected,
                              uint8* Icon)
{
    if (ListBox == NULL)
    {
        return;
    }

    if (ListBox->ItemCount >= LISTBOX_MAX_COUNT)
    {
        return;
    }
    /*Modify by linhx 2012-09-03*/
    if (Caption != PNULL)
    {
        ListBox->ItemList[ListBox->ItemCount] = PNULL;
        TP_AllocMemory(strlen((char *)Caption)+1,(void**)&ListBox->ItemList[ListBox->ItemCount]);
        if(ListBox->ItemList[ListBox->ItemCount] == PNULL)
        {
            return;
        }
        memset(ListBox->ItemList[ListBox->ItemCount],0x00,strlen((char *)Caption)+1);
        memcpy(ListBox->ItemList[ListBox->ItemCount],Caption,strlen((char *)Caption));
    }
    else
    {
        ListBox->ItemList[ListBox->ItemCount] = PNULL;
    }
    /*old*/
    //ListBox->ItemList[ListBox->ItemCount] = Caption;

    /*End modify by linhx 2012-09-03*/

    ListBox->ItemValueList[ListBox->ItemCount] = Value;
    ListBox->ItemIconList[ListBox->ItemCount] = Icon;


    if (IsSelected)
    {
        switch (ListBox->listboxstyle)
        {
            case TL_NO_EFFECT :
            case TL_ICONLIST:
            case TL_LIST :
            case TL_RADIOBOX :
                ListBox->SelectItems[0] = ListBox->ItemCount;
                ListBox->SelectItemValue[0] = Value;
                ListBox->SelectItemCount =1;
                ListBox->SelectItemIndex = ListBox->ItemCount;
                break;
            case TL_CHECKBOX :
                ListBox->SelectItems[ListBox->SelectItemCount] = ListBox->ItemCount;
                ListBox->SelectItemValue[ListBox->SelectItemCount] = Value;
                ListBox->SelectItemCount++;
                break;
        }


    }

    ListBox->ItemCount++;
}

//²åÈëItemµ½Ö¸¶¨µÄÎ»ÖÃ
//¸uTP_ListBoxAddItem²»i¬µÄÊÇ¶à¶¨ÒåaËÒ»¸ö²åÈëµÄÎ»ÖÃ£¬oÃouÌØÊâÐèÒªÔö¼oµÄÏîÄ¿
void TP_ListBoxInsertItem(TP_ListBox* ListBox,
                                uint8 index,
                                uint8* Caption,
                                uint32 Value,
                                uint8 IsSelected,
                                uint8* Icon,
                                Boolean UpdateNow)
{
    uint8 tempi;

    if (ListBox == NULL)
    {
        return;
    }

    if ( index >= LISTBOX_MAX_COUNT)
    {
        return;
    }
    if( index >= ListBox->ItemCount )//³¬¹ýitemcount£¬·Åµ½×îºoÃæ
    {
        TP_ListBoxAddItem(ListBox,Caption,Value,IsSelected,Icon);
        if(UpdateNow)
        {
            DrawListBox(ListBox,1);
        }
        return;
    }
    //½«ÄuÈÝiùºoÒÆ¶¯
    tempi=(ListBox->ItemCount<LISTBOX_MAX_COUNT) ? ListBox->ItemCount : (ListBox->ItemCount-1);//·ÀÖ¹Ô½½ç
    for( ; tempi>index; tempi--)
    {
        ListBox->ItemList[tempi]      = ListBox->ItemList[tempi-1];
        ListBox->ItemValueList[tempi] = ListBox->ItemValueList[tempi-1];
        ListBox->ItemIconList[tempi]  = ListBox->ItemIconList[tempi-1];
    }
    if(Caption != PNULL)
    {
        ListBox->ItemList[index] = PNULL;
        TP_AllocMemory(strlen((char *)Caption)+1, (void * *)& ListBox->ItemList[index]);
        if(ListBox->ItemList[index] == PNULL)
        {
            return;
        }
        memset(ListBox->ItemList[index],0x00,strlen((char *)Caption)+1);
        memcpy(ListBox->ItemList[index],Caption,strlen((char *)Caption));
    }
    ListBox->ItemValueList[index] = Value;
    ListBox->ItemIconList[index]  = Icon;


    if (IsSelected)
    {
        switch (ListBox->listboxstyle)
        {
            case TL_ICONLIST:
            case TL_LIST :
            case TL_RADIOBOX :
                ListBox->SelectItems[0] = ListBox->ItemCount;
                ListBox->SelectItemValue[0] = Value;
                ListBox->SelectItemCount =1;
                ListBox->SelectItemIndex = ListBox->ItemCount;
                break;
            case TL_CHECKBOX :
                ListBox->SelectItems[index] = ListBox->ItemCount;
                ListBox->SelectItemValue[index] = Value;
                ListBox->SelectItemCount++;
                break;
        }
    }
    else
    {
        if(ListBox->SelectItemIndex >= index)
        {
            ListBox->SelectItemIndex++;
            //ListBox->StartPos = 0xff;
            //ListBox->CurrPage = 0xff;
            if(ListBox->CurrPage != 0)//´oi·ÏÔÊ¾Ê±²»¸ü¸Ä
            {
                ListBox->CurrPage++;
            }
        }
    }
    if(ListBox->ItemCount < LISTBOX_MAX_COUNT )
    {
        ListBox->ItemCount++;
    }
    ListBox->LoopDisplayPos = 0;

    if(UpdateNow)
    {
        DrawListBox(ListBox,1);
    }

}


/**
 * Function description of DrawListBox() £ºdraw list items
 * @param ListBox  list items
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20120205
 * @note
 */
void DrawListBox(TP_ListBox* ListBox,uint8 UpdateAll)
{
    uint8 MaxRowOnePage;
    ST_FONT SFont;
    ST_FONT MutlFont;
    uint32 charSpace,lineSpace;
    uint16 Index;
    static uint8 StartPos = 0xFF;
    uint8 CurrPos;
    //uint8 len =0;

    uint8 Count;
    uint8 TmpBuffer[60];
    uint8 offset;
    uint8 loopdisplay;
    float BlockHight;

    TP_ListBoxRect  DisplayRect;
    TP_ListBoxRect  TitleRect;
    TP_ListBoxRect  ButtonRect;
    TP_ListBoxRect  IconRect;
    TP_ListBoxRect  FillRect;
    TP_ListBoxRect  ScrBarRect;
    uint8 SelectIndex;
    TP_CharSetT CurrCharSet = {0};
    uint8 startPosX = 0;

    TP_GetCurrentCharSet(&CurrCharSet);
    TP_GetFontInfo(&SFont,&MutlFont);
    TP_ScrAttrSet(0);
    TP_ScrSpaceSet(0, 0);
    TP_ScrSpaceGet(&charSpace,&lineSpace);
    //lineSpace =0;
    TP_ScrFontSet(CFONT);

    if (UpdateAll == 1 && ListBox->ifDrawRect )
    {
        ///Draw outside rectangle first
        TP_ScrDrawRect(ListBox->DisplayRect.left,ListBox->DisplayRect.top,ListBox->DisplayRect.right,ListBox->DisplayRect.bottom);
    }

    DisplayRect.left = ListBox->DisplayRect.left;
    DisplayRect.top = ListBox->DisplayRect.top;
    DisplayRect.right = ListBox->DisplayRect.right;
    DisplayRect.bottom = ListBox->DisplayRect.bottom;


    //if (ListBox->Title != NULL)
    if (strlen((char *)ListBox->Title)>0)
    {
        TitleRect.left = DisplayRect.left;
        TitleRect.top= DisplayRect.top;
        TitleRect.right= DisplayRect.right;
        TitleRect.bottom= DisplayRect.top +MutlFont.Height;
        TP_ScrAttrSet(0);
        if (UpdateAll == 1)
        {
            uint16 poxX = 0;
            int16 titleLen = 0;

            TP_SetDisplayArea(TitleRect.left,
                              TitleRect.top,
                              TitleRect.right,
                              TitleRect.bottom);
            TP_ScrClsDisplayArea();
            if(CurrCharSet.textDirection == 0)
            {
                poxX = TitleRect.left;//default: ALIGN_LEFT
                titleLen = TP_ScrDisplayWidthGet(ListBox->Title, strlen(ListBox->Title));
                titleLen = TitleRect.right-TitleRect.left+1 - titleLen;
                if(ListBox->TitleAlign == ALIGN_MIDDLE)
                {
                    if( titleLen > 0 )
                    {
                        poxX = TitleRect.left + titleLen/2;
                    }
                }
                else if(ListBox->TitleAlign == ALIGN_RIGHT)
                {
                    if( titleLen > 0 )
                    {
                        poxX = TitleRect.right - titleLen/2;
                    }
                }
            }
            else
            {
                poxX = TitleRect.right;//default: ALIGN_RIGHT
                titleLen = TP_ScrDisplayWidthGet(ListBox->Title, strlen(ListBox->Title));
                titleLen = TitleRect.right-TitleRect.left+1 - titleLen;
                if(ListBox->TitleAlign == ALIGN_MIDDLE)
                {
                    if( titleLen > 0 )
                    {
                        poxX = TitleRect.right - titleLen/2;
                    }
                }
                else if(ListBox->TitleAlign == ALIGN_LEFT)
                {
                    if( titleLen > 0 )
                    {
                        poxX = TP_ScrDisplayWidthGet(ListBox->Title, strlen(ListBox->Title));
                    }
                }
            }
            TP_ScrGotoxyEx(poxX, TitleRect.top);
            TP_LcdPrintf((uint8*)"%s",ListBox->Title);

        }

        ///modify top boundary of display area
        DisplayRect.top = TitleRect.bottom;
    }

//  if (ListBox->Button != NULL)
    if (strlen((char *)ListBox->Button) > 0)
    {
        ButtonRect.left = DisplayRect.left;
        ButtonRect.right= DisplayRect.right;
        ButtonRect.bottom= DisplayRect.bottom;
        ButtonRect.top= DisplayRect.bottom - MutlFont.Height;
        TP_ScrAttrSet(0);
        if (UpdateAll == 1)
        {
            TP_SetDisplayArea(ButtonRect.left,ButtonRect.top,ButtonRect.right,ButtonRect.bottom);
            TP_ScrClsDisplayArea();
            if(CurrCharSet.textDirection == 0)
            {
                TP_ScrGotoxyEx(ButtonRect.left,ButtonRect.top);
            }
            else
            {
                TP_ScrGotoxyEx(ButtonRect.right, ButtonRect.top);
            }
            TP_LcdPrintf("%s",ListBox->Button);
        }
        ///modify bottom boundary of display area
        DisplayRect.bottom= ButtonRect.top ;
    }

    MaxRowOnePage = (DisplayRect.bottom-DisplayRect.top+1) / (MutlFont.Height+lineSpace);

    if ((DisplayRect.bottom-DisplayRect.top+1) % (MutlFont.Height+lineSpace) >= MutlFont.Height)
    {
        MaxRowOnePage++;
    }

    if ( ListBox->ItemCount > MaxRowOnePage )
    {
        ListBox->ifDrawScrBall = TRUE;
        if(CurrCharSet.textDirection == 0)
        {
            ScrBarRect.left = DisplayRect.right-8;
            ScrBarRect.right = DisplayRect.right;
            DisplayRect.right = ScrBarRect.left -1;
        }
        else
        {
            ScrBarRect.left = DisplayRect.left;
            ScrBarRect.right = ScrBarRect.left+8;
            DisplayRect.left = ScrBarRect.right +1;
        }
        ScrBarRect.top = DisplayRect.top;
        ScrBarRect.bottom = DisplayRect.bottom;

    }
    else
    {
        ListBox->ifDrawScrBall = FALSE;
    }

    if (ListBox->StartPos == 0xFF)
    {
        ListBox->StartPos = ListBox->SelectItemIndex;
    }
    CurrPos = ListBox->SelectItemIndex;


    ///Judge whether the displayed information is in one page



    if (ListBox->CurrPage == 0xFF)
    {
        ListBox->CurrPage = ListBox->StartPos;
    }
    if (ListBox->CurrPage == 0xFF)
    {
        ListBox->CurrPage = ListBox->SelectItemIndex;
    }

    if(MaxRowOnePage >= ListBox->ItemCount)
    {
        ListBox->CurrPage = 0;
    }
    else
    {
        if  (ListBox->CurrPage < CurrPos)
        {
            if ((ListBox->CurrPage + MaxRowOnePage-1) > CurrPos)
            {
                  //ÄÇÃ´±iÊ¾Ôui¬Ò»Ò³Äu

            }
            else   ///±iÊ¾²»Ôui¬Ò»Ò³Äu
            {
                ListBox->CurrPage = CurrPos - MaxRowOnePage+1;
            }
        }
        else
        {
            ListBox->CurrPage = CurrPos;
        }
        if( (ListBox->CurrPage+MaxRowOnePage-1)>=ListBox->ItemCount)//ÏÔÊ¾µ½Ä©Î²
        {
            ListBox->CurrPage = ListBox->ItemCount - MaxRowOnePage ;
        }
    }


    if (UpdateAll == 1)
    {
        TP_SetDisplayArea(DisplayRect.left,DisplayRect.top,DisplayRect.right,DisplayRect.bottom);
        TP_ScrClsDisplayArea();
    }


    Index = ListBox->CurrPage;
    Count =0;

    while (Count < MaxRowOnePage)
    {
        TP_ScrAttrSet(0);
        ///Draw icon first
        switch (ListBox->listboxstyle)
        {
            case TL_CHECKBOX :  ///Check Box Method
                if(CurrCharSet.textDirection == 0)
                {
                    startPosX = DisplayRect.left;
                    IconRect.left =DisplayRect.left;
                }
                else
                {
                    startPosX = DisplayRect.right - TP_ListBoxCheckBoxcheck[1];
                    IconRect.left = startPosX;
                }
                TP_ScrGotoxyEx(startPosX, DisplayRect.top+(Index - ListBox->CurrPage) *(MutlFont.Height+lineSpace));
                IconRect.top =DisplayRect.top;
                IconRect.right = TP_ListBoxCheckBoxcheck[1];
                IconRect.bottom = TP_ListBoxCheckBoxcheck[0];
                if (UpdateAll == 1)
                {
                    for (SelectIndex = 0;SelectIndex <= ListBox->SelectItemCount;SelectIndex++)
                    {
                        if (SelectIndex == ListBox->SelectItemCount)
                        {
                            SelectIndex = 0xFF;
                            break;
                        }
                        if (ListBox->SelectItems[SelectIndex] == Index)
                        {
                            TP_ScrDrLogo(TP_ListBoxCheckBoxcheck);
                            break;
                        }
                    }


                    if (SelectIndex == 0xFF)
                    {
                        TP_ScrDrLogo(TP_ListBoxCheckBoxUncheck);
                    }
                }

                break;
            case TL_RADIOBOX :  ///Radio Box Method

                if(CurrCharSet.textDirection == 0)
                {
                    startPosX = DisplayRect.left;
                    IconRect.left =DisplayRect.left;
                }
                else
                {
                    startPosX = DisplayRect.right - TP_ListBoxCheckBoxcheck[1];
                    IconRect.left = startPosX;
                }
                TP_ScrGotoxyEx(startPosX, DisplayRect.top+(Index - ListBox->CurrPage) *(MutlFont.Height+lineSpace));
                IconRect.top =DisplayRect.top;
                IconRect.right = TP_ListBoxCheckBoxcheck[1];
                IconRect.bottom = TP_ListBoxCheckBoxcheck[0];
                if (UpdateAll == 1)
                {
                    if (Index == ListBox->SelectItemIndex)
                    {
                        TP_ScrDrLogo(TP_ListBoxRadiocheck);
                    }
                    else
                    {
                        TP_ScrDrLogo(TP_ListBoxRadioUncheck);
                    }
                }

                break;
            case TL_ICONLIST:
                if(CurrCharSet.textDirection == 0)
                {
                    startPosX = DisplayRect.left;
                    IconRect.left =DisplayRect.left;
                }
                else
                {
                    startPosX = DisplayRect.right - TP_ListBoxCheckBoxcheck[1];
                    IconRect.left = startPosX;
                }
                TP_ScrGotoxyEx(startPosX, DisplayRect.top+(Index - ListBox->CurrPage) *(MutlFont.Height+lineSpace));
                IconRect.top =DisplayRect.top;
                if (ListBox->ItemIconList[Index])
                {
                    if ((ListBox->ItemIconList[Index][0] == 1) || (ListBox->ItemIconList[Index][0] == 2))
                    {
                        IconRect.right = ListBox->ItemIconList[Index][2];
                        IconRect.bottom = ListBox->ItemIconList[Index][1];

                        if (UpdateAll == 1)
                        {
                            TP_ScrDrLogo(&ListBox->ItemIconList[Index][1]);
                        }
                    }
                    else
                    {
                        IconRect.right = ListBox->ItemIconList[Index][1];
                        IconRect.bottom = ListBox->ItemIconList[Index][0];

                        if (UpdateAll == 1)
                        {
                            TP_ScrDrLogo(ListBox->ItemIconList[Index]);
                        }
                    }
                }
                else
                {
                    IconRect.right = 0x0C;
                    IconRect.bottom = 0x0C;
                }

                break;
            case TL_LIST :   ///List box Method
                {
                    uint8 AllCount = ListBox->ItemCount;
                    uint8 Itemindex = Index;
                    uint8 TmpBuffer[5];
                    uint8 TmpFormat[20];
                    uint8 ItemLen;
                    uint8 ItemWidth;
                    TP_DotsInfoT DotsInfo = {0};
                    uint16 tempUcs[2] = {'8', 0};

                    TP_ScrFontInfoGet(tempUcs,
                                      1,
                                      &DotsInfo);
                    memset(TmpBuffer,0,sizeof(TmpBuffer));
                    sprintf((char *)TmpBuffer,"%d",AllCount);
                    ItemLen = strlen((char *)TmpBuffer);
                    if ( ItemLen <= 2 )
                    {
                        ItemLen = 2;
                    }

                    ItemWidth = (ItemLen+1) * (DotsInfo.Width+charSpace) - charSpace;

                    IconRect.left =0;
                    IconRect.top =0;
                    IconRect.right = ItemWidth;
                    IconRect.bottom = MutlFont.Height;

                    memset(TmpFormat,0,sizeof(TmpFormat));
                    memset(TmpBuffer,0,sizeof(TmpBuffer));
                    sprintf((char *)TmpBuffer,(char*)"%d",ItemLen);
                    strcat((char *)TmpFormat,(char*)"%0");
                    strcat((char *)TmpFormat,(char*)TmpBuffer);
                    strcat((char *)TmpFormat,(char*)"d.");
                    memset(TmpBuffer,0,sizeof(TmpBuffer));

                    sprintf((char *)TmpBuffer,(char*)TmpFormat,Itemindex+1);

                    if(CurrCharSet.textDirection == 0)
                    {
                        TP_ScrGotoxyEx(DisplayRect.left,
                                       DisplayRect.top+(Index - ListBox->CurrPage) *(MutlFont.Height+lineSpace));
                        TP_SetDisplayArea(DisplayRect.left ,
                                          DisplayRect.top+(Index - ListBox->CurrPage) *(MutlFont.Height+lineSpace),
                                          DisplayRect.left+ItemWidth,
                                          DisplayRect.top+(Index - ListBox->CurrPage+1) *(MutlFont.Height+lineSpace)-1);
                    }
                    else
                    {
                        TP_ScrGotoxyEx(DisplayRect.right,
                                       DisplayRect.top+(Index - ListBox->CurrPage) *(MutlFont.Height+lineSpace));
                        TP_SetDisplayArea(DisplayRect.right - ItemWidth,
                                          DisplayRect.top+(Index - ListBox->CurrPage) *(MutlFont.Height+lineSpace),
                                          DisplayRect.right,
                                          DisplayRect.top+(Index - ListBox->CurrPage+1) *(MutlFont.Height+lineSpace)-1);
                    }
                    if (UpdateAll == 1)
                    {
                        TP_LcdPrintf("%s",TmpBuffer);
                        //TP_DbgSerialPrn("\r\n list:%d,%s", Count,TmpBuffer);
                    }
                }

                break;
        /*Add by linhx 2012-09-03*/
        case TL_NO_EFFECT :   ///List box Method

                IconRect.left =0;
                IconRect.top =0;
                IconRect.right = 0;
                IconRect.bottom = 0;

                break;
        /*End add by linhx 2012-09-03*/
        }

        if(CurrCharSet.textDirection == 0)
        {
            startPosX = DisplayRect.left+IconRect.right + 2;
            FillRect.left = startPosX;
            FillRect.right =DisplayRect.right;
        }
        else
        {
            startPosX = DisplayRect.right-IconRect.right - 2;
            FillRect.left = DisplayRect.left;
            FillRect.right = startPosX;
        }
        TP_ScrGotoxyEx(startPosX,
                       DisplayRect.top+(Index - ListBox->CurrPage) *(MutlFont.Height+lineSpace));
        FillRect.top = DisplayRect.top+(Index - ListBox->CurrPage) *(MutlFont.Height+lineSpace);
        if (DisplayRect.top+(Index - ListBox->CurrPage+1) *(MutlFont.Height+lineSpace) >= (DisplayRect.bottom -1))
        {
            FillRect.bottom = DisplayRect.bottom;
        }
        else
        {
            FillRect.bottom = DisplayRect.top+(Index - ListBox->CurrPage+1) *(MutlFont.Height+lineSpace)-lineSpace;
        }
        TP_SetDisplayArea(FillRect.left,
                          FillRect.top,
                          FillRect.right,
                          FillRect.bottom);
        if (Index != CurrPos)
        {
            if (UpdateAll == 1)
            {
                TP_ScrClsDisplayArea();
                TP_ScrAttrSet(0);
                TP_LcdPrintf("%s",ListBox->ItemList[Index]);
            }
        }
        else
        {
            TP_ScrFillRect(FillRect.left,
                          FillRect.top,
                          FillRect.right,
                          FillRect.bottom);
            TP_ScrClsPrint(1);
            //TP_ScrClsDisplayArea();
            TP_ScrAttrSet(1);
            //memset(TmpBuffer,0x20,sizeof(TmpBuffer));
            //TmpBuffer[sizeof(TmpBuffer)-1] = 0;
            //TP_DbgSerialPrn("\r\n loopdisp:width:%d-%d",
            //                TP_ScrDisplayWidthGet(ListBox->ItemList[Index],strlen((char *)ListBox->ItemList[Index]) ),
            //                (FillRect.right-FillRect.left+1));

            if (//strlen((char *)ListBox->ItemList[Index]) > TP_GetMaxCharInRow()
                TP_ScrDisplayWidthGet(ListBox->ItemList[Index],
                                      strlen((char *)ListBox->ItemList[Index]) ) > (FillRect.right-FillRect.left+1) )
            {
                ///LoopDisplayPos  it means the start location of characters;
                #if 0
                offset =0;
                loopdisplay = ListBox->LoopDisplayPos;
                while ((offset < strlen((char *)ListBox->ItemList[Index])) && (loopdisplay>0))
                {
                    if (loopdisplay == 0)
                    {
                        break;
                    }
                    if (ListBox->ItemList[Index][offset] > 0x80)
                    {
                        offset += 2;
                    }
                    else
                    {
                        offset++;
                    }

                    if (offset >= strlen((char *)ListBox->ItemList[Index]))
                    {
                        ListBox->LoopDisplayPos=0;
                        offset =0;
                    }
                    loopdisplay --;
                }
                #else
                {
                    uint16 tempUnicode[50+1] = {0};
                    uint8 tempScr[50+1] = {0};
                    uint16 tranLen = 0;
                    offset = ListBox->LoopDisplayPos;
                    tranLen = offset*2;//Õý¶ÔÖÐÎÄµÈÇe¿ö
                    if(tranLen > strlen((char *)ListBox->ItemList[Index]))
                    {
                        tranLen = strlen((char *)ListBox->ItemList[Index]);
                    }

                    TP_ScrToUnicode(ListBox->ItemList[Index],
                                    tempUnicode,
                                    tranLen);
                    TP_UnicodeToScr(tempScr,
                                    tempUnicode,
                                    offset);
                    offset = strlen(tempScr);
                    if (offset >= strlen((char *)ListBox->ItemList[Index]))
                    {
                        ListBox->LoopDisplayPos=0;
                    }


                }
                #endif
                //TP_ScrClsDisplayArea();
                if(CurrCharSet.textDirection == 0)
                {
                    //memcpy(TmpBuffer,ListBox->ItemList[Index],strlen((char *)ListBox->ItemList[Index]));
                    TP_LcdPrintf("%s",&ListBox->ItemList[Index][offset]);
                }
                else
                {
                    uint8 textWidth = 0;
                    textWidth = TP_ScrDisplayWidthGet(&ListBox->ItemList[Index][offset],strlen(&ListBox->ItemList[Index][offset]));
                    if(textWidth >= (FillRect.right-FillRect.left+1))
                    {
                        //memcpy(TmpBuffer,ListBox->ItemList[Index],strlen((char *)ListBox->ItemList[Index]));
                        TP_LcdPrintf("%s",&ListBox->ItemList[Index][offset]);
                    }
                    else
                    {
                        //TP_LcdPrintf("%s", TmpBuffer);
                        TP_ScrGotoxyEx(FillRect.left+textWidth, FillRect.top);
                        TP_LcdPrintf("%s", &ListBox->ItemList[Index][offset]);
                    }
                }
            }
            else
            {
                //memcpy(TmpBuffer,ListBox->ItemList[Index],strlen((char *)ListBox->ItemList[Index]));
                TP_LcdPrintf("%s",ListBox->ItemList[Index]);
            }
        }
        TP_ScrClsPrint(0);


        //add by guangx 2012-03-21 begin
        if ( ListBox->ifDrawScrBall )
        {
            uint8 endPosX = 0;
            if(CurrCharSet.textDirection == 0)
            {
                startPosX = ScrBarRect.left + 3;
                endPosX = ScrBarRect.right;
            }
            else
            {
                startPosX = ScrBarRect.left;
                endPosX = ScrBarRect.right - 3;
            }
            BlockHight = (float)(ScrBarRect.bottom - ScrBarRect.top) / ListBox->ItemCount;//ºu¿e¸ß¶È
            TP_SetDisplayArea( ScrBarRect.left,ScrBarRect.top,
                            ScrBarRect.right,ScrBarRect.bottom);
            TP_ScrClsDisplayArea();
            TP_ScrDrawRect( startPosX,ScrBarRect.top,
                    endPosX,ScrBarRect.bottom);
            if ( BlockHight<1 )
            {
                TP_ScrFillRect( startPosX,
                                (int)(ScrBarRect.top+CurrPos*BlockHight),
                                endPosX,
                                (int)(ScrBarRect.top+CurrPos*BlockHight+1) );
            }
            else
            {
                TP_ScrFillRect( startPosX,
                                (int)(ScrBarRect.top+CurrPos*BlockHight),
                                endPosX,
                                (int)(ScrBarRect.top+(CurrPos+1)*BlockHight));
            }

        }
        //add by guangx 2012-03-21 end

        Index++;
        Count++;
        if (Index >= ListBox->ItemCount)
        {
            break;
        }
    }

    if(UpdateAll && ListBox->listboxfunc != PNULL)
    {
        ListBox->listboxfunc(ListBox);
    }

}

void  TP_ListLoopDisplay(TP_ListBox* ListBox)
{

    ListBox->LoopDisplayPos++;
    DrawListBox(ListBox,0);
}



/*
º¯ÊýÃû³Æ£ºTP_GetNumByKeyCode
oÃi¾£º    ¶ÔÊý×Ö°´¼üµÄ¼üÖµ½øÐÐ×ª»»£¬×ª»»³eÏào¦µÄÊý×ÖÖµ
×÷Õß£º    guangx
·µ»ØÖµËµÃ÷£º
    0£º×ª»»Ê§°Ü     >0£º×ª»»³e¹¦
ÊäÈë²ÎÊý£º
    ReturnKey: oÒ¼ü¼üÖµ
    pButtonTiltle£ºoÒ¼ü±êÌâ
    fmt£º¸nÊ½»¯Êä³ö×Ö·û´®
    ...
*/

uint8 TP_GetNumByKeyCode(const uint8 KeyCode)
{
    uint8 buf[9] = { TP_KEY_1,TP_KEY_2,TP_KEY_3,TP_KEY_4,TP_KEY_5,TP_KEY_6,TP_KEY_7,TP_KEY_8,TP_KEY_9};
    uint8 Index = 0;
    uint8 i;
    for ( i=0;i<9;++i)
    {
        if ( KeyCode == buf[i])
        {
            Index = ++i;
            break;
        }
    }
    return Index;
}


/**
 * Function description of TP_ShowListBox()£ºdisplay list items
 * @param ListBox  list items
 * @return >0 means press confirm to return£¬0 means press cancel to return
 * @author  Zuo Jun
 * @version 1.0
 * @date 20120205
 * @note it is block function, return only when confirm or cancel button is pressed.
 */
uint8 TP_ShowListBox(TP_ListBox* ListBox)
{

    uint8 Index;
    uint8 KeyCode;
    uint8 KeyValue = 0;
    Boolean IsFastKey = FALSE;

    if (ListBox == NULL)
    {
        return 0;
    }



    TP_SetDisplayArea(ListBox->DisplayRect.left , ListBox->DisplayRect.top ,ListBox->DisplayRect.right,ListBox->DisplayRect.bottom);
    TP_ScrClsDisplayArea();


    //add by wyq @ 201205.13
    if(ListBox->SelectItemIndex>0 && ListBox->SelectItemIndex<3)
        ListBox->StartPos = 0;

    if(ListBox->SelectItemIndex>=3)
        ListBox->StartPos = ListBox->SelectItemIndex-2;

    ///////////////////////////

    DrawListBox(ListBox,1);
    TP_Kbflush();
    TP_TimerSet(6, 0);
    TP_TimerSet(7, 0);

    while(1)
    {
        TP_NotifyCheck();

        if (TP_TimerCheck(6) == 0 && TP_TimerCheck(7) == 0)
        {
            TP_LcdFreeze();
            TP_ListLoopDisplay(ListBox);
            DrawListBox(ListBox,1);
            TP_ScrUpdate();
            TP_TimerSet(7,500);
        }

        if (TP_Kbhit())
        {
            KeyCode = TP_GetKey();
            TP_Kbflush();
            /*Add by linhx for selecting by number key on 2012-09-25*/
            switch (KeyCode)
            {
            case TP_KEY_1:
                KeyValue = 1;
                break;
            case TP_KEY_2:
                KeyValue = 2;
                break;
            case TP_KEY_3:
                KeyValue = 3;
                break;
            case TP_KEY_4:
                KeyValue = 4;
                break;
            case TP_KEY_5:
                KeyValue = 5;
                break;
            case TP_KEY_6:
                KeyValue = 6;
                break;
            case TP_KEY_7:
                KeyValue = 7;
                break;
            case TP_KEY_8:
                KeyValue = 8;
                break;
            case TP_KEY_9:
                KeyValue = 9;
                break;
            }
            if (KeyValue == 0)//Not any number key pressed
            {
                IsFastKey = FALSE;
            }
            else if(KeyValue <=ListBox->ItemCount)//Valid number key
            {
                ListBox->SelectItemIndex = KeyValue-1;
                IsFastKey = TRUE;
                KeyValue = 0;
            }
            else{//Invalid number key
                IsFastKey = FALSE;
                KeyValue = 0;
            }
            /*End of add by linhx for selecting by number key on 2012-09-25*/

            if (KeyCode == ListBox->ConfirmKey ||
                KeyCode == ListBox->LF_KeyValue ||
                IsFastKey == TRUE)
            {
                switch (ListBox->listboxstyle)
                {
                    case TL_NO_EFFECT :
                    case TL_LIST :
                    case TL_RADIOBOX :
                    case TL_ICONLIST:
                        ListBox->SelectItems[0] = ListBox->SelectItemIndex;
                        ListBox->SelectItemValue[0] = ListBox->ItemValueList[ListBox->SelectItemIndex];
                        ListBox->SelectItemCount =1;
                        ListBox->LoopDisplayPos = 0;
                        return 1;
                    case TL_CHECKBOX :
                        if (KeyCode != ListBox->ConfirmKey)
                        {
                            break;
                        }
                        if(ListBox->SelectItemCount>0)
                        {
                            return 1;
                        }
                        /*
                        for (Index =0;Index < ListBox->SelectItemCount;Index++)
                        {
                            if (ListBox->SelectItems[Index] == ListBox->SelectItemIndex)
                            {
                                ///item is selected£¬return directly
                                return 1;
                            }
                        }*/
                        //°´aËÈ·¶¨£¬±iÊ¾Òn¾­n¡ÔnaË£¬²»o¦¸ÃÔÙn¡
                        //ListBox->SelectItems[ListBox->SelectItemCount] = ListBox->SelectItemIndex;
                        //ListBox->SelectItemValue[ListBox->SelectItemCount] = ListBox->ItemValueList[ListBox->SelectItemIndex];
                        //ListBox->SelectItemCount++;
                }
            }

            if (KeyCode == ListBox->CancelKey ||
                KeyCode == ListBox->RF_KeyValue )
            {
                ListBox->SelectItemCount = 0;
                ListBox->LoopDisplayPos = 0;
                return 0;
            }

            if (KeyCode == ListBox->SelectKey ||
                IsFastKey == TRUE)
            {
                ListBox->LoopDisplayPos = 0;
                switch (ListBox->listboxstyle)
                {
                    case TL_LIST :
                    case TL_RADIOBOX :
                    case TL_ICONLIST :
                        ListBox->SelectItems[0] = ListBox->SelectItemIndex;
                        ListBox->SelectItemValue[0] = ListBox->ItemValueList[ListBox->SelectItemIndex];
                        ListBox->SelectItemCount =1;
                        break;
                    case TL_CHECKBOX :
                        {
                        uint8 IsSelected= 0;
                        for (Index =0;Index < ListBox->SelectItemCount;Index++)
                        {
                            if (ListBox->SelectItems[Index] == ListBox->SelectItemIndex)
                            {
                                IsSelected= 1;
                                while (Index <  ListBox->SelectItemCount)
                                {
                                    ///item is selected,then set it to be un-selected
                                    if (Index < LISTBOX_MAX_COUNT-1)
                                    {
                                        ListBox->SelectItems[Index] = ListBox->SelectItems[Index+1];
                                        ListBox->SelectItemValue[Index] = ListBox->SelectItemValue[Index+1];
                                    }
                                    Index++;
                                }
                                ListBox->SelectItemCount--;
                                break;
                            }
                        }
                        if (IsSelected == 0)
                        {
                            ListBox->SelectItems[ListBox->SelectItemCount] = ListBox->SelectItemIndex;
                            ListBox->SelectItemValue[ListBox->SelectItemCount] = ListBox->ItemValueList[ListBox->SelectItemIndex];
                            ListBox->SelectItemCount++;
                        }
                        }
                        break;
                }
                DrawListBox(ListBox,1);
                continue;
            }

            switch (KeyCode)
            {
                case TP_KEY_UP:
                    ListBox->SelectItemIndex--;
                    if (ListBox->SelectItemIndex >= ListBox->ItemCount)
                    {
                        ListBox->SelectItemIndex = ListBox->ItemCount -1;
                    }
                    ListBox->LoopDisplayPos = 0;
                    DrawListBox(ListBox,1);
                    TP_TimerSet(6, 2000);
                    break;
                case TP_KEY_DOWN:
                    ListBox->SelectItemIndex++;
                    ListBox->SelectItemIndex = ListBox->SelectItemIndex % ListBox->ItemCount;
                    ListBox->LoopDisplayPos = 0;
                    DrawListBox(ListBox,1);
                    TP_TimerSet(6, 2000);
                    break;
            }
        }
    }

}


/**
 * Function descripton of TP_ListBoxFree() £ºrelease list items
 * @param ListBox  list items
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20120205
 * @note
 */
void TP_ListBoxFree(TP_ListBox* ListBox)
{
    uint8 iChar;
    if (ListBox == NULL)
    {
        return;
    }
    /*Add by linhx 2012-09-03*/
    for (iChar=0;iChar<ListBox->ItemCount;iChar++)
    {
        if (ListBox->ItemList[iChar] != PNULL)
        {
            TP_FreeMemory((void**)&ListBox->ItemList[iChar]);
            //TP_MessageDispatch();
        }
    }
    /*End add by linhx 2012-09-03*/
    TP_FreeMemory((void**)&ListBox);
}




