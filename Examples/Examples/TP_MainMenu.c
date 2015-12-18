#include <TPTypedef.h>
#include <TPKeypad.h>
#include <TPBase.h>
#include <TPFonts.h>
#include <TPLCD.h>
#include "TP_DemoDefine.h"
#include "TP_MainMenu.h"
#include "TP_DisplayAPI.h"


typedef struct
{
    uint8 left;
    uint8 top;
    uint8 right;
    uint8 bottom;
}TP_MenuRect;


typedef struct
{

    TP_MenuRect TitleRect;
    TP_MenuRect BodyRect;
    TP_MenuRect FootRect;
    uint8 IsLoopDisplay;
    uint8 IsShowTitle;
    uint8 IsShowFoot;
}TP_MenuTheme;




typedef struct
{
    uint8 MenuItemCount;        ///Amount of Menu Items
    TP_MenuItem SubMenus[256];///Sub Menu Items
    uint8 Showicon;            ///Show Icons on Menu
    TP_MenuThemeType tpmenuthemetype; ///Types of Menu Special Effect
}TP_Menu;





static TP_Menu  TP_MainMenu = {0};
static uint8  SelectParentId = 0;
static uint8  SelectItemId = 0;



static uint16 CurrPage = 0xFF;
static uint8 LoopDisplayPos =0;

TP_MenuTheme tp_menutheme[9]=
{
    {{0,0,127,12},{0,13,127,51},{0,52,127,63},1,1,1},
    {{0,0,0,0},{0,0,127,63},{0,0,0,0},0,0,0},
    {{0,0,0,0},{0,0,127,63},{0,0,0,0},1,0,0},
    {{0,0,127,12},{0,13,127,63},{0,0,0,0},1,1,0},
    {{0,0,0,0},{0,0,127,51},{0,52,127,63},1,0,1},
    {{0,0,0,0},{0,0,127,51},{0,52,127,63},0,0,1},
    {{0,0,127,12},{0,13,127,63},{0,0,0,0},0,1,0},
    {{0,0,127,12},{0,13,127,51},{0,52,127,63},0,1,1},
    {{0,0,0,0},{0,19,127,33},{0,52,127,63},1,0,0},
};



static TP_MenuItemClick ON_TP_MENUITEM_CLICK;
static TP_MenuItemFoot  ON_TP_MENUITEM_FOOT;
static TP_MenuThemeType currthemetype;
static uint8 menuFontSize = CFONT;


/**************************************************************
*
***************************************************************/
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


/**************************************************************
*
***************************************************************/

uint8 TP_GetSubMenuCount(uint8 ParentItem)
{
    uint8 Result =0;
    uint8 index;
    for (index=0;index < TP_MainMenu.MenuItemCount;index++)
    {
        if (TP_MainMenu.SubMenus[index].ParentID == ParentItem)
        {
            Result ++;
        }

        if (TP_MainMenu.SubMenus[index].ParentID > ParentItem)
        {
            break;
        }
    }
    return Result;
}

uint8 TP_GetSubMenuIndexByID(uint8 ItemID,uint8 ParentItem)
{
    uint8 Result =0;
    uint8 index;
    for (index=0;index < TP_MainMenu.MenuItemCount;index++)
    {
        if (TP_MainMenu.SubMenus[index].ParentID == ParentItem)
        {
            Result++;
        }
        if (TP_MainMenu.SubMenus[index].ItemID == ItemID)
        {
            Result--;
            break;
        }
    }

    return Result;
}

void TP_SortMenuItem()
{
    uint16 index,indey;
    uint8 itemid = 0;
    TP_MenuItem  Item1,Item2;

    for (index=0;index < TP_MainMenu.MenuItemCount;index++)
    {
        for (indey=TP_MainMenu.MenuItemCount-1;indey>index;indey--)
        {
            memcpy(&Item1,&TP_MainMenu.SubMenus[indey],sizeof(TP_MenuItem));
            memcpy(&Item2,&TP_MainMenu.SubMenus[indey-1],sizeof(TP_MenuItem));
            if ((Item1.ParentID < Item2.ParentID)
                || ((Item1.ParentID == Item2.ParentID) && (Item1.ItemID< Item2.ItemID)))
            {
                ///Make Exchange
                memcpy(&TP_MainMenu.SubMenus[indey],&Item2,sizeof(TP_MenuItem));
                memcpy(&TP_MainMenu.SubMenus[indey-1],&Item1,sizeof(TP_MenuItem));
            }

        }
    }

    //¼æÈÝ·À´i
    //¼ì²eSelectParentId SelectItemId´æ²»´æÔu
    for (index=0;index < TP_MainMenu.MenuItemCount;index++)
    {
        if( TP_MainMenu.SubMenus[index].ParentID == SelectParentId)
        {
            break;
        }
        else if(index == (TP_MainMenu.MenuItemCount -1) )
        {
            SelectParentId = 0;
        }
    }
    indey = 0;
    for (index=0;index < TP_MainMenu.MenuItemCount;index++)
    {
        if( TP_MainMenu.SubMenus[index].ParentID == SelectParentId )
        {
            if( TP_MainMenu.SubMenus[index].ItemID == SelectItemId)
            {
                break;
            }
            if(indey == 0)
            {
                itemid = TP_MainMenu.SubMenus[index].ItemID;
                indey++;
            }
            else
            {
                indey++;
            }
        }
        if(index == (TP_MainMenu.MenuItemCount - 1))
        {
            SelectItemId = itemid;
        }
    }

}

