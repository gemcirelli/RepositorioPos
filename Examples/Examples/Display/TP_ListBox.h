
#ifndef TP_LISTBOX_H
#define TP_LISTBOX_H
#ifdef __cplusplus
extern "C" {
#endif

#include "TPTypedef.h"

typedef struct TP_ListBoxTag TP_ListBox;
typedef void (*ListBoxFunc)(TP_ListBox* listbox);

/*文本显示对齐方式*/
typedef enum TP_DisplayAlignTag
{
    ALIGN_LEFT,     //左对齐
    ALIGN_MIDDLE,   //居中
    ALIGN_RIGHT,    //右对齐
    ALIGN_NUM
}TP_DisplayAlign;


///style of list box
typedef enum
{
    ///no special effects
    TL_NO_EFFECT,
    ///list
    TL_LIST,
    ///radio box
    TL_RADIOBOX,
    ///check box
    TL_CHECKBOX,
    ///list with icon
    TL_ICONLIST,
    TL_MAX
}TP_ListBoxStyle;

///display area of list box
typedef struct
{
    ///left boundary
    uint8 left;
    ///top boundary
    uint8 top;
    ///right boundary
    uint8 right;
    ///bottom boundary
    uint8 bottom;
}TP_ListBoxRect;


#define LISTBOX_MAX_COUNT 200
#define LISTBOX_TITIL_LENGTH (32+1)
#define LISTBOX_BUTTON_STRING_LENGTH (26+1)

///structure of list objects
struct TP_ListBoxTag
{
    ///Confirm key
    uint8 ConfirmKey;
    ///Cancel key
    uint8 CancelKey;
    ///Select key
    uint8 SelectKey;
    ///Item list
    uint8* ItemList[LISTBOX_MAX_COUNT];
    ///Item icom list
    uint8* ItemIconList[LISTBOX_MAX_COUNT];
    ///value of list item
    uint32 ItemValueList[LISTBOX_MAX_COUNT];
    ///number of list
    uint8 ItemCount;
    ///select item
    uint8 SelectItems[LISTBOX_MAX_COUNT];
    ///value of selected item
    uint32 SelectItemValue[LISTBOX_MAX_COUNT];
    ///number of selected items
    uint8 SelectItemCount;
    ///display area
    TP_ListBoxRect DisplayRect;
    ///style of list item
    TP_ListBoxStyle listboxstyle;
    ///title
    uint8 Title[LISTBOX_TITIL_LENGTH];
    ///button
    uint8 Button[LISTBOX_BUTTON_STRING_LENGTH];
    ///item of current cursor
    uint8 SelectItemIndex;
    ///location of the last character of loop display
    uint8 LoopDisplayPos;
    
    uint16 StartPos;

    uint8 CurrPage;

    ListBoxFunc listboxfunc;
    
    //add by guangx 2012-03-21 begin
    Boolean ifNumSelect;
    Boolean ifDrawScrBall;
    Boolean ifDrawRect;
    uint8 LF_KeyValue;
    uint8 RF_KeyValue;
    TP_DisplayAlign TitleAlign;
    //add by guangx 2012-03-21 end

};


/**
 * Function describtion of TP_NewListBox()：Initialization of list item
 * @param listboxstyle  style of list items
 * @param rect  display area of list
 * @param Title  
 * @param Button  
 * @return list item
 * @author  Zuo Jun
 * @version 1.0
 * @date 20120205
 * @note for list items, check box can select multiple items, radio box and list box can select only one item
 */
extern TP_ListBox* TP_NewListBox(TP_ListBoxStyle listboxstyle,TP_ListBoxRect rect,uint8* Title,uint8* Button,ListBoxFunc listboxfunc_p);

/**
 * Function description of TP_ListBoxAddItem() ：Initialization of list item
 * @param ListBox  list item
 * @param Caption  description
 * @param Value   
 * @param IsSelected selected item
 * @param Icon point   only when parameter is TP_ListBoxStyle, it is set to be TL_ICONLIST
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20120205
 * @note 
 */
extern void TP_ListBoxAddItem(TP_ListBox* ListBox,
                                    uint8* Caption,
                                    uint32 Value,
                                    uint8 IsSelected,
                                    uint8* Icon);


extern void TP_ListBoxInsertItem(TP_ListBox* ListBox,
                                       uint8 index, 
                                       uint8* Caption,
                                       uint32 Value,
                                       uint8 IsSelected,
                                       uint8* Icon,
                                       Boolean UpdateNow);


extern void TP_ListBoxDeleteItem(TP_ListBox* ListBox,uint8 Index);

/**
 * Function description of TP_ShowListBox() ：display list items
 * @param ListBox  list item
 * @return >0 means press confirm to return，0 means press cancel to return
 * @author  Zuo Jun
 * @version 1.0
 * @date 20120205
 * @note it is block function, return only when confirm or cancel button is pressed
 */
extern uint8 TP_ShowListBox(TP_ListBox* ListBox);

/**
 * Function description of TP_ListBoxFree() ：release list item
 * @param ListBox  list item
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20120205
 * @note 
 */
extern void TP_ListBoxFree(TP_ListBox* ListBox);

#ifdef __cplusplus
}
#endif

#endif
