#include <TPTypedef.h>
#include <TPBase.h>
#include <TPLCD.h>
#include <TPKEYPAD.h>
#include <TPFile.h>
#include <TPPhone.h>
#include "TP_ListBox.h"
#include "TP_ResourceMM.h"
#include "TP_CallRecord.h"
#include "TP_DisplayAPI.h"
#include "TP_Phone.h"
//#include "TP_SMSStore.h"
//#include "TP_MsgMenu.h"

#define MAX_DIALCALL_COUNT 20
#define MAX_RECEIVECALL_COUNT 10
#define MAX_MISSCALL_COUNT 10

extern
uint8 TP_MsgBoxNew( uint8 *Phone_p,
                          uint8 *Sms_p,
                          uint16 SmsLen,
                          uint8 ShowOption);

///Call Information struct
typedef struct
{
    ///Call Phone num
    uint8 Phone[20];
    ///Call Start TIme
    TP_DateTime StartTime;
    ///Call End TIme
    TP_DateTime EndTime;
    ///Call TImes
    uint32 CallTimes;
}TP_Callecord;

typedef struct
{
    ///Dial Call Count
    uint8 DialCallCount;
    ///Miss Call Count
    uint8 MissCallCount;
    ///Receice Call Count
    uint8 ReceiveCallCount;
    ///IsNew MissCall
    uint8 NewMissCall;

    TP_Callecord DialCall[MAX_DIALCALL_COUNT];
    TP_Callecord MissCall[MAX_MISSCALL_COUNT];
    TP_Callecord ReceiveCall[MAX_RECEIVECALL_COUNT];
}TP_CallRecords;

static Boolean UpdateCallList = FALSE;
static TP_CallRecords CallRecords;
static uint8 SelectItemIndex = 0;

static TP_ListBox * listbox = NULL;

static uint8 ShowCallListType = 0;

#define CALLRECORD_FILE "CallRecordFile"


//eèÖÃÊÇ·nÐèÒª¸üÐÂaÐ±i
void TP_JudgeIsNeedUpdateCallList(uint8 CallType)
{
    if(ShowCallListType == CallType)
    {
        UpdateCallList = TRUE;
    }
}


void TP_CallRecordInit()
{
    int32 FileHandle;
    uint32 BufferSize;
    memset(&CallRecords,0,sizeof(TP_CallRecords));

    if(TP_FExist("CallRecordFile")==0)
    {
        //Èç¹ûË÷ÒýÎÄ¼þ²»´æÔu£¬´´½¨
        if(TP_Create(CALLRECORD_FILE, sizeof(TP_CallRecords))==0)
        {

            return;
        }
    }
    FileHandle= TP_Open(CALLRECORD_FILE);
    if (FileHandle == -1)
    {
        return;
    }
    TP_Seek(FileHandle, 0, SEEK_FOREWARD, SEEK_SET);
    BufferSize = TP_Read(FileHandle, (uint8 *)&CallRecords,sizeof(TP_CallRecords));

    if (BufferSize != sizeof(TP_CallRecords))
    {
        memset(&CallRecords,0,sizeof(TP_CallRecords));
    }

    TP_Close(FileHandle);

#if 0
    /*µ÷ÊÔÊ¹oÃ*/
    {
        TP_Callecord   callrecord;
        memset(&callrecord,0,sizeof(TP_Callecord));
        memcpy(callrecord.Phone,"13420742026",strlen((char *)"13420742026"));
        callrecord.StartTime.Date.Year = 2012;
        callrecord.StartTime.Date.Month= 3;
        callrecord.StartTime.Date.Day  = 20;
        callrecord.StartTime.Time.Hours = 10;
        callrecord.StartTime.Time.Minutes = 10;
        callrecord.StartTime.Time.Seconds = 10;

        callrecord.EndTime.Date.Year = 2012;
        callrecord.EndTime.Date.Month= 3;
        callrecord.EndTime.Date.Day  = 20;
        callrecord.EndTime.Time.Hours = 10;
        callrecord.EndTime.Time.Minutes = 10;
        callrecord.EndTime.Time.Seconds = 10;
        callrecord.CallTimes = 5890;


        CallRecords.DialCallCount = 20;
        CallRecords.ReceiveCallCount = 10;
        CallRecords.MissCallCount   =10;

        memcpy(&CallRecords.DialCall[0],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[1],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[2],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[3],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[4],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[5],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[6],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[7],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[8],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[9],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[10],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[11],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[12],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[13],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[14],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[15],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[16],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[17],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[18],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.DialCall[19],&callrecord,sizeof(TP_Callecord));

        memcpy(&CallRecords.ReceiveCall[0],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.ReceiveCall[1],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.ReceiveCall[2],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.ReceiveCall[3],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.ReceiveCall[4],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.ReceiveCall[5],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.ReceiveCall[6],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.ReceiveCall[7],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.ReceiveCall[8],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.ReceiveCall[9],&callrecord,sizeof(TP_Callecord));

        memcpy(&CallRecords.MissCall[0],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.MissCall[1],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.MissCall[2],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.MissCall[3],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.MissCall[4],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.MissCall[5],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.MissCall[6],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.MissCall[7],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.MissCall[8],&callrecord,sizeof(TP_Callecord));
        memcpy(&CallRecords.MissCall[9],&callrecord,sizeof(TP_Callecord));



    }
    /*************/

#endif

}





void  TP_FormatTime(uint8* CallTimes,uint32 times)
{
    uint8 hour,minutes,sec;

    sec = times % 60;

    minutes = (times / 60) % 60;

    hour = times / 3600;

    sprintf((char*)CallTimes,"%02d:%02d:%02d",hour,minutes,sec);

}


