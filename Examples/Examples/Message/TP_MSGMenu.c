
#include <TPTypedef.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TPBase.h>
#include <TPPrint.h>
#include <TPPhone.h>
#include <TPSMS.h>
#include <TPMobileCard.h>
#include <TPSerial.h>
#include "TP_SMSStore.h"

#include "TP_ListBox.h"
#include <TPSMS.h>
#include "TP_SMSStore.h"
#include "TP_MSGMenu.h"
#include "TP_ParamsManager.h"
//#include <TP_Parameter.h>
//#include <TP_Interface.h>
#ifdef MSGMENU_USE_RESOURCEMM
#include "TPResIndex.h"
#endif


#include "TP_ExitToIdleScr.h"//Ö»oÃµ½aËiË³öµ½´ý»u½çÃæµÄ¶«Î÷

#include  "TP_Singleime.h"
#include  "TP_DisplayAPI.h"
#include  "TP_Phone.h"

#include "tb_upgrade.h"

#define min(x, y) ((x) < (y) ? (x) : (y))

#define SMSTIME 4
//static uint16 UnReadSMSCount = 0;

#ifndef MSGMENU_USE_RESOURCEMM
//Î´¶a¶ÌÐÅi¼±ê
static uint8 SMSUnReadIcon[] =
{
    0x0C,0x0C,
    0x00,0xF8,0x18,0x28,0x48,0x88,0x88,0x48,0x28,0x18,0xF8,0x00,
    0x00,0x07,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x07,0x00
};
//Òn¶a¶ÌÐÅi¼±ê
static uint8 SMSReadedIcon[] =
{
    0x0C,0x0C,
    0x00,0xF8,0x14,0x22,0x42,0x82,0x82,0x42,0x22,0x14,0xF8,0x00,
    0x00,0x07,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x07,0x00
};
#endif


typedef enum
{
    SMSBOX_DO_OPENBOX = 0,  //´ò¿ªÐÅÏä£¬½çÃæ»æÖÆ
    SMSBOX_DO_REOPEN,       //ÖØÐÂ´ò¿ªÐÅÏä
    SMSBOX_DO_SHOWLIST,     //ÏÔÊ¾¶ÌÐÅaÐ±i
    SMSBOX_DO_VIEWSMS,      //oÃ»§n¡Ôn¶ÌÐÅºi²e¿´
    SMSBOX_DO_OPTION,       //Option½çÃæ»æÖÆ
    SMSBOX_DO_DEL,          //e¾³ýÈ·ÈÏ´¦Ài
    SMSBOX_DO_REPLY,        //»Ø¸´´¦Ài
    SMSBOX_DO_SEND,         //¶ÌÐÅ·¢Ëi´¦Ài
    SMSBOX_DO_FORWARD,      //¶ÌÐÅ×ª·¢´¦Ài
    SMSBOX_DO_EXIT,         //ÊÕÎ²´¦Ài£¬ÈçÊi·ÅÏà¹ØÄu´æµÈ¶¯×÷

    SMSBOX_DO_MAX = 0xff    //ÕæÕýµÄiË³ö±êÖ¾
}
TP_SMSBox_Do;

typedef struct MsgInfoTag
{
    uint16      SmsIndex;
    uint8       Phone[SMS_MAX_ADDR_LEN+1];
    TP_DateTime DateTime;
}
TP_MsgInfo;
Boolean TP_SetSMSSaveOn(uint8 value);
void TP_MsgBox_UpdateNewSms(uint16 smsindex);



/**
* To receive a new SMS£¬User wirte code such as store accroding to requiement, prompt of receive SMS.
* @author  He Wendong
* @version 1.0
* @param   Phone        phone no
* @param   Sms          SMS content
* @param   SmsLength    SMS length
* @return  ÎÞ
* @since   2011 Nov 30
*///modify by linhx 20120828
void TP_OnNewSMSNotify(uint8 *Phone,  uint8 *Sms,  uint16 SmsLength, TP_DateTime *DateTime)
{
    TB_Upgrade_UpgradeInfoT upgradeInfo;
    TP_SMSDOResult storeresult;
    uint16 storeindex = 0;
    uint8  result;

    if (TB_Upgrade_IsUpgradeSMS(Sms, SmsLength,
                                (char*)Phone, strlen((char*)Phone),
                                &upgradeInfo) == TRUE)
    {
        result = TB_Upgrade_SMSUpgrade(&upgradeInfo, &AppAPNContext);
        TP_UpgradeTest_ResultPrompt(result);
    }
    /*
    TP_DbgSerialPrn((uint8*)"New SMS Received,DateTime:%d-%d-%d %d:%d:%d ",
                    DateTime->Date.Year,
                    DateTime->Date.Month,
                    DateTime->Date.Day,
                    DateTime->Time.Hours,
                    DateTime->Time.Minutes,
                    DateTime->Time.Seconds);
    */
    //storeresult = TP_StoreOverMsg(SMS_BOX_INBOX,&storeindex,Phone,Sms,SmsLength,SMS_RECORD_RECEIVED_UNREAD);
    storeresult = TP_StoreMsg(SMS_BOX_INBOX,&storeindex,Phone,Sms,SmsLength,SMS_RECORD_RECEIVED_UNREAD,DateTime);
    TP_DbgSerialPrn("\r\nStore sms result is %d",storeresult);
    if(storeresult == SMSDO_SUCCESS)
    {
        #ifdef WIN32
        #else
        //if (TP_GetCallStatus() == TP_HOOKUP)
        //{
        //    StopHookTone();
        //}
        if(TP_GetCallStatus()!=TP_CALLCONNECTED)
        {
            //modify by linhx 20120828
            TP_BeepExTimed(BEEP_ALERT_SMS,1000);
            //TP_Beef(BEEP_ALERT_SMS,500);
        }
        #endif
        TP_MsgBox_UpdateNewSms(storeindex);//Èç¹û´ò¿ªaËÊÕ¼þÏä£¬Ôò¸üÐÂÏÔÊ¾
    }
    else if(storeresult == SMSDO_FULL)
    {
        //
    }

}


/**
* oÃouÔu´ý»u½çÃæ¸üÐÂ¶ÌÐÅi¼±êÏÔÊ¾(³£aa±iÊ¾oÐÐÂ¶ÌÐÅ£¬eaË¸±iÊ¾¶ÌÐÅÏä½«Âu)
* @author ºÎÎÄ¶«
* @param ÎÞ
* @return ÎÞ
* @since 2012Äê7ÔÂ3ÈÕ
*/
void TP_IdleScreenShowMsgIcon()
{
    static Boolean FirstRun = FALSE;
    TP_DateTime DateTime;
    if(FirstRun == FALSE)
    {
        FirstRun = TRUE;
        return;
    }
    if( TP_IsSMSFull(2) )
    {
        TP_GetDateTime(&DateTime);
        if( (DateTime.Time.Seconds%2) ==0 )
        {
            TP_ScrSetIcon(ICON_NEWMESSAGE, OPENICON);
        }
        else
        {
            TP_ScrSetIcon(ICON_NEWMESSAGE, CLOSEICON);
        }
    }
    else if(TP_GetUnReadMsgCount()>0)
    {
        TP_ScrSetIcon(ICON_NEWMESSAGE, OPENICON);
    }
    else
    {
        TP_ScrSetIcon(ICON_NEWMESSAGE, CLOSEICON);
    }
}



