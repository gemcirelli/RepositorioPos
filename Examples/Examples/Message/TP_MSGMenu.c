
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


#include "TP_ExitToIdleScr.h"//只o玫絘薸顺龅酱u界面的东西

#include  "TP_Singleime.h"
#include  "TP_DisplayAPI.h"
#include  "TP_Phone.h"

#include "tb_upgrade.h"

#define min(x, y) ((x) < (y) ? (x) : (y))

#define SMSTIME 4
//static uint16 UnReadSMSCount = 0;

#ifndef MSGMENU_USE_RESOURCEMM
//未禷短信i急�
static uint8 SMSUnReadIcon[] =
{
    0x0C,0x0C,
    0x00,0xF8,0x18,0x28,0x48,0x88,0x88,0x48,0x28,0x18,0xF8,0x00,
    0x00,0x07,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x07,0x00
};
//襫禷短信i急�
static uint8 SMSReadedIcon[] =
{
    0x0C,0x0C,
    0x00,0xF8,0x14,0x22,0x42,0x82,0x82,0x42,0x22,0x14,0xF8,0x00,
    0x00,0x07,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x07,0x00
};
#endif


typedef enum
{
    SMSBOX_DO_OPENBOX = 0,  //打开信箱，界面绘制
    SMSBOX_DO_REOPEN,       //重新打开信箱
    SMSBOX_DO_SHOWLIST,     //显示短信a斜i
    SMSBOX_DO_VIEWSMS,      //o没≡n短信篿瞖看
    SMSBOX_DO_OPTION,       //Option界面绘制
    SMSBOX_DO_DEL,          //e境啡洗i
    SMSBOX_DO_REPLY,        //回复处纈
    SMSBOX_DO_SEND,         //短信发薸处纈
    SMSBOX_DO_FORWARD,      //短信转发处纈
    SMSBOX_DO_EXIT,         //收尾处纈，如蔵放相关膗存等动作

    SMSBOX_DO_MAX = 0xff    //真正的i顺霰曛�
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
* To receive a new SMS，User wirte code such as store accroding to requiement, prompt of receive SMS.
* @author  He Wendong
* @version 1.0
* @param   Phone        phone no
* @param   Sms          SMS content
* @param   SmsLength    SMS length
* @return  无
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
        TP_MsgBox_UpdateNewSms(storeindex);//如果打开a耸占洌蚋孪允�
    }
    else if(storeresult == SMSDO_FULL)
    {
        //
    }

}


