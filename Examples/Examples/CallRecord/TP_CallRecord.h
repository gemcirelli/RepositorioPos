#ifndef TP_CALLRECORD_H
#define TP_CALLRECORD_H
#ifdef __cplusplus
extern "C" {
#endif

//#define CALLRECORD_CHINESE          //提示语言选择，不选为英文

/*字符串定义*/
#ifdef CALLRECORD_CHINESE
    #define TEXT_CALLRECORD_CONFIRM         "确定"
    #define TEXT_CALLRECORD_CANCEL          "取消"
    #define TEXT_CALLRECORD_SELECT          "选择"
    #define TEXT_CALLRECORD_EXIT            "退出"
    #define TEXT_CALLRECORD_OPTION          "选项"
    #define TEXT_CALLRECORD_PHONE           "电话"
    #define TEXT_CALLRECORD_STARTTIME       "开始时间"
    #define TEXT_CALLRECORD_ENDTIME         "结束时间"
    #define TEXT_CALLRECORD_CALLTIME        "通话时间"

    #define TEXT_DEL_CALL_DATA_FAIL         "删除通话数据       \r失败 "
    #define TEXT_DEL_CALL_DATA_SUCC         "删除通话数据       \r成功 "

    #define TEXT_CALLRECORD                 "通话记录"
    #define TEXT_MISSED_CALLS               "未接来电"
    #define TEXT_MISSED                     "未接"
    #define TEXT_RECEIVED_CALLS             "已接来电"
    #define TEXT_DAILED_CALLS               "已拨电话"
    #define TEXT_CLEAR_ALL_CALLS            "清除所有"

    #define TEXT_MISSED_CALLS_EMPTY         "未接来电为空"
    #define TEXT_MISSED_CALLS_LOST          "未接来电数据丢失 "
    #define TEXT_RECEIVED_CALLS_EMPTY       "已接来电为空"
    #define TEXT_RECEIVED_CALLS_LOST        "已接来电数据丢失 "
    #define TEXT_DAILED_CALLS_EMPTY         "已拨电话为空"
    #define TEXT_DAILED_CALLS_LOST          "已拨电话数据丢失 "
    #define TEXT_CLEAR_MISSED_CALLS         "清除未接来电 "
    #define TEXT_CLEAR_RECEIVED_CALLS       "清除已接来电 "
    #define TEXT_CLEAR_DAILED_CALLS         "清除已拨电话 "

    #define TEXT_CLEAR_CALLS_CONFIRM        "       确定?     "
    #define TEXT_DEL_CALL_CONFIRM           "     删除记录?    "
    #define TEXT_CLEAR_CALLDATA_FAIL        "清除通话数据     \r失败 "
    #define TEXT_CLEAR_CALLDATA_SUCC        "清除通话数据     \r成功 "
    #define TEXT_CLEAR_CALLRECORD_FAIL      "  清除失败!"
    #define TEXT_CLEAR_CALLRECORD_SUCC      "  清除成功!"
    #define TEXT_CREATE_CALL_LIST_FAIL      "创建列表出错 "
    #define TEXT_NEW_CALL_ERROR             "新通话记录 \r错误 "
    #define TEXT_CALL_OPTION_DETAIL         "详细"
    #define TEXT_CALL_OPTION_SMS            "短信"
    #define TEXT_CALL_OPTION_CALL           "拨打"
    #define TEXT_CALL_OPTION_DEL            "删除"
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


/*按键定义*/
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