/**
* ÐÂ¶ÌÐÅ£¬´«ÈëÏà¹Ø²ÎÊý¼´¿eoÃou»Ø¸´»òÕß×ª·¢Ê¹oÃ£¬Èô²ÎÊýÎª0£¬ÔòÎªÐÂ½¨¶ÌÐÅ
* @author ºÎÎÄ¶«
* @param *Phone_p µç»°ºÅÂëÖ¸Õë
         *Sms_p ¶ÌÐÅÄuÈÝÖ¸Õë
         SmsLen ¶ÌÐÅÄuÈÝ³¤¶È
         ShowOption ÊÇ·nÏÔÊ¾n¡Ïî²Ëµ¥(±£´æ£¬·¢Ëi£¬±£´æºi·¢Ëi)
* @return 1 ·¢Ëi³e¹¦ 0 ·¢ËiÊ§°Ü 2 Ã»oÐ·¢ËiiË³ö
* @since 2012Äê2ÔÂ12ÈÕ
*/
uint8 TP_MsgBoxNew( uint8 *Phone_p,
                          uint8 *Sms_p,
                          uint16 SmsLen,
                          uint8 ShowOption)
{
    uint8 *Sms;
    uint16 SmsLength = 0;
    uint8 Phone[21];
    uint8 PhoneTmp[21];
    uint8 State =0;
    uint8 userKey;//È·¶¨»¹ÊÇÈ¡Ïû
    uint8 option = 0;
    TP_SMSSendResult sendResult = SMSSEND_NORESULT;
    TP_SMSDOResult saveResult = SMSDO_MAX;
    uint8 *caption[5];
    uint8 tempi;
    Boolean SendOrSaved = FALSE;
    uint16 StoreIndex = 0;
    TP_DateTime DateTime;
    TP_DisplayInfo displayInfo;
    TP_InputInfo inputInfo;
    TP_KeyMaps PhoneNumberKeyMap[12] =
    {
        12,
        {
        {TP_KEY_0,(uint8*)"\0"},
        {TP_KEY_1,(uint8*)"\0"},
        {TP_KEY_2,(uint8*)"\0"},
        {TP_KEY_3,(uint8*)"\0"},
        {TP_KEY_4,(uint8*)"\0"},
        {TP_KEY_5,(uint8*)"\0"},
        {TP_KEY_6,(uint8*)"\0"},
        {TP_KEY_7,(uint8*)"\0"},
        {TP_KEY_8,(uint8*)"\0"},
        {TP_KEY_9,(uint8*)"\0"},
        {TP_KEY_STAR,(uint8*)"+\0"},
        {TP_KEY_POUND,(uint8*)"\0"}
        }
    };
    TP_DisplayInfo SaveDisplayInfo =
    {
        (uint8*)"Save Message?",
        ALIGN_MIDDLE,
        (uint8*)"Yes",
        (uint8*)"No",
        KEY_SMS_CONFIRM1,
        KEY_SMS_CANCEL1,
        KEY_SMS_CONFIRM2,
        KEY_SMS_CANCEL2,
        CFONT
    };

    memset(caption,0,sizeof(caption));
    TP_AllocMemory(1025, (void**)&Sms);
    memset(Sms,0,1025);
    memset(Phone,0,sizeof(Phone));
    memset(PhoneTmp,0,sizeof(PhoneTmp));
    if( Phone_p!=0 && strlen((char *)Phone_p)>0)
    {
        if(strlen((char *)Phone_p)>=SMS_MAX_ADDR_LEN)
        {
            memcpy(Phone,Phone_p,(SMS_MAX_ADDR_LEN-1));
        }
        else
        {
            memcpy(Phone,Phone_p,strlen((char *)Phone_p));
        }
        //clear not number string
        memcpy(PhoneTmp,Phone,strlen((char *)(char*)Phone));
        memset(Phone,0,sizeof(Phone));
        for(tempi=0;tempi<strlen((char *)(char*)PhoneTmp);tempi++)
        {
            if((PhoneTmp[tempi]>='0' && PhoneTmp[tempi]<='9')
                || PhoneTmp[tempi] == '+')
            {
                Phone[strlen((char *)(char*)Phone)] = PhoneTmp[tempi];
            }
        }

    }
    if( Sms_p!=0 && strlen((char *)Sms_p)>0 && SmsLen>0)
    {
        SendOrSaved = TRUE;
        if(SmsLen>=1024)
        {
            memcpy(Sms,Sms_p,1024);
            SmsLength = 1024;
        }
        else
        {
            memcpy(Sms,Sms_p,SmsLen);
            SmsLength = SmsLen;
        }
    }


    while(State != 0xff)
    {
        EXIT_TO_IDLE_GOTO_TAG;
        switch(State)
        {

        case 0:
        case 1:
            displayInfo.FONT = CFONT;
            displayInfo.LF_KeyValue = KEY_SMS_CONFIRM2;
            displayInfo.LKeyValue = KEY_SMS_CONFIRM1;
            displayInfo.RKeyValue = KEY_SMS_CANCEL1;
            displayInfo.RF_KeyValue = TP_KEY_NONE;
            displayInfo.strTitle = PNULL;//²»´ø±êÌâ
            displayInfo.align = ALIGN_LEFT;
            displayInfo.strLButtonTitle = (uint8*)TEXT_CONFIRM;
            displayInfo.strRButtonTitle = (uint8*)TEXT_CANCEL;

            inputInfo.defaultMethod  = INPUTMODE_MAX;
            inputInfo.maxLen = 512;
            inputInfo.minLen = 1;
            inputInfo.showIMIcon = TRUE;
            inputInfo.showInputNumber = TRUE;
            inputInfo.timeoutInMS = 0;

            userKey = TP_TextInput(&displayInfo,&inputInfo,Sms);

            if (userKey==0xFB)
            {
                SmsLength = strlen((char *)Sms);
                if(SmsLength>0)
                {
                    if(ShowOption)
                    {
                        State = 2;
                    }
                    else
                    {
                        State = 3;
                    }
                }
            }
            else if(userKey == 0xFF)//È¡Ïû
            {
                State = 6;//iË³ö
            }
            else
            {
                State  = 0xff;
            }
            break;

        case 2://n¡Ôn·¢Ëi ±£´æ£¬»¹ÊÇ·¢Ëiºi±£´æ
            memset(caption,0,sizeof(caption));
            caption[0] = (uint8*)TEXT_SEND;
            caption[1] = (uint8*)TEXT_SAVE;
            caption[2] = (uint8*)TEXT_SAVE_SEND;
            displayInfo.strLButtonTitle = (uint8*)TEXT_CONFIRM;
            displayInfo.strRButtonTitle = (uint8*)TEXT_CANCEL;
            TP_ScrSpaceSet(0,0);
            if(TP_CreateListBoxInterface(TL_LIST,
                                         &displayInfo,
                                         (uint8**)&caption,
                                         3,
                                         1,
                                         &option,
                                         NULL) )
            {
                switch(option)
                {
                    case 1://send
                        State = 3;
                        break;

                    case 2://save
                        SendOrSaved = TRUE;
                        TP_GetDateTime(&DateTime);
                        saveResult = TP_StoreMsg(SMS_BOX_DRAFT,&StoreIndex,Phone,Sms,SmsLength,SMS_RECORD_DRAFT,&DateTime);
                        State  = 0;
                        break;

                    case 3://send and save
                        SendOrSaved = TRUE;
                        TP_GetDateTime(&DateTime);
                        saveResult = TP_StoreMsg(SMS_BOX_DRAFT,&StoreIndex,Phone,Sms,SmsLength,SMS_RECORD_DRAFT,&DateTime);
                        State = 3;
                        break;
                    }
                }
                else
                {
                    State  = 0;
                }
                break;
        case 3:
        case 4:
            displayInfo.FONT = CFONT;
            displayInfo.LF_KeyValue = KEY_SMS_CONFIRM2;
            displayInfo.LKeyValue = KEY_SMS_CONFIRM1;
            displayInfo.RKeyValue = KEY_SMS_CANCEL1;
            displayInfo.RF_KeyValue = TP_KEY_NONE;
            displayInfo.strTitle = (uint8*)TEXT_ENTER_NUMBER;
            displayInfo.align = ALIGN_MIDDLE;
            displayInfo.strLButtonTitle = (uint8*)TEXT_CONFIRM;
            displayInfo.strRButtonTitle = (uint8*)TEXT_BACK;

            inputInfo.maxLen = SMS_MAX_ADDR_LEN-1;
            inputInfo.minLen = 1;
            inputInfo.showIMIcon = TRUE;
            inputInfo.showInputNumber = TRUE;
            inputInfo.inputMode = 0xF4;
            inputInfo.timeoutInMS = 0;
            TP_ConfigInputModeExtern(INPUTMODE_NUMBER,  PhoneNumberKeyMap);
            userKey = TP_NumberInput(&displayInfo,&inputInfo,Phone);
            TP_ConfigInputModeExtern(INPUTMODE_NUMBER,  0);
            if(userKey == 0xFB)//È·¶¨
            {
                if((Phone[0] =='+' && strlen((char*)Phone) >1)
                    || strlen((char *)Phone) >0 )
                {
                    State = 5; //·¢Ëi¶ÌÐÅ
                }
            }
            else if(userKey == 0xFF)//È¡Ïû,iË»Øµ½¶ÌÐÅÄuÈÝ±à¼­¿ò
            {
                State = 0;
            }
            break;
        case 5:
            SendOrSaved = TRUE;
            TP_ScrRestore(0);
            //ShowPopupDialogJust(TEXT_SENDING);
            TP_PopupMessage((uchar*)TEXT_SENDING);
            sendResult = TP_NewSms(Phone, Sms, SmsLength, 60000);

            switch(sendResult)
            {
                case SMSSEND_SUCCESS:
                    TP_ScrRestore(1);
                    TP_PopupMessageTimed((uchar*)TEXT_SEND_SUCESS, 1000);
                    TP_GetDateTime(&DateTime);
                    saveResult = TP_StoreMsg(SMS_BOX_SENTBOX,&StoreIndex,Phone,Sms,SmsLength,SMS_RECORD_SENT,&DateTime);
                    State = 6;
                    break;
                default:
                case SMSSEND_NOSIGNAL:
                case SMSSEND_FAIL:
                    TP_ScrRestore(1);
                    TP_PopupMessageTimed((uchar*)TEXT_SEND_FAIL, 1000);
                    TP_GetDateTime(&DateTime);
                    saveResult = TP_StoreMsg(SMS_BOX_OUTBOX,&StoreIndex,Phone,Sms,SmsLength,SMS_RECORD_NOT_SENT,&DateTime);
                    State = 3;
                    break;
                //case SMSSEND_NOSIGNAL:
                    //ShowPopupDialogMsg("    Send Fail!\n    No Signal! ", TP_KEY_CONFIRM,TP_KEY_CANCEL);
                    //ShowTimePopupDialogMsg("    Send Fail!\n    No Signal! ", 1000);
                    //State = 3;
                    //break;
                case SMSSEND_TIMEOUT:
                    TP_ScrRestore(1);
                    TP_PopupMessageTimed((uchar*)TEXT_SEND_TIMEOUT, 1000);
                    TP_GetDateTime(&DateTime);
                    saveResult = TP_StoreMsg(SMS_BOX_OUTBOX,&StoreIndex,Phone,Sms,SmsLength,SMS_RECORD_NOT_SENT,&DateTime);
                    State = 3;
                    break;
            }
            break;
        case 6://iË³ö
            EXIT_TO_IDLE_TAG:
            State = 0xff;
            break;

        }
    }

    if(sendResult == SMSSEND_SUCCESS)
    {
        TP_FreeMemory((void**)&Sms);
        return 1;
    }
    else if( sendResult == SMSSEND_NORESULT)
    {
        /*
        if(SendOrSaved == FALSE && strlen((char*)Sms)>0)
        {
            if(TP_PopupMessageWithButton(&SaveDisplayInfo))
            {
                TP_GetDateTime(&DateTime);
                TP_StoreMsg(SMS_BOX_DRAFT,&StoreIndex,Phone,Sms,SmsLength,SMS_RECORD_DRAFT,&DateTime);
            }
        }
        */
        TP_FreeMemory((void**)&Sms);
        return 2;
    }
    else
    {
        TP_FreeMemory((void**)&Sms);
        return 0;
    }

}