void ShowCallInfo(uint8 CallType,uint8 Index)
{
    uint8 CallInfo[256];
    uint8 CallTime[20];
    TP_DisplayInfo displayInfo =
    {
        PNULL,
        ALIGN_MIDDLE,
        (uint8*)TEXT_CALLRECORD_CONFIRM,
        (uint8*)TEXT_CALLRECORD_CANCEL,
        KEY_CALLRECORD_CONFIRM1,
        KEY_CALLRECORD_CANCEL1,
        KEY_CALLRECORD_CONFIRM2,
        KEY_CALLRECORD_CANCEL2,
        CFONT
    };
    memset(CallInfo,0,sizeof(CallInfo));
    memset(CallTime,0,sizeof(CallTime));
    switch (CallType)
    {
        case 0 :
            if (Index >= CallRecords.DialCallCount)
            {
                TP_PopupMessageTimed((uint8*)TEXT_DAILED_CALLS_LOST,3000);
                return;
            }

            TP_FormatTime(CallTime,CallRecords.DialCall[Index].CallTimes);
            if (CallRecords.DialCall[Index].EndTime.Date.Year != 0)
            {
                sprintf((char*)CallInfo,"%s:\n%s\n%s:\n%04d-%02d-%02d %02d:%02d:%02d\n%s:\n%04d-%02d-%02d %02d:%02d:%02d\n%s:\n%s",
                            TEXT_CALLRECORD_PHONE,
                            CallRecords.DialCall[Index].Phone,
                            TEXT_CALLRECORD_STARTTIME,
                            CallRecords.DialCall[Index].StartTime.Date.Year,
                            CallRecords.DialCall[Index].StartTime.Date.Month,
                            CallRecords.DialCall[Index].StartTime.Date.Day,
                            CallRecords.DialCall[Index].StartTime.Time.Hours,
                            CallRecords.DialCall[Index].StartTime.Time.Minutes,
                            CallRecords.DialCall[Index].StartTime.Time.Seconds,
                            TEXT_CALLRECORD_ENDTIME,
                            CallRecords.DialCall[Index].EndTime.Date.Year,
                            CallRecords.DialCall[Index].EndTime.Date.Month,
                            CallRecords.DialCall[Index].EndTime.Date.Day,
                            CallRecords.DialCall[Index].EndTime.Time.Hours,
                            CallRecords.DialCall[Index].EndTime.Time.Minutes,
                            CallRecords.DialCall[Index].EndTime.Time.Seconds,
                            TEXT_CALLRECORD_CALLTIME,
                            CallTime);
            }
            else
            {
                sprintf((char*)CallInfo,"%s:\n%s\n%s:\n%04d-%02d-%02d %02d:%02d:%02d\n%s:\n%s",
                            TEXT_CALLRECORD_PHONE,
                            CallRecords.DialCall[Index].Phone,
                            TEXT_CALLRECORD_STARTTIME,
                            CallRecords.DialCall[Index].StartTime.Date.Year,
                            CallRecords.DialCall[Index].StartTime.Date.Month,
                            CallRecords.DialCall[Index].StartTime.Date.Day,
                            CallRecords.DialCall[Index].StartTime.Time.Hours,
                            CallRecords.DialCall[Index].StartTime.Time.Minutes,
                            CallRecords.DialCall[Index].StartTime.Time.Seconds,
                            TEXT_CALLRECORD_CALLTIME,
                            CallTime);
            }
            break;
        case 1 :
            if (Index >= CallRecords.ReceiveCallCount)
            {
                TP_PopupMessageTimed((uint8*)TEXT_RECEIVED_CALLS_LOST,3000);
                return;
            }
            TP_FormatTime(CallTime,CallRecords.ReceiveCall[Index].CallTimes);
            if (CallRecords.ReceiveCall[Index].EndTime.Date.Year != 0)
            {
                sprintf((char*)CallInfo,"%s:\n%s\n%s:\n%04d-%02d-%02d %02d:%02d:%02d\n%s:\n%04d-%02d-%02d %02d:%02d:%02d\n%s:\n%s",
                            TEXT_CALLRECORD_PHONE,
                            CallRecords.ReceiveCall[Index].Phone,
                            TEXT_CALLRECORD_STARTTIME,
                            CallRecords.ReceiveCall[Index].StartTime.Date.Year,
                            CallRecords.ReceiveCall[Index].StartTime.Date.Month,
                            CallRecords.ReceiveCall[Index].StartTime.Date.Day,
                            CallRecords.ReceiveCall[Index].StartTime.Time.Hours,
                            CallRecords.ReceiveCall[Index].StartTime.Time.Minutes,
                            CallRecords.ReceiveCall[Index].StartTime.Time.Seconds,
                            TEXT_CALLRECORD_ENDTIME,
                            CallRecords.ReceiveCall[Index].EndTime.Date.Year,
                            CallRecords.ReceiveCall[Index].EndTime.Date.Month,
                            CallRecords.ReceiveCall[Index].EndTime.Date.Day,
                            CallRecords.ReceiveCall[Index].EndTime.Time.Hours,
                            CallRecords.ReceiveCall[Index].EndTime.Time.Minutes,
                            CallRecords.ReceiveCall[Index].EndTime.Time.Seconds,
                            TEXT_CALLRECORD_CALLTIME,
                            CallTime);
            }
            else
            {
                sprintf((char*)CallInfo,"%s:\n%s\n%s:\n%04d-%02d-%02d %02d:%02d:%02d\n%s:\n%s",
                            TEXT_CALLRECORD_PHONE,
                            CallRecords.ReceiveCall[Index].Phone,
                            TEXT_CALLRECORD_STARTTIME,
                            CallRecords.ReceiveCall[Index].StartTime.Date.Year,
                            CallRecords.ReceiveCall[Index].StartTime.Date.Month,
                            CallRecords.ReceiveCall[Index].StartTime.Date.Day,
                            CallRecords.ReceiveCall[Index].StartTime.Time.Hours,
                            CallRecords.ReceiveCall[Index].StartTime.Time.Minutes,
                            CallRecords.ReceiveCall[Index].StartTime.Time.Seconds,
                            TEXT_CALLRECORD_CALLTIME,
                            CallTime);
            }
            break;
        case 2 :
            if (Index >= CallRecords.MissCallCount)
            {
                TP_PopupMessageTimed((uint8*)TEXT_MISSED_CALLS_LOST,3000);
                return;
            }
            sprintf((char*)CallInfo,"%s:\n%s\n%s:\n%04d-%02d-%02d %02d:%02d:%02d",
                        TEXT_CALLRECORD_PHONE,
                        CallRecords.MissCall[Index].Phone,
                        TEXT_CALLRECORD_STARTTIME,
                        CallRecords.MissCall[Index].StartTime.Date.Year,
                        CallRecords.MissCall[Index].StartTime.Date.Month,
                        CallRecords.MissCall[Index].StartTime.Date.Day,
                        CallRecords.MissCall[Index].StartTime.Time.Hours,
                        CallRecords.MissCall[Index].StartTime.Time.Minutes,
                        CallRecords.MissCall[Index].StartTime.Time.Seconds
                        );

            break;
    }

    //TP_ShowText(&displayInfo,rect,CFONT,CallInfo);
    TP_ShowTextFullScr(&displayInfo,(uint8*)"%s",CallInfo);
}