/**
* o胦u評待籾界面更新短信i急晗允�(常aa眎示o行露绦牛琫a烁眎示短信箱将聈)
* @author 何文东
* @param 无
* @return 无
* @since 2012年7月3日
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
* 新短信，传入相关参数即縠o胦u回复或者转发使o茫舨问�0，则为新建短信
* @author 何文东
* @param *Phone_p 电话号码指针
         *Sms_p 短信膗容指针
         SmsLen 短信膗容长度
         ShowOption 是穘显示n∠畈说�(保存，发薸，保存篿发薸)
* @return 1 发薸砮功 0 发薸失败 2 没o蟹⑺ii顺�
* @since 2012年2月12日
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
    uint8 userKey;//确定还是取消
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
            displayInfo.strTitle = PNULL;//不带标题
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
            else if(userKey == 0xFF)//取消
            {
                State = 6;//i顺�
            }
            else
            {
                State  = 0xff;
            }
            break;

        case 2://n≡n发薸 保存，还是发薸篿保存
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
            if(userKey == 0xFB)//确定
            {
                if((Phone[0] =='+' && strlen((char*)Phone) >1)
                    || strlen((char *)Phone) >0 )
                {
                    State = 5; //发薸短信
                }
            }
            else if(userKey == 0xFF)//取消,i嘶氐蕉绦拍u容编辑框
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
        case 6://i顺�
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
* 短信信箱操作
* @author 何文东
* @param Box 信箱类衖
         index 短信索引号
         do 处纈类衖 1:发薸，2:转发 3:回复 0: e境�
* @return 0 禷取失败 1:操作砮功 2 发薸失败 3 e境О�  0xff 处纈类衖磇蝟 0xfe 未处纈i顺�
* @since 2012年2月12日
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
    if(deal==0)//e境�
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
            case 1://发薸
                result = TP_MsgBoxNew(Phone, Sms, SmsLength,FALSE);
                break;
            case 2://转发
                result = TP_MsgBoxNew(0, Sms, SmsLength,FALSE);
                break;
            case 3://回复
                result = TP_MsgBoxNew(Phone, 0, 0,FALSE);
                break;
        }
        TP_FreeMemory((void**)&Sms);
        switch(result)
        {
            case 0://发薸失败
                if(Box==SMS_BOX_DRAFT || Box==SMS_BOX_OUTBOX)
                {
                    TP_DelMsg(Box,index);
                }
                return 2;
            case 1://发薸砮功
                if(Box==SMS_BOX_DRAFT || Box==SMS_BOX_OUTBOX)
                {
                    TP_DelMsg(Box,index);
                }
                return 1;
            case 2://未发薸i顺�
                return 0xfe;
        }
    }
    else
    {
        TP_FreeMemory((void**)&Sms);
        return 0;
    }

}

//显示短信数a�
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

//替换短信中的回车为空竛，o胦u短信a斜i预览时的显示问题
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

//TP_ShowText时的按键处纈(e舷录u部处纈)
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
* 短信信箱操作
* @author 何文东
* @param Box 信箱类衖
* @return 无
* @since 2012年2月12日
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
                    if(Box == SMS_BOX_DRAFT)//草稿箱显示短信膗容
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
                        //根据索引号禷出显示膗容紀入显示a斜i
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
                                //异常短信，e境�
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
                        //无短信的处纈
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
                    //无短信的处纈
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
                            //如果保存評flash中，还oΩ酶亩绦抛刺�
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
* 清除短信操作
* @author 何文东
* @param 无
* @return 无
* @since 2012年4月17日
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

//当o行露绦爬词奔笆备滦畔鋋斜i
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
                    && msginfo[tempi].DateTime.Date.Year>0)//是穘o行В欠n評a斜i中
                {
                    if(TP_CmpDataTime(&DateTime, &msginfo[tempi].DateTime) >= 0)//较新，排前
                    {
                        for( tempj = MAX_SMS_STORE-1;tempj>tempi;tempj--)
                        {
                            memset(&msginfo[tempj],0,sizeof(TP_MsgInfo));
                            memcpy(&msginfo[tempj],&msginfo[tempj-1],sizeof(TP_MsgInfo));
                            /*
                            //更新listbox里的显示膗容指针 修改listbox的caption为动态e昵牒o不需要此段磚码
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
* 短信中心e柚貌说ゴi
* @author 何文东
* @param 无
* @return 无
* @since 2012年2月12日
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
        //保存
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
        return TRUE;//保存砮功
    else
        return FALSE;//保存失败


    //EE_PUTVAR(SysSetting.Machine.SMSSaveOn,value);
}


/**
* 短信保存位置菜单e柚�
* @author 何文东
* @param 无
* @return 无
* @since 2012年2月12日
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
        return TRUE;//保存砮功
    else
        return FALSE;//保存失败


    //EE_PUTVAR(SysSetting.Machine.SMSSaveOn,value);
}


/**
* 短信保存覆盖菜单
* @author 何文东
* @param 无
* @return 无
* @since 2012年7月19日
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
* 根据短信保存位置篿信箱类衖a谐龆绦臿斜i
* @author 何文东
* @param Box 信箱类衖<BR>
         *SmsIndex 短信索引号保存指针<BR>
         *SmsCount 短信条数
* @return TP_SMSDOResult
* @since 2012年2月12日
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
* 根据短信保存位置预览短信
* @author 何文东
* @param Box 信箱类衖<BR>
         index 索引号<BR>
         *Phone 号码指针<BR>
         *Sms 短信膗容指针<BR>
         PreviewLength 预览短信长度<BR>
         *SmsState 短信状态<BR>
         *DateTime 日苪时间
* @return TP_SMSDOResult
* @since 2012年2月12日
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
* 根据短信保存位置禷取短信
* @author 何文东
* @param Box 信箱类衖<BR>
         index 索引号<BR>
         *Phone 号码指针<BR>
         *Sms 短信膗容指针<BR>
         *SmsLength 短信长度<BR>
         *SmsState 短信状态<BR>
         *DateTime 日苪时间
* @return TP_SMSDOResult
* @since 2012年2月12日
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
* 根据短信保存位置e境绦�
* @author 何文东
* @param idnex 索引号<BR>
* @return TP_SMSDOResult
* @since 2012年2月12日
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
* e境畔涞娜慷绦�
* @author 何文东
* @param Box 要e境男畔�<BR>
* @return TP_SMSDOResult
* @since 2012年4月17日
* @note: 如果e境讨谐龃i，则i顺�
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
* e境鵲卸绦�
* @author 何文东
* @return TP_SMSDOResult
* @since 2012年4月17日
* @note: 如果e境讨谐龃i，则i顺�
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
* 根据短信保存位置保存短信
* @author 何文东
* @param Box 信箱类衖<BR>
         *StoreIndex 保存索引号<BR>
         *Phone 号码指针<BR>
         *Sms 短信膗容指针<BR>
         *SmsLength 短信长度，预览时为预览的短信膗容长度<BR>
*         SmsState 短信状态<BR>
*         DateTime 日苪时间
* @return TP_SMSDOResult
* @since 2012年2月12日
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
* 根据短信保存位置保存短信(如果存储襫聈，则e境罹e的收件箱短信)
* @author 何文东
* @param Box 信箱类衖<BR>
         *StoreIndex 保存索引号<BR>
         *Phone 号码指针<BR>
         *Sms 短信膗容指针<BR>
         *SmsLength 短信长度，预览时为预览的短信膗容长度<BR>
*         SmsState 短信状态<BR>
*         DateTime 日苪时间
* @return TP_SMSDOResult
* @since 2012年2月12日
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
            if(delcount == 0)//只禷取一次
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