/**
* ¶ÌÐÅÐÅÏä²Ù×÷
* @author ºÎÎÄ¶«
* @param Box ÐÅÏäÀàÐi
         index ¶ÌÐÅË÷ÒýºÅ
         do ´¦ÀiÀàÐi 1:·¢Ëi£¬2:×ª·¢ 3:»Ø¸´ 0: e¾³ý
* @return 0 ¶aÈ¡Ê§°Ü 1:²Ù×÷³e¹¦ 2 ·¢ËiÊ§°Ü 3 e¾³ýÊ§°Ü  0xff ´¦ÀiÀàÐi´iÎo 0xfe Î´´¦ÀiiË³ö
* @since 2012Äê2ÔÂ12ÈÕ
*/
uint8 TP_SMSDo(TP_SMSBox Box,uint16 index,uint8 deal)
{
    uint8 *Sms;
    uint8 Phone[21];
    uint16 SmsLength = 0;
    TP_SMSRecordState SmsState = SMS_RECORD_MAX;
    TP_DateTime DateTime;
    uint8 result = 0;

    if(deal > 3)
    {
        return 0xff;
    }
    if(deal==0)//e¾³ý
    {
        if(TP_DelMsg(Box,index) == SMSDO_SUCCESS)
        {
            return 1;
        }
        else
        {
            return 3;
        }
    }
    TP_AllocMemory(MAX_SMS_CONT_NUM*SMS_SHORT_MSG_LEN, (void**)&Sms);
    memset(Sms,0,MAX_SMS_CONT_NUM*SMS_SHORT_MSG_LEN);
    memset(&DateTime,0,sizeof(TP_DateTime));
    memset(Phone,0,sizeof(Phone));
    if( SMSDO_SUCCESS == TP_ReadMsg(Box,
                                    index,
                                    Phone,
                                    Sms,
                                    &SmsLength,
                                    &SmsState,
                                    &DateTime) )
    {
        switch(deal)
        {
            case 1://·¢Ëi
                result = TP_MsgBoxNew(Phone, Sms, SmsLength,FALSE);
                break;
            case 2://×ª·¢
                result = TP_MsgBoxNew(0, Sms, SmsLength,FALSE);
                break;
            case 3://»Ø¸´
                result = TP_MsgBoxNew(Phone, 0, 0,FALSE);
                break;
        }
        TP_FreeMemory((void**)&Sms);
        switch(result)
        {
            case 0://·¢ËiÊ§°Ü
                if(Box==SMS_BOX_DRAFT || Box==SMS_BOX_OUTBOX)
                {
                    TP_DelMsg(Box,index);
                }
                return 2;
            case 1://·¢Ëi³e¹¦
                if(Box==SMS_BOX_DRAFT || Box==SMS_BOX_OUTBOX)
                {
                    TP_DelMsg(Box,index);
                }
                return 1;
            case 2://Î´·¢ËiiË³ö
                return 0xfe;
        }
    }
    else
    {
        TP_FreeMemory((void**)&Sms);
        return 0;
    }

}

//ÏÔÊ¾¶ÌÐÅÊýa¿
void TP_MsgBox_ShowMsgCount(TP_ListBox *listbox)
{
    uint8 disppos = 0;
    TP_ScrAttrSet(0);
    disppos = listbox->DisplayRect.right-6*7-1;
//  TP_ScrFontSet(ASCII);
//  TP_SetDisplayArea(disppos-1, listbox->DisplayRect.top+3-1, 126, listbox->DisplayRect.top+3+8);
//  TP_ScrClsDisplayArea();
//  TP_ScrGotoxyEx(disppos, listbox->DisplayRect.top+3);
//  TP_LcdPrintf("%3d/%d",listbox->SelectItemIndex+1,listbox->ItemCount);
    TP_ShowTextSingleLine(disppos-1, listbox->DisplayRect.top+3-1, 126, listbox->DisplayRect.top+3+8,
                          ASCII,ALIGN_LEFT,(uint8*)"%3d/%d",listbox->SelectItemIndex+1,listbox->ItemCount);
    TP_ScrFontSet(CFONT);

}

#ifdef MSGMENU_MSG_PRINT
void TP_PrintData(uint8 *data, uint8 Steps)
{
    uint8 result = 0;
    //uint16 datalen = strlen((char*)data);
    //uint16 addlen = 0;
    //uint8 adddata[128]={0};
    TP_PrnInit();
    if(data != PNULL)
    {
        #ifdef TPS300
        TP_ScrSetIcon(ICON_PRINTER, OPENICON);
        #endif
        TP_PrnSessionStart();
        /*
        do
        {
            memset(adddata, 0, sizeof(adddata));
            if( (datalen-addlen)>= (sizeof(adddata)-1))
            {
                memcpy(adddata, &data[addlen], sizeof(adddata)-1);
                addlen += (sizeof(adddata)-1);
            }
            else
            {
                memcpy(adddata, &data[addlen], datalen-addlen);
                addlen += (datalen-addlen);
            }
            TP_PrnAddString((uint8*)"%s",adddata);
        }while(addlen < datalen);
        */
        TP_PrnAddString((uint8*)"%s",data);
        result = TP_PrnString();
    }
    if(Steps > 0 && result == 0)
    {
        result = TP_PrnStep(Steps);
    }
    switch(result)
    {
        case 0xfa://no paper
            TP_PopupMessageTimed((uchar*)TEXT_NO_PAPER,2000);
            break;

        case 0xfc://temp to hight
            TP_PopupMessageTimed((uchar*)TEXT_TEMPER_TOO_HIG,2000);
            break;

        case 0://sucess
            break;
    }
    if(data != PNULL)
    {
        TP_PrnClearString();
        TP_PrnSessionEnd();
        #ifdef TPS300
        TP_ScrSetIcon(ICON_PRINTER, CLOSEICON);
        #endif
    }
}
#endif

//Ìæ»»¶ÌÐÅÖÐµÄ»Ø³µÎª¿Õ¸n£¬oÃou¶ÌÐÅaÐ±iÔ¤ÀÀÊ±µÄÏÔÊ¾ÎÊÌâ
void TP_ReplaceEnterWithSpace(uint8 *MsgData, uint16 MsgLen )
{
    uint16 tempi;
    for(tempi=0; tempi<MsgLen; tempi++)
    {
        if(TP_IsGBK(MsgData[tempi]) == TRUE)
        {
            tempi++;
        }
        else if(MsgData[tempi] == '\n' )
        {
            MsgData[tempi] = ' ';
        }
    }
}

//TP_ShowTextÊ±µÄ°´¼ü´¦Ài(eÏÏÂ¼üÄu²¿´¦Ài)
void TP_ShowMsgKeyFunc(uint8 keycode,uint8 *msgData)
{
    //TP_DbgSerialPrn("\r\nTP_ShowMsgKeyFunc:%s",msgData);
    #ifdef MSGMENU_MSG_PRINT
    if(keycode == TP_KEY_PRINT)
    {
        TP_Kbflush();
        TP_PrintData(msgData, 24);
    }
    else if(keycode == TP_KEY_FEED)
    {
        TP_Kbflush();
        TP_PrintData(0,8);
    }
    #endif
}

static TP_MsgInfo   *msginfo = PNULL;
static TP_ListBox   *SmsListBox = PNULL;
static TP_SMSBox    ShowBox = SMS_BOX_MAX;
static Boolean      NeedUpdate = FALSE;
static Boolean      OpenBoxFinished = FALSE;

