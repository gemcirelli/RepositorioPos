#include <TPTypedef.h>
#include <TPBase.h>
#include <TPPhone.h>
#include <TPLCD.h>
#include <TPKEYPAD.h>
#include <TPMobileCard.h>

#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"

#define min(x, y) ((x) < (y) ? (x) : (y))

#define CHW 12
#define CHH 12

void TP_CreateSLInputWindow(uint8* Caption,
                            uint8* leftCaption,
                            uint8* rightCaption,
                            Boolean showInputNumber)
{
    uint16 xPos;

    /*Window Init*/
    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrSpaceSet(0, 0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127, 63);

    /*Draw Window Border*/
    TP_ScrDrawRect(0, 0, 127, 63);

    /*Draw Input Prompt*/
    TP_ScrFontSet(CFONT);
    TP_ScrGotoxyEx(3, 3);
    TP_LcdPrintf(Caption);

    /*Draw Input Area Border*/
    TP_ScrDrawRect(20, 3 + CHH + 1 + 4, 105, 3 + CHH + 9 + 1 + CHH + 1 + 1);

    if (showInputNumber == TRUE)
    {
        TP_ScrClrRect( 70, 3 + CHH + 1, 100, 3 + CHH + 7);
        TP_ScrDrawRect(70, 3 + CHH + 1, 100, 3 + CHH + 7);
        TP_ShowInputNum(1, 71, 3 + CHH + 3);
    }
    else
    {
        TP_ShowInputNum(0, 0, 0);
    }

    TP_SetInputModePosition(128, 64);

    /*Draw Left & Right Key Tips*/
    if(leftCaption != 0)
    {
        TP_ScrGotoxyEx(2, 50);
        TP_LcdPrintf(leftCaption);
    }

    if(rightCaption != 0)
    {
        xPos = 127 - 6 * strlen((char *)rightCaption);
        TP_ScrGotoxyEx(xPos, 50);
        TP_LcdPrintf(rightCaption);
    }

    /*Set the Valid Input Area*/
    TP_SetDisplayArea(22, 3 + CHH + 9 + 1, 103, 3 + CHH + 9 + 1 + CHH);
    TP_ScrGotoxyEx(22, 3 + CHH + 9 + 1);
}


void TP_ShowIMSIDialDilog()
{
    uint8 str[60];

    uint8 KeyCode;
    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrDrawRect(0, 0, 127, 63);//Draw the window's border

    TP_ScrGotoxyEx(3,3);
    TP_SetDisplayArea(3,3,126,15);
    TP_LcdPrintf((uint8*)"IMSI:");

    TP_ScrDrawRect(3, 17, 125, 48);//Draw the input content's window
    TP_ScrAttrSet(0);
    TP_ScrGotoxyEx(3,49);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(1,48,126,63);
    TP_LcdPrintf((uint8*)"                Exit");

    TP_ScrGotoxyEx(4,19);
    TP_SetDisplayArea(4,19,124,47);

    TP_Kbflush();
    memset(str,0,sizeof(str));
    TP_ScrClsDisplayArea();
    TP_GetMobileCardIMSI(str, sizeof(str));
    TP_LcdPrintf(str);
    while (TRUE)
    {
        ///Key Operation
        if (TP_Kbhit())
        {
            KeyCode = TP_GetKey();
            if (KeyCode == KEY_DEMO_CANCEL)
            {
                return;
            }
        }

    }

}


void TP_ShowIMEIDialDilog()
{
    uint8 str[60];
    uint8 CallTimes[10];
    uint8 KeyCode;

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrDrawRect(0, 0, 127, 63);//Draw the window's border

    TP_ScrGotoxyEx(3,3);
    TP_SetDisplayArea(3,3,126,15);
    TP_LcdPrintf((uint8*)"IMEI:");

    TP_ScrDrawRect(3, 17, 125, 48);//Draw the input content's window
    TP_ScrAttrSet(0);
    TP_ScrGotoxyEx(3,49);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(1,48,126,63);
    TP_LcdPrintf((uint8*)"Edit            Exit");

    TP_ScrGotoxyEx(4,19);
    TP_SetDisplayArea(4,19,124,47);

    TP_Kbflush();
    memset(str,0,sizeof(str));
    TP_ScrClsDisplayArea();
    TP_GetModalIMEI(str, sizeof(str));
    TP_SetInputModePosition(126, 64);
    TP_ShowInputNum(FALSE, 0, 0);
    TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
    KeyCode = TP_GetString((char*)str, 0xF4, 15, 15);
    if (KeyCode == 0xFB)
    {
        TP_SetModalIMEI((char*)str);
    }
}

