#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPLCD.h>

#include "TP_DemoDefine.h"

typedef struct
{
    uint8 KeyCode;
    uint8 KeyDisplay[20];
    uint8 KeyBlank[20];
    uint8 IsPress;
}TP_KeyPadItem;

#if   defined (TPS320)
TP_KeyPadItem KeyPadList[] =
{
    {TP_KEY_F1,     "F1 ","   ",0},
    {TP_KEY_PRINT,  "PRINT ","      ",0},
    {TP_KEY_FEED,   "FEED ","     ",0},
    {TP_KEY_CONFIRM,"CONFIRM ","        ",0},
    {TP_KEY_CANCEL, "CANCEL ","       ",0},
    {TP_KEY_1,      "1  ","   ",0},
    {TP_KEY_2,      "2  ","   ",0},
    {TP_KEY_3,      "3  ","   ",0},
    {TP_KEY_4,      "4  ","   ",0},
    {TP_KEY_5,      "5  ","   ",0},

    {TP_KEY_6,      "6  ","   ",0},
    {TP_KEY_7,      "7  ","   ",0},
    {TP_KEY_8,      "8  ","   ",0},
    {TP_KEY_9,      "9  ","   ",0},
    {TP_KEY_0,      "0  ","   ",0},
    {TP_KEY_STAR,   "*  ","   ",0},
    {TP_KEY_POUND,  "#  ","   ",0},
    {TP_KEY_UP,     "UP ","   ",0},
    {TP_KEY_DOWN,   "DOWN ","     ",0},
    {TP_KEY_LEFT,   "LEFT ","     ",0},

    {TP_KEY_RIGHT,  "RIGHT ","      ",0},
    {TP_KEY_CLEAR,  "CLEAR ","      ",0},
    {TP_KEY_OK,     "OK ","   ",0},
    {TP_KEY_HANDFREE,     "HANDFREE","          ",0},

};

#endif

#ifdef TPS300
TP_KeyPadItem KeyPadList[] =
{
    {TP_KEY_POWER,  "POWER ",  "      ",  0},
    {TP_KEY_FUNC,   "FUNC ",   "     ",   0},
    {TP_KEY_FEED,   "FEED ",   "     ",   0},
    {TP_KEY_MENU,   "MENU ",   "   ",     0},
    {TP_KEY_CANCEL, "CANCEL ", "       ", 0},

    {TP_KEY_1,      "1  ","   ",0},
    {TP_KEY_2,      "2  ","   ",0},
    {TP_KEY_3,      "3  ","   ",0},
    {TP_KEY_4,      "4  ","   ",0},
    {TP_KEY_5,      "5  ","   ",0},
    {TP_KEY_6,      "6  ","   ",0},
    {TP_KEY_7,      "7  ","   ",0},
    {TP_KEY_8,      "8  ","   ",0},
    {TP_KEY_9,      "9  ","   ",0},
    {TP_KEY_0,      "0  ","   ",0},
    {TP_KEY_STAR,   "*  ","   ",0},

    {TP_KEY_UP,     "UP ","   ",0},
    {TP_KEY_DOWN,   "DOWN ","     ",0},

    {TP_KEY_CLEAR,  "CLEAR ","      ",0},
    {TP_KEY_OK,     "OK ","   ",0},
};
#endif

#if defined(TP_PUBLIC_G)
TP_KeyPadItem KeyPadList[] =
{
    {TP_KEY_POWER,  "POWER ",  "      ",  0},
    {TP_KEY_F1,     "F1 ","   ",0},
    {TP_KEY_F2,     "F2 ","   ",0},
    {TP_KEY_F3,     "F3 ","   ",0},
    {TP_KEY_F4,     "F4 ","   ",0},
    {TP_KEY_F5,     "F5 ","   ",0},
    {TP_KEY_CONFIRM,"CONFIRM ","        ",0},
    {TP_KEY_CANCEL, "CANCEL ","       ",0},
    {TP_KEY_1,      "1  ","   ",0},
    {TP_KEY_2,      "2  ","   ",0},
    {TP_KEY_3,      "3  ","   ",0},
    {TP_KEY_4,      "4  ","   ",0},
    {TP_KEY_5,      "5  ","   ",0},

    {TP_KEY_6,      "6  ","   ",0},
    {TP_KEY_7,      "7  ","   ",0},
    {TP_KEY_8,      "8  ","   ",0},
    {TP_KEY_9,      "9  ","   ",0},
    {TP_KEY_0,      "0  ","   ",0},
    {TP_KEY_STAR,   "*  ","   ",0},
    {TP_KEY_POUND,  "#  ","   ",0},
    {TP_KEY_UP,     "UP ","   ",0},
    {TP_KEY_DOWN,   "DOWN ","     ",0},
    {TP_KEY_LEFT,   "LEFT ","     ",0},
    {TP_KEY_RIGHT,  "RIGHT ","      ",0},
    {TP_KEY_HANDFREE,     "HANDFREE","          ",0}

};