/**
* ¶ÌÐÅÐÅÏä²Ù×÷
* @author ºÎÎÄ¶«
* @param Box ÐÅÏäÀàÐi
* @return ÎÞ
* @since 2012Äê2ÔÂ12ÈÕ
*/
void TP_MsgBoxShow(TP_SMSBox Box)
{
    uint16          tempi;
    uint16          SmsCount = 0;
    //TP_ListBox      *SmsListBox = PNULL;
    //uint8           *DispPhone = PNULL;
    uint8           *PreviewSms = PNULL;
    TP_SMSBox_Do    state = SMSBOX_DO_OPENBOX;
    uint8           Title[18];
    uint16          SmsIndex[MAX_SMS_STORE];
    uint8           Phone[SMS_MAX_ADDR_LEN+1];
    uint8           ChooseValue = 0;
    uint16          SelectIndex = 0;
    uint8           *caption[5];
    Boolean         OpenTimeout = FALSE;//Open Box Timeout
    TP_SMSDOResult  PreviewResult;
    uint16          msginfocount =0;
    uint8           ConfirmKeyExternArray[1] = { KEY_SMS_CONFIRM2 };
    TP_DisplayInfo displayInfo =
    {
        0,
        ALIGN_LEFT,
        (uint8*)TEXT_OPTIONS,
        (uint8*)TEXT_BACK,
        KEY_SMS_CONFIRM1,
        KEY_SMS_CANCEL1,
        KEY_SMS_CONFIRM2,
        KEY_SMS_CANCEL2,
        CFONT
    };

    memset(Title,0,sizeof(Title));
    //TP_ScrCls();
    TP_ScrSpaceSet(0,0);
    TP_AllocMemory(sizeof(TP_MsgInfo)*MAX_SMS_STORE, (void**)&msginfo);
    if(msginfo == PNULL)
    {
        return;
    }


    while(state != SMSBOX_DO_MAX)
    {
        EXIT_TO_IDLE_GOTO_TAG;
        switch(state)
        {
            case SMSBOX_DO_OPENBOX:
                TP_ScrCls();
                //ShowPopupDialogJust(TEXT_OPEN_WAIT);
                TP_PopupMessage((uchar*)TEXT_OPEN_WAIT);
                TP_ScrUpdate();
                {
                    TP_ListBoxRect rect;
                    rect.left = 0;
                    rect.top = 0;
                    rect.right = 127;
                    rect.bottom = 63;

                    SmsListBox = PNULL;
                    memset(msginfo,0,sizeof(TP_MsgInfo)*MAX_SMS_STORE);
                    ShowBox = Box;
                    NeedUpdate = FALSE;
                    OpenBoxFinished = FALSE;
                    msginfocount = 0;

                    switch(Box)
                    {
                        case SMS_BOX_INBOX:
                            sprintf((char *)Title,"%s",TEXT_INBOX);
                            break;
                        case SMS_BOX_OUTBOX:
                            sprintf((char *)Title,"%s",TEXT_OUTBOX);
                            break;
                        case SMS_BOX_SENTBOX:
                            sprintf((char *)Title,"%s",TEXT_SENTBOX);
                            break;
                        case SMS_BOX_DRAFT:
                            sprintf((char *)Title,"%s",TEXT_DRAFTBOX);
                            break;
                    }
                    {
                        uint8 Button[LISTBOX_BUTTON_STRING_LENGTH+1] ={0};
                        TP_MenuTitleCat(Button, LISTBOX_BUTTON_STRING_LENGTH, rect.right-rect.left+1, TEXT_READ, TEXT_EXIT, ' ');
                        SmsListBox = TP_NewListBox(TL_ICONLIST, rect, Title, Button,TP_MsgBox_ShowMsgCount);
                    }
                    //listbox = TP_NewListBox(TL_RADIOBOX, rect, NULL, NULL);
                    SmsListBox->ConfirmKey = KEY_SMS_CONFIRM1;
                    SmsListBox->CancelKey = KEY_SMS_CANCEL1;
                    SmsListBox->SelectKey = TP_KEY_NONE;
                    SmsListBox->LF_KeyValue = KEY_SMS_CONFIRM2;
                    SmsListBox->RF_KeyValue = KEY_SMS_CANCEL2;
                    SmsListBox->ifDrawScrBall = TRUE;
                    SmsListBox->TitleAlign = ALIGN_LEFT;
                    //SmsListBox->ifDrawRect = TRUE;
                    //TP_ListBoxAddFunc(SmsListBox,TP_MsgBox_ShowMsgCount);
                    state = SMSBOX_DO_SHOWLIST;
                }
                break;
            case SMSBOX_DO_SHOWLIST:
                memset(SmsIndex,0,sizeof(SmsIndex));
                SmsCount = 0;

                if( TP_GetSMSSaveOn()==1 && Box == SMS_BOX_INBOX)
                {
                    if(TP_GetMobileCardStatus()== TP_MOBILECARDNONE)
                    {
                        TP_ScrCls();
                        TP_PopupMessageTimed((uchar*)TEXT_NOT_SIMCARD,2000);
                        state = SMSBOX_DO_EXIT;
                        break;
                    }
                    else if(TP_GetMobileCardStatus()== TP_MOBILECARDINVALID)
                    {
                        TP_ScrCls();
                        TP_PopupMessageTimed((uchar*)TEXT_SIM_INVALID,2000);
                        state = SMSBOX_DO_EXIT;
                        break;
                    }

                }
                OpenTimeout = FALSE;
                TP_TimerSet(SMSTIME, 60000);
                tempi = 0;
                SmsCount = 0;
                //TP_DbgSerialPrn("\n listMsgIndex:%d",Box);
                while(TP_ListMsgIndex(Box,SmsIndex,MAX_SMS_STORE,&SmsCount)== SMSDO_NOT_READY)
                {
                    if(TP_TimerCheck(SMSTIME) == 0)
                    {
                        OpenTimeout = TRUE;
                        break;
                    }
                    SmsCount = 0;
                    if(tempi == 0)
                    {
                        TP_ScrCls();
                        //ShowPopupDialogJust(TEXT_NOT_READY_WAIT);
                        TP_PopupMessage((uchar*)TEXT_NOT_READY_WAIT);
                        tempi++;
                    }
                }
                TP_DbgSerialPrnEx(5,"\r\n listMsgIndex finished:%d",SmsCount);

                if( OpenTimeout == TRUE )
                {
                    TP_ScrCls();
                    TP_PopupMessageTimed((uchar*)TEXT_OPEN_TIMEOUT, 2000);
                    state = SMSBOX_DO_EXIT;
                    break;
                }
                if(SmsCount>0)
                {
                    if(Box == SMS_BOX_DRAFT)//²Ý¸åÏäÏÔÊ¾¶ÌÐÅÄuÈÝ
                    {
                        if(PreviewSms != 0)
                        {
                            TP_FreeMemory((void**)&PreviewSms);
                        }
                        TP_AllocMemory((MAX_SMS_PREVIEW_LEN*2+1)*SmsCount, (void**)&PreviewSms);
                        memset(PreviewSms,0,(MAX_SMS_PREVIEW_LEN*2+1)*SmsCount);
                    }

                    for(tempi=0;tempi<SmsCount;tempi++)
                    {
                        //¸ù¾ÝË÷ÒýºÅ¶a³öÏÔÊ¾ÄuÈÝ¼oÈëÏÔÊ¾aÐ±i
                        //uint8 Phone[SMS_MAX_ADDR_LEN];
                        uint8 Sms[MAX_SMS_PREVIEW_LEN*2+1];
                        TP_SMSRecordState SmsSate;
                        TP_DateTime DateTime;
                        uint8 *Icon;
                        //Boolean IsUnread;
                        memset(Phone,0,sizeof(Phone));
                        memset(Sms,0,sizeof(Sms));
                        TP_DbgSerialPrnEx(5, "\n preview start,%d/%d,%d",tempi,SmsCount,SmsIndex[tempi]);
                        PreviewResult = TP_PreviewMsg(Box,SmsIndex[tempi],Phone, Sms, MAX_SMS_PREVIEW_LEN,&SmsSate,&DateTime);
                        TP_DbgSerialPrnEx(5, "\n PreviewResult:%d,%d",PreviewResult,SmsIndex[tempi]);
                        if(SMSDO_SUCCESS == PreviewResult )
                        {
                            if(strlen((char *)Phone)==0 && strlen((char *)Sms)==0)
                            {
                                //TP_DbgSerialPrn("\n del err msg");
                                //Òì³£¶ÌÐÅ£¬e¾³ý
                                TP_DelMsg(Box,SmsIndex[tempi]);
                            }
                            else
                            {
                                switch(SmsSate)
                                {
                                    case SMS_RECORD_RECEIVED_UNREAD:
                                        #ifdef MSGMENU_USE_RESOURCEMM
                                        Icon = TPRes_GetImageByID(SMUnReadLogo);
                                        #else
                                        Icon = SMSUnReadIcon;
                                        #endif
                                        break;
                                    case SMS_RECORD_RECEIVED_READ:
                                        #ifdef MSGMENU_USE_RESOURCEMM
                                        Icon = TPRes_GetImageByID(SMReadedLogo);
                                        #else
                                        Icon = SMSReadedIcon;
                                        #endif
                                        break;
                                    case SMS_RECORD_SENT:
                                        #ifdef MSGMENU_USE_RESOURCEMM
                                        Icon = TPRes_GetImageByID(SMReadedLogo);
                                        #else
                                        Icon = SMSReadedIcon;
                                        #endif
                                        break;
                                    case SMS_RECORD_NOT_SENT:
                                        #ifdef MSGMENU_USE_RESOURCEMM
                                        Icon = TPRes_GetImageByID(SMReadedLogo);
                                        #else
                                        Icon = SMSReadedIcon;
                                        #endif
                                        break;
                                    case SMS_RECORD_DRAFT:
                                        #ifdef MSGMENU_USE_RESOURCEMM
                                        Icon = TPRes_GetImageByID(SMReadedLogo);
                                        #else
                                        Icon = SMSReadedIcon;
                                        #endif
                                        break;
                                }
                                if(Box == SMS_BOX_DRAFT)
                                {
                                    memcpy(&(PreviewSms[(MAX_SMS_PREVIEW_LEN*2+1)*tempi]),
                                           Sms,
                                           strlen((char *)Sms));
                                    TP_ReplaceEnterWithSpace(&(PreviewSms[(MAX_SMS_PREVIEW_LEN*2+1)*tempi]),
                                                             strlen((char *)&(PreviewSms[(MAX_SMS_PREVIEW_LEN*2+1)*tempi])) );
                                    if(strlen((char *)Sms) > 0 )
                                    {

                                        TP_ListBoxAddItem(SmsListBox,
                                                          &(PreviewSms[(MAX_SMS_PREVIEW_LEN*2+1)*tempi]),
                                                          SmsIndex[tempi],
                                                          (tempi==0?1:0),
                                                          Icon);
                                    }
                                }
                                else
                                {
                                    //memcpy(msginfo[tempi].Phone,
                                    //        Phone,
                                    //        strlen((char *)Phone));
                                    if(strlen((char *)Phone) > 0 )
                                    {
                                        msginfo[msginfocount].SmsIndex = SmsIndex[tempi];
                                        msginfo[msginfocount].DateTime = DateTime;
                                        memcpy(msginfo[msginfocount].Phone,
                                               Phone,
                                               strlen((char *)Phone)>=SMS_MAX_ADDR_LEN?(SMS_MAX_ADDR_LEN):strlen((char *)Phone));
                                        TP_DbgSerialPrnEx(5,"\r\n add item:%d-%d,%d-%d,%s",msginfocount,tempi,msginfo[msginfocount].SmsIndex,msginfo[tempi].SmsIndex,Phone);
                                        TP_ListBoxAddItem(SmsListBox,
                                                          msginfo[msginfocount].Phone,
                                                          msginfo[msginfocount].SmsIndex,
                                                          (tempi==0?1:0),
                                                          Icon);
                                        msginfocount++;
                                    }
                                }
                            }
                        }
                    }
                    TP_DbgSerialPrnEx(5, "\r\n msg box added count:%d",msginfocount);
                    if(msginfocount == 0)
                    {
                        //ÎÞ¶ÌÐÅµÄ´¦Ài
                        TP_ScrCls();
                        TP_PopupMessageTimed((uchar*)TEXT_OPEN_FAIL, 1000);
                        state = SMSBOX_DO_EXIT;
                    }
                    else
                    {
                    SmsListBox->SelectItemIndex = SelectIndex;
                    OpenBoxFinished = TRUE;
                    //TP_DbgSerialPrn("\n add item finished");
                    state = SMSBOX_DO_VIEWSMS;
                    }
                }
                else
                {
                    //ÎÞ¶ÌÐÅµÄ´¦Ài
                    TP_ScrCls();
                    TP_PopupMessageTimed((uchar*)TEXT_NO_SMS, 1000);
                    state = SMSBOX_DO_EXIT;
                }
                break;

            case SMSBOX_DO_VIEWSMS:
                NeedUpdate = TRUE;
                if (TP_ShowListBox(SmsListBox))
                {
                    //uint8 Sms[MAX_SMS_CONT_NUM*SMS_SHORT_MSG_LEN];
                    uint8 *Sms = PNULL;
                    uint8 SmsLength;
                    TP_SMSRecordState SmsState;
                    TP_DateTime DateTime;

                    NeedUpdate = FALSE;
                    memset(Phone,0,sizeof(Phone));
                    memset(&DateTime,0,sizeof(TP_DateTime));
                    TP_AllocMemory(MAX_SMS_CONT_NUM*SMS_SHORT_MSG_LEN, (void**)&Sms);
                    memset(Sms,0,MAX_SMS_CONT_NUM*SMS_SHORT_MSG_LEN);
                    TP_ScrCls();
                    TP_ScrRestore(0);
                    TP_PopupMessage((uchar*)TEXT_OPEN_WAIT);
                    TP_DbgSerialPrnEx(1,"\r\n Read Index:%d ", SmsListBox->SelectItemValue[0]);
                    if(TP_ReadMsg(Box,SmsListBox->SelectItemValue[0],Phone, Sms, (uint16 *)&SmsLength,&SmsState,&DateTime)== SMSDO_SUCCESS)
                    {
                        TP_ScrRestore(1);
                        //TP_CreateMLInputWindow(TRUE, TEXT_OPTIONS, TEXT_BACK,FALSE,FALSE);
                        if(Box == SMS_BOX_INBOX)
                        {
                            #ifdef MSGMENU_USE_RESOURCEMM
                            if(SmsListBox->ItemIconList[SmsListBox->SelectItemIndex] == (uint8 *)TPRes_GetImageByID(SMUnReadLogo))
                            {
                                SmsListBox->ItemIconList[SmsListBox->SelectItemIndex] = TPRes_GetImageByID(SMReadedLogo);
                            }
                            #else
                            if(SmsListBox->ItemIconList[SmsListBox->SelectItemIndex] == (uint8 *)SMSUnReadIcon)
                            {
                                SmsListBox->ItemIconList[SmsListBox->SelectItemIndex] = SMSReadedIcon;
                            }
                            #endif
                            //Èç¹û±£´æÔuflashÖÐ£¬»¹o¦¸Ã¸ü¸Ä¶ÌÐÅ×´Ì¬
                        }
                        TP_ShowTextKeyFuncSet(TP_ShowMsgKeyFunc);

                        if(TRUE == TP_ShowTextFullScr(&displayInfo,
                                        (uint8*)"%s\n%s\n%s\n%s\n%s\n%04d-%02d-%02d %02d:%02d:%02d",
                                        TEXT_PHONE_NUMBER,
                                        Phone,
                                        TEXT_CONTENT,
                                        Sms,
                                        TEXT_DATETIME,
                                        DateTime.Date.Year,
                                        DateTime.Date.Month,
                                        DateTime.Date.Day,
                                        DateTime.Time.Hours,
                                        DateTime.Time.Minutes,
                                        DateTime.Time.Seconds))
                        {
                            state = SMSBOX_DO_OPTION;
                        }
                        //TP_ShowTextKeyFuncClear();
                        //TP_ShowTextControlKeyListClear();
                    }
                    else
                    {
                        TP_ScrRestore(1);
                        TP_PopupMessageTimed((uchar*)TEXT_OPEN_FAIL, 2000);
                    }
                    TP_FreeMemory((void**)&Sms);
                }
                else//cancle
                {
                    NeedUpdate = FALSE;
                    state = SMSBOX_DO_EXIT;
                }
                break;

            case SMSBOX_DO_OPTION:
                switch(Box)
                {
                    case SMS_BOX_INBOX:
                        memset(caption,0,sizeof(caption));
                        caption[0] = (uint8*)TEXT_DELETE;
                        caption[1] = (uint8*)TEXT_TEXT_REPLY;
                        caption[2] = (uint8*)TEXT_FORWARD;
                        caption[3] = (uint8*)TEXT_CALL;
                        displayInfo.strLButtonTitle = (uint8*)TEXT_CONFIRM;
                        displayInfo.strRButtonTitle = (uint8*)TEXT_CANCEL;
                        if(TP_CreateListBoxInterface(TL_LIST,
                                                     &displayInfo,
                                                     caption,
                                                     4,
                                                     1,
                                                     &ChooseValue,
                                                     NULL) )
                        {
                            switch(ChooseValue)
                            {
                                case 1://delete
                                    state = SMSBOX_DO_DEL;
                                    break;
                                case 2://text replay
                                    state = SMSBOX_DO_REPLY;
                                    break;
                                case 3://forward
                                    state = SMSBOX_DO_FORWARD;
                                    break;
                                case 4://call
                                    {
                                        uint8 *Phone_p = PNULL;
                                        Phone_p = SmsListBox->ItemList[SmsListBox->SelectItemIndex];
                                        /*
                                        if(strlen((char *)Phone_p)>11)
                                        {
                                            Phone_p = &(Phone_p[strlen((char *)Phone_p)-11]);
                                        }
                                        */
                                        TP_ScrCls();
                                        TP_HookUp();
                                        TP_ShowDialDilog(Phone_p);
                                        //App_Dialing(Phone_p);
                                        state = SMSBOX_DO_VIEWSMS;
                                    }
                                    break;
                            }
                        }
                        else
                        {
                            state = SMSBOX_DO_VIEWSMS;
                        }
                        break;
                    case SMS_BOX_OUTBOX:
                        memset(caption,0,sizeof(caption));
                        caption[0] = (uint8*)TEXT_SEND;
                        caption[1] = (uint8*)TEXT_DELETE;
                        displayInfo.strLButtonTitle = (uint8*)TEXT_CONFIRM;
                        displayInfo.strRButtonTitle = (uint8*)TEXT_CANCEL;
                        if(TP_CreateListBoxInterface(TL_LIST,
                                                     &displayInfo,
                                                     caption,
                                                     2,
                                                     1,
                                                     &ChooseValue,
                                                     NULL) )
                        {
                            switch(ChooseValue)
                            {
                                case 1://Send
                                    state = SMSBOX_DO_SEND;
                                    break;

                                case 2://del
                                    state = SMSBOX_DO_DEL;
                                    break;
                            }
                        }
                        else
                        {
                            state = SMSBOX_DO_VIEWSMS;
                        }
                        break;
                    case SMS_BOX_SENTBOX:
                        memset(caption,0,sizeof(caption));
                        caption[0] = (uint8*)TEXT_SEND;
                        caption[1] = (uint8*)TEXT_DELETE;
                        displayInfo.strLButtonTitle = (uint8*)TEXT_CONFIRM;
                        displayInfo.strRButtonTitle = (uint8*)TEXT_CANCEL;
                        if(TP_CreateListBoxInterface(TL_LIST,
                                                     &displayInfo,
                                                     caption,
                                                     2,
                                                     1,
                                                     &ChooseValue,
                                                     NULL) )
                        {
                            switch(ChooseValue)
                            {
                                case 1://send
                                    state = SMSBOX_DO_SEND;
                                    break;
                                case 2://del
                                    state = SMSBOX_DO_DEL;
                                    break;
                            }
                        }
                        else
                        {
                            state = SMSBOX_DO_VIEWSMS;
                        }
                        break;
                    case SMS_BOX_DRAFT:
                        memset(caption,0,sizeof(caption));
                        caption[0] = (uint8*)TEXT_SEND;
                        caption[1] = (uint8*)TEXT_DELETE;
                        displayInfo.strLButtonTitle = (uint8*)TEXT_CONFIRM;
                        displayInfo.strRButtonTitle = (uint8*)TEXT_CANCEL;
                        if(TP_CreateListBoxInterface(TL_LIST,
                                                     &displayInfo,
                                                     caption,
                                                     2,
                                                     1,
                                                     &ChooseValue,
                                                     NULL))
                        {
                            switch(ChooseValue)
                            {
                                case 1://Send
                                    state = SMSBOX_DO_SEND;
                                    break;

                                case 2://del
                                    state = SMSBOX_DO_DEL;
                                    break;
                            }
                        }
                        else
                        {
                            state = SMSBOX_DO_VIEWSMS;
                        }
                        break;
                }
                break;

            case SMSBOX_DO_DEL:
                memset(caption,0,sizeof(caption));
                caption[0] = (uint8*)TEXT_YES;
                caption[1] = (uint8*)TEXT_NO;
                displayInfo.strLButtonTitle = (uint8*)TEXT_CONFIRM;
                displayInfo.strRButtonTitle = (uint8*)TEXT_CANCEL;
                if(TP_CreateListBoxInterface(TL_LIST,
                                             &displayInfo,
                                             caption,
                                             2,
                                             1,
                                             &ChooseValue,
                                             NULL) )
                {
                    switch(ChooseValue)
                    {
                        case 1://YES
                            //TP_DelMsg(Box,SmsListBox->SelectItemValue[0]);
                            TP_SMSDo(Box,SmsListBox->SelectItemValue[0],0);
                            /*TP_DbgSerialPrn("\n delindex=%d,%s,msginfo index = %d,%s",
                                            SmsListBox->SelectItemValue[0],
                                            SmsListBox->ItemList[SmsListBox->SelectItemIndex],
                                            msginfo[SmsListBox->SelectItemIndex].SmsIndex,
                                            msginfo[SmsListBox->SelectItemIndex].Phone );
                            */
                            TP_ListBoxDeleteItem(SmsListBox, SmsListBox->SelectItemIndex);
                            //memset(&msginfo[SmsListBox->SelectItemIndex],0,sizeof(TP_MsgInfo));
                            msginfo[SmsListBox->SelectItemIndex].SmsIndex = 0;
                            SelectIndex = SmsListBox->SelectItemIndex;
                            if(SmsListBox->ItemCount > 0)
                            {
                                state = SMSBOX_DO_VIEWSMS;
                            }
                            else
                            {
                                TP_ScrCls();
                                TP_PopupMessageTimed((uchar*)TEXT_NO_SMS, 1000);
                                state = SMSBOX_DO_EXIT;
                                //return;
                            }
                            //state = SMSBOX_DO_REOPEN;
                            /*
                            if(TP_SMSDo(Box,SmsListBox->SelectItemValue[0],0)==1)
                            {
                                state = SMSBOX_DO_REOPEN;
                            }
                            else
                            {
                                state = SMSBOX_DO_VIEWSMS;
                            }
                            */
                            break;

                        case 2://no
                            state = SMSBOX_DO_VIEWSMS;
                            break;
                    }
                }
                else
                {
                    state = SMSBOX_DO_VIEWSMS;
                }
                break;

            case SMSBOX_DO_REPLY:
                //TP_MsgBoxNew(Phone, 0, 0,FALSE);
                TP_SMSDo(Box,SmsListBox->SelectItemValue[0],3);
                state = SMSBOX_DO_VIEWSMS;
                break;

            case SMSBOX_DO_SEND:
                TP_SMSDo(Box,SmsListBox->SelectItemValue[0],1);
                state = SMSBOX_DO_REOPEN;
                break;

            case SMSBOX_DO_FORWARD:
                TP_SMSDo(Box,SmsListBox->SelectItemValue[0],2);
                state = SMSBOX_DO_VIEWSMS;
                break;

            case SMSBOX_DO_REOPEN:
                if(SmsListBox->SelectItemIndex>=1)
                {
                    SmsListBox->SelectItemIndex -= 1;
                }
                SelectIndex = SmsListBox->SelectItemIndex;
                TP_ListBoxFree(SmsListBox);
                SmsListBox = PNULL;
                state = SMSBOX_DO_OPENBOX;
                break;

            case SMSBOX_DO_EXIT:
                EXIT_TO_IDLE_TAG:
                if(PreviewSms != PNULL)
                {
                    TP_FreeMemory((void**)&PreviewSms);
                }
                TP_ListBoxFree(SmsListBox);
                SmsListBox = PNULL;
                if(msginfo != PNULL)
                {
                    TP_FreeMemory( (void**)&msginfo);
                    msginfo = PNULL;
                }
                state = SMSBOX_DO_MAX;
                ShowBox = SMS_BOX_MAX;
                break;
        }
    }
}