/**
* Create a textbox with caption for single line text input
* @author He wendong
* @version 1.0
* @param *Caption Caption text
* @param *leftCaption Left button text
* @param *rightCaption Right button text
* @return None
* @since 2011-12-31
*/

#define CHW 12
#define CHH 12

Boolean TP_EditPassword(uint8 *Caption,
                        uint8 *LeftCaption,
                        uint8 *RightCaption,
                        uint8 *data,
                        uint16 minlen,
                        uint16 maxlen)
{
    uint8 userKey;
    uint8 leftKey;
    uint8 rightKey;
    uint16 datalen = 0;


    if (LeftCaption == NULL)
    {
        leftKey = TP_KEY_NONE;
    }
    else
    {
        leftKey = KEY_DEMO_CONFIRM;
    }

    if (RightCaption == NULL)
    {
        rightKey = TP_KEY_NONE;
    }
    else
    {
        rightKey = KEY_DEMO_CANCEL;
    }

    TP_SetInputModeControl(leftKey, rightKey, TP_KEY_NONE);

    while(TRUE)
    {
        TP_ScrCls();
        TP_CreateSLInputWindow(Caption, LeftCaption, RightCaption, FALSE);
        TP_LcdPrintf(data);
        userKey = TP_GetString((char *)data, 0xfc, minlen, maxlen);
        if(userKey == 0xFB)//confirm
        {
            datalen = strlen((char *)data);
            if( datalen >= minlen)
            {
                return TRUE;
            }
        }
        else if(userKey == 0xFF)//cancel
        {
            return FALSE;
        }
    }
}


/**
* Show the Dialog when Check SimCard Not Insert
* @author  zuojun
* @version 1.0
* @param   None
* @return  нч
* @date   2011 Nov 30
* note the Dialog Display 3 Second
*/
void ShowNoneCardDialog()
{
    TP_PopupMessageTimed((uint8*)"SIM Card Not Insert",3000);
}

void ShowInvalidCardDialog()
{
    TP_PopupMessageTimed((uint8*)"SIM Card Invalid ",3000);
}


/**
* Show the Dialog For SimCard Pin Verify
* @author  zuojun
* @version 1.0
* @param   None
* @return  it Pin Verify Success return 1  else return 0
* @date   2011 Nov 30
* note the Dialog Display 3 Second
*/
Boolean ShowPinVerifDialog()
{
    uint8 PinVerfy = 0;
    uint8 TmpBuffer[100];
    uint8 Data[10];


    //while (PinVerfy != 0xFF)
    {
        memset(TmpBuffer,0,sizeof(TmpBuffer));
        memset(Data,0,sizeof(Data));

        sprintf((char*)TmpBuffer,"Input PIN(%d):",TP_GetMobileCardPinTryRemainNum());
        TP_EditPassword(TmpBuffer,(uint8*)"Confirm",NULL,Data,4,8) ;

        PinVerfy = TP_MobileCardPinVerification(Data);

        if (PinVerfy == 0xFF)
        {
            return TRUE;
        }

        if (PinVerfy == 0xFD)
        {
            return FALSE;
        }
    }
    return  TRUE;
}