void SendSMSByCallRecord(uint8 CallType,uint8 Index)
{
    uint8 Sms[100];
    uint8* Phone;
    memset(Sms,0,sizeof(Sms));
    switch (CallType)
    {
        case 0 :
            if (Index >= CallRecords.DialCallCount)
            {
                TP_PopupMessageTimed((uint8*)TEXT_DAILED_CALLS_LOST,3000);
                return;
            }
            Phone = CallRecords.DialCall[Index].Phone;


            break;
        case 1 :
            if (Index >= CallRecords.ReceiveCallCount)
            {
                TP_PopupMessageTimed((uint8*)TEXT_RECEIVED_CALLS_LOST,3000);
                return;
            }
            Phone = CallRecords.ReceiveCall[Index].Phone;

            break;
        case 2 :
            if (Index >= CallRecords.MissCallCount)
            {
                TP_PopupMessageTimed((uint8*)TEXT_MISSED_CALLS_LOST,3000);
                return;
            }
            Phone = CallRecords.MissCall[Index].Phone;


            break;
    }

    TP_MsgBoxNew(Phone,NULL,0,0);
}

void DialCallByCallRecord(uint8 CallType,uint8 Index)
{
    uint8 Sms[100];
    uint8* Phone;
    memset(Sms,0,sizeof(Sms));
    switch (CallType)
    {
        case 0 :
            if (Index >= CallRecords.DialCallCount)
            {
                TP_PopupMessageTimed((uint8*)TEXT_DAILED_CALLS_LOST,3000);
                return;
            }
            Phone = CallRecords.DialCall[Index].Phone;


            break;
        case 1 :
            if (Index >= CallRecords.ReceiveCallCount)
            {
                TP_PopupMessageTimed((uint8*)TEXT_RECEIVED_CALLS_LOST,3000);
                return;
            }
            Phone = CallRecords.ReceiveCall[Index].Phone;

            break;
        case 2 :
            if (Index >= CallRecords.MissCallCount)
            {
                TP_PopupMessageTimed((uint8*)TEXT_MISSED_CALLS_LOST,3000);
                return;
            }
            Phone = CallRecords.MissCall[Index].Phone;


            break;
    }
    //TP_DialCall(Phone);
    TP_HookUp();
    TP_ShowDialDilog(Phone);
}

void DeleteCallRecord(uint8 CallType,uint8 Index)
{
    int32 FileHandle;
    uint32 BufferSize;
    uint8 col;

    switch (CallType)
    {
        case 0 :
            if (Index >= CallRecords.DialCallCount)
            {
                TP_PopupMessageTimed((uint8*)TEXT_DAILED_CALLS_LOST,3000);
                return;
            }

            for (col = Index;col < CallRecords.DialCallCount-1;col++)
            {
                memcpy(&CallRecords.DialCall[col],&CallRecords.DialCall[col+1],sizeof(TP_Callecord));
            }

            CallRecords.DialCallCount--;

            break;
        case 1 :
            if (Index >= CallRecords.ReceiveCallCount)
            {
                TP_PopupMessageTimed((uint8*)TEXT_RECEIVED_CALLS_LOST,3000);
                return;
            }
            for (col = Index;col < CallRecords.ReceiveCallCount-1;col++)
            {
                memcpy(&CallRecords.ReceiveCall[col],&CallRecords.ReceiveCall[col+1],sizeof(TP_Callecord));
            }

            CallRecords.ReceiveCallCount--;
            break;
        case 2 :
            if (Index >= CallRecords.MissCallCount)
            {
                TP_PopupMessageTimed((uint8*)TEXT_MISSED_CALLS_LOST,3000);
                return;
            }
            for (col = Index;col < CallRecords.MissCallCount-1;col++)
            {
                memcpy(&CallRecords.MissCall[col],&CallRecords.MissCall[col+1],sizeof(TP_Callecord));
            }

            CallRecords.MissCallCount--;
            break;
    }

    if(TP_FExist(CALLRECORD_FILE)==0)
    {
        //Èç¹ûË÷ÒýÎÄ¼þ²»´æÔu£¬´´½¨
        if(TP_Create(CALLRECORD_FILE, sizeof(TP_CallRecords))==0)
        {
            TP_PopupMessageTimed((uint8*)TEXT_DEL_CALL_DATA_FAIL,3000);
            return;
        }
    }
    FileHandle= TP_Open(CALLRECORD_FILE);
    if (FileHandle == -1)
    {
        TP_PopupMessageTimed((uint8*)TEXT_DEL_CALL_DATA_FAIL,3000);
        return;
    }
    TP_Seek(FileHandle, 0, SEEK_FOREWARD, SEEK_SET);
    BufferSize = TP_Write(FileHandle, (uint8*)&CallRecords,sizeof(TP_CallRecords));

    if (BufferSize != sizeof(TP_CallRecords))
    {

    }

    TP_Close(FileHandle);
    TP_PopupMessageTimed((uint8*)TEXT_DEL_CALL_DATA_SUCC,3000);
}

void ClearCallRecord()
{
    int32 FileHandle;
    uint32 BufferSize;

    memset(&CallRecords,0,sizeof(TP_CallRecords));

    if(TP_FExist(CALLRECORD_FILE)==0)
    {
        //Èç¹ûË÷ÒýÎÄ¼þ²»´æÔu£¬´´½¨
        if(TP_Create(CALLRECORD_FILE, sizeof(TP_CallRecords))==0)
        {
            TP_PopupMessageTimed((uint8*)TEXT_CLEAR_CALLDATA_FAIL,3000);
            return;
        }
    }
    FileHandle= TP_Open(CALLRECORD_FILE);
    if (FileHandle == -1)
    {
        TP_PopupMessageTimed((uint8*)TEXT_CLEAR_CALLDATA_FAIL,3000);
        return;
    }
    TP_Seek(FileHandle, 0, SEEK_FOREWARD, SEEK_SET);
    BufferSize = TP_Write(FileHandle, (uint8*)&CallRecords,sizeof(TP_CallRecords));

    if (BufferSize != sizeof(TP_CallRecords))
    {

    }

    TP_Close(FileHandle);
    TP_PopupMessageTimed((uint8*)TEXT_CLEAR_CALLDATA_SUCC,3000);
}