/**
* Çå³ý¶ÌÐÅ²Ù×÷
* @author ºÎÎÄ¶«
* @param ÎÞ
* @return ÎÞ
* @since 2012Äê4ÔÂ17ÈÕ
*/
void TP_MsgMenu_Clear()
{
    /*
    uint8 *caption[] =
        {
            &("Clear InBox"),
            &("Clear OutBox"),
            &("Clear SentBox"),
            &("Clear DraftBox"),
            &("Clear All")
        };
    */
    uint8 *caption[] =
    {
        (uint8*)TEXT_CLEAR_INBOX,
        (uint8*)TEXT_CLEAR_OUTBOX,
        (uint8*)TEXT_CLEAR_SENTBOX,
        (uint8*)TEXT_CLEAE_DRAFTBOX,
        (uint8*)TEXT_CLEAE_ALL
    };
    uint8 ChooseValue;
    TP_SMSDOResult delResult;
    TP_DisplayInfo displayInfo=
    {
        (uint8*)TEXT_CLEAE_ALL,
        ALIGN_MIDDLE,
        (uint8*)TEXT_CONFIRM,
        (uint8*)TEXT_CANCEL,
        KEY_SMS_CONFIRM1,
        KEY_SMS_CANCEL1,
        KEY_SMS_CONFIRM2,
        KEY_SMS_CANCEL2,
        CFONT
    };
    if( TP_CreateListBoxInterface( TL_LIST,
                                   &displayInfo,
                                   caption,
                                   sizeof(caption)/sizeof(uint8 *),
                                   1,
                                   &ChooseValue,
                                   NULL ) )
    {
        displayInfo.strTitle = (uint8*)TEXT_ARE_YOU_SURE;
        if(TP_PopupMessageWithButton(&displayInfo)==FALSE)
        {
            return;
        }
        if(ChooseValue == 5)
        {
            delResult = TP_DelAllMsg();
        }
        else
        {
            delResult = TP_DelBoxMsg((TP_SMSBox)ChooseValue);
        }
        if( delResult == SMSDO_SUCCESS )
        {
            TP_PopupMessageTimed((uchar*)TEXT_CLEAR_SUCESS,2000);
        }
        else
        {
            TP_PopupMessageTimed((uchar*)TEXT_CLEAE_FAIL,2000);
        }
    }

}

