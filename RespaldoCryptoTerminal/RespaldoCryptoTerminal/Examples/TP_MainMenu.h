


#ifndef TP_MAIMENU_H
#define TP_MAIMENU_H
#ifdef __cplusplus
extern "C" {
#endif

#include "TPTypedef.h"

typedef struct
{
    uint8 ParentID;///Parent ID
    uint8 ItemID;  /// Menu Item ID;
    uint8 Caption[32]; ///Discription
    uint8* Icon;   ///Show Icon
    uint8 IsHide;     ///Hide or not
}TP_MenuItem;




///Types of Menu Special Effect 
typedef enum
{
    ///Show all Special Effect on Menus, including loop, title and foot
    TP_MT_ALL,
    ///Cancel all Special Effect on Menus
    TP_MT_NONE,
    ///Show parts of Special Effect on Menus, only loop
    TP_MT_SHOWLOOP,
    ///Show parts of Special Effect on Menus,include loop and title
    TP_MT_SHOWLOOPTITLE,
    ///Show parts of Special Effect on Menus,include loop and foot
    TP_MT_SHOWLOOPFOOT,
    ///Show parts of Special Effect on Menus,only foot
    TP_MT_SHOWFOOT, 
    ///Show parts of Special Effect on Menus,only title
    TP_MT_SHOWTITLE,    
    ///Show parts of Special Effect on Menus,include title and foot
    TP_MT_SHOWTITLEFOOT,
    ///Show parts of Special Effect on Menus,show one menu item on one page
    TP_MT_SHOWONEITEM,
}TP_MenuThemeType;

///No Display
#define TP_MENU_SHOWNONE 0
///Show Icons
#define TP_MENU_SHOWICON 1
///Show Index
#define TP_MENU_SHOWINDEX 2
///Show animation
#define TP_MENU_SHOWANIMATION 3



typedef void(*TP_MenuItemClick)(uint8 ItemKey); 
typedef void(*TP_MenuItemFoot)(uint8 ItemKey,uint8* Value);


void TP_SortMenuItem(void);
void TP_MenuGetParent(void);
/**
 * Function TP_AddMenuItem() Function Discription: Add a Menu Item
 * @param Pid  Parent id£¬first level parent id is 0
 * @param id  Menu id
 * @param itemcaption Discription on menu
 * @param itemicon item icons on menu
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note show icon and time at the same time
 */
void TP_AddMenuItem(uint8 Pid,uint8 Id,const uint8* ItemCaption,uint8* ItemIcon);

/**
 * Function TP_DeleteMenuItemById() Function Discription: Delete a Menu Item
 * @param id  Menu id
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note Delete a Menu Item
 */
void TP_DeleteMenuItemById(uint8 Id);


/**
 * Function TP_DelectMenuItem() Function Discription: Delete a Menu Item
 * @param MenuItem  Menu Item
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note Delete a Menu Item
 */
void TP_DelectMenuItem(TP_MenuItem* MenuItem);

/**
 * Function TP_FindMenuItemByID() Function Discription: Search Menu Item
 * @param Id  Menu Item ID
 * @return MenuItem Menu Item Structure
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note Search Menu Item
 */
TP_MenuItem* TP_FindMenuItemByID(uint8 Id);


/**
 * Function TP_FindMenuItemByCaption() Function Discription: Search Menu Item
 * @param caption   Menu Item Discription
 * @return MenuItem Menu Item Structure
 * @author  Zuo Jun
 * @date 20110712
 * @note Search Menu Item
 */
TP_MenuItem* TP_FindMenuItemByCaption(uint8* caption);


/**
 * Function TP_CreateMenu() Function Discription: Create Menu
 * @param themetype   Show themetype
 * @return None
 * @author  Zuo Jun
 * @date 20110712
 * @note Creat Menu
 */
void TP_CreateMenu(TP_MenuThemeType themetype ,TP_MenuItemClick OnClick,TP_MenuItemFoot OnFootEvent);


/**
 * Function TP_ShowMenu() Function Discription: Show Menu
 * @return None
 * @author  Zuo Jun
 * @date 20110712
 * @note 
 */
void TP_ShowMenu(uint8 pid,uint8 id);

/**
 * Function TP_SetMenuIconShowMode() Function Discription: Set Menu Display Mode
 * @param IconShowMode Show Mode
 *   Not to display
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
void TP_SetMenuIconShowMode(uint8 IconShowMode);


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
void TP_SetMenuTheme(TP_MenuThemeType themetype);


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
void TP_SetMenuTheme(TP_MenuThemeType themetype);

/**
 * Set Menu id what is jumping to
 * @param ParentID parent ID
          SelectID item id
 * @author hewd
 * @version 1.0
 * @date 20120306
 * @note
 */
void TP_SetJumpMenuID(uint8 ParentID,uint8 SelectID);


#ifdef __cplusplus
}
#endif

#endif