/**
 * Function TP_AddMenuItem() Function Discription: Add a Menu Item
 * @param Pid  Parent ID, Parent ID for First level Menu is 0
 * @param id  Menu id
 * @param itemcaption Menu discription
 * @param itemicon Icon Discription on Menus
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note Show Icon and time at the same time
 */
void TP_AddMenuItem(uint8 Pid,uint8 Id,const uint8* ItemCaption,uint8* ItemIcon)
{
    uint8 CaptionLen;
    TP_MainMenu.SubMenus[TP_MainMenu.MenuItemCount].ParentID = Pid;
    TP_MainMenu.SubMenus[TP_MainMenu.MenuItemCount].ItemID = Id;
    TP_MainMenu.SubMenus[TP_MainMenu.MenuItemCount].IsHide = 0;
    memset(TP_MainMenu.SubMenus[TP_MainMenu.MenuItemCount].Caption,0,32);
    if (strlen((char *)ItemCaption) > 30)
    {
        CaptionLen = 30;
    }
    else
    {
        CaptionLen = strlen((char *)ItemCaption);
    }
    memcpy(TP_MainMenu.SubMenus[TP_MainMenu.MenuItemCount].Caption,ItemCaption,CaptionLen);

    TP_MainMenu.SubMenus[TP_MainMenu.MenuItemCount].Icon = ItemIcon;

    TP_MainMenu.MenuItemCount++;


}

/**
 * Function TP_DeleteMenuItemById() Function Discription: Delete a Menu Item
 * @param id  Menu id
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note Delete a Menu Item
 */
void TP_DeleteMenuItemById(uint8 Id)
{
    uint16 index;
    TP_MenuItem* Item;
    for (index =0;index < TP_MainMenu.MenuItemCount;index++)
    {
        Item = &TP_MainMenu.SubMenus[index];
        if (Item->ItemID == Id)
        {
            memset(Item,0xFF,sizeof(TP_MenuItem));
            TP_MainMenu.MenuItemCount--;
            TP_SortMenuItem();
            return;
        }
    }
}


/**
 * Function TP_DelectMenuItem() Function Discription: Delete a Menu Item
 * @param MenuItem  Menu Item
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note Delete a Menu Item
 */
void TP_DelectMenuItem(TP_MenuItem* MenuItem)
{
    uint16 index;
    TP_MenuItem* Item;
    for (index =0;index < TP_MainMenu.MenuItemCount;index++)
    {
        Item = &TP_MainMenu.SubMenus[index];
        if (Item == MenuItem)
        {
            memset(Item,0xFF,sizeof(TP_MenuItem));
            TP_MainMenu.MenuItemCount--;
            TP_SortMenuItem();
            return;
        }
    }
}

/**
 * Function TP_FindMenuItemByID() Function Discription: Search a Menu Item
 * @param Id  Menu Item ID
 * @return MenuItem Menu Item Structure
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note Search a Menu Item
 */
TP_MenuItem* TP_FindMenuItemByID(uint8 Id)
{
    uint16 index;
    TP_MenuItem* Item;
    for (index =0;index < TP_MainMenu.MenuItemCount;index++)
    {
        Item = &TP_MainMenu.SubMenus[index];
        if (Item->ItemID == Id)
        {

            return Item;
        }
    }
}



/**
 * Function TP_FindMenuItemByCaption() Function Discription: Search a Menu Item
 * @param caption   Discription on Menu Item
 * @return MenuItem Structure of Menu Item
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note Search a Menu Item
 */
TP_MenuItem* TP_FindMenuItemByCaption(uint8* caption)
{
    uint16 index;
    TP_MenuItem* Item;
    for (index =0;index < TP_MainMenu.MenuItemCount;index++)
    {
        Item = &TP_MainMenu.SubMenus[index];
        if ((strlen((char *)Item->Caption) == strlen((char *)caption)) && (memcmp(Item->Caption,caption,strlen((char *)caption)) == 0))
        {

            return Item;
        }
    }
}



/**
 * Function TP_CreateMenu() Function Discription: Create a Menu
 * @param showtitle   Show title, when in sub menu.
 * @param showfoot   Show menu foot
 * @param Loopshow   Loopshow function
 * @param showicon   Show Icons
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note Create a Menu
 */
void TP_CreateMenu(TP_MenuThemeType themetype ,TP_MenuItemClick OnClick,TP_MenuItemFoot OnFootEvent)
{
    memset(&TP_MainMenu,0xFF,sizeof(TP_MainMenu));
    TP_MainMenu.MenuItemCount =0;
    if ((themetype >=0 ) && (themetype <=8))
    {
        TP_MainMenu.tpmenuthemetype = themetype;
    }
    ON_TP_MENUITEM_CLICK = OnClick;
    ON_TP_MENUITEM_FOOT = OnFootEvent;
    SelectParentId = 0;
    SelectItemId = 0;
}



/**
 * Function TP_DrawMenu() Function Discription: Draw Menu
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note
 */