//µ±oÐÐÂ¶ÌÐÅÀ´Ê±¼°Ê±¸üÐÂÐÅÏäaÐ±i
void TP_MsgBox_UpdateNewSms(uint16 smsindex)
{
    uint8 Phone[SMS_MAX_ADDR_LEN] = {0};
    uint8 Sms[MAX_SMS_PREVIEW_LEN*2+1] = {0};
    TP_DateTime DateTime;
    uint16 tempi,tempj,tempk;
    uint16 indexcount = 0;
    uint8 *Icon;
    TP_SMSRecordState SmsState;
    TP_SMSDOResult    PreviewResult;

    memset(&DateTime,0,sizeof(TP_DateTime));
    //TP_DbgSerialPrn("\n TP_MsgBox_UpdateNewSms:%d,%d,%d",smsindex,ShowBox,SmsListBox->ItemCount);
    if(ShowBox == SMS_BOX_INBOX
        && SmsListBox->ItemCount <= MAX_SMS_STORE
        && SmsListBox->ItemCount <= LISTBOX_MAX_COUNT
        && msginfo != PNULL
        && OpenBoxFinished == TRUE )
    {
        PreviewResult = TP_PreviewMsg(ShowBox,
                                      smsindex,
                                      Phone,
                                      Sms,
                                      MAX_SMS_PREVIEW_LEN,
                                      &SmsState,
                                      &DateTime);

        if( SMSDO_SUCCESS == PreviewResult )
        {
            for(tempi=0;tempi<MAX_SMS_STORE;tempi++)
            {
                //TP_DbgSerialPrn("\nmsginfo:%d,%d,%s,%d",tempi,msginfo[tempi].SmsIndex,msginfo[tempi].Phone,msginfo[tempi].DateTime.Date.Year);
                if( msginfo[tempi].SmsIndex >0
                    && msginfo[tempi].DateTime.Date.Year>0)//ÊÇ·noÐÐ§£¬ÊÇ·nÔuaÐ±iÖÐ
                {
                    if(TP_CmpDataTime(&DateTime, &msginfo[tempi].DateTime) >= 0)//½ÏÐÂ£¬ÅÅÇ°
                    {
                        for( tempj = MAX_SMS_STORE-1;tempj>tempi;tempj--)
                        {
                            memset(&msginfo[tempj],0,sizeof(TP_MsgInfo));
                            memcpy(&msginfo[tempj],&msginfo[tempj-1],sizeof(TP_MsgInfo));
                            /*
                            //¸üÐÂlistboxÀïµÄÏÔÊ¾ÄuÈÝÖ¸Õë ÐÞ¸ÄlistboxµÄcaptionÎª¶¯Ì¬eêÇëºo²»ÐèÒª´Ë¶Î´uÂë
                            for( tempk = 0; tempk<SmsListBox->ItemCount; tempk++)
                            {
                                if(msginfo[tempj-1].SmsIndex == SmsListBox->ItemValueList[tempk])
                                {
                                    SmsListBox->ItemList[tempk] = msginfo[tempj].Phone;
                                    break;
                                }
                            }
                            */
                        }
                        msginfo[tempi].SmsIndex = smsindex;
                        msginfo[tempi].DateTime = DateTime;
                        memset(msginfo[tempi].Phone,0,sizeof(msginfo[tempi].Phone));
                        memcpy(msginfo[tempi].Phone, Phone, sizeof(msginfo[tempi].Phone));
                        switch(SmsState)
                        {
                            case SMS_RECORD_RECEIVED_UNREAD:
                                #ifdef MSGMENU_USE_RESOURCEMM
                                Icon = TPRes_GetImageByID(SMUnReadLogo);
                                #else
                                Icon = SMSUnReadIcon;
                                #endif
                                break;
                            case SMS_RECORD_RECEIVED_READ:
                                #ifdef MSGMENU_USE_RESOURCEMM
                                Icon = TPRes_GetImageByID(SMReadedLogo);
                                #else
                                Icon = SMSReadedIcon;
                                #endif
                                break;
                            case SMS_RECORD_SENT:
                                #ifdef MSGMENU_USE_RESOURCEMM
                                Icon = TPRes_GetImageByID(SMReadedLogo);
                                #else
                                Icon = SMSReadedIcon;
                                #endif
                                break;
                            case SMS_RECORD_NOT_SENT:
                                #ifdef MSGMENU_USE_RESOURCEMM
                                Icon = TPRes_GetImageByID(SMReadedLogo);
                                #else
                                Icon = SMSReadedIcon;
                                #endif
                                break;
                            case SMS_RECORD_DRAFT:
                                #ifdef MSGMENU_USE_RESOURCEMM
                                Icon = TPRes_GetImageByID(SMReadedLogo);
                                #else
                                Icon = SMSReadedIcon;
                                #endif
                                break;
                        }
                        TP_DbgSerialPrnEx(1,"\r\n Inbox list Add:%d/%d,%d,%s", indexcount, SmsListBox->ItemCount,msginfo[tempi].SmsIndex, msginfo[tempi].Phone);
                        TP_ListBoxInsertItem(SmsListBox,
                                             (uint8)indexcount,
                                             msginfo[tempi].Phone,
                                             msginfo[tempi].SmsIndex,
                                             FALSE,
                                             Icon,
                                             NeedUpdate);
                        SmsListBox->ItemCount = min(SmsListBox->ItemCount,TP_GetMsgCount(SMS_BOX_INBOX));
                        if( SmsListBox->SelectItemIndex >= SmsListBox->ItemCount)
                        {
                            SmsListBox->SelectItemIndex = SmsListBox->ItemCount -1;
                        }

                        break;
                    }
                    indexcount++;
                }
            }
        }
        //TP_DbgSerialPrn("\n prewview fail:%d",PreviewResult);
    }
}


/**
* ¶ÌÐÅÖÐÐÄeèÖÃ²Ëµ¥´¦Ài
* @author ºÎÎÄ¶«
* @param ÎÞ
* @return ÎÞ
* @since 2012Äê2ÔÂ12ÈÕ
*/
void TP_MsgMenu_MSGCentreSet()
{
    uint8 SMSCenter[SMS_MAX_ADDR_LEN + 1];
    TP_SMSDOResult result;
    TP_KeyMaps PhoneNumberKeyMap[12] =
    {
        12,
        {
        {TP_KEY_0,(uint8*)"\0"},
        {TP_KEY_1,(uint8*)"\0"},
        {TP_KEY_2,(uint8*)"\0"},
        {TP_KEY_3,(uint8*)"\0"},
        {TP_KEY_4,(uint8*)"\0"},
        {TP_KEY_5,(uint8*)"\0"},
        {TP_KEY_6,(uint8*)"\0"},
        {TP_KEY_7,(uint8*)"\0"},
        {TP_KEY_8,(uint8*)"\0"},
        {TP_KEY_9,(uint8*)"\0"},
        {TP_KEY_STAR,(uint8*)"+\0"},
        {TP_KEY_POUND,(uint8*)"\0"}
        }
    };
    TP_DisplayInfo displayInfo =
                                    {
                                        (uint8*)TEXT_MSG_CENTRE,
                                        ALIGN_LEFT,
                                        (uint8*)TEXT_SAVE,
                                        (uint8*)TEXT_EXIT,
                                        KEY_SMS_CONFIRM1,
                                        KEY_SMS_CANCEL1,
                                        KEY_SMS_CONFIRM2,
                                        KEY_SMS_CANCEL2,
                                        CFONT
                                    };
    TP_InputInfo inputInfo =
                            {
                                INPUTMODE_NUMBER,
                                0xF4,
                                1,
                                SMS_MAX_ADDR_LEN,
                                0,
                                FALSE,
                                FALSE,
                            };
    memset(SMSCenter,0,sizeof(SMSCenter));

    TP_ScrCls();
    result = TP_GetMessageCentre(SMSCenter);
    if(result != SMSDO_SUCCESS)
    {
        TP_PopupMessageTimed((uint8*)TEXT_OPEN_FAIL, 2000);
        return;
    }
    TP_ConfigInputModeExtern(INPUTMODE_NUMBER,  PhoneNumberKeyMap);
    if (0xFB == TP_NumberInput(&displayInfo, &inputInfo, SMSCenter))
    {
        TP_ConfigInputModeExtern(INPUTMODE_NUMBER,  0);
        //±£´æ
        /*
        if(SMSCenter[0] == '+')
        {
            result = TP_SetMessageCentre(&SMSCenter[1]);
        }
        else
        */
        {
            result = TP_SetMessageCentre(SMSCenter);
        }

        if(result == SMSDO_NOT_READY)
        {
            TP_PopupMessageTimed((uchar*)TEXT_SIM_NOT_READY, 2000);
            return;
        }
        else if(result == SMSDO_FAIL)
        {
            TP_PopupMessageTimed((uchar*)TEXT_SAVE_FAIL, 2000);
            return;
        }
    }
    TP_ConfigInputModeExtern(INPUTMODE_NUMBER,  0);
}

