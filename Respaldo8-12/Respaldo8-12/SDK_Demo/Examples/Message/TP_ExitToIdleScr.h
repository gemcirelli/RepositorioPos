#ifndef TP_EXITTOIDLESCR_H
#define TP_EXITTOIDLESCR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <TPTypedef.h>
#include <TPKeyPad.h>

/******************************************************************************
*
*                           以下代码用于按键回到待机界面(测试)
* Write By Hewd
* start
******************************************************************************/

//Boolean IsExitToIdleScreen = FALSE;//快捷键到待机界面全局变量
//uint8 ExitToIdleKeyList[1]={EXIT_TO_IDLE_KEYCODE};

#define EXIT_TO_IDLE_KEYCODE    TP_KEY_NONE

//用于输入法处理增加取消的按键列表
#define EXIT_TO_IDLE_KEYLIST_ADD            {                                  \
                                                extern uint8 ExitToIdleKeyList[1];\
                                                TP_SetInputModeControlList(0, ExitToIdleKeyList);\
                                            }

//取消输入法列表
#define EXIT_TO_IDLE_KEYLIST_CLEAR          TP_ClearInputModeControlList()

//用于按键处理的switch结构
#define EXIT_TO_IDLE_KEYPAD_SWITCH_CASE     case EXIT_TO_IDLE_KEYCODE:          \
                                                {                               \
                                                extern Boolean IsExitToIdleScreen;\
                                                IsExitToIdleScreen = TRUE;      \
                                                TP_Kbflush();                   \
                                                TP_LcdFreeze();                 \
                                                }                               \
                                                break;

//用于按键处理的if结构
#define EXIT_TO_IDLE_KEYPAD_IF(KeyCode)     if((KeyCode) == EXIT_TO_IDLE_KEYCODE) \
                                            {                                   \
                                                extern Boolean IsExitToIdleScreen;  \
                                                IsExitToIdleScreen = TRUE;      \
                                                TP_Kbflush();                   \
                                                TP_LcdFreeze();                 \
                                            }

//清除退出到待机界面的标志位，用在待机处理函数中
#define EXIT_TO_IDLE_CLEAR_FLAG             {                                   \
                                                extern Boolean IsExitToIdleScreen;\
                                                if(IsExitToIdleScreen)          \
                                                {                               \
                                                    IsExitToIdleScreen = FALSE; \
                                                    TP_ScrUpdate();             \
                                                }                               \
                                            }

//循环处理函数中调用
//BreakOrReturn:0 break形式退出循环  1:return形式退出循环
//ReturnValue:返回值设定 -1:表示没有返回值
#define EXIT_TO_IDLE_POLLING(BreakOrReturn, ReturnValue)                            \
                                            {                                       \
                                                extern Boolean IsExitToIdleScreen;  \
                                                if(IsExitToIdleScreen)              \
                                                {                                   \
                                                    if(BreakOrReturn == 0)          \
                                                    {                               \
                                                        break;                      \
                                                    }                               \
                                                    else                            \
                                                    {                               \
                                                        if((int16)ReturnValue == (-1))\
                                                        {                           \
                                                            return;                 \
                                                        }                           \
                                                        else                        \
                                                        {                           \
                                                            return ReturnValue;     \
                                                        }                           \
                                                    }                               \
                                                }                                   \
                                            }

#define EXIT_TO_IDLE_POLLING_VOID(BreakOrReturn, ReturnValue)                            \
                                            {                                       \
                                                extern Boolean IsExitToIdleScreen;  \
                                                if(IsExitToIdleScreen)              \
                                                {                                   \
                                                    if(BreakOrReturn == 0)          \
                                                    {                               \
                                                        break;                      \
                                                    }                               \
                                                    else                            \
                                                    {                               \
                                                        return;                 \
                                                    }                               \
                                                }                                   \
                                            }


//跳转处理
#define EXIT_TO_IDLE_GOTO_TAG               {                                   \
                                                extern Boolean IsExitToIdleScreen;\
                                                if(IsExitToIdleScreen)          \
                                                goto  EXIT_TO_IDLE;              \
                                            }

//跳转标志位设定
#define EXIT_TO_IDLE_TAG                    EXIT_TO_IDLE

/******************************************************************************
*
* end
*
******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif//TP_EXITTOIDLESCR_H