void  ShowOptionDialog(uint8 CallType,uint8 SelectIndex,TP_ListBoxRect optionrect)
{
    TP_ListBox * optionlistbox;
    uint8 Selindex;
    TP_DisplayInfo displayInfo=
    {
        (uint8*)TEXT_DEL_CALL_CONFIRM,
        ALIGN_MIDDLE,
        (uint8*)TEXT_CALLRECORD_CONFIRM,
        (uint8*)TEXT_CALLRECORD_CANCEL,
        KEY_CALLRECORD_CONFIRM1,
        KEY_CALLRECORD_CANCEL1,
        KEY_CALLRECORD_CONFIRM2,
        KEY_CALLRECORD_CANCEL2,
        CFONT
    };

    optionlistbox = TP_NewListBox(TL_LIST, optionrect, NULL, NULL,0);

    if (optionlistbox == NULL)
    {
        TP_PopupMessageTimed((uint8*)TEXT_CREATE_CALL_LIST_FAIL,3000);
        return;
    }
    optionlistbox->ConfirmKey = KEY_CALLRECORD_CONFIRM1;
    optionlistbox->CancelKey = KEY_CALLRECORD_CANCEL1;
    TP_ListBoxAddItem(optionlistbox,(uint8*)TEXT_CALL_OPTION_DETAIL,0,FALSE,NULL);
    TP_ListBoxAddItem(optionlistbox,(uint8*)TEXT_CALL_OPTION_SMS,1,FALSE,NULL);
    TP_ListBoxAddItem(optionlistbox,(uint8*)TEXT_CALL_OPTION_CALL,2,FALSE,NULL);
    TP_ListBoxAddItem(optionlistbox,(uint8*)TEXT_CALL_OPTION_DEL,4,FALSE,NULL);

    if (TP_ShowListBox(optionlistbox)>0)
    {

        if (optionlistbox->SelectItemCount >0)
        {
            Selindex = optionlistbox->SelectItemValue[0];

            switch (Selindex)
            {
                case 0:   ///showDetail
                    ShowCallInfo(CallType,SelectIndex);
                    break;
                case 1:   ///Send SMS
                    SendSMSByCallRecord(CallType,SelectIndex);
                    break;
                case 2:    /// Dial Call
                    DialCallByCallRecord(CallType,SelectIndex);
                    break;
                case 4:   ///Delete
                    if(TP_PopupMessageWithButton(&displayInfo))
                    {
                        DeleteCallRecord(CallType,SelectIndex);
                        TP_ListBoxDeleteItem(listbox,SelectIndex);
                    }
                    TP_ListBoxFree(listbox);
                    listbox = PNULL;
                    break;
            }
        }
    }

    TP_ListBoxFree(optionlistbox);
}

void TP_CallRecord_ShowCount(TP_ListBox *listbox)
{
    uint8 disppos = 0;
    uint8 Index;
    TP_ScrFontSet(ASCII);
    TP_ScrAttrSet(0);
    disppos = 127-6*5-1;
    TP_ScrGotoxyEx(disppos, listbox->DisplayRect.top+3);
    TP_LcdPrintf((uint8*)"%2d/%d",listbox->SelectItemIndex+1,listbox->ItemCount);
    TP_ScrFontSet(CFONT);
    TP_ScrAttrSet(0);
    //TP_DbgSerialPrn("\n TP_CallRecord_ShowCount:%d,%d ",UpdateCallList,ShowCallListType);
    if( UpdateCallList )
    {
        TP_Kbflush();
        TP_SetKey(KEY_CALLRECORD_CANCEL1);
        switch(ShowCallListType)
        {
            case 0:
                if(listbox->SelectItemIndex < (MAX_DIALCALL_COUNT-1))
                {
                    listbox->SelectItemIndex++;
                }
                SelectItemIndex = listbox->ItemCount - listbox->SelectItemIndex ;
                break;
            case 1:
                if(listbox->SelectItemIndex < (MAX_RECEIVECALL_COUNT - 1))
                {
                    listbox->SelectItemIndex++;
                }
                SelectItemIndex = listbox->ItemCount - listbox->SelectItemIndex ;
                break;
            case 2:
                if(listbox->SelectItemIndex < (MAX_MISSCALL_COUNT -1))
                {
                    listbox->SelectItemIndex++;
                }
                SelectItemIndex = listbox->ItemCount - listbox->SelectItemIndex ;
                break;
        }
    }

    #if 0
    if( UpdateCallList )
    {
        UpdateCallList = FALSE;
        switch(ShowCallListType)
        {
            case 0:
                /*
                TP_ListBoxInsertItem(listbox,
                                     0,
                                     CallRecords.DialCall[CallRecords.DialCallCount-1].Phone,
                                     CallRecords.DialCallCount-1,
                                     FALSE,
                                     TPRes_GetImageByID(CallDialedLogo),
                                     TRUE);
                if(listbox->ItemCount > MAX_DIALCALL_COUNT)
                {
                    TP_ListBoxDeleteItem(listbox, listbox->ItemCount-1);
                }
                */
                if(listbox->SelectItemIndex < (MAX_DIALCALL_COUNT-1))
                {
                    listbox->SelectItemIndex++;
                }
                //listbox->SelectItemIndex = CallRecords.DialCallCount-listbox->SelectItemIndex;
                memset(listbox->ItemIconList, 0,sizeof(listbox->ItemIconList));
                memset(listbox->ItemList,     0,sizeof(listbox->ItemList));
                memset(listbox->ItemValueList,0,sizeof(listbox->ItemValueList));
                listbox->ItemCount = 0;
                for (Index=CallRecords.DialCallCount-1;Index>=0;Index--)
                {

                    TP_ListBoxAddItem(listbox,CallRecords.DialCall[Index].Phone,Index,/*(Index==listbox->SelectItemIndex?TRUE:FALSE)*/FALSE,TPRes_GetImageByID(CallDialedLogo));
                    if (Index ==0)
                    {
                        break;
                    }
                }
                break;
            case 1:
                /*
                TP_ListBoxInsertItem(listbox,
                                     0,
                                     CallRecords.ReceiveCall[CallRecords.ReceiveCallCount-1].Phone,
                                     CallRecords.ReceiveCallCount-1,
                                     FALSE,
                                     TPRes_GetImageByID(CallReceivedLogo),
                                     TRUE);

                if(listbox->ItemCount > MAX_RECEIVECALL_COUNT)
                {
                    TP_ListBoxDeleteItem(listbox, listbox->ItemCount-1);
                }
                */
                if(listbox->SelectItemIndex < (MAX_RECEIVECALL_COUNT - 1))
                {
                    listbox->SelectItemIndex++;
                }
                //listbox->SelectItemIndex = CallRecords.ReceiveCallCount-listbox->SelectItemIndex;
                memset(listbox->ItemIconList, 0,sizeof(listbox->ItemIconList));
                memset(listbox->ItemList,     0,sizeof(listbox->ItemList));
                memset(listbox->ItemValueList,0,sizeof(listbox->ItemValueList));
                listbox->ItemCount = 0;
                for (Index=CallRecords.ReceiveCallCount-1;Index>=0;Index--)
                {

                    TP_ListBoxAddItem(listbox,CallRecords.ReceiveCall[Index].Phone,Index,/*(Index==listbox->SelectItemIndex?TRUE:FALSE)*/FALSE,TPRes_GetImageByID(CallReceivedLogo));
                    if (Index ==0)
                    {
                        break;
                    }
                }
                break;
            case 2:
                /*
                TP_ListBoxInsertItem(listbox,
                                     0,
                                     CallRecords.MissCall[CallRecords.MissCallCount-1].Phone,
                                     CallRecords.MissCallCount-1,
                                     FALSE,
                                     TPRes_GetImageByID(CallMissedLogo),
                                     TRUE);

                if(listbox->ItemCount > MAX_MISSCALL_COUNT)
                {
                    TP_ListBoxDeleteItem(listbox, listbox->ItemCount-1);
                }
                */
                if(listbox->SelectItemIndex < (MAX_MISSCALL_COUNT -1))
                {
                    listbox->SelectItemIndex++;
                }
                //listbox->SelectItemIndex = CallRecords.MissCallCount-listbox->SelectItemIndex;
                memset(listbox->ItemIconList, 0,sizeof(listbox->ItemIconList));
                memset(listbox->ItemList,     0,sizeof(listbox->ItemList));
                memset(listbox->ItemValueList,0,sizeof(listbox->ItemValueList));
                listbox->ItemCount = 0;
                for (Index=CallRecords.MissCallCount-1;Index>=0;Index--)
                {

                    TP_ListBoxAddItem(listbox,CallRecords.MissCall[Index].Phone,Index,/*(Index==listbox->SelectItemIndex?TRUE:FALSE)*/FALSE,TPRes_GetImageByID(CallMissedLogo));
                    if (Index ==0)
                    {
                        break;
                    }
                }
                break;
        }
    }
    #endif

}