void TP_DrawMenu(uint8 UpdateAll)
{
    uint8 MaxRowOnePage;
    ST_FONT SFont;
    ST_FONT MutlFont;
    ST_FONT *pFont = PNULL;
    uint32 charSpace,lineSpace;
    uint16 Index;
    uint16 StartPos,CurrPos;
    uint8 Count;
    uint8 TmpBuffer[60];
    uint8 offset;
    uint8 loopdisplay;
    uint8 FootValue[30];
    TP_MenuRect BodyRect = {0};

    TP_CharSetT CurrCharSet = {0};
    uint16 startXPos = 0,endXPos = 0;
    uint8 FontSize = 0;
    int32 tempDispWidth;
    uint16 dispAreaWidth;


    TP_GetFontInfo(&SFont,&MutlFont);
    if (UpdateAll == 1)
    {
        TP_ScrCls();
    }
    TP_ScrAttrSet(0);
    TP_ScrSpaceSet(0, 0);
    TP_ScrSpaceGet(&charSpace,&lineSpace);
    if(menuFontSize == ASCII)
    {
        TP_ScrFontSet(ASCII);
        pFont = &SFont;
    }
    else
    {
        TP_ScrFontSet(CFONT);
        pFont = &MutlFont;
    }
    TP_ScrFontGet(&FontSize);
    TP_GetCurrentCharSet(&CurrCharSet);
    memcpy(&BodyRect, &tp_menutheme[currthemetype].BodyRect, sizeof(TP_MenuRect));
    if(CurrCharSet.textDirection == 0)
    {
        BodyRect.right -= 7;
    }
    else
    {
        BodyRect.left += 7;
    }


    if ((SelectParentId ==0) && (TP_MainMenu.tpmenuthemetype != TP_MT_SHOWONEITEM))
    {
        currthemetype = TP_MT_SHOWLOOP;
    }
    else
    {
        currthemetype = TP_MainMenu.tpmenuthemetype;
    }

    if((tp_menutheme[currthemetype].IsShowTitle) && (SelectParentId > 0))
    {

        if (UpdateAll == 1)
        {
            TP_SetDisplayArea(tp_menutheme[currthemetype].TitleRect.left,
                              tp_menutheme[currthemetype].TitleRect.top,
                              tp_menutheme[currthemetype].TitleRect.right,
                              tp_menutheme[currthemetype].TitleRect.bottom);
            TP_ScrClsDisplayArea();
            TP_ScrAttrSet(0);
            for (Index =0;Index < TP_MainMenu.MenuItemCount;Index++)
            {
                if (TP_MainMenu.SubMenus[Index].ItemID == SelectParentId)
                {
                    /*
                    memset(TmpBuffer,0x20,sizeof(TmpBuffer));
                    TmpBuffer[sizeof(TmpBuffer)-1] = 0;
                    ////calculate the midpoint location
                    if(strlen((char *)TP_MainMenu.SubMenus[Index].Caption) > TP_GetMaxCharInRow())
                    {
                        memcpy(TmpBuffer,TP_MainMenu.SubMenus[Index].Caption,strlen((char *)TP_MainMenu.SubMenus[Index].Caption));
                    }
                    else
                    {
                        memcpy(&TmpBuffer[(TP_GetMaxCharInRow()-strlen((char *)TP_MainMenu.SubMenus[Index].Caption))/2],TP_MainMenu.SubMenus[Index].Caption,strlen((char *)TP_MainMenu.SubMenus[Index].Caption));
                    }
                    TP_ScrGotoxyEx(tp_menutheme[currthemetype].TitleRect.left,tp_menutheme[currthemetype].TitleRect.top);
                    TP_LcdPrintf(TmpBuffer);
                    */
                    tempDispWidth = TP_ScrDisplayWidthGet(TP_MainMenu.SubMenus[Index].Caption,
                                                          strlen((char*)TP_MainMenu.SubMenus[Index].Caption) );
                    dispAreaWidth = tp_menutheme[currthemetype].TitleRect.right - tp_menutheme[currthemetype].TitleRect.left + 1;
                    if(tempDispWidth >= dispAreaWidth)
                    {
                        offset = 0;
                    }
                    else
                    {
                        offset = (dispAreaWidth - tempDispWidth)/2;
                    }
                    if(CurrCharSet.textDirection == 0)
                    {
                        startXPos = tp_menutheme[currthemetype].TitleRect.left + offset ;
                    }
                    else
                    {
                        startXPos = tp_menutheme[currthemetype].TitleRect.right - offset;
                    }
                    TP_ScrGotoxyEx(startXPos, tp_menutheme[currthemetype].TitleRect.top);
                    TP_LcdPrintf("%s",TP_MainMenu.SubMenus[Index].Caption);
                }
            }
        }
    }

    if ((tp_menutheme[currthemetype].IsShowFoot) && (SelectParentId > 0))
    {

        if (UpdateAll == 1)
        {
            TP_SetDisplayArea(tp_menutheme[currthemetype].FootRect.left,tp_menutheme[currthemetype].FootRect.top,
                          tp_menutheme[currthemetype].FootRect.right,tp_menutheme[currthemetype].FootRect.bottom);
            TP_ScrClsDisplayArea();
            TP_ScrDrawRect(tp_menutheme[currthemetype].FootRect.left,
                           tp_menutheme[currthemetype].FootRect.top,
                           tp_menutheme[currthemetype].FootRect.right,
                           tp_menutheme[currthemetype].FootRect.bottom);
            TP_ScrAttrSet(0);
            if (ON_TP_MENUITEM_FOOT != 0)
            {
                memset(FootValue,0,sizeof(FootValue));
                ON_TP_MENUITEM_FOOT(SelectItemId,FootValue);
                if (strlen((char *)FootValue) >0)
                {
                    /*
                    memset(TmpBuffer,0x20,sizeof(TmpBuffer));
                    TmpBuffer[59] = 0x00;
                    ////calculate the midpoint location

                    memcpy(&TmpBuffer[(TP_GetMaxCharInRow()-strlen((char *)FootValue))/2],FootValue,strlen((char *)FootValue));
                    TP_ScrGotoxyEx(tp_menutheme[currthemetype].FootRect.left,tp_menutheme[currthemetype].FootRect.top+1);
                    TP_LcdPrintf((uint8*)"%s", TmpBuffer);
                    */
                    tempDispWidth = TP_ScrDisplayWidthGet(FootValue,
                                                          strlen((char*)FootValue) );
                    dispAreaWidth = tp_menutheme[currthemetype].FootRect.right - tp_menutheme[currthemetype].FootRect.left + 1;
                    if(tempDispWidth >= dispAreaWidth)
                    {
                        offset = 0;
                    }
                    else
                    {
                        offset = (dispAreaWidth - tempDispWidth)/2;
                    }
                    if(CurrCharSet.textDirection == 0)
                    {
                        startXPos = tp_menutheme[currthemetype].TitleRect.left + offset;
                    }
                    else
                    {
                        startXPos = tp_menutheme[currthemetype].TitleRect.right - offset;
                    }
                    TP_ScrGotoxyEx(startXPos, tp_menutheme[currthemetype].FootRect.top+1);
                    TP_LcdPrintf("%s",FootValue);
                }
            }
        }
    }


    //if(TP_MainMenu.Showicon)
    //{
    //  BodyRect.left = 14;
    //}

    MaxRowOnePage = (BodyRect.bottom-BodyRect.top+1) / (pFont->Height+lineSpace);

    if ((BodyRect.bottom-BodyRect.top+1) % (pFont->Height+lineSpace) >= pFont->Height)
    {
        MaxRowOnePage++;
    }
    StartPos = 0xFFFF;
    for (Index=0;Index< TP_MainMenu.MenuItemCount;Index++)
    {
        if (TP_MainMenu.SubMenus[Index].ParentID == SelectParentId)
        {
            if (StartPos == 0xFFFF)
            {
                StartPos = Index;
                CurrPos = StartPos;
            }

            if (TP_MainMenu.SubMenus[Index].ItemID == SelectItemId)
            {
                CurrPos = Index;
                break;
            }
        }
    }

    ///To judge whether the contents on display are on the same page



    if (CurrPage == 0xFF)
    {
        CurrPage = StartPos;
    }

    if  (CurrPage < CurrPos)
    {
        if ((CurrPage + MaxRowOnePage-1) > CurrPos)
        {
              //then they are on the same page

        }
        else   ///then they are not on the same page
        {
            CurrPage = CurrPos - MaxRowOnePage+1;
        }
    }
    else
    {
        CurrPage = CurrPos;
    }


    ///Start draw menus
    if (UpdateAll == 1)
    {

        TP_SetDisplayArea(BodyRect.left,
                          BodyRect.top,
                          BodyRect.right,
                          BodyRect.bottom);
        if (currthemetype != TP_MT_SHOWONEITEM)
        {
            TP_ScrClsDisplayArea();
        }
        else
        {
            TP_ScrCls();
        }
    }

    Index = CurrPage;
    Count =0;
    while (Count < MaxRowOnePage)
    {
        if (TP_MainMenu.SubMenus[Index].ParentID != SelectParentId)
        {
            break;
        }
        if (TP_MainMenu.SubMenus[Index].IsHide)
        {
            Index++;
            continue;
        }


        ///Draw icon first
        if ((TP_MainMenu.Showicon == TP_MENU_SHOWICON) || (TP_MainMenu.Showicon == TP_MENU_SHOWINDEX))
        {
            TP_ScrAttrSet(0);
            if (TP_MainMenu.Showicon == TP_MENU_SHOWICON)
            {

                if ((TP_MainMenu.SubMenus[Index].Icon != 0) && (TP_MainMenu.SubMenus[Index].Icon[1] <= pFont->Width)
                    && (TP_MainMenu.SubMenus[Index].Icon[0] <= pFont->Height))
                {
                    if(CurrCharSet.textDirection == 0)
                    {
                        startXPos = BodyRect.left;
                    }
                    else
                    {
                        startXPos = BodyRect.right - TP_MainMenu.SubMenus[Index].Icon[1];
                    }
                    TP_ScrGotoxyEx(startXPos,
                                   BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace));
                    TP_ScrDrLogo(TP_MainMenu.SubMenus[Index].Icon);

                    if(CurrCharSet.textDirection == 0)
                    {
                        startXPos += TP_MainMenu.SubMenus[Index].Icon[1];
                    }

                    TP_ScrGotoxyEx(startXPos ,
                                   BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace));

                    if(CurrCharSet.textDirection == 0)
                    {
                        startXPos = BodyRect.left +TP_MainMenu.SubMenus[Index].Icon[1];
                        endXPos = BodyRect.right;
                    }
                    else
                    {
                        startXPos = BodyRect.left;
                        endXPos = BodyRect.right - TP_MainMenu.SubMenus[Index].Icon[1];
                    }
                    if (BodyRect.top+(Index - CurrPage+1) *(pFont->Height+lineSpace) > (MAX_SCREEN_HEIGHT-1))
                    {
                        TP_SetDisplayArea(startXPos ,
                                          BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace),
                                          endXPos,
                                          MAX_SCREEN_HEIGHT-1);
                    }
                    else
                    {
                        TP_SetDisplayArea(startXPos ,
                                          BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace),
                                          endXPos,
                                          BodyRect.top+(Index - CurrPage+1) *(pFont->Height+lineSpace)-1);
                    }
                }
                else
                {
                    if(CurrCharSet.textDirection == 0)
                    {
                        startXPos = BodyRect.left + pFont->Height;
                    }
                    else
                    {
                        startXPos = BodyRect.right - pFont->Height;
                    }
                    TP_ScrGotoxyEx(startXPos,
                                   BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace));

                    if(CurrCharSet.textDirection == 0)
                    {
                        startXPos = BodyRect.left + pFont->Height;
                        endXPos = BodyRect.right;
                    }
                    else
                    {
                        startXPos = BodyRect.left;
                        endXPos = BodyRect.right - pFont->Height;
                    }
                    if (BodyRect.top+(Index - CurrPage+1) *(pFont->Height+lineSpace) > (MAX_SCREEN_HEIGHT-1))
                    {
                        TP_SetDisplayArea(startXPos ,
                                          BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace),
                                          endXPos,
                                          MAX_SCREEN_HEIGHT-1);
                    }
                    else
                    {
                        TP_SetDisplayArea(startXPos,
                                          BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace),
                                          endXPos,
                                          BodyRect.top+(Index - CurrPage+1) *(pFont->Height+lineSpace)-1);
                    }
                }
            }
            else if (TP_MainMenu.Showicon == TP_MENU_SHOWINDEX)
            {
                ///First need to calculate the total amount of sub-menus,then calculate the needed width
                uint8 AllCount = TP_GetSubMenuCount(SelectParentId);
                uint8 Itemindex = TP_GetSubMenuIndexByID(TP_MainMenu.SubMenus[Index].ItemID,SelectParentId);
                uint8 TmpBuffer[5];
                uint8 TmpFormat[20];
                uint8 ItemLen;
                uint8 ItemWidth;

                TP_DotsInfoT DotsInfo = {0};
                uint16 tempUcs[2] = {'8', 0};

                memset(TmpBuffer,0,sizeof(TmpBuffer));
                sprintf((char*)TmpBuffer,"%d",AllCount);
                ItemLen = strlen((char *)TmpBuffer);
                TP_ScrFontInfoGet(tempUcs,
                                  1,
                                  &DotsInfo);
                ItemWidth = (ItemLen+1) * (DotsInfo.Width);

                if(CurrCharSet.textDirection == 0)
                {
                    startXPos = BodyRect.left;
                }
                else
                {
                    startXPos = BodyRect.left + ItemWidth;
                }
                TP_ScrGotoxyEx(startXPos, BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace));
                TP_SetDisplayArea(BodyRect.left ,
                                  BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace),
                                  BodyRect.left+ItemWidth,
                                  BodyRect.top+(Index - CurrPage+1) *(pFont->Height+lineSpace)-1);

                memset(TmpFormat,0,sizeof(TmpFormat));
                memset(TmpBuffer,0,sizeof(TmpBuffer));
                sprintf((char*)TmpBuffer,"%d",ItemLen);
                strcat((char*)TmpFormat,"%0");
                strcat((char*)TmpFormat,(char*)TmpBuffer);
                strcat((char*)TmpFormat,"d.");
                memset(TmpBuffer,0,sizeof(TmpBuffer));

                sprintf((char*)TmpBuffer,(char*)TmpFormat,Itemindex+1);
                TP_LcdPrintf("%s",TmpBuffer);

                if(CurrCharSet.textDirection == 0)
                {
                    startXPos = BodyRect.left + ItemWidth;
                }
                else
                {
                    startXPos = BodyRect.right - ItemWidth;
                }
                TP_ScrGotoxyEx(startXPos,
                               BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace));

                if(CurrCharSet.textDirection == 0)
                {
                    startXPos = BodyRect.left + ItemWidth;
                    endXPos = BodyRect.right;
                }
                else
                {
                    startXPos = BodyRect.left;
                    endXPos = BodyRect.right - ItemWidth;
                }
                if (BodyRect.top+(Index - CurrPage+1) *(pFont->Height+lineSpace) > (MAX_SCREEN_HEIGHT-1))
                {
                    TP_SetDisplayArea(startXPos,
                                      BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace),
                                      endXPos,
                                      MAX_SCREEN_HEIGHT-1);
                }
                else
                {
                    TP_SetDisplayArea(startXPos,
                                      BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace),
                                      endXPos,
                                      BodyRect.top+(Index - CurrPage+1) *(pFont->Height+lineSpace)-1);
                }
            }
            else if (TP_MainMenu.Showicon == TP_MENU_SHOWANIMATION)
            {

            }
            //TP_ScrClsDisplayArea();
        }
        else
        {
            if(CurrCharSet.textDirection == 0)
            {
                startXPos = BodyRect.left;
            }
            else
            {
                startXPos = BodyRect.right;
            }
            TP_ScrGotoxyEx(startXPos,
                           BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace));

            if (BodyRect.top+(Index - CurrPage+1) *(pFont->Height+lineSpace) > (MAX_SCREEN_HEIGHT-1))
            {
                TP_SetDisplayArea(BodyRect.left ,
                                  BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace),
                                  BodyRect.right,
                                  MAX_SCREEN_HEIGHT-1);
            }
            else
            {
                TP_SetDisplayArea(BodyRect.left ,
                                  BodyRect.top+(Index - CurrPage) *(pFont->Height+lineSpace),
                                  BodyRect.right,
                                  BodyRect.top+(Index - CurrPage+1) *(pFont->Height+lineSpace)-1);
            }
        }



        if (Index != CurrPos)
        {
            if (UpdateAll == 1)
            {
                TP_ScrClsDisplayArea();
                TP_ScrAttrSet(0);
                TP_LcdPrintf("%s",TP_MainMenu.SubMenus[Index].Caption);
            }
        }
        else
        {
            uint8 dispLeft,dispRight,dispTop,dispBottom;
            TP_GetDisplayArea(&dispLeft, &dispTop, &dispRight, &dispBottom);
            TP_ScrClsDisplayArea();
            TP_ScrFillRect(dispLeft, dispTop, dispRight, dispBottom);
            TP_ScrClsPrint(1);
            if (currthemetype != TP_MT_SHOWONEITEM)
            {
                TP_ScrAttrSet(1);
            }
            //memset(TmpBuffer,0x20,sizeof(TmpBuffer));
            //TmpBuffer[sizeof(TmpBuffer)-1] = 0;
            //memcpy(TmpBuffer,
            //       TP_MainMenu.SubMenus[Index].Caption,
            //       strlen((char *)TP_MainMenu.SubMenus[Index].Caption));
            if ((tp_menutheme[currthemetype].IsLoopDisplay)
                && //(strlen((char *)TP_MainMenu.SubMenus[Index].Caption) > TP_GetMaxCharInRow())
                TP_ScrDisplayWidthGet(TP_MainMenu.SubMenus[Index].Caption,
                                      strlen((char *)TP_MainMenu.SubMenus[Index].Caption) ) > (dispRight-dispLeft+1) )
            {
                ///LoopDisplayPos Indicate to show from which ordinal Chinese character;
                #if 0
                offset =0;
                loopdisplay = LoopDisplayPos;
                while ((offset < strlen((char *)TP_MainMenu.SubMenus[Index].Caption)) && (loopdisplay>0))
                {
                    if (loopdisplay == 0)
                    {
                        break;
                    }

                    if (TP_MainMenu.SubMenus[Index].Caption[offset] > 0x80)
                    {
                        offset += 2;
                    }
                    else
                    {
                        offset++;
                    }

                    if (offset >= strlen((char *)TP_MainMenu.SubMenus[Index].Caption))
                    {
                        LoopDisplayPos=0;
                        offset =0;
                    }
                    loopdisplay --;
                }
                #else
                {
                    uint16 tempUnicode[32+1] = {0};
                    uint8 tempScr[32+1] = {0};
                    uint16 tranLen = 0;
                    offset = LoopDisplayPos;
                    tranLen = offset*2;//Õý¶ÔÖÐÎÄµÈÇe¿ö
                    if(tranLen > strlen((char *)TP_MainMenu.SubMenus[Index].Caption))
                    {
                        tranLen = strlen((char *)TP_MainMenu.SubMenus[Index].Caption);
                    }

                    TP_ScrToUnicode(TP_MainMenu.SubMenus[Index].Caption,
                                    tempUnicode,
                                    tranLen);
                    TP_UnicodeToScr(tempScr,
                                    tempUnicode,
                                    offset);
                    offset = strlen(tempScr);
                    if (offset >= strlen((char *)TP_MainMenu.SubMenus[Index].Caption))
                    {
                        LoopDisplayPos=(0-1);//ÎªaËÏÔÊ¾iêÈ«±êÌâ
                    }


                }
                #endif
                if(CurrCharSet.textDirection == 0)
                {
                    TP_LcdPrintf("%s",&TP_MainMenu.SubMenus[Index].Caption[offset]);
                }
                else
                {
                    uint8 textWidth = 0;
                    textWidth = TP_ScrDisplayWidthGet(&TP_MainMenu.SubMenus[Index].Caption[offset],strlen(&TP_MainMenu.SubMenus[Index].Caption[offset]));
                    if(textWidth >= (dispRight-dispLeft+1))
                    {
                        TP_LcdPrintf("%s",&TP_MainMenu.SubMenus[Index].Caption[offset]);
                    }
                    else
                    {
                        TP_ScrGotoxyEx(dispLeft+textWidth, dispTop);
                        TP_LcdPrintf("%s", &TP_MainMenu.SubMenus[Index].Caption[offset]);
                    }
                }
            }
            else
            {
                TP_LcdPrintf("%s",TP_MainMenu.SubMenus[Index].Caption);
            }
            TP_ScrAttrSet(0);
            TP_ScrClsPrint(0);
        }
        Index++;
        Count++;
    }



    ///Draw Scroll Bar
    {
        uint8 AllCount = TP_GetSubMenuCount(SelectParentId);
        uint8 CurrIndex = TP_GetSubMenuIndexByID(SelectItemId,SelectParentId);
        TP_MenuRect ScrollBarRect;
        TP_MenuRect ScrollBlockRect;
        uint8 BlockHeight = 10;

        if(CurrCharSet.textDirection == 0)
        {
            ScrollBarRect.left = BodyRect.right+2;
            ScrollBarRect.right = BodyRect.right+7;
        }
        else
        {
            ScrollBarRect.left = BodyRect.left - 7;
            ScrollBarRect.right = BodyRect.left - 2;
        }

        if (currthemetype == TP_MT_SHOWONEITEM)
        {
            ScrollBarRect.top= 0;
            ScrollBarRect.bottom = 51;
        }
        else
        {
            ScrollBarRect.top= BodyRect.top;
            ScrollBarRect.bottom = BodyRect.bottom;
        }
        ///Draw whole outer frame of Scroll Bar
        TP_ScrClrRect(ScrollBarRect.left, ScrollBarRect.top,
                      ScrollBarRect.right, ScrollBarRect.bottom);
        TP_ScrDrawRect(ScrollBarRect.left, ScrollBarRect.top,
                      ScrollBarRect.right, ScrollBarRect.bottom);
        ///Fill the Block of current index location
        ///Calculate the height of filled block
        BlockHeight = (ScrollBarRect.bottom - ScrollBarRect.top) / AllCount;
        if (currthemetype == TP_MT_SHOWONEITEM)
        {
            ScrollBlockRect.top= (CurrIndex * (ScrollBarRect.bottom - ScrollBarRect.top))/AllCount;

        }
        else
        {
            ScrollBlockRect.top  = BodyRect.top+(CurrIndex * (ScrollBarRect.bottom - ScrollBarRect.top))/AllCount;

        }
        ScrollBlockRect.left = ScrollBarRect.left;
        ScrollBlockRect.right = ScrollBarRect.right;
        ScrollBlockRect.bottom = ScrollBlockRect.top+ BlockHeight;
        TP_ScrFillRect(ScrollBlockRect.left,ScrollBlockRect.top,ScrollBlockRect.right,ScrollBlockRect.bottom);

    }

    if (currthemetype == TP_MT_SHOWONEITEM)
    {

        TP_SetDisplayArea(tp_menutheme[currthemetype].FootRect.left,
                          tp_menutheme[currthemetype].FootRect.top,
                          tp_menutheme[currthemetype].FootRect.right,
                          tp_menutheme[currthemetype].FootRect.bottom);
        TP_ScrClsDisplayArea();
        if(CurrCharSet.textDirection == 0)
        {
            startXPos = 0;
        }
        else
        {
            startXPos = 127;
        }
        TP_ScrGotoxyEx(startXPos, tp_menutheme[currthemetype].FootRect.top);
        TP_LcdPrintf("%s",(uint8*)"Confirm        Cancel");

    }
}

