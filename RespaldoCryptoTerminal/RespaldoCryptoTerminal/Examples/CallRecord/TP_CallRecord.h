#ifndef TP_CALLRECORD_H
#define TP_CALLRECORD_H
#ifdef __cplusplus
extern "C" {
#endif

//#define CALLRECORD_CHINESE          //��ʾ����ѡ�񣬲�ѡΪӢ��

/*�ַ�������*/
#ifdef CALLRECORD_CHINESE
    #define TEXT_CALLRECORD_CONFIRM         "ȷ��"
    #define TEXT_CALLRECORD_CANCEL          "ȡ��"
    #define TEXT_CALLRECORD_SELECT          "ѡ��"
    #define TEXT_CALLRECORD_EXIT            "�˳�"
    #define TEXT_CALLRECORD_OPTION          "ѡ��"
    #define TEXT_CALLRECORD_PHONE           "�绰"
    #define TEXT_CALLRECORD_STARTTIME       "��ʼʱ��"
    #define TEXT_CALLRECORD_ENDTIME         "����ʱ��"
    #define TEXT_CALLRECORD_CALLTIME        "ͨ��ʱ��"

    #define TEXT_DEL_CALL_DATA_FAIL         "ɾ��ͨ������       \rʧ�� "
    #define TEXT_DEL_CALL_DATA_SUCC         "ɾ��ͨ������       \r�ɹ� "

    #define TEXT_CALLRECORD                 "ͨ����¼"
    #define TEXT_MISSED_CALLS               "δ������"
    #define TEXT_MISSED                     "δ��"
    #define TEXT_RECEIVED_CALLS             "�ѽ�����"
    #define TEXT_DAILED_CALLS               "�Ѳ��绰"
    #define TEXT_CLEAR_ALL_CALLS            "�������"

    #define TEXT_MISSED_CALLS_EMPTY         "δ������Ϊ��"
    #define TEXT_MISSED_CALLS_LOST          "δ���������ݶ�ʧ "
    #define TEXT_RECEIVED_CALLS_EMPTY       "�ѽ�����Ϊ��"
    #define TEXT_RECEIVED_CALLS_LOST        "�ѽ��������ݶ�ʧ "
    #define TEXT_DAILED_CALLS_EMPTY         "�Ѳ��绰Ϊ��"
    #define TEXT_DAILED_CALLS_LOST          "�Ѳ��绰���ݶ�ʧ "
    #define TEXT_CLEAR_MISSED_CALLS         "���δ������ "
    #define TEXT_CLEAR_RECEIVED_CALLS       "����ѽ����� "
    #define TEXT_CLEAR_DAILED_CALLS         "����Ѳ��绰 "

    #define TEXT_CLEAR_CALLS_CONFIRM        "       ȷ��?     "
    #define TEXT_DEL_CALL_CONFIRM           "     ɾ����¼?    "
    #define TEXT_CLEAR_CALLDATA_FAIL        "���ͨ������     \rʧ�� "
    #define TEXT_CLEAR_CALLDATA_SUCC        "���ͨ������     \r�ɹ� "
    #define TEXT_CLEAR_CALLRECORD_FAIL      "  ���ʧ��!"
    #define TEXT_CLEAR_CALLRECORD_SUCC      "  ����ɹ�!"
    #define TEXT_CREATE_CALL_LIST_FAIL      "�����б���� "
    #define TEXT_NEW_CALL_ERROR             "��ͨ����¼ \r���� "
    #define TEXT_CALL_OPTION_DETAIL         "��ϸ"
    #define TEXT_CALL_OPTION_SMS            "����"
    #define TEXT_CALL_OPTION_CALL           "����"
    #define TEXT_CALL_OPTION_DEL            "ɾ��"
#else
    #define TEXT_CALLRECORD_CONFIRM         "Confirm"
    #define TEXT_CALLRECORD_CANCEL          "Cancel"
    #define TEXT_CALLRECORD_SELECT          "Select"
    #define TEXT_CALLRECORD_EXIT            "Exit"
    #define TEXT_CALLRECORD_OPTION          "Options"
    #define TEXT_CALLRECORD_PHONE           "Phone"
    #define TEXT_CALLRECORD_STARTTIME       "Start time"
    #define TEXT_CALLRECORD_ENDTIME         "End time"
    #define TEXT_CALLRECORD_CALLTIME        "Call time"

    #define TEXT_DEL_CALL_DATA_FAIL         "Delete Call Data \rFail "
    #define TEXT_DEL_CALL_DATA_SUCC         "Delete Call Data \rSuccess "

    #define TEXT_CALLRECORD                 "Call Record"
    #define TEXT_MISSED_CALLS               "Missed Calls"
    #define TEXT_MISSED                     "Missed"
    #define TEXT_RECEIVED_CALLS             "Received Calls"
    #define TEXT_DAILED_CALLS               "Dialed Calls"
    #define TEXT_CLEAR_ALL_CALLS            "Clear All"

    #define TEXT_MISSED_CALLS_EMPTY         "Missed Calls Empty"
    #define TEXT_MISSED_CALLS_LOST          "Missed Calls Data Lost "
    #define TEXT_RECEIVED_CALLS_EMPTY       "Received Calls Empty"
    #define TEXT_RECEIVED_CALLS_LOST        "Received Calls Data Lost "
    #define TEXT_DAILED_CALLS_EMPTY         "Dailed Calls Empty"
    #define TEXT_DAILED_CALLS_LOST          "Dialed Calls Data Lost "
    #define TEXT_CLEAR_MISSED_CALLS         "Clear Missed Calls"
    #define TEXT_CLEAR_RECEIVED_CALLS       "Clear Received Calls"
    #define TEXT_CLEAR_DAILED_CALLS         "Clear Dailed Calls"

    #define TEXT_CLEAR_CALLS_CONFIRM        "    Are you sure?    "
    #define TEXT_DEL_CALL_CONFIRM           "Delete the record?"
    #define TEXT_CLEAR_CALLDATA_FAIL        "Clear Call Data \rFail "
    #define TEXT_CLEAR_CALLDATA_SUCC        "Clear Call Data \rSuccess "
    #define TEXT_CLEAR_CALLRECORD_FAIL      "  Clear Fail!"
    #define TEXT_CLEAR_CALLRECORD_SUCC      "  Clear Sucess!"
    #define TEXT_CREATE_CALL_LIST_FAIL      "Create List Error "
    #define TEXT_NEW_CALL_ERROR             "New Call Record \rError "
    #define TEXT_CALL_OPTION_DETAIL         "Detail"
    #define TEXT_CALL_OPTION_SMS            "SMS"
    #define TEXT_CALL_OPTION_CALL           "Call"
    #define TEXT_CALL_OPTION_DEL            "Delete"

#endif


/*��������*/
#ifdef TPS300
#define KEY_CALLRECORD_CONFIRM1     TP_KEY_MENU
#else
#define KEY_CALLRECORD_CONFIRM1     TP_KEY_CONFIRM
#endif

#ifdef TP_PUBLIC_G
#define KEY_CALLRECORD_CONFIRM2     TP_KEY_NONE
#else
#define KEY_CALLRECORD_CONFIRM2     TP_KEY_OK
#endif

#define KEY_CALLRECORD_CANCEL1      TP_KEY_CANCEL
#define KEY_CALLRECORD_CANCEL2      TP_KEY_NONE


void TP_JudgeIsNeedUpdateCallList(uint8 CallType);
void TP_CallRecordInit(void);
void ShowCallRecordList(uint8 CallType);
void NewCallRecordInfo(uint8 CallType,uint8* Phone);
void UpdateCallRecordInfo(uint8 CallType);
uint8 GetMissCallCount(void);
void TP_CallRecordMenu_Clear();
void TP_IdleScreenShowMissCallIcon(void);


#ifdef __cplusplus
}
#endif

#endif