uint8 TP_GetSMSSaveOn()
{
    uint8 value = 0;
    uint32 SSymbol;
    Data temp;
    SSymbol = TP_Get_ParamsItem_Val(NAME_PARAMS(SMSSaveOn),&temp);
    value = temp.dBuffer.nVal;

    //EE_GETVAR(SysSetting.Machine.SMSSaveOn,value);
    return value;
}

Boolean TP_SetSMSSaveOn(uint8 value)
{
    uint32 SSymbol;
    Data temp;
    TP_Get_ParamsItem_Val(NAME_PARAMS(SMSSaveOn),&temp);
    temp.dBuffer.nVal = value;
    SSymbol = TP_Set_ParamsItem_Val(NAME_PARAMS(SMSSaveOn),&temp);
    if(SSymbol == 0)
        return TRUE;//±£´æ³e¹¦
    else
        return FALSE;//±£´æÊ§°Ü


    //EE_PUTVAR(SysSetting.Machine.SMSSaveOn,value);
}


/**
* ¶ÌÐÅ±£´æÎ»ÖÃ²Ëµ¥eèÖÃ
* @author ºÎÎÄ¶«
* @param ÎÞ
* @return ÎÞ
* @since 2012Äê2ÔÂ12ÈÕ
*/
void TP_MsgMenu_SaveTo()
{
    uint8 ChooseValue = 0;
    uint8 saveon = 0;
    uint8 *caption[5];
    TP_DisplayInfo displayInfo=
    {
        NULL,
        ALIGN_MIDDLE,
        (uint8*)TEXT_CONFIRM,
        (uint8*)TEXT_CANCEL,
        KEY_SMS_CONFIRM1,
        KEY_SMS_CANCEL1,
        KEY_SMS_CONFIRM2,
        KEY_SMS_CANCEL2,
        CFONT
    };
    saveon = TP_GetSMSSaveOn();
    memset(caption,0,sizeof(caption));
    caption[0] = (uint8*)TEXT_SIMCARD;
    caption[1] = (uint8*)TEXT_PHONE_MEMORY;
    if(TP_CreateListBoxInterface(TL_RADIOBOX,
                                 &displayInfo,
                                 caption,
                                 2,
                                 (saveon>0?1:2),
                                 &ChooseValue,
                                 NULL) )
    {
        ChooseValue = (ChooseValue==1?1:0);
        TP_SetSMSSaveOn(ChooseValue);
        #ifdef MSG_SIM800
        if(ChooseValue == 0)
        {
            TP_SetNewSMSStoreOnSim(FALSE);
        }
        else
        {
            TP_SetNewSMSStoreOnSim(TRUE);
        }
        #endif

    }

}

uint8 TP_GetSMSOverwrite()
{
    uint8 value = 0;
    uint32 SSymbol;
    Data temp;
    SSymbol = TP_Get_ParamsItem_Val(NAME_PARAMS(SMSOverwrite),&temp);
    value = temp.dBuffer.nVal;

    //EE_GETVAR(SysSetting.Machine.SMSSaveOn,value);
    return value;
}

Boolean TP_SetSMSOverwrite(uint8 value)
{
    uint32 SSymbol;
    Data temp;
    TP_Get_ParamsItem_Val(NAME_PARAMS(SMSOverwrite),&temp);
    temp.dBuffer.nVal = value;
    SSymbol = TP_Set_ParamsItem_Val(NAME_PARAMS(SMSOverwrite),&temp);
    if(SSymbol == 0)
        return TRUE;//±£´æ³e¹¦
    else
        return FALSE;//±£´æÊ§°Ü


    //EE_PUTVAR(SysSetting.Machine.SMSSaveOn,value);
}


/**
* ¶ÌÐÅ±£´æ¸²¸Ç²Ëµ¥
* @author ºÎÎÄ¶«
* @param ÎÞ
* @return ÎÞ
* @since 2012Äê7ÔÂ19ÈÕ
*/
void TP_MsgMenu_OverWrite()
{
    uint8 ChooseValue = 0;
    uint8 Overwrite = 0;
    uint8 *caption[5];
    TP_DisplayInfo displayInfo=
    {
        (uint8*)TEXT_SMS_OVERWRITE,
        ALIGN_MIDDLE,
        (uint8*)TEXT_CONFIRM,
        (uint8*)TEXT_CANCEL,
        KEY_SMS_CONFIRM1,
        KEY_SMS_CANCEL1,
        KEY_SMS_CONFIRM2,
        KEY_SMS_CANCEL2,
        CFONT
    };
    Overwrite = TP_GetSMSOverwrite();
    memset(caption,0,sizeof(caption));
    caption[0] = (uint8*)TEXT_OFF;
    caption[1] = (uint8*)TEXT_ON;
    if(TP_CreateListBoxInterface(TL_RADIOBOX,
                                 &displayInfo,
                                 caption,
                                 2,
                                 Overwrite,
                                 &ChooseValue,
                                 NULL) )
    {
        TP_SetSMSOverwrite(ChooseValue);
    }
}

/**
* ¸ù¾Ý¶ÌÐÅ±£´æÎ»ÖÃºiÐÅÏäÀàÐiaÐ³ö¶ÌÐÅaÐ±i
* @author ºÎÎÄ¶«
* @param Box ÐÅÏäÀàÐi<BR>
         *SmsIndex ¶ÌÐÅË÷ÒýºÅ±£´æÖ¸Õë<BR>
         *SmsCount ¶ÌÐÅÌõÊý
* @return TP_SMSDOResult
* @since 2012Äê2ÔÂ12ÈÕ
*/
TP_SMSDOResult TP_ListMsgIndex(TP_SMSBox Box,
                                      uint16 *SmsIndex,
                                      uint16 MaxSmsCount,
                                      uint16 *SmsCount)
{
    uint8 SmsSaveOn = 0;
    SmsSaveOn = TP_GetSMSSaveOn();
    if(Box == SMS_BOX_INBOX)
    {
        if(SmsSaveOn == 1)//sim
        {
            return TP_ListSIMSMSIndex(SmsIndex,MaxSmsCount,SmsCount);
        }
        else
        {
            return TP_ListIndexByBox(Box,SmsIndex,MaxSmsCount,SmsCount);
        }
    }
    else
    {
        return TP_ListIndexByBox(Box,SmsIndex,MaxSmsCount,SmsCount);
    }

}

/**
* ¸ù¾Ý¶ÌÐÅ±£´æÎ»ÖÃÔ¤ÀÀ¶ÌÐÅ
* @author ºÎÎÄ¶«
* @param Box ÐÅÏäÀàÐi<BR>
         index Ë÷ÒýºÅ<BR>
         *Phone ºÅÂëÖ¸Õë<BR>
         *Sms ¶ÌÐÅÄuÈÝÖ¸Õë<BR>
         PreviewLength Ô¤ÀÀ¶ÌÐÅ³¤¶È<BR>
         *SmsState ¶ÌÐÅ×´Ì¬<BR>
         *DateTime ÈÕÆuÊ±¼ä
* @return TP_SMSDOResult
* @since 2012Äê2ÔÂ12ÈÕ
*/
TP_SMSDOResult TP_PreviewMsg(TP_SMSBox Box,
                                  uint16 index,
                                  uint8 *Phone,
                                  uint8 *Sms,
                                  uint16 PreviewLength,
                                  TP_SMSRecordState *SmsState,
                                  TP_DateTime *DateTime)
{
    uint8 SmsSaveOn = 0;
    Boolean IsUnread = FALSE;
    TP_SMSDOResult result;
    SmsSaveOn = TP_GetSMSSaveOn();
    if(Box == SMS_BOX_INBOX)
    {
        if(SmsSaveOn == 1)
        {
            result = TP_PreviewSms(index,Phone,Sms,PreviewLength,&IsUnread,DateTime);
            if(IsUnread == TRUE)
            {
                *SmsState = SMS_RECORD_RECEIVED_UNREAD;
            }
            else
            {
                *SmsState = SMS_RECORD_RECEIVED_READ;
            }
            return result;
        }
        else
        {
            return TP_SMSPreview(index,Phone,Sms,PreviewLength,SmsState,DateTime);
        }
    }
    else
    {
        return TP_SMSPreview(index,Phone,Sms,PreviewLength,SmsState,DateTime);
    }
}



/**
* ¸ù¾Ý¶ÌÐÅ±£´æÎ»ÖÃ¶aÈ¡¶ÌÐÅ
* @author ºÎÎÄ¶«
* @param Box ÐÅÏäÀàÐi<BR>
         index Ë÷ÒýºÅ<BR>
         *Phone ºÅÂëÖ¸Õë<BR>
         *Sms ¶ÌÐÅÄuÈÝÖ¸Õë<BR>
         *SmsLength ¶ÌÐÅ³¤¶È<BR>
         *SmsState ¶ÌÐÅ×´Ì¬<BR>
         *DateTime ÈÕÆuÊ±¼ä
* @return TP_SMSDOResult
* @since 2012Äê2ÔÂ12ÈÕ
*/
TP_SMSDOResult TP_ReadMsg(TP_SMSBox Box,
                              uint16 index,
                              uint8 *Phone,
                              uint8 *Sms,
                              uint16 *SmsLength,
                              TP_SMSRecordState *SmsState,
                              TP_DateTime *DateTime)
{
    uint8 SmsSaveOn = 0;
    Boolean IsUnread = FALSE;
    TP_SMSDOResult result;
    SmsSaveOn = TP_GetSMSSaveOn();
    if(Box == SMS_BOX_INBOX)
    {
        if(SmsSaveOn == 1)
        {
            result = TP_ReadSms(index,Phone,Sms,SmsLength,&IsUnread,DateTime);
            if(IsUnread == TRUE)
            {
                *SmsState = SMS_RECORD_RECEIVED_UNREAD;
            }
            else
            {
                *SmsState = SMS_RECORD_RECEIVED_READ;
            }
            return result;
        }
        else
        {
            return TP_SMSRead(index,Phone,Sms,SmsLength,SmsState,DateTime);
        }
    }
    else
    {
        return TP_SMSRead(index,Phone,Sms,SmsLength,SmsState,DateTime);
    }

}