#endif



void ShowKeyPadTest()
{
    uint8 Index;
    uint8 Lastpos;
    uint8 TmpBuffer[256];
    uint8 Canreturn;
    uint8 maxkeycount = 0;

    maxkeycount = sizeof(KeyPadList)/sizeof(TP_KeyPadItem);
    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(ASCII);
    TP_SetDisplayArea(0,0,127,63);
    TP_ScrGotoxyEx(0,0);
    TP_ScrClsDisplayArea();
    memset(TmpBuffer,0,sizeof(TmpBuffer));
    Lastpos=0;

    for (Index =0;Index< maxkeycount;Index++)
    {
        KeyPadList[Index].IsPress =0;
    }
    for (Index =0;Index< maxkeycount;Index++)
    {
        if ((strlen((char *)TmpBuffer) - Lastpos+strlen((char *)KeyPadList[Index].KeyBlank)) > TP_GetMaxCharInRow())
        {
            strcat((char *)TmpBuffer,"\n");
            Lastpos = strlen((char *)TmpBuffer);
        }
        if (KeyPadList[Index].IsPress ==0)
        {
            strcat((char *)TmpBuffer,(char *)KeyPadList[Index].KeyDisplay);
        }
        else
        {
            strcat((char *)TmpBuffer,(char *)KeyPadList[Index].KeyBlank);
        }
    }

    TP_LcdPrintf(TmpBuffer);
    while(1)
    {

        if (TP_Kbhit())
        {
            uint8 KeyCode = TP_GetKey();


            TP_ScrClsDisplayArea();

            memset(TmpBuffer,0,sizeof(TmpBuffer));
            Lastpos=0;
            for (Index =0;Index< maxkeycount;Index++)
            {
                if (KeyCode == KeyPadList[Index].KeyCode)
                {
                    KeyPadList[Index].IsPress =1;
                }
                if ((strlen((char *)TmpBuffer) - Lastpos+strlen((char *)KeyPadList[Index].KeyBlank)) > TP_GetMaxCharInRow())
                {
                    strcat((char *)TmpBuffer,"\n");
                    Lastpos = strlen((char *)TmpBuffer);
                }
                if (KeyPadList[Index].IsPress ==0)
                {
                    strcat((char *)TmpBuffer,(char *)KeyPadList[Index].KeyDisplay);
                }
                else
                {
                    strcat((char *)TmpBuffer,(char *)KeyPadList[Index].KeyBlank);
                }
            }

            TP_LcdPrintf(TmpBuffer);
            Canreturn = 1;
            for (Index =0;Index< maxkeycount;Index++)
            {
                if (KeyPadList[Index].IsPress ==0)
                {
                    Canreturn =0;
                    break;
                }
            }

            if (Canreturn)
            {
                return;
            }

        }
    }
}

