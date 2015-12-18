/**
 *
 * @file 短信 信箱处理
 * @author   何文东
 * @date    2012-2-11
 * @brief  SMS 信箱模块
 * @version V1.0.1
 * @note
 */


#ifndef TP_MSGMENU_H
#define TP_MSGMENU_H

#ifdef __cplusplus
extern "C" {
#endif


//#define MSGMENU_USE_RESOURCEMM    //是否使用资源文件，短信图标显示用到
#ifndef TPS300
#define MSGMENU_MSG_PRINT         //是否开启短信打印功能
#endif
//#define MSGMENU_CHINESE           //提示语言选择，不选为英文

#define MSG_SIM800


#ifdef MSGMENU_CHINESE
    #define TEXT_MESSAGE        "短信"
    #define TEXT_INBOX          "收件箱"
    #define TEXT_OUTBOX         "发件箱"
    #define TEXT_SENTBOX        "已发信息"
    #define TEXT_DRAFTBOX       "草稿箱"
    #define TEXT_ENTER_NUMBER   "输入号码:"
    #define TEXT_CONFIRM        "确定"
    #define TEXT_CANCEL         "取消"
    #define TEXT_BACK           "返回"
    #define TEXT_EXIT           "退出"
    #define TEXT_OPTIONS        "选项"
    #define TEXT_SELECT_BACK    "选择             返回"
    #define TEXT_SELECT_EXIT    "选择             退出"
    #define TEXT_READ           "阅读"
    #define TEXT_CLEAE_ALL      "清空所有"
    #define TEXT_CLEAR_INBOX    "清空收件箱"
    #define TEXT_CLEAR_OUTBOX   "清空发件箱"
    #define TEXT_CLEAR_SENTBOX  "清空已发信息"
    #define TEXT_CLEAE_DRAFTBOX "清空草稿箱"
    #define TEXT_ARE_YOU_SURE   "      你确定吗?    "
    #define TEXT_CLEAR_SUCESS   "  清除成功!"
    #define TEXT_CLEAE_FAIL     "  清除失败!"
    #define TEXT_SENDING        "   发送中... "
    #define TEXT_SEND_SUCESS    "  发送成功! "
    #define TEXT_SEND_FAIL      "  发送失败!  "
    #define TEXT_SEND_TIMEOUT   "  发送超时! "
    #define TEXT_NO_PAPER       "  缺纸!"
    #define TEXT_TEMPER_TOO_HIG " 打印头温度过高!"
    #define TEXT_NOT_SIMCARD    "   没插SIM卡! "
    #define TEXT_SIM_INVALID    "   SIM卡无效 "
    #define TEXT_OPEN_WAIT      "   打开中....\n    请稍候 "
    #define TEXT_NOT_READY_WAIT "  SIM尚未准备好\n      请稍候 "
    #define TEXT_OPEN_TIMEOUT   "  打开超时 !"
    #define TEXT_NO_SMS         " 没有短信!!"
    #define TEXT_OPEN_FAIL      "   读取失败!"
    #define TEXT_SIM_NOT_READY  "  SIM未准备好! "
    #define TEXT_SAVE_FAIL      "  保存失败! "
    #define TEXT_MSG_CENTRE     "短信中心:"
    #define TEXT_MSG_CENTRE2    "短信中心"
    #define TEXT_PHONE_NUMBER   "手机号码:"
    #define TEXT_CONTENT        "短信内容:"
    #define TEXT_DATETIME       "日期时间:"
    #define TEXT_CALL           "呼叫"
    #define TEXT_DELETE         "删除"
    #define TEXT_SEND           "发送"
    #define TEXT_SAVE           "保存"
    #define TEXT_SAVE_SEND      "保存并发送"
    #define TEXT_TEXT_REPLY     "文本回复"
    #define TEXT_YES            "确定"
    #define TEXT_NO             "取消"
    #define TEXT_NEW_SMS        "新信息"
    #define TEXT_MSG_SETTINGS   "短信设置"
    #define TEXT_SAVE_SMS_TO    "短信保存"
    #define TEXT_SIMCARD        "SIM卡"
    #define TEXT_PHONE_MEMORY   "手机内存"
    #define TEXT_FORWARD        "转发"
    #define TEXT_SMS_OVERWRITE  "短信保存覆盖"
    #define TEXT_ON             "开启"
    #define TEXT_OFF            "关闭"

#else
    #define TEXT_MESSAGE        "Message"
    #define TEXT_INBOX          "InBox"
    #define TEXT_OUTBOX         "OutBox"
    #define TEXT_SENTBOX        "SentBox"
    #define TEXT_DRAFTBOX       "DraftBox"
    #define TEXT_ENTER_NUMBER   "Enter Number:"
    #define TEXT_CONFIRM        "Confirm"
    #define TEXT_CANCEL         "Cancel"
    #define TEXT_BACK           "Back"
    #define TEXT_EXIT           "Exit"
    #define TEXT_OPTIONS        "Options"
    #define TEXT_SELECT_BACK    "Select          Back"
    #define TEXT_SELECT_EXIT    "Select          Exit"
    #define TEXT_READ           "Read"
    #define TEXT_CLEAE_ALL      "Clear All"
    #define TEXT_CLEAR_INBOX    "Clear InBox"
    #define TEXT_CLEAR_OUTBOX   "Clear OutBox"
    #define TEXT_CLEAR_SENTBOX  "Clear SentBox"
    #define TEXT_CLEAE_DRAFTBOX "Clear DraftBox"
    #define TEXT_ARE_YOU_SURE   "    Are you sure?    "
    #define TEXT_CLEAR_SUCESS   "  Clear Sucess!"
    #define TEXT_CLEAE_FAIL     "  Clear Fail!"
    #define TEXT_SENDING        "   Sending... "
    #define TEXT_SEND_SUCESS    " Send Success!"
    #define TEXT_SEND_FAIL      "  Send Fail!  "
    #define TEXT_SEND_TIMEOUT   "TimeOut to send! "
    #define TEXT_NO_PAPER       " No Paper!"
    #define TEXT_TEMPER_TOO_HIG " Temperature too high!"
    #define TEXT_NOT_SIMCARD    "   No SIM Card "
    #define TEXT_SIM_INVALID    "   SIM Card Invalid "
    #define TEXT_OPEN_WAIT      "   Opening....\n  Please Wait "
    #define TEXT_NOT_READY_WAIT "  SIM Not Ready\n   Please Wait "
    #define TEXT_OPEN_TIMEOUT   " Open Timeout !"
    #define TEXT_NO_SMS         " No SMS !!"
    #define TEXT_OPEN_FAIL      "    Read Fail"
    #define TEXT_SIM_NOT_READY  "  SIM Not Ready! "
    #define TEXT_SAVE_FAIL      "  Save Fail! "
    #define TEXT_MSG_CENTRE     "MSG Centre:"
    #define TEXT_MSG_CENTRE2    "MSG Centre"
    #define TEXT_PHONE_NUMBER   "Phone Num:"
    #define TEXT_CONTENT        "Content:"
    #define TEXT_DATETIME       "Date Time:"
    #define TEXT_CALL           "Call"
    #define TEXT_DELETE         "Delete"
    #define TEXT_SEND           "Send"
    #define TEXT_SAVE           "Save"
    #define TEXT_SAVE_SEND      "Save and Send"
    #define TEXT_TEXT_REPLY     "Text reply"
    #define TEXT_YES            "YES"
    #define TEXT_NO             "NO"
    #define TEXT_NEW_SMS        "New"
    #define TEXT_MSG_SETTINGS   "Msg Settings"
    #define TEXT_SAVE_SMS_TO    "Save SMS to"
    #define TEXT_SIMCARD        "SIM Card"
    #define TEXT_PHONE_MEMORY   "Phone memory"
    #define TEXT_FORWARD        "Forward"
    #define TEXT_SMS_OVERWRITE  "SMS Overwrite?"
    #define TEXT_ON             "ON"
    #define TEXT_OFF            "OFF"
#endif
#define MAX_SMS_PREVIEW_LEN 12

/*按键定义*/
#ifdef TPS300
#define KEY_SMS_CONFIRM1     TP_KEY_MENU
#else
#define KEY_SMS_CONFIRM1     TP_KEY_CONFIRM
#endif

#ifdef TP_PUBLIC_G
#define KEY_SMS_CONFIRM2     TP_KEY_NONE
#else
#define KEY_SMS_CONFIRM2     TP_KEY_OK
#endif

#define KEY_SMS_CANCEL1      TP_KEY_CANCEL
#define KEY_SMS_CANCEL2      TP_KEY_NONE


/**
* 用于在待机界面更新短信图标显示(常亮表示有新短信，闪烁表示短信箱将满)
* @author 何文东
* @param 无
* @return 无
* @since 2012年7月3日
*/
void TP_IdleScreenShowMsgIcon(void);


/**
* 新短信，传入相关参数即可用于回复或者转发使用，若参数为0，则为新建短信
* @author 何文东
* @param *Phone_p 电话号码指针
         *Sms_p 短信内容指针
         SmsLen 短信内容长度
         ShowOption 是否显示选项菜单(保存，发送，保存和发送)
* @return 1 发送成功 0 发送失败 2 没有发送退出
* @since 2012年2月12日
*/
uint8 TP_MsgBoxNew( uint8 *Phone_p,
                              uint8 *Sms_p,
                              uint16 SmsLen,
                              uint8 ShowOption);


/**
* 短信信箱操作
* @author 何文东
* @param Box 信箱类型
* @return 无
* @since 2012年2月12日
*/
void TP_MsgBoxShow(TP_SMSBox Box);

/**
* 清除短信
* @author 何文东
* @param 无
* @return 无
* @since 2012年4月17日
*/
void TP_MsgMenu_Clear(void);

/**
* 短信中心设置菜单处理
* @author 何文东
* @param 无
* @return 无
* @since 2012年2月12日
*/
void TP_MsgMenu_MSGCentreSet(void);


/**
* 短信保存位置菜单设置
* @author 何文东
* @param 无
* @return 无
* @since 2012年2月12日
*/
void TP_MsgMenu_SaveTo(void);


/**
* 根据短信保存位置和信箱类型列出短信列表
* @author 何文东
* @param Box 信箱类型<BR>
         *SmsIndex 短信索引号保存指针<BR>
         *SmsCount 短信条数
* @return TP_SMSDOResult
* @since 2012年2月12日
*/
TP_SMSDOResult TP_ListMsgIndex(TP_SMSBox Box,
                                      uint16 *SmsIndex,
                                      uint16 MaxSmsCount,
                                      uint16 *SmsCount);



/**
* 根据短信保存位置预览短信
* @author 何文东
* @param Box 信箱类型<BR>
         index 索引号<BR>
         *Phone 号码指针<BR>
         *Sms 短信内容指针<BR>
         PreviewLength 预览短信长度<BR>
         *SmsState 短信状态<BR>
         *DateTime 日期时间
* @return TP_SMSDOResult
* @since 2012年2月12日
*/
TP_SMSDOResult TP_PreviewMsg(TP_SMSBox Box,
                                  uint16 index,
                                  uint8 *Phone,
                                  uint8 *Sms,
                                  uint16 PreviewLength,
                                  TP_SMSRecordState *SmsState,
                                  TP_DateTime *DateTime);


/**
* 根据短信保存位置读取短信
* @author 何文东
* @param Box 信箱类型<BR>
         index 索引号<BR>
         *Phone 号码指针<BR>
         *Sms 短信内容指针<BR>
         *SmsLength 短信长度<BR>
         *SmsState 短信状态<BR>
         *DateTime 日期时间
* @return TP_SMSDOResult
* @since 2012年2月12日
*/
TP_SMSDOResult TP_ReadMsg(TP_SMSBox Box,
                              uint16 index,
                              uint8 *Phone,
                              uint8 *Sms,
                              uint16 *SmsLength,
                              TP_SMSRecordState *SmsState,
                              TP_DateTime *DateTime);


/**
* 根据短信保存位置删除短信
* @author 何文东
* @param idnex 索引号<BR>
* @return TP_SMSDOResult
* @since 2012年2月12日
*/
TP_SMSDOResult TP_DelMsg(TP_SMSBox Box,uint16 index);

/**
* 删除信箱的全部短信
* @author 何文东
* @param Box 要删除的信箱<BR>
* @return TP_SMSDOResult
* @since 2012年4月17日
* @note: 如果删除过程中出错，则退出
*/
TP_SMSDOResult TP_DelBoxMsg(TP_SMSBox Box);

/**
* 删除所有短信
* @author 何文东
* @return TP_SMSDOResult
* @since 2012年4月17日
* @note: 如果删除过程中出错，则退出
*/
TP_SMSDOResult TP_DelAllMsg(void);

/**
* 根据短信保存位置保存短信
* @author 何文东
* @param Box 信箱类型<BR>
         *StoreIndex 保存索引号<BR>
         *Phone 号码指针<BR>
         *Sms 短信内容指针<BR>
         *SmsLength 短信长度，预览时为预览的短信内容长度<BR>
*         SmsState 短信状态<BR>
*         DateTime 日期时间
* @return TP_SMSDOResult
* @since 2012年2月12日
*/
TP_SMSDOResult TP_StoreMsg(TP_SMSBox Box,
                                  uint16 *StoreIndex,
                                  uint8 *Phone,
                                  uint8 *Sms,
                                  uint16 SmsLength,
                                  TP_SMSRecordState SmsSate,
                                  TP_DateTime *DateTime);


/**
* 根据短信保存位置保存短信(如果存储已满，则删除最旧的收件箱短信)
* @author 何文东
* @param Box 信箱类型<BR>
         *StoreIndex 保存索引号<BR>
         *Phone 号码指针<BR>
         *Sms 短信内容指针<BR>
         *SmsLength 短信长度，预览时为预览的短信内容长度<BR>
*         SmsState 短信状态<BR>
*         DateTime 日期时间
* @return TP_SMSDOResult
* @since 2012年2月12日
*/
TP_SMSDOResult TP_StoreOverMsg(TP_SMSBox Box,
                                      uint16 *StoreIndex,
                                      uint8 *Phone,
                                      uint8 *Sms,
                                      uint16 SmsLength,
                                      TP_SMSRecordState SmsSate,
                                      TP_DateTime *DateTime);


uint16 TP_GetMsgAllCount(TP_SMSBox Box);
uint16 TP_GetMsgCount(TP_SMSBox Box);
Boolean TP_IsSMSFull(uint16 left);
uint16 TP_GetUnReadMsgCount(void);
uint8 TP_GetSMSSaveOn(void);




#ifdef __cplusplus
}
#endif

#endif//TP_MSGMENU_H