/**
 * Function TP_MainLoopDisplay() Function Discription: Loop Effect
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note
 */
void  TP_MainLoopDisplay()
{
    if (!tp_menutheme[currthemetype].IsLoopDisplay)
    {
        return;
    }
    LoopDisplayPos++;
    TP_DrawMenu(1);
}



void TP_MenuGetParent()
{
    uint16 Index;
    if (SelectParentId == 0)
    {
        return;
    }
    CurrPage = 0xFF;
    for (Index=0;Index<TP_MainMenu.MenuItemCount;Index++)
    {
        if (TP_MainMenu.SubMenus[Index].ItemID == SelectParentId)
        {
            SelectParentId =  TP_MainMenu.SubMenus[Index].ParentID;
            SelectItemId   = TP_MainMenu.SubMenus[Index].ItemID;
            return;
        }
    }
}

void TP_MenuGetSub()
{
    uint16 Index;
    CurrPage = 0xFF;
    for (Index=0;Index<TP_MainMenu.MenuItemCount;Index++)
    {
        if (TP_MainMenu.SubMenus[Index].ParentID == SelectItemId)
        {
            SelectParentId =  TP_MainMenu.SubMenus[Index].ParentID;
            SelectItemId   = TP_MainMenu.SubMenus[Index].ItemID;
            return;
        }
    }

    if (ON_TP_MENUITEM_CLICK != 0)
    {

        ON_TP_MENUITEM_CLICK(SelectItemId);

        //TP_DrawMenu(1);
    }
}