/**
* Show the Dialog For SimCard Puk Verify
* @author  zuojun
* @version 1.0
* @param   None
* @return  it Puk Verify Success return TRUE  else return FALSE
* @date   2011 Nov 30
* note
*/
Boolean ShowPukVerifDialog()
{
    uint8 PukVerfy = 0;
    uint8 PinCheck=0; ///Pin Check
    uint8 TmpBuffer[100];
    uint8 OldPin[10];
    uint8 NewPin[10];
    uint8 Puk[10];

    uint8 Data[10];


    //while (PukVerfy != 0xFF)
    {
        memset(TmpBuffer,0,sizeof(TmpBuffer));
        memset(Puk,0,sizeof(Puk));
        memset(OldPin,0,sizeof(OldPin));
        memset(NewPin,0,sizeof(NewPin));

        sprintf((char*)TmpBuffer,"Input PUK(%d):",TP_GetMobileCardPukTryRemainNum());
        TP_EditPassword(TmpBuffer,(uint8*)"Confirm",NULL,Puk,8,8) ;
        PinCheck =0;
        while (TRUE)
        {
            memset(TmpBuffer,0,sizeof(TmpBuffer));
            sprintf((char*)TmpBuffer,"Input new PIN:");
            TP_EditPassword(TmpBuffer,(uint8*)"Confirm",NULL,OldPin,4,8) ;

            memset(TmpBuffer,0,sizeof(TmpBuffer));
            sprintf((char*)TmpBuffer,"Confirm new PIN:");
            TP_EditPassword(TmpBuffer,(uint8*)"Confirm",NULL,NewPin,4,8) ;

            if (strcmp(OldPin, NewPin) == 0)
            {
                break;
            }

            TP_PopupMessageTimed((uint8*)"PIN confirm error\rPlease reinput ",3000);
        }

        PukVerfy = TP_MobileCardPukVerification(NewPin,Puk);

        if (PukVerfy == 0xFF)
        {
            return TRUE;
        }

        if (PukVerfy == 0xFC)
        {
            TP_PopupMessageTimed((uint8*)"PUK Error",3000);

            return FALSE;
        }
    }

    return  TRUE;
}


/**
* Show the Dialog For SimCard Pin On
* @author  zuojun
* @version 1.0
* @param   None
* @return  if Pin On Success return 1  else return 0
* @date   2011 Nov 30
* note the Dialog Display 3 Second
*/
Boolean ShowPinOnDialog()
{
    uint8 PinOper = 0;
    uint8 TmpBuffer[100];
    uint8 Data[10];

    if (TP_IsMobileCardPinLock())
    {
        TP_PopupMessageTimed((uint8*)"PIN already enabled",3000);
        return FALSE;
    }

    memset(TmpBuffer,0,sizeof(TmpBuffer));
    memset(Data,0,sizeof(Data));

    sprintf((char*)TmpBuffer,"Input PIN:");
    TP_EditPassword(TmpBuffer,(uint8*)"Confirm",(uint8*)"Cancel",Data,4,8) ;
    if (strlen((char *)Data) ==0)
    {
        return FALSE;
    }
    PinOper = TP_MobileCardPinOnOff(1,Data);

    if (PinOper == 0xFF) ///Operate Success
    {
        TP_PopupMessageTimed((uint8*)"PIN enabled",3000);
        return TRUE;
    }

    if (PinOper == 0xFE) ///Pin Num Error
    {
        TP_PopupMessageTimed((uint8*)"PIN error",3000);
        return FALSE;
    }
    TP_PopupMessageTimed((uint8*)"PIN enable failed",3000);
    return  FALSE;
}


/**
* Show the Dialog For SimCard Pin Off
* @author  zuojun
* @version 1.0
* @param   None
* @return  if Pin Off Success return 1  else return 0
* @date   2011 Nov 30
* note the Dialog Display 3 Second
*/
Boolean ShowPinOffDialog()
{
    uint8 PinOper = 0;
    uint8 TmpBuffer[100];
    uint8 Data[10];

    if (TP_IsMobileCardPinLock() == FALSE)
    {
        TP_PopupMessageTimed((uint8*)"PIN already disabled",3000);
        return FALSE;
    }

    memset(TmpBuffer,0,sizeof(TmpBuffer));
    memset(Data,0,sizeof(Data));

    sprintf((char*)TmpBuffer,(char*)"Input PIN:");
    TP_EditPassword(TmpBuffer,(uint8*)"Confirm",(uint8*)"Cancel",Data,4,8) ;
    if (strlen((char *)Data) ==0)
    {
        return FALSE;
    }
    PinOper = TP_MobileCardPinOnOff(0,Data);

    if (PinOper == 0xFF) ///Operate Success
    {
        TP_PopupMessageTimed((uint8*)"PIN disabled", 3000);

        return TRUE;
    }

    if (PinOper == 0xFE) ///Pin Num Error
    {
        TP_PopupMessageTimed((uint8*)"PIN error", 3000);

        return FALSE;
    }

    TP_PopupMessageTimed((uint8*)"PIN disable failed",3000);

    return  FALSE;
}