Boolean TP_KeyPadVolumeSetTest()
{
    uint8 volume = 0;
    uint8 volumeback;
    uint8 KeyCode;
    volume = TP_GetVolume(KEYPAD_VOLUMN);
    volumeback = volume;

    TP_Kbflush();
    TP_ScrCls();
    TP_ScrDrawRect(1, 1, 127, 63);
    TP_SetDisplayArea(14, 24, 126, 47);
    TP_ScrGotoxyEx(14, 24);
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_LcdPrintf((uint8*)"Keypad Volume:%d",volume);

    TP_Kbmute(0);
    while(1)
    {

        if (TP_Kbhit())
        {
            KeyCode = TP_GetKey();
            switch (KeyCode)
            {
                case KEY_DEMO_CANCEL:
                    TP_Kbmute(1);
                    TP_SetVolume(KEYPAD_VOLUMN,volumeback);
                    TP_BeepEx(BEEP_MMI_VALID);
                    return FALSE;
                    break;
                case KEY_DEMO_CONFIRM:
                    TP_Kbmute(1);
                    TP_SetVolume(KEYPAD_VOLUMN,volume);
                    TP_BeepEx(BEEP_MMI_VALID);
                    return TRUE;
                    break;
                case TP_KEY_UP:
                    if (volume <VOLUME_7)
                    {
                        volume++;
                    }
                    else
                    {
                        volume = VOLUME_7;
                    }
                    TP_SetVolume(KEYPAD_VOLUMN,volume);
                    TP_BeepEx(BEEP_MMI_VALID);
                    TP_ScrClsDisplayArea();
                    TP_LcdPrintf((uint8*)"Keypad Volume:%d",volume);
                    break;
                case TP_KEY_DOWN:
                    if (volume > VOLUME_0)
                    {
                        volume--;
                    }
                    else
                    {
                        volume = 0;
                    }
                    TP_SetVolume(KEYPAD_VOLUMN,volume);
                    TP_BeepEx(BEEP_MMI_VALID);
                    TP_ScrClsDisplayArea();
                    TP_LcdPrintf((uint8*)"Keypad Volume:%d",volume);
                    break;
                default:
                    TP_SetVolume(KEYPAD_VOLUMN,volume);
                    TP_BeepEx(BEEP_MMI_VALID);
                    break;
            }
        }
    }

}

void TP_BeepTest()
{
    uint8 KeyCode;
    uint16 freq = 0;
    TP_Kbflush();
    TP_ScrCls();
    TP_ScrDrawRect(1, 1, 127, 63);
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);

#if defined(WITH_175X_ARM) && defined(WITH_SOUNDER_DRIVER)
    TP_SoundInit();

    freq = 300;
    while (freq <= 3000)
    {
        TP_SoundAddFreqPoint(freq, 100);
        freq += 10;
    }

    TP_SetDisplayArea(10, 24, 126, 47);
    TP_ScrClsDisplayArea();
    TP_ScrGotoxyEx(10, 24);
    TP_LcdPrintf((uint8*)"Frequency:300 ~ 3000");

    TP_SoundGenerate();
    TP_SoundSleep();

    TP_SetDisplayArea(3, 50, 126, 63);
    TP_ScrGotoxyEx(3, 51);
    TP_LcdPrintf((uint8*)"                Quit");
    TP_Kbflush();
    while (TRUE)
    {
        if(TP_Kbhit() != 0xFF)
        {
            continue;
        }

        TP_Kbflush();
        break;
    }
#else
    while (TRUE)
    {

        freq = 100;
        TP_SetDisplayArea(10, 24, 126, 47);
        TP_ScrGotoxyEx(10, 24);
        TP_LcdPrintf((uint8*)"Beep Frequency:%d",freq);
        while(freq <= 3000)
        {
            TP_ScrClsDisplayArea();
            TP_LcdPrintf((uint8*)"Beep Frequency:%d",freq);
            TP_Beef(freq, 300);
            TP_ScrClsDisplayArea();
            TP_LcdPrintf((uint8*)"Beep Frequency:%d",freq);
            TP_Beef(1000, 300);
            freq += 100;
        }
        TP_TimerSet(0, 1000);
        while(TP_TimerCheck(0)!= 0 );

        TP_ScrClsDisplayArea();
        TP_SetDisplayArea(3, 50, 126, 63);
        TP_ScrGotoxyEx(3, 51);
        TP_LcdPrintf((uint8*)"              Cancel");
        TP_Kbflush();
        while ( 1 )
        {
            if(TP_Kbhit())
            {
                KeyCode = TP_GetKey();
                switch (KeyCode)
                {
                //case TP_KEY_CONFIRM:
                //    TP_Kbflush();
                //    break;
                case TP_KEY_CANCEL:
                    return;
                    break;
                }
            }
        }
        TP_ScrClsDisplayArea();
    }
#endif
}



