#include <TPTypedef.h>
#include <TPBASE.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include "TP_DemoDefine.h"


Boolean TP_ConstractChange()
{
    uint8 KeyCode;
    uint8 Gray = 22;
    uint8 Gray2;

    TP_Kbflush();
    TP_ScrCls();
    TP_ScrDrawRect(1, 1, 127, 63);
    TP_SetDisplayArea(14, 24, 126, 47);
    TP_ScrGotoxyEx(14, 24);
    TP_ScrFontSet(CFONT);

    Gray = TP_ScrGrayGet();
    Gray2 = Gray;
    TP_ScrGray(Gray);
    TP_ScrClsDisplayArea();
    TP_LcdPrintf((uint8*)"Gray: %d",Gray);
    while(1)
    {

        if (TP_Kbhit())
        {
            KeyCode = TP_GetKey();
            switch (KeyCode)
            {
                case KEY_DEMO_CANCEL:
                    TP_ScrGray(Gray2);
                    return FALSE;
                    break;
                case KEY_DEMO_CONFIRM:
                    TP_ScrGray(Gray);
                    return TRUE;
                    break;
                case TP_KEY_UP :

                    if (Gray >0)
                    {
                        Gray--;
                        TP_ScrGray(Gray);
                    }
                    TP_ScrClsDisplayArea();
                    TP_LcdPrintf((uint8*)"Gray: %d",Gray);
                    break;
                case TP_KEY_DOWN:
                    if (Gray < 63)
                    {
                        Gray++;
                        TP_ScrGray(Gray);
                    }


                    TP_ScrClsDisplayArea();
                    TP_LcdPrintf((uint8*)"Gray: %d",Gray);
                    break;
            }
        }


    }
}

