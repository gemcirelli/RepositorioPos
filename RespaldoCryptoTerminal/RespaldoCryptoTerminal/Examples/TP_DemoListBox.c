#include <TPTypedef.h>
#include <TPKeyPad.h>
#include <TPBase.h>
#include <TPLCD.h>
#include "TP_DemoDefine.h"
#include "TP_ListBox.h"
#include "TP_DemoListBox.h"

const uint8 Title[] = "Hello World";
const uint8 Button[]="Confirm       Cancel";
const uint8 Caption1[] = "Caption1";
const uint8 Caption2[] = "Caption2";
const uint8 Caption3[] = "Caption3";
const uint8 Caption4[] = "Caption4";

const uint8 MessageIcon[] =
{
    0x0C,0x0C,0x00,0xE0,0x10,0x28,0x44,0x84,0x84,0x44,0x28,0x10,0xE0,0x00,
    0x00,0x0F,0x0A,0x09,0x08,0x08,0x08,0x08,0x09,0x0A,0x0F,0x00
};
TP_ListBox*  listbox;


void ShowListSelectDialog(uint32 Ms,uint8* Buffer)
{
    uint8 TmpBuffer[256];
    uint8 Canreturn;
    TP_ScrRestore(0);
    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(10,10,117,53);
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect(11,11,116,52);


    TP_SetDisplayArea(12,12,115,51);

    TP_ScrGotoxyEx(12,12);

    //memset(TmpBuffer,0,sizeof(TmpBuffer));
   // sprintf(TmpBuffer,"USSD Call Error");
    TP_LcdPrintf(Buffer);
    TP_TimerSet(0, Ms);
    while(1)
    {

        if(TP_TimerCheck(0) == 0)
        {
            TP_ScrRestore(1);
            return;
        }
        if (TP_Kbhit())
        {
            ///Press Any Key to Quit
            TP_Kbflush(); ///Key Value of The Key To Be Flushed
            TP_ScrRestore(1);
            return;

        }
    }
}

/**
 * Function description of TP_ListBoxShow()£º display list
 * @return None
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note
 */
void TP_ListBoxShow(TP_ListBoxStyle style)
{
   TP_ListBoxRect rect;
   uint8 Displaystr[200];
   rect.left = 0;
   rect.top = 0;
   rect.right = 127;
   rect.bottom = 63;
   listbox = TP_NewListBox(style, rect, NULL, Button,0);
   listbox->ConfirmKey = KEY_DEMO_CONFIRM;
   listbox->CancelKey = KEY_DEMO_CANCEL;
   listbox->SelectKey = TP_KEY_OK;
   TP_ListBoxAddItem(listbox, Caption1, 1, 0,MessageIcon);
   TP_ListBoxAddItem(listbox, Caption2, 1, 0,MessageIcon);

    while(1)
    {
        if (TP_ShowListBox(listbox))
        {
            if (listbox->SelectItemCount >0)
            {
                uint8 index;
                memset(Displaystr,0,sizeof(Displaystr));
                strcat((char *)Displaystr,"Select item:\n");
                for (index=0;index<listbox->SelectItemCount;index++)
                {
                    strcat((char *)Displaystr,(char *)listbox->ItemList[listbox->SelectItems[index]]);
                    if(index != listbox->SelectItemCount-1)
                    {
                        strcat((char *)Displaystr,",");
                    }
                }
                ShowListSelectDialog(3000,Displaystr);
            }
            break;
        }
        else
        {
            break;
        }
    }
    TP_ListBoxFree(listbox);

    return;
}
