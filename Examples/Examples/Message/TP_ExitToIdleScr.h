#ifndef TP_EXITTOIDLESCR_H
#define TP_EXITTOIDLESCR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <TPTypedef.h>
#include <TPKeyPad.h>

/******************************************************************************
*
*                           ���´������ڰ����ص���������(����)
* Write By Hewd
* start
******************************************************************************/

//Boolean IsExitToIdleScreen = FALSE;//��ݼ�����������ȫ�ֱ���
//uint8 ExitToIdleKeyList[1]={EXIT_TO_IDLE_KEYCODE};

#define EXIT_TO_IDLE_KEYCODE    TP_KEY_NONE

//�������뷨��������ȡ���İ����б�
#define EXIT_TO_IDLE_KEYLIST_ADD            {                                  \
                                                extern uint8 ExitToIdleKeyList[1];\
                                                TP_SetInputModeControlList(0, ExitToIdleKeyList);\
                                            }

//ȡ�����뷨�б�
#define EXIT_TO_IDLE_KEYLIST_CLEAR          TP_ClearInputModeControlList()

//���ڰ��������switch�ṹ
#define EXIT_TO_IDLE_KEYPAD_SWITCH_CASE     case EXIT_TO_IDLE_KEYCODE:          \
                                                {                               \
                                                extern Boolean IsExitToIdleScreen;\
                                                IsExitToIdleScreen = TRUE;      \
                                                TP_Kbflush();                   \
                                                TP_LcdFreeze();                 \
                                                }                               \
                                                break;

//���ڰ��������if�ṹ
#define EXIT_TO_IDLE_KEYPAD_IF(KeyCode)     if((KeyCode) == EXIT_TO_IDLE_KEYCODE) \
                                            {                                   \
                                                extern Boolean IsExitToIdleScreen;  \
                                                IsExitToIdleScreen = TRUE;      \
                                                TP_Kbflush();                   \
                                                TP_LcdFreeze();                 \
                                            }

//����˳�����������ı�־λ�����ڴ�����������
#define EXIT_TO_IDLE_CLEAR_FLAG             {                                   \
                                                extern Boolean IsExitToIdleScreen;\
                                                if(IsExitToIdleScreen)          \
                                                {                               \
                                                    IsExitToIdleScreen = FALSE; \
                                                    TP_ScrUpdate();             \
                                                }                               \
                                            }

//ѭ���������е���
//BreakOrReturn:0 break��ʽ�˳�ѭ��  1:return��ʽ�˳�ѭ��
//ReturnValue:����ֵ�趨 -1:��ʾû�з���ֵ
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


//��ת����
#define EXIT_TO_IDLE_GOTO_TAG               {                                   \
                                                extern Boolean IsExitToIdleScreen;\
                                                if(IsExitToIdleScreen)          \
                                                goto  EXIT_TO_IDLE;              \
                                            }

//��ת��־λ�趨
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

