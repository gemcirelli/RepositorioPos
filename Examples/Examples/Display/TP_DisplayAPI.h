#ifndef TP_PAGE_API_H
#define TP_PAGE_API_H

#include <TPFonts.h>
#include "TP_ListBox.h"

#define MAX_LR_BUTTON_TITLE_LEN 32 //左右按键标题最大长度

#ifdef TPS300
#define DISPLAY_KEY_CONFIRM TP_KEY_MENU
#define DISPLAY_KEY_CANCEL  TP_KEY_CANCEL
#define DISPLAY_KEY_POUND   TP_KEY_FUNC
#else
#define DISPLAY_KEY_CONFIRM TP_KEY_CONFIRM
#define DISPLAY_KEY_CANCEL  TP_KEY_CANCEL
#define DISPLAY_KEY_POUND   TP_KEY_POUND
#endif

//显示控制
typedef struct
{
    uchar* strTitle;                 //主标题，为空则不显示
    TP_DisplayAlign  align;         //标题对齐方式，只针对主标题，不设置默认左对齐
    uchar* strLButtonTitle;          //左键菜单标题
    uchar* strRButtonTitle;          //右键菜单标题
    uint8 LKeyValue;                //左操作键键值
    uint8 RKeyValue;                //右操作键键值
    uint8 LF_KeyValue;              //左操作快捷键键值
    uint8 RF_KeyValue;              //右操作快捷键键值
    uint8 FONT;
}TP_DisplayInfo;

//输入控制
typedef struct
{
    uint8 defaultMethod;            //默认输入法
    uint16 inputMode;               //输入法模式，用于数字数字、密码输入模式
    uint16 minLen;
    uint16 maxLen;
    uint32 timeoutInMS;
    Boolean showInputNumber;        // 是否显示当前输入字数及最大限制
    Boolean showIMIcon;             // 是否显示输入法图标
}TP_InputInfo;


typedef struct
{
    uint8* caption;                 //列表项内容
    uint32 value;                   //列表项的值
    uint8* icon;
    Boolean selected;               //是否选择 TRUE:已选 FALSE:未选
}TP_ListBoxInfo;

typedef enum
{
    ALERT_OK,
    ALERT_ERROR,
    ALERT_WARNING,
    ALERT_INFO,
    ALERT_QUESTION,
    ALERT_MAX
}TP_AlertType;

/**
* 只是显示一个提示语，不响应任何操作
* @author 何文东
* @version 1.0
* @param *msg 显示的字符串
* @return 无
* @since 2011年12月31日
*/
extern
void TP_PopupMessage(uchar *msg);

/**
* 显示一个窗体，带时间控制,在时间段内按任意键退出
* @author 何文东
* @version 1.0
* @param *msg 显示的字符串
* @param Ms 显示时间，毫秒单位
* @return 无
* @since 2011年12月31日
*/
extern
void TP_PopupMessageTimed( uchar *msg,uint32 Ms );

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
extern
void TP_PopupMessageWithIcon( TP_AlertType alertType,uchar *msg,uint32 Ms);

/*
函数名称：TP_PopupMesWithButton
用途：    用于居中显示弹出界面，且响应左右按键返回
作者：    guangx
返回值说明：
    TRUE：左键返回
    FALSE：右键返回或错误返回
输入参数：
    displayInfo: 左右按键信息结构体
*/
extern
Boolean TP_PopupMessageWithButton( TP_DisplayInfo *displayInfo );



void TP_ShowTextSingleLine( uint8 left, uint8 top, uint8 right, uint8 bottom,
                            uint8 FONT,TP_DisplayAlign  align,uchar* fmt,...);

//显示文本时候注册按键回调函数
typedef void (*ShowTextKeyFunc)(uint8 keycode,uint8 *DisplayData);