//return TRUE Õý³£ FALSE³ö´i
Boolean ClearMissedCallCount()
{
    int32 FileHandle;
    uint32 BufferSize;
    if (CallRecords.NewMissCall > 0)
    {
        CallRecords.NewMissCall = 0;

        if(TP_FExist(CALLRECORD_FILE)==0)
        {
            //Èç¹ûË÷ÒýÎÄ¼þ²»´æÔu£¬´´½¨
            if(TP_Create(CALLRECORD_FILE, sizeof(TP_CallRecords))==0)
            {
                //TP_PopupMessageTimed((uint8*)TEXT_NEW_CALL_ERROR,3000);
                return FALSE;
            }
        }
        FileHandle= TP_Open(CALLRECORD_FILE);
        if (FileHandle == -1)
        {
            //TP_PopupMessageTimed((uint8*)TEXT_NEW_CALL_ERROR,3000);
            return FALSE;
        }
        TP_Seek(FileHandle, 0, SEEK_FOREWARD, SEEK_SET);
        BufferSize = TP_Write(FileHandle, (uint8*)&CallRecords,sizeof(TP_CallRecords));

        if (BufferSize != sizeof(TP_CallRecords))
        {

        }

        TP_Close(FileHandle);
    }
    return TRUE;

}

void ShowCallRecordList(uint8 CallType)
{
    int8 Index;
    uint8 SelectIndex=0;
    uint8 fontHeight;

    TP_ListBoxRect rect;
    TP_ListBoxRect optionrect;

    UpdateCallList = FALSE;
REOPEN:
    Index = 0;
    SelectIndex = 0;
    memset(&rect, 0, sizeof(TP_ListBoxRect));
    memset(&optionrect, 0, sizeof(TP_ListBoxRect));

    TP_ScrFontSet(CFONT);
    TP_ScrAttrSet(0);
    fontHeight = TP_GetFontHeight();

    rect.left =0;
    rect.top =0;
    rect.right=127;
    rect.bottom =63;

    optionrect.left =0;
    optionrect.top =63-fontHeight*3-2;
    optionrect.right=60;
    optionrect.bottom =63-fontHeight-1;
    listbox = NULL;

    switch (CallType)
    {
        case 0 :
            SelectIndex = CallRecords.DialCallCount-1;
            break;
        case 1 :
            SelectIndex = CallRecords.ReceiveCallCount-1;
            break;
        case 2 :
            SelectIndex = CallRecords.MissCallCount-1;
            break;
    }
    //UpdateCallList = FALSE;
    ShowCallListType = CallType;
    while (1)
    {
        switch (CallType)
        {
            case 0 :

                if (0 == CallRecords.DialCallCount)
                {
                    TP_PopupMessageTimed((uint8*)TEXT_DAILED_CALLS_EMPTY,3000);
                    return;
                }

                if (listbox == NULL)
                {
                    uint8 Button[LISTBOX_BUTTON_STRING_LENGTH+1] ={0};
                    TP_MenuTitleCat(Button, LISTBOX_BUTTON_STRING_LENGTH, rect.right-rect.left+1, TEXT_CALLRECORD_OPTION, TEXT_CALLRECORD_EXIT, ' ');
                    listbox = TP_NewListBox(TL_ICONLIST, rect, (uint8*)TEXT_DAILED_CALLS,
                                            Button,TP_CallRecord_ShowCount);
                    //listbox = TP_NewListBox(TL_LIST, rect, NULL, NULL);

                    if (listbox == NULL)
                    {
                        TP_PopupMessageTimed((uint8*)TEXT_CREATE_CALL_LIST_FAIL,3000);
                        return;
                    }
                    listbox->ConfirmKey = KEY_CALLRECORD_CONFIRM1;
                    listbox->CancelKey = KEY_CALLRECORD_CANCEL1;
                    listbox->TitleAlign = ALIGN_LEFT;
                    if(UpdateCallList == TRUE)
                    {
                        //SelectIndex = SelectItemIndex;
                    }

                    for (Index=CallRecords.DialCallCount-1;Index>=0;Index--)
                    {

                        TP_ListBoxAddItem(listbox,CallRecords.DialCall[Index].Phone,Index,(SelectIndex == Index),TPRes_GetImageByID(CallDialedLogo));
                        //TP_ListBoxAddItem(listbox,"AAAAAAAAA",Index,(SelectIndex == Index),NULL);
                        if (Index ==0)
                        {
                            break;
                        }
                    }

                }

                if (TP_ShowListBox(listbox) ==0)
                {
                    TP_ListBoxFree(listbox);
                    listbox = PNULL;
                    if(UpdateCallList == TRUE)
                    {
                        UpdateCallList = FALSE;
                        goto REOPEN;
                    }
                    else
                    {
                        return;
                    }
                }



                if (listbox->SelectItemCount >0)
                {

                    SelectIndex = listbox->SelectItemValue[0];
                }


                ShowOptionDialog(CallType,SelectIndex,optionrect);
                break;
            case 1 :
                if (0 == CallRecords.ReceiveCallCount)
                {
                    TP_PopupMessageTimed((uint8*)TEXT_RECEIVED_CALLS_EMPTY,3000);
                    return;
                }

                if (listbox == NULL)
                {
                    uint8 Button[LISTBOX_BUTTON_STRING_LENGTH+1] ={0};
                    TP_MenuTitleCat(Button, LISTBOX_BUTTON_STRING_LENGTH, rect.right-rect.left+1, TEXT_CALLRECORD_OPTION, TEXT_CALLRECORD_EXIT, ' ');
                    listbox = TP_NewListBox(TL_ICONLIST, rect, (uint8*)TEXT_RECEIVED_CALLS, Button,TP_CallRecord_ShowCount);

                    if (listbox == NULL)
                    {
                        TP_PopupMessageTimed((uint8*)TEXT_CREATE_CALL_LIST_FAIL,3000);
                        return;
                    }

                    listbox->ConfirmKey = KEY_CALLRECORD_CONFIRM1;
                    listbox->CancelKey = KEY_CALLRECORD_CANCEL1;
                    listbox->TitleAlign = ALIGN_LEFT;
                    if(UpdateCallList == TRUE)
                    {
                        ///SelectIndex = SelectItemIndex;
                    }
                    for (Index=CallRecords.ReceiveCallCount-1;Index>=0;Index--)
                    {
                        TP_ListBoxAddItem(listbox,CallRecords.ReceiveCall[Index].Phone,Index,(SelectIndex == Index),TPRes_GetImageByID(CallReceivedLogo));
                        if (Index ==0)
                        {
                            break;
                        }
                    }
                }

                if (TP_ShowListBox(listbox) ==0)
                {
                    TP_ListBoxFree(listbox);
                    listbox = PNULL;
                    if(UpdateCallList == TRUE)
                    {
                        UpdateCallList = FALSE;
                        goto REOPEN;
                    }
                    else
                    {
                        return;
                    }
                }

                if (listbox->SelectItemCount >0)
                {
                    SelectIndex = listbox->SelectItemValue[0];
                }


                ShowOptionDialog(CallType,SelectIndex,optionrect);
                break;
            case 2 :
                if (0 == CallRecords.MissCallCount)
                {
                    TP_PopupMessageTimed((uint8*)TEXT_MISSED_CALLS_EMPTY,3000);
                    return;
                }

                if (listbox == NULL)
                {
                    uint8 Button[LISTBOX_BUTTON_STRING_LENGTH+1] ={0};
                    TP_MenuTitleCat(Button, LISTBOX_BUTTON_STRING_LENGTH, rect.right-rect.left+1, TEXT_CALLRECORD_OPTION, TEXT_CALLRECORD_EXIT, ' ');
                    listbox = TP_NewListBox(TL_ICONLIST, rect, (uint8*)TEXT_MISSED_CALLS,
                                            Button,TP_CallRecord_ShowCount);

                    if (listbox == NULL)
                    {
                        TP_PopupMessageTimed((uint8*)TEXT_CREATE_CALL_LIST_FAIL,3000);
                        return;
                    }

                    listbox->ConfirmKey = KEY_CALLRECORD_CONFIRM1;
                    listbox->CancelKey = KEY_CALLRECORD_CANCEL1;
                    listbox->TitleAlign = ALIGN_LEFT;
                    if(UpdateCallList == TRUE)
                    {
                        //SelectIndex = SelectItemIndex;
                    }
                    for (Index=CallRecords.MissCallCount-1;Index>=0;Index--)
                    {
                        TP_ListBoxAddItem(listbox,CallRecords.MissCall[Index].Phone,Index,(SelectIndex == Index),TPRes_GetImageByID(CallMissedLogo));
                        if (Index ==0)
                        {
                            break;
                        }
                    }
                    /*
                    {
                        int32 FileHandle;
                        uint32 BufferSize;
                        if (CallRecords.NewMissCall > 0)
                        {
                            CallRecords.NewMissCall = 0;

                            if(TP_FExist(CALLRECORD_FILE)==0)
                            {
                                //Èç¹ûË÷ÒýÎÄ¼þ²»´æÔu£¬´´½¨
                                if(TP_Create(CALLRECORD_FILE, sizeof(TP_CallRecords))==0)
                                {
                                    TP_PopupMessageTimed((uint8*)TEXT_NEW_CALL_ERROR,3000);
                                    return;
                                }
                            }
                            FileHandle= TP_Open(CALLRECORD_FILE);
                            if (FileHandle == -1)
                            {
                                TP_PopupMessageTimed((uint8*)TEXT_NEW_CALL_ERROR,3000);
                                return;
                            }
                            TP_Seek(FileHandle, 0, SEEK_FOREWARD, SEEK_SET);
                            BufferSize = TP_Write(FileHandle, (uint8*)&CallRecords,sizeof(TP_CallRecords));

                            if (BufferSize != sizeof(TP_CallRecords))
                            {

                            }

                            TP_Close(FileHandle);
                        }
                    }
                    */
                }
                if( ClearMissedCallCount()== FALSE)
                {
                    TP_PopupMessageTimed((uint8*)TEXT_NEW_CALL_ERROR,3000);
                    //return;
                }

                if (TP_ShowListBox(listbox) ==0)
                {
                    TP_ListBoxFree(listbox);
                    listbox = PNULL;
                    if( ClearMissedCallCount()== FALSE)
                    {
                        //TP_PopupMessageTimed((uint8*)TEXT_NEW_CALL_ERROR,3000);
                    }
                    if(UpdateCallList == TRUE)
                    {
                        UpdateCallList = FALSE;
                        goto REOPEN;
                    }
                    else
                    {
                        return;
                    }
                }

                if (listbox->SelectItemCount >0)
                {
                    SelectIndex = listbox->SelectItemValue[0];
                }

                //TP_ListBoxFree(listbox);

                ShowOptionDialog(CallType,SelectIndex,optionrect);

                break;
        }
    }

    if (listbox != NULL)
    {
        TP_ListBoxFree(listbox);
        listbox = PNULL;
    }
}


