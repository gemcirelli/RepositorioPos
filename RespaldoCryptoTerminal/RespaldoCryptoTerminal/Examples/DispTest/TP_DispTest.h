
#ifndef TP_DISP_TEST_H
#define TP_DISP_TEST_H

#ifdef TPS300
#define KEY_DISP_CONFIRM1   TP_KEY_MENU
#else
#define KEY_DISP_CONFIRM1   TP_KEY_CONFIRM
#endif
#define KEY_DISP_CONFIRM2   TP_KEY_NONE
#define KEY_DISP_CANCEL1    TP_KEY_CANCEL
#define KEY_DISP_CANCEL2    TP_KEY_NONE

void TP_DisplayTest();

#endif/*TP_DISP_TEST_H*/