void TP_MenuGetNext()
{
    uint16 Index;
    uint16  FindFirst;

    FindFirst = 0xFFFF;
    for (Index=0;Index<TP_MainMenu.MenuItemCount;Index++)
    {
        if ((TP_MainMenu.SubMenus[Index].ParentID == SelectParentId)
             && (TP_MainMenu.SubMenus[Index].ItemID == SelectItemId))
        {
            if (TP_MainMenu.SubMenus[Index+1].ParentID == SelectParentId)  ///  Choose the next one
            {
                SelectParentId =  TP_MainMenu.SubMenus[Index+1].ParentID;
                SelectItemId   = TP_MainMenu.SubMenus[Index+1].ItemID;
                return;
            }
            else    ///or Choose the first one
            {
                FindFirst = Index;
                break;
            }
        }
    }

    for (Index=FindFirst;Index>0;Index--)
    {
        if ((TP_MainMenu.SubMenus[Index].ParentID == SelectParentId)
             && (TP_MainMenu.SubMenus[Index-1].ParentID != SelectParentId))
        {

            SelectParentId =  TP_MainMenu.SubMenus[Index].ParentID;
            SelectItemId   = TP_MainMenu.SubMenus[Index].ItemID;
            return;

        }
    }
    SelectParentId =  TP_MainMenu.SubMenus[0].ParentID;
    SelectItemId   = TP_MainMenu.SubMenus[0].ItemID;
}