/**
* Show the Dialog For SimCard Pin change
* @author  zuojun
* @version 1.0
* @param   None
* @return  if pin change Success return TRUE  else return FALSE
* @date   2011 Nov 30
* note
*/
Boolean ShowPinChangeDialog()
{
    uint8 PinVerfy = 0;
    uint8 PinCheck=0; ///Pin Check
    uint8 TmpBuffer[100];
    uint8 OldPin[10];
    uint8 NewPin[10];
    uint8 NewPin1[10];

    if (TP_IsMobileCardPinLock() == FALSE)
    {
        TP_PopupMessageTimed((uint8*)"PIN not enabled",3000);
        return FALSE;
    }
    memset(TmpBuffer,0,sizeof(TmpBuffer));

    memset(OldPin,0,sizeof(OldPin));

    sprintf((char*)TmpBuffer,(char*)"Input PIN:");
    TP_EditPassword(TmpBuffer,(uint8*)"Confirm",(uint8*)"Cancel",OldPin,4,8) ;
    if (strlen((char *)OldPin) == 0)
    {
        return FALSE;
    }
    PinCheck =0;
    while(PinCheck == 0)
    {
        memset(NewPin,0,sizeof(NewPin));
        memset(NewPin1,0,sizeof(NewPin1));
        memset(TmpBuffer,0,sizeof(TmpBuffer));
        sprintf((char*)TmpBuffer,"Input new PIN:");
        TP_EditPassword(TmpBuffer,(uint8*)"Confirm", "Cancel", NewPin,4,8) ;
        if (strlen((char *)NewPin) == 0)
        {
            return FALSE;
        }

        memset(TmpBuffer,0,sizeof(TmpBuffer));
        sprintf((char*)TmpBuffer,(char*)"Confirm new PIN:");
        TP_EditPassword(TmpBuffer,(uint8*)"Confirm", "Cancel", NewPin1,4,8) ;
        if (strlen((char *)NewPin1) == 0)
        {
            return FALSE;
        }

        if (strlen((char *)NewPin1) == strlen((char *)NewPin))
        {
            if (memcmp(NewPin1,NewPin,strlen((char *)NewPin)) == 0)
            {
                PinCheck = 1;
            }
            else
            {
                TP_PopupMessageTimed((uint8*)"PIN confirm error\rPlease Reinput ",3000);
            }
        }
        else
        {
            TP_PopupMessageTimed((uint8*)"PIN confirm error\rPlease Reinput ",3000);
        }
    }

    PinVerfy = TP_MobileCardChangePin(OldPin,NewPin);

    if (PinVerfy == 0xFF)
    {
        TP_PopupMessageTimed((uint8*)"PIN changed",3000);
        return TRUE;
    }

    TP_PopupMessageTimed((uint8*)"PIN change failed",3000);
    return FALSE;
}



/**
* Show the Dialog when Term StartUp
* @author  zuojun
* @version 1.0
* @param   None
* @return  None
* @date   2011 Nov 30
* note
*/
void ShowSimCardStartUPProcess()
{
    uint8 SimCardStatue;
    ///Check SimCard Statue


    SimCardStatue = TP_GetMobileCardStatus();

    while (SimCardStatue != TP_MOBILECARDOK)
    {
        SimCardStatue = TP_GetMobileCardStatus();
        switch (SimCardStatue)
        {
        case  TP_MOBILECARDNONE :
            ShowNoneCardDialog();
            break;

        case  TP_MOBILECARDPINLOCK :
            ShowPinVerifDialog();

            break;
        case  TP_MOBILECARDPUKLOCK :
            ShowPukVerifDialog();
            break;

        case TP_MOBILECARDINVALID:
            ShowInvalidCardDialog();
            break;

        default :
            break;
        }

        /// if SimCard  is not insert then Continue next Step
        if (SimCardStatue == TP_MOBILECARDNONE)
        {
            break;
        }
    }
}