void NewCallRecordInfo(uint8 CallType,uint8* Phone)
{
    uint8 Index;
    TP_DateTime StartTime;
    int32 FileHandle;
    uint32 BufferSize;
    uint8 phonelen = 0;
    if (CallType >2)
    {
        return;
    }
    phonelen = strlen((char *)Phone);
    if( phonelen > 19 )
    {
        phonelen = 19;
    }
    switch (CallType)
    {
        case 0:
            if (CallRecords.DialCallCount == MAX_DIALCALL_COUNT)
            {
                for (Index=0;Index < CallRecords.DialCallCount -1;Index++)
                {
                    memcpy(&CallRecords.DialCall[Index],&CallRecords.DialCall[Index+1],sizeof(TP_Callecord));
                }
                CallRecords.DialCallCount --;
            }

            memset(&CallRecords.DialCall[CallRecords.DialCallCount],0,sizeof(TP_Callecord));
            memcpy(CallRecords.DialCall[CallRecords.DialCallCount].Phone,Phone,phonelen);
            TP_GetDateTime(&StartTime);
            memcpy(&CallRecords.DialCall[CallRecords.DialCallCount].StartTime,&StartTime,sizeof(TP_DateTime));
            CallRecords.DialCallCount++;
            break;
        case 1 :

            if (CallRecords.ReceiveCallCount == MAX_RECEIVECALL_COUNT)
            {
                for (Index=0;Index < CallRecords.ReceiveCallCount -1;Index++)
                {
                    memcpy(&CallRecords.ReceiveCall[Index],&CallRecords.ReceiveCall[Index+1],sizeof(TP_Callecord));
                }
                CallRecords.ReceiveCallCount--;
            }

            memset(&CallRecords.ReceiveCall[CallRecords.ReceiveCallCount],0,sizeof(TP_Callecord));
            memcpy(CallRecords.ReceiveCall[CallRecords.ReceiveCallCount].Phone,Phone,phonelen);
            TP_GetDateTime(&StartTime);
            memcpy(&CallRecords.ReceiveCall[CallRecords.ReceiveCallCount].StartTime,&StartTime,sizeof(TP_DateTime));
            CallRecords.ReceiveCallCount++;
            break;
        case 2 :
            if(CallRecords.NewMissCall < MAX_MISSCALL_COUNT )
            {
                CallRecords.NewMissCall +=1;
            }
            if (CallRecords.MissCallCount == MAX_MISSCALL_COUNT)
            {
                for (Index=0;Index < CallRecords.MissCallCount -1;Index++)
                {
                    memcpy(&CallRecords.MissCall[Index],&CallRecords.MissCall[Index+1],sizeof(TP_Callecord));
                }
                CallRecords.MissCallCount --;
            }

            memset(&CallRecords.MissCall[CallRecords.MissCallCount],0,sizeof(TP_Callecord));
            memcpy(CallRecords.MissCall[CallRecords.MissCallCount].Phone,Phone,phonelen);
            TP_GetDateTime(&StartTime);
            memcpy(&CallRecords.MissCall[CallRecords.MissCallCount].StartTime,&StartTime,sizeof(TP_DateTime));
            CallRecords.MissCallCount++;
            TP_JudgeIsNeedUpdateCallList(CallType);
            break;
    }

    if(TP_FExist(CALLRECORD_FILE)==0)
    {
        //Èç¹ûË÷ÒýÎÄ¼þ²»´æÔu£¬´´½¨
        if(TP_Create(CALLRECORD_FILE, sizeof(TP_CallRecords))==0)
        {
            TP_PopupMessageTimed((uint8*)TEXT_NEW_CALL_ERROR,3000);
            return;
        }
    }
    FileHandle= TP_Open(CALLRECORD_FILE);
    if (FileHandle == -1)
    {
        TP_PopupMessageTimed((uint8*)TEXT_NEW_CALL_ERROR,3000);
        return;
    }
    TP_Seek(FileHandle, 0, SEEK_FOREWARD, SEEK_SET);
    BufferSize = TP_Write(FileHandle, (uint8*)&CallRecords,sizeof(TP_CallRecords));

    if (BufferSize != sizeof(TP_CallRecords))
    {

    }

    TP_Close(FileHandle);


}