void TP_MenuGetPrior()
{
    uint16 Index;
    uint16  FindLast;

    FindLast = 0xFFFF;
    for (Index=0;Index<TP_MainMenu.MenuItemCount;Index++)
    {
        if ((TP_MainMenu.SubMenus[Index].ParentID == SelectParentId)
             && (TP_MainMenu.SubMenus[Index].ItemID == SelectItemId))
        {
            if (Index ==0)
            {
                FindLast = Index;
                break;
            }
            if (TP_MainMenu.SubMenus[Index-1].ParentID == SelectParentId)  ///  Choose the next one
            {
                SelectParentId =  TP_MainMenu.SubMenus[Index-1].ParentID;
                SelectItemId   = TP_MainMenu.SubMenus[Index-1].ItemID;
                return;
            }
            else    ///or Choose the last one
            {
                FindLast = Index;
                break;
            }
        }
    }

    for (Index=FindLast;Index<TP_MainMenu.MenuItemCount;Index++)
    {
        if ((TP_MainMenu.SubMenus[Index].ParentID == SelectParentId)
             && (TP_MainMenu.SubMenus[Index+1].ParentID != SelectParentId))
        {

            SelectParentId =  TP_MainMenu.SubMenus[Index].ParentID;
            SelectItemId   = TP_MainMenu.SubMenus[Index].ItemID;
            return;

        }
    }




}