/**
* ¸ù¾Ý¶ÌÐÅ±£´æÎ»ÖÃe¾³ý¶ÌÐÅ
* @author ºÎÎÄ¶«
* @param idnex Ë÷ÒýºÅ<BR>
* @return TP_SMSDOResult
* @since 2012Äê2ÔÂ12ÈÕ
*/
TP_SMSDOResult TP_DelMsg(TP_SMSBox Box,uint16 index)
{
    uint8 SmsSaveOn = 0;
    SmsSaveOn = TP_GetSMSSaveOn();
    if(Box == SMS_BOX_INBOX)
    {
        if(SmsSaveOn == 1)
        {
            return TP_DelSMS(index);
        }
        else
        {
            return TP_SMSDel(index);
        }
    }
    else
    {
        return TP_SMSDel(index);
    }
}

/**
* e¾³ýÐÅÏäµÄÈ«²¿¶ÌÐÅ
* @author ºÎÎÄ¶«
* @param Box Òªe¾³ýµÄÐÅÏä<BR>
* @return TP_SMSDOResult
* @since 2012Äê4ÔÂ17ÈÕ
* @note: Èç¹ûe¾³ý¹ý³ÌÖÐ³ö´i£¬ÔòiË³ö
*/
TP_SMSDOResult TP_DelBoxMsg(TP_SMSBox Box)
{
    uint16 SmsIndex[MAX_SMS_STORE]={0};
    uint16 SmsCount = 0;
    uint16 temp;
    TP_SMSDOResult delResult;
    memset(SmsIndex,0,sizeof(SmsIndex));
    TP_ListMsgIndex(Box,SmsIndex,MAX_SMS_STORE,&SmsCount);
    for( temp=0; temp<SmsCount; temp++ )
    {
        delResult = TP_DelMsg(Box, SmsIndex[temp]);
        if( delResult != SMSDO_SUCCESS )
        {
            return SMSDO_FAIL;
        }
    }
    return SMSDO_SUCCESS;
}

/**
* e¾³ýËùoÐ¶ÌÐÅ
* @author ºÎÎÄ¶«
* @return TP_SMSDOResult
* @since 2012Äê4ÔÂ17ÈÕ
* @note: Èç¹ûe¾³ý¹ý³ÌÖÐ³ö´i£¬ÔòiË³ö
*/
TP_SMSDOResult TP_DelAllMsg()
{
    TP_SMSBox box;

    for(box=SMS_BOX_INBOX; box<SMS_BOX_MAX; box++)
    {
        if(TP_DelBoxMsg(box) != SMSDO_SUCCESS)
        {
            return SMSDO_FAIL;
        }
    }
    return SMSDO_SUCCESS;
}


/**
* ¸ù¾Ý¶ÌÐÅ±£´æÎ»ÖÃ±£´æ¶ÌÐÅ
* @author ºÎÎÄ¶«
* @param Box ÐÅÏäÀàÐi<BR>
         *StoreIndex ±£´æË÷ÒýºÅ<BR>
         *Phone ºÅÂëÖ¸Õë<BR>
         *Sms ¶ÌÐÅÄuÈÝÖ¸Õë<BR>
         *SmsLength ¶ÌÐÅ³¤¶È£¬Ô¤ÀÀÊ±ÎªÔ¤ÀÀµÄ¶ÌÐÅÄuÈÝ³¤¶È<BR>
*         SmsState ¶ÌÐÅ×´Ì¬<BR>
*         DateTime ÈÕÆuÊ±¼ä
* @return TP_SMSDOResult
* @since 2012Äê2ÔÂ12ÈÕ
*/
TP_SMSDOResult TP_StoreMsg(TP_SMSBox Box,
                           uint16 *StoreIndex,
                           uint8 *Phone,
                           uint8 *Sms,
                           uint16 SmsLength,
                           TP_SMSRecordState SmsSate,
                           TP_DateTime *DateTime)
{
    uint8 SmsSaveOn = 0;
    //uint16 StoreIndex = 0;
    SmsSaveOn = TP_GetSMSSaveOn();
    if(Box == SMS_BOX_INBOX)
    {
        if(SmsSaveOn == 1)
        {
            #ifdef MSG_SIM800
            *StoreIndex = TP_GetSMSNotifySimIndex();
            return SMSDO_SUCCESS;
            #else
            return TP_StoreSms(StoreIndex,
                               Phone,
                               Sms,
                               SmsLength,
                               (SmsSate==SMS_RECORD_RECEIVED_UNREAD?TRUE:FALSE)/*,
                               DateTime */);
            #endif
        }
        else
        {
            return TP_SMSSave(StoreIndex,
                              Phone,
                              Sms,
                              SmsLength,
                              Box,
                              SmsSate,
                              DateTime);
        }
    }
    else
    {
        return TP_SMSSave(StoreIndex,
                          Phone,
                          Sms,
                          SmsLength,
                          Box,
                          SmsSate,
                          DateTime);
    }
}


/**
* ¸ù¾Ý¶ÌÐÅ±£´æÎ»ÖÃ±£´æ¶ÌÐÅ(Èç¹û´æ´¢ÒnÂu£¬Ôòe¾³ý×î¾eµÄÊÕ¼þÏä¶ÌÐÅ)
* @author ºÎÎÄ¶«
* @param Box ÐÅÏäÀàÐi<BR>
         *StoreIndex ±£´æË÷ÒýºÅ<BR>
         *Phone ºÅÂëÖ¸Õë<BR>
         *Sms ¶ÌÐÅÄuÈÝÖ¸Õë<BR>
         *SmsLength ¶ÌÐÅ³¤¶È£¬Ô¤ÀÀÊ±ÎªÔ¤ÀÀµÄ¶ÌÐÅÄuÈÝ³¤¶È<BR>
*         SmsState ¶ÌÐÅ×´Ì¬<BR>
*         DateTime ÈÕÆuÊ±¼ä
* @return TP_SMSDOResult
* @since 2012Äê2ÔÂ12ÈÕ
*/
TP_SMSDOResult TP_StoreOverMsg(TP_SMSBox Box,
                                      uint16 *StoreIndex,
                                      uint8 *Phone,
                                      uint8 *Sms,
                                      uint16 SmsLength,
                                      TP_SMSRecordState SmsSate,
                                      TP_DateTime *DateTime)
{
    TP_SMSDOResult  storeResult,Result;
    uint16          SmsIndex[MAX_SMS_STORE];
    uint16          SmsCount = 0;
    uint16          saveindex = 0;
    uint16          delcount = 0;

    memset(SmsIndex,0,sizeof(SmsIndex));

    do
    {
        storeResult = TP_StoreMsg( Box,
                                 &saveindex,
                                 Phone,
                                 Sms,
                                 SmsLength,
                                 SmsSate,
                                 DateTime);
        if( storeResult == SMSDO_FULL )
        {
            //TP_DelMsg( Box, saveindex);
            if(delcount == 0)//Ö»¶aÈ¡Ò»´Î
            {
                Result = TP_ListMsgIndex(Box,SmsIndex,MAX_SMS_STORE,&SmsCount);
            }
            Result = TP_DelMsg(Box,SmsIndex[SmsCount-delcount-1]);
            delcount++;
        }
        else
        {
            break;
        }
    }while(SmsCount >0 && SmsCount>=delcount && delcount < MAX_SMS_CONT_NUM );

    if(storeResult == SMSDO_SUCCESS)
    {
        *StoreIndex = saveindex;
    }
    return storeResult;
}


uint16 TP_GetMsgAllCount(TP_SMSBox Box)
{
    uint8 SmsSaveOn = 0;
    SmsSaveOn = TP_GetSMSSaveOn();
    if(Box == SMS_BOX_INBOX)
    {
        if(SmsSaveOn == 1)
        {
            return TP_GetSIMSMSAllCount();
        }
        else
        {
            return TP_GetFlashSMSAllCount(Box);
        }
    }
    else
    {
        return TP_GetFlashSMSAllCount(Box);
    }
}

uint16 TP_GetMsgCount(TP_SMSBox Box)
{
    uint8 SmsSaveOn = 0;
    SmsSaveOn = TP_GetSMSSaveOn();
    if(Box == SMS_BOX_INBOX)
    {
        if(SmsSaveOn == 1)
        {
            return TP_GetSIMSMSCount();
        }
        else
        {
            return TP_GetFlashSMSCount(Box);
        }
    }
    else
    {
        return TP_GetFlashSMSCount(Box);
    }
}

uint16 TP_GetUnReadMsgCount()
{
    uint8 SmsSaveOn = 0;
    SmsSaveOn = TP_GetSMSSaveOn();
    if(SmsSaveOn == 1)
    {
        return TP_GetSIMUnReadSMSCount();
    }
    else
    {
        return TP_GetFlashUnReadSMSCount();
    }
}

Boolean TP_IsSMSFull(uint16 left)
{
    uint8 SmsSaveOn = 0;
    uint16 totalcount = 0;
    uint16 usedcount = 0;
    SmsSaveOn = TP_GetSMSSaveOn();
    if(SmsSaveOn == 1)
    {
        //modify by linhx 2012-08-28
        totalcount = TP_GetSIMSMSMemorySize();
        usedcount  = TP_GetSIMSMSAllCount();
        if(totalcount == 0)//sim card not ready
        {
            return FALSE;
        }
        if( (usedcount+left) >= totalcount)
        {
            return TRUE;
        }
    }
    else
    {
        /*
        totalcount = TP_GetFlashSMSMemorySize();
        usedcount  = TP_GetFlashSMSAllCount(SMS_BOX_INBOX) +
                     TP_GetFlashSMSAllCount(SMS_BOX_OUTBOX) +
                     TP_GetFlashSMSAllCount(SMS_BOX_SENTBOX) +
                     TP_GetFlashSMSAllCount(SMS_BOX_DRAFT);
        */
        if(TP_GetBoxFreeSpace(SMS_BOX_INBOX) <= left )
        {
            return TRUE;
        }

    }
    //TP_DbgSerialPrn("\n TP_IsSMSFull:%d,%d,%d",SmsSaveOn,usedcount,totalcount);
    return FALSE;
}




