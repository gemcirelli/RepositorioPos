#include <TPTypedef.h>
#include <TPKeyPad.h>
#include <TPBase.h>
#include <TPLCD.h>
#include "TP_ListBox.h"
#include "TP_DemoDefine.h"


const uint8 RingCaption1[] = "Ring1";
const uint8 RingCaption2[] = "Ring2";
const uint8 RingCaption3[] = "Ring3";
const uint8 RingCaption4[] = "Ring4";
const uint8 RingCaption5[] = "Ring5";
const uint8 RingCaption6[] = "Ring6";
const uint8 RingCaption7[] = "Ring7";
const uint8 RingCaption8[] = "Ring8";
const uint8 RingCaption9[] = "Ring9";
const uint8 RingCaption10[] = "Ring10";








/**
 * Function description TP_ListBoxShow() £º Show list box
 * @return none
 * @author  Zuo Jun
 * @version 1.0
 * @date 20110712
 * @note
 */
void TP_RingTest()
{
   TP_ListBox*  listbox;
   TP_ListBoxRect rect;
   uint8 keyCode;

   TP_Kbmute(0);

   rect.left = 0;
   rect.top = 0;
   rect.right = 127;
   rect.bottom = 63;
   listbox = TP_NewListBox(TL_RADIOBOX, rect, NULL, NULL,PNULL);
   listbox->ConfirmKey = KEY_DEMO_CONFIRM;
   listbox->CancelKey = KEY_DEMO_CANCEL;
   listbox->SelectKey = TP_KEY_OK;
   TP_ListBoxAddItem(listbox, RingCaption1, 1, 0,NULL);
   TP_ListBoxAddItem(listbox, RingCaption2, 2, 0,NULL);
   TP_ListBoxAddItem(listbox, RingCaption3, 3, 1,NULL);
   TP_ListBoxAddItem(listbox, RingCaption4, 4, 0,NULL);
   TP_ListBoxAddItem(listbox, RingCaption5, 5, 0,NULL);
   TP_ListBoxAddItem(listbox, RingCaption6, 6, 0,NULL);
   TP_ListBoxAddItem(listbox, RingCaption7, 7, 0,NULL);
   TP_ListBoxAddItem(listbox, RingCaption8, 8, 0,NULL);
   TP_ListBoxAddItem(listbox, RingCaption9, 9, 0,NULL);
   TP_ListBoxAddItem(listbox, RingCaption10, 10, 0,NULL);

   while (TRUE)
   {
       if (TP_ShowListBox(listbox) == 1)
       {
           TP_PlayRing((uint8)listbox->SelectItemValue[0]);
           TP_ScrCls();
           TP_ScrAttrSet(0);
           TP_ScrGotoxyEx(0, 1);
           TP_LcdPrintf((uint8*)"Play Ring %d", listbox->SelectItemValue[0]);
           TP_ScrGotoxyEx(0, 52);
           TP_LcdPrintf((uint8*)"Stop");
           while (TRUE)
           {
               if (TP_Kbhit() == 0xFF)
               {
                   keyCode = TP_GetKey();
                   if (keyCode == KEY_DEMO_CONFIRM)
                   {
                       TP_StopRing();
                       break;
                   }
               }
           }
       }
       else
       {
           break;
       }
   }

   TP_ListBoxFree(listbox);

   TP_Kbmute(1);
   return;

}



