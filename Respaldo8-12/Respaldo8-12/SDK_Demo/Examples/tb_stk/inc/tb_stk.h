#ifndef TB_STK_H
#define TB_STK_H

#if defined(TPS300)
#define STK_KEY_CONFIRM TP_KEY_MENU
#define STK_KEY_CANCEL  TP_KEY_CANCEL
#define STK_KEY_OK      TP_KEY_OK
#define STK_KEY_POUND   TP_KEY_FUNC
#elif defined(TP_PUBLIC_G)
#define STK_KEY_CONFIRM TP_KEY_CONFIRM
#define STK_KEY_CANCEL  TP_KEY_CANCEL
#define STK_KEY_OK      TP_KEY_F1
#define STK_KEY_POUND   TP_KEY_POUND
#else
#define STK_KEY_CONFIRM TP_KEY_CONFIRM
#define STK_KEY_CANCEL  TP_KEY_CANCEL
#define STK_KEY_OK      TP_KEY_OK
#define STK_KEY_POUND   TP_KEY_POUND
#endif

/**
 * <b>Enter the main menu of STK</b>
 *
 * This is the only function that provided by STKManager CBB.
 * It provides UI and business logic all about STK.
 * Call this function when you want to active the STK session and it will take over control your APP until user exit STK application.
 *
 * @author zhangl
 *
 * @version 2.0
 *
 * @date 2012-12-12
 */
extern void
TB_STK_Enter(void);

#endif