/**
* TP_ShowText()中按键事件处理函数定义
* @author 何文东
* @version 1.0
* @param func_p 按键处理函数指针<BR>
* @since 2012年5月18日
* @note
*/
extern
void TP_ShowTextKeyFuncSet(ShowTextKeyFunc func_p);

/**
 * 在指定的列表框区域Rect显示文本，不显示标题，响应左右键，响应已注册的函数
 * @author guangx
 * @param displayInfo
 * @param Rect
 * @param Text
 *
 * @return Boolean
 */
//modify by linhx 2012-08-24
extern
Boolean TP_ShowText( TP_DisplayInfo *displayInfo,
                     TP_ListBoxRect Rect, uchar *Text );

/**
 * 用于全屏多行文本显示，可显示标题，响应左右键，响应已注册的函数
 *
 * @author guangx
 *
 * @param displayInfo 显示结构体信息
 * @param fmt: 格式化输出字符串
 *
 * @return TRUE:  左键返回<BR>
 *          FALSE: 右键返回或错误返回
 * @date 2012-08-02
 */
extern
Boolean TP_ShowTextFullScr( TP_DisplayInfo *displayInfo, uchar* fmt,... );

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
extern
void TP_ShowPicture(uint8 left, uint8 top, uint8 right,
                   uint8 bottom,uint8 *picture);
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
extern
uint8 TP_TextInput(TP_DisplayInfo *displayInfo,
                   TP_InputInfo* inputInfo, uchar *pBuf );

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
extern
uint8 TP_NumberInput(TP_DisplayInfo *displayInfo,
                     TP_InputInfo * inputInfo,uchar *pBuf );

/**
 * Password string input with timeout controlled
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
                          TP_InputInfo * inputInfo,
                          uint8 echo, uchar *pBuf );

/**
 * 函 数 TP_CreateListBoxInterface() 功能描述：绘制列表界面
 * @param ListBox  列表对象
 * @return 大于0 表示是按确认键返回的，返回0 表示是按取消键返回的
 * @author 何文东
 * @version 1.1
 * @date 20120211
 * @note 只有当按下确认键或者取消键时才返回
 */
extern
Boolean TP_CreateListBoxInterface(TP_ListBoxStyle listboxStyle,
                                 TP_DisplayInfo *displayInfo,
                                 uint8 *item[],
                                 uint8 itemNum,
                                 uint8 selectIndex,
                                 uint8 *chooseValue,
                                 uint8 *icon);


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
extern
Boolean TP_CreateCheckBoxInterface(TP_DisplayInfo *displayInfo,
                                   TP_ListBoxInfo listBoxInfo[],
                                   uint8 itemNum,uint8 selectKey);




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
extern
Boolean TP_CreateListBoxWithOp(TP_DisplayInfo *displayInfo,
                               TP_ListBoxStyle listboxStyle,
                               TP_ListBoxInfo listBoxInfo[],uint8 itemNum,
                               TP_ListBoxStyle optionStyle,
                               TP_ListBoxInfo optionInfo[],uint8 optionNum,
                               uint8 optionSelectKey);

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
extern
void TP_ShowProgressBar(uchar* note,uint32 current,uint32 total);

extern
Boolean TP_IsGBK(uint8 WordCode);

extern
Boolean TP_DateInput( TP_DisplayInfo *displayInfo, TP_Date *pDate );

extern
Boolean TP_TimeInput( TP_DisplayInfo *displayInfo, TP_Time *pTime );

extern
void TP_GetFontInfo( ST_FONT *AsciiFont,ST_FONT *CfontFont);

extern
uint8 TP_GetFontHeight();

extern
void TP_DrawFootMenu( uint8 left,
                      uint8 top,
                      uint8 right,
                      uint8 bottom,
                      uint8 fontSize,
                      uint8 *leftCaption,
                      uint8 *rightCaption );

extern
void TP_MenuTitleCat( char *strbuf,uint16 bufLen, uint16 dispAreaWidth,
                      char *Lstr, char *Rstr, char Fillchar );

#endif
