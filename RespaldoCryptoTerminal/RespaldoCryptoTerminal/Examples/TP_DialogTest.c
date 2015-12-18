#include <TPtypedef.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TPBase.h>


uint8 Bmp[]=
{

    0x20, //Height
    0x20, //Width
    0xC0,0xF0,0xF0,0x30,0x30,0xFC,0xFC,0x04,0x04,0x08,0x08,0x10,0x10,0x10,0x20,0xE0,0xE0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0xF0,0xF0,0xC0,
    0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0xCE,0xDF,0xA2,0xB6,0x00,0xC3,0xFF,0xBE,0xC3,0x00,0x00,0x80,0xBE,0xBE,0xA2,0xBE,0x9C,0x80,0x00,0x00,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xC0,0x80,0x36,0x36,0x6D,0x6D,0x00,0xC0,0xFF,0xBF,0xC0,0x00,0x00,0x6D,0x6D,0x6D,0x6D,0x6D,0x6D,0x6D,0x00,0x00,0xFF,0xFF,0xFF,
    0x1F,0x1F,0x1F,0x1C,0x1C,0x1C,0x1C,0x1D,0x1D,0x1F,0x1F,0x1E,0x1E,0x1E,0x3C,0x3F,0x3F,0x3C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1F,0x1F,0x1F
};

void TP_ShowPopupDialog(uint8 ConfrmKey,uint8 CanclearKey)
{
    uint8 TmpBuffer[256];
    uint8 Canreturn;
    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(10,10,117,53);
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect(11,11,116,52);

    ///Draw Picture
    TP_ScrGotoxyEx(12,12);
    TP_ScrDrLogo(Bmp);
    ///Display Characters
    TP_SetDisplayArea(45,12,115,51);

    TP_ScrGotoxyEx(45,12);

    memset(TmpBuffer,0,sizeof(TmpBuffer));
    memcpy(TmpBuffer,"Please Any Key to exit",strlen((char *)"Please Any Key to exit"));
    TP_LcdPrintf(TmpBuffer);
    while(1)
    {

        if (TP_Kbhit())
        {
            uint8 KeyCode = TP_GetKey();
            /*
            if ((KeyCode == ConfrmKey) || (KeyCode == CanclearKey))
            {
                TP_ScrBackLight(0);
                return;
            }
            */
                return;
        }
    }
}

void TP_ShowTimePopupDialog(uint32 Ms)
{
    uint8 TmpBuffer[256];
    uint8 Canreturn;
    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_ScrClrRect(10,10,117,53);
    TP_SetDisplayArea(10,10,117,53);
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect(11,11,116,52);


    TP_SetDisplayArea(12,12,115,51);

    TP_ScrGotoxyEx(12,12);

    memset(TmpBuffer,0,sizeof(TmpBuffer));
    sprintf((char*)TmpBuffer,(char*)"Remain %d S",Ms/1000);
    TP_LcdPrintf(TmpBuffer);
    TP_TimerSet(0, Ms);
    while(1)
    {
        memset(TmpBuffer,0,sizeof(TmpBuffer));
        sprintf((char*)TmpBuffer,"Remain %d S",TP_TimerCheck(0)/1000);
        TP_ScrClsDisplayArea();
        TP_LcdPrintf(TmpBuffer);
        if(TP_TimerCheck(0) == 0)
        {
            return;
        }
        if (TP_Kbhit())
        {
            ///Press Any Key to Quit
            TP_Kbflush(); ///Key Value of The Key To Be Flushed
            return;

        }
    }
}