/**
 * Function TP_ShowMenu() Function Discription: Show Menus
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note
 */
void TP_ShowMenu(uint8 pid,uint8 id)
{
    SelectParentId = pid;
    SelectItemId = id;


    LoopDisplayPos =0;
    CurrPage = 0xFF;
    TP_ScrCls();
    TP_SortMenuItem();
    TP_DrawMenu(1);
    TP_DrawMenu(1);
    TP_Kbflush();


    while(1)
    {
        TP_NotifyCheck();
        if (TP_TimerCheck(6)==0 && TP_TimerCheck(7) == 0)
        {
            TP_MainLoopDisplay();
            TP_TimerSet(6,500);
        }
        if (TP_Kbhit())
        {
            uint8 KeyCode = TP_GetKey();

            switch (KeyCode)
            {
                case KEY_DEMO_CONFIRM: ///Confirm
                    ///Show Menus
                    TP_MenuGetSub();
                    LoopDisplayPos =0;
                    //TP_SortMenuItem();
                    TP_ScrCls();
                    TP_DrawMenu(1);
                    TP_Kbflush();
                    break;

                case KEY_DEMO_CANCEL:
                    if (SelectParentId ==0)
                    {
                        //KiStopTimer(&TP_LoopDisplayTimer);
                        return;
                    }

                    TP_MenuGetParent();
                    LoopDisplayPos =0;
                    //TP_SortMenuItem();
                    TP_ScrCls();
                    TP_DrawMenu(1);
                    TP_Kbflush();
                    break;
                case TP_KEY_UP:
                    TP_MenuGetPrior();
                    LoopDisplayPos =0;
                    TP_ScrCls();
                    TP_DrawMenu(1);
                    TP_TimerSet(7, 2000);
                    break;
                case TP_KEY_DOWN:

                    TP_MenuGetNext();
                    LoopDisplayPos =0;
                    //TP_SortMenuItem();
                    TP_ScrCls();
                    TP_DrawMenu(1);
                    TP_TimerSet(7, 2000);
                    break;
            }
        }
    }
}