void UpdateCallRecordInfo(uint8 CallType)
{
    uint8 Index;
    TP_DateTime EndTime;
    int32 FileHandle;
    uint32 BufferSize;
    if (CallType >2)
    {
        return;
    }
    switch (CallType)
    {
        case 0:


            //memset(&CallRecords.DialCall[CallRecords.DialCallCount],0,sizeof(TP_Callecord));

            TP_GetDateTime(&EndTime);
            memcpy(&CallRecords.DialCall[CallRecords.DialCallCount-1].EndTime,&EndTime,sizeof(TP_DateTime));
            CallRecords.DialCall[CallRecords.DialCallCount-1].CallTimes = TP_GetCallTime();
            break;
        case 1 :



            //memset(&CallRecords.ReceiveCall[CallRecords.ReceiveCallCount],0,sizeof(TP_Callecord));

            TP_GetDateTime(&EndTime);
            memcpy(&CallRecords.ReceiveCall[CallRecords.ReceiveCallCount-1].EndTime,&EndTime,sizeof(TP_DateTime));
            CallRecords.ReceiveCall[CallRecords.ReceiveCallCount-1].CallTimes = TP_GetCallTime();
            break;
        case 2 :

            //memset(&CallRecords.MissCall[CallRecords.MissCallCount],0,sizeof(TP_Callecord));
            TP_GetDateTime(&EndTime);
            memcpy(&CallRecords.MissCall[CallRecords.MissCallCount-1].EndTime,&EndTime,sizeof(TP_DateTime));

            break;
    }

    if(TP_FExist(CALLRECORD_FILE)==0)
    {
        //Èç¹ûË÷ÒýÎÄ¼þ²»´æÔu£¬´´½¨
        if(TP_Create(CALLRECORD_FILE, sizeof(TP_CallRecords))==0)
        {
            TP_PopupMessageTimed((uint8*)TEXT_NEW_CALL_ERROR,3000);
            return;
        }
    }
    FileHandle= TP_Open(CALLRECORD_FILE);
    if (FileHandle == -1)
    {
        TP_PopupMessageTimed((uint8*)TEXT_NEW_CALL_ERROR,3000);
        return;
    }
    TP_Seek(FileHandle, 0, SEEK_FOREWARD, SEEK_SET);
    BufferSize = TP_Write(FileHandle, (uint8*)&CallRecords,sizeof(TP_CallRecords));

    if (BufferSize != sizeof(TP_CallRecords))
    {

    }

    TP_Close(FileHandle);
    TP_JudgeIsNeedUpdateCallList(CallType);
}