/**
 * Function TP_SetMenuIconShowMode() Function Discription: Setting Display Mode in Menus
 * @param IconShowMode Show Mode
 *   No Display
 *  TP_MENU_SHOWNONE 0
 *    Show Icon
 *  TP_MENU_SHOWICON 1
 *   Show Index
 *   TP_MENU_SHOWINDEX 2
 *   Show Animation
 * TP_MENU_SHOWANIMATION 3
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note
 */
void TP_SetMenuIconShowMode(uint8 IconShowMode)
{
    if ((IconShowMode != TP_MENU_SHOWNONE) && (IconShowMode != TP_MENU_SHOWICON)
        && (IconShowMode != TP_MENU_SHOWINDEX) && (IconShowMode != TP_MENU_SHOWANIMATION))
    {
        return;
    }
    TP_MainMenu.Showicon = IconShowMode;
}



/**
 * Function TP_SetMenuTheme() Function Discription: Setting Display Theme in Menus
 * @param themetype theme style
 *   refreance enum TP_MenuThemeType
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note
 */
void TP_SetMenuTheme(TP_MenuThemeType themetype)
{
    if ((themetype >=0 ) && (themetype <=8))
    {
        TP_MainMenu.tpmenuthemetype = themetype;
    }
}

/**
 * Set Menu id what is jumping to
 * @param ParentID parent ID
          SelectID item id
 * @author hewd
 * @version 1.0
 * @date 20120306
 * @note
 */
void TP_SetJumpMenuID(uint8 ParentID,uint8 SelectID)
{
    SelectParentId = ParentID;
    SelectItemId   = SelectID;
}

/**
 * Get current menu id
 * @param ParentID parent ID
          SelectID item id
 * @author hewd
 * @version 1.0
 * @date 20120402
 * @note
 */
void TP_GetCurrentMenuId(uint8 *ParentId,uint8 *ItemId)
{
    *ParentId = SelectParentId;
    *ItemId   = SelectItemId;
}

void TP_SetMenuFontSize(uint8 fontSize)
{
    menuFontSize = fontSize;
}