uint8 GetMissCallCount()
{
    return  CallRecords.NewMissCall;
}
void TP_CallRecordMenu_Clear()
{
    int32 FileHandle;
    uint32 BufferSize;
    uint8 *caption[] =
        {
            (uint8 *)TEXT_CLEAR_MISSED_CALLS,
            (uint8 *)TEXT_CLEAR_RECEIVED_CALLS,
            (uint8 *)TEXT_CLEAR_DAILED_CALLS,
            (uint8 *)TEXT_CLEAR_ALL_CALLS
        };
    uint8 ChooseValue;
    TP_DisplayInfo displayInfo=
    {
        (uint8 *)TEXT_CLEAR_ALL_CALLS,
        ALIGN_MIDDLE,
        (uint8 *)TEXT_CALLRECORD_SELECT,
        (uint8 *)TEXT_CALLRECORD_EXIT,
        KEY_CALLRECORD_CONFIRM1,
        KEY_CALLRECORD_CANCEL1,
        KEY_CALLRECORD_CONFIRM2,
        KEY_CALLRECORD_CANCEL2,
        CFONT
    };

    if( TP_CreateListBoxInterface( TL_LIST,
                                   &displayInfo,
                                   caption,
                                   sizeof(caption)/sizeof(uint8 *),
                                   1,
                                   &ChooseValue,
                                   NULL))
    {
        displayInfo.strTitle = (uint8 *)TEXT_CLEAR_CALLS_CONFIRM;
        displayInfo.strLButtonTitle = (uint8 *)TEXT_CALLRECORD_CONFIRM;
        displayInfo.strRButtonTitle = (uint8 *)TEXT_CALLRECORD_CANCEL;
        if(TP_PopupMessageWithButton(&displayInfo)==FALSE)
        {
            return;
        }
        switch(ChooseValue)
        {
            case 1:
                CallRecords.MissCallCount = 0;
                CallRecords.NewMissCall = 0;
                memset(CallRecords.MissCall,0,sizeof(CallRecords.MissCall));
                break;
            case 2:
                CallRecords.ReceiveCallCount = 0;
                memset(CallRecords.ReceiveCall,0,sizeof(CallRecords.ReceiveCall));
                break;
            case 3:
                CallRecords.DialCallCount = 0;
                memset(CallRecords.DialCall,0,sizeof(CallRecords.DialCall));
                break;
            case 4:
                memset(&CallRecords,0,sizeof(TP_CallRecords));
                break;
        }

        if(TP_FExist(CALLRECORD_FILE)==0)
        {
            //Èç¹ûË÷ÒýÎÄ¼þ²»´æÔu£¬´´½¨
            if(TP_Create(CALLRECORD_FILE, sizeof(TP_CallRecords))==0)
            {
                TP_PopupMessageTimed((uint8*)TEXT_CLEAR_CALLRECORD_FAIL,2000);
                return;
            }
        }
        FileHandle= TP_Open(CALLRECORD_FILE);
        if (FileHandle == -1)
        {
            TP_PopupMessageTimed((uint8*)TEXT_CLEAR_CALLRECORD_FAIL,2000);
            return;
        }
        TP_Seek(FileHandle, 0, SEEK_FOREWARD, SEEK_SET);
        BufferSize = TP_Write(FileHandle, (uint8 *)&CallRecords,sizeof(TP_CallRecords));

        if (BufferSize != sizeof(TP_CallRecords))
        {

        }

        TP_Close(FileHandle);
        TP_PopupMessageTimed((uint8*)TEXT_CLEAR_CALLRECORD_SUCC,2000);

        TP_ScrCls();

    }

}
void TP_IdleScreenShowMissCallIcon()
{
    static Boolean FirstRun = FALSE;
    if(FirstRun == FALSE)
    {
        FirstRun = TRUE;
        return;
    }
    if( GetMissCallCount()>0 )
    {
        TP_ShowTextSingleLine(50, 52, 127, 63,CFONT,ALIGN_RIGHT,(uint8 *)"%d%s",GetMissCallCount(),TEXT_MISSED);
        /*
        if(GetMissCallCount()>9)
        {
            TP_ShowTextSingleLine(44, 52, 127, 63,CFONT,ALIGN_RIGHT,"%d%s",GetMissCallCount(),TEXT_MISSED);
        }
        else
        {
            TP_ShowTextSingleLine(50, 52, 127, 63,CFONT,ALIGN_RIGHT,"%d%s",GetMissCallCount(),TEXT_MISSED);
        }
        */
    }
}
