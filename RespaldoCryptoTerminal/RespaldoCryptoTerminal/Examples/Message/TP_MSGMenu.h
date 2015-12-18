/**
 *
 * @file ���� ���䴦��
 * @author   ���Ķ�
 * @date    2012-2-11
 * @brief  SMS ����ģ��
 * @version V1.0.1
 * @note
 */


#ifndef TP_MSGMENU_H
#define TP_MSGMENU_H

#ifdef __cplusplus
extern "C" {
#endif


//#define MSGMENU_USE_RESOURCEMM    //�Ƿ�ʹ����Դ�ļ�������ͼ����ʾ�õ�
#ifndef TPS300
#define MSGMENU_MSG_PRINT         //�Ƿ������Ŵ�ӡ����
#endif
//#define MSGMENU_CHINESE           //��ʾ����ѡ�񣬲�ѡΪӢ��

#define MSG_SIM800


#ifdef MSGMENU_CHINESE
    #define TEXT_MESSAGE        "����"
    #define TEXT_INBOX          "�ռ���"
    #define TEXT_OUTBOX         "������"
    #define TEXT_SENTBOX        "�ѷ���Ϣ"
    #define TEXT_DRAFTBOX       "�ݸ���"
    #define TEXT_ENTER_NUMBER   "�������:"
    #define TEXT_CONFIRM        "ȷ��"
    #define TEXT_CANCEL         "ȡ��"
    #define TEXT_BACK           "����"
    #define TEXT_EXIT           "�˳�"
    #define TEXT_OPTIONS        "ѡ��"
    #define TEXT_SELECT_BACK    "ѡ��             ����"
    #define TEXT_SELECT_EXIT    "ѡ��             �˳�"
    #define TEXT_READ           "�Ķ�"
    #define TEXT_CLEAE_ALL      "�������"
    #define TEXT_CLEAR_INBOX    "����ռ���"
    #define TEXT_CLEAR_OUTBOX   "��շ�����"
    #define TEXT_CLEAR_SENTBOX  "����ѷ���Ϣ"
    #define TEXT_CLEAE_DRAFTBOX "��ղݸ���"
    #define TEXT_ARE_YOU_SURE   "      ��ȷ����?    "
    #define TEXT_CLEAR_SUCESS   "  ����ɹ�!"
    #define TEXT_CLEAE_FAIL     "  ���ʧ��!"
    #define TEXT_SENDING        "   ������... "
    #define TEXT_SEND_SUCESS    "  ���ͳɹ�! "
    #define TEXT_SEND_FAIL      "  ����ʧ��!  "
    #define TEXT_SEND_TIMEOUT   "  ���ͳ�ʱ! "
    #define TEXT_NO_PAPER       "  ȱֽ!"
    #define TEXT_TEMPER_TOO_HIG " ��ӡͷ�¶ȹ���!"
    #define TEXT_NOT_SIMCARD    "   û��SIM��! "
    #define TEXT_SIM_INVALID    "   SIM����Ч "
    #define TEXT_OPEN_WAIT      "   ����....\n    ���Ժ� "
    #define TEXT_NOT_READY_WAIT "  SIM��δ׼����\n      ���Ժ� "
    #define TEXT_OPEN_TIMEOUT   "  �򿪳�ʱ !"
    #define TEXT_NO_SMS         " û�ж���!!"
    #define TEXT_OPEN_FAIL      "   ��ȡʧ��!"
    #define TEXT_SIM_NOT_READY  "  SIMδ׼����! "
    #define TEXT_SAVE_FAIL      "  ����ʧ��! "
    #define TEXT_MSG_CENTRE     "��������:"
    #define TEXT_MSG_CENTRE2    "��������"
    #define TEXT_PHONE_NUMBER   "�ֻ�����:"
    #define TEXT_CONTENT        "��������:"
    #define TEXT_DATETIME       "����ʱ��:"
    #define TEXT_CALL           "����"
    #define TEXT_DELETE         "ɾ��"
    #define TEXT_SEND           "����"
    #define TEXT_SAVE           "����"
    #define TEXT_SAVE_SEND      "���沢����"
    #define TEXT_TEXT_REPLY     "�ı��ظ�"
    #define TEXT_YES            "ȷ��"
    #define TEXT_NO             "ȡ��"
    #define TEXT_NEW_SMS        "����Ϣ"
    #define TEXT_MSG_SETTINGS   "��������"
    #define TEXT_SAVE_SMS_TO    "���ű���"
    #define TEXT_SIMCARD        "SIM��"
    #define TEXT_PHONE_MEMORY   "�ֻ��ڴ�"
    #define TEXT_FORWARD        "ת��"
    #define TEXT_SMS_OVERWRITE  "���ű��渲��"
    #define TEXT_ON             "����"
    #define TEXT_OFF            "�ر�"

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

/*��������*/
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
* �����ڴ���������¶���ͼ����ʾ(������ʾ���¶��ţ���˸��ʾ�����佫��)
* @author ���Ķ�
* @param ��
* @return ��
* @since 2012��7��3��
*/
void TP_IdleScreenShowMsgIcon(void);


/**
* �¶��ţ�������ز����������ڻظ�����ת��ʹ�ã�������Ϊ0����Ϊ�½�����
* @author ���Ķ�
* @param *Phone_p �绰����ָ��
         *Sms_p ��������ָ��
         SmsLen �������ݳ���
         ShowOption �Ƿ���ʾѡ��˵�(���棬���ͣ�����ͷ���)
* @return 1 ���ͳɹ� 0 ����ʧ�� 2 û�з����˳�
* @since 2012��2��12��
*/
uint8 TP_MsgBoxNew( uint8 *Phone_p,
                              uint8 *Sms_p,
                              uint16 SmsLen,
                              uint8 ShowOption);


/**
* �����������
* @author ���Ķ�
* @param Box ��������
* @return ��
* @since 2012��2��12��
*/
void TP_MsgBoxShow(TP_SMSBox Box);

/**
* �������
* @author ���Ķ�
* @param ��
* @return ��
* @since 2012��4��17��
*/
void TP_MsgMenu_Clear(void);

/**
* �����������ò˵�����
* @author ���Ķ�
* @param ��
* @return ��
* @since 2012��2��12��
*/
void TP_MsgMenu_MSGCentreSet(void);


/**
* ���ű���λ�ò˵�����
* @author ���Ķ�
* @param ��
* @return ��
* @since 2012��2��12��
*/
void TP_MsgMenu_SaveTo(void);


/**
* ���ݶ��ű���λ�ú����������г������б�
* @author ���Ķ�
* @param Box ��������<BR>
         *SmsIndex ���������ű���ָ��<BR>
         *SmsCount ��������
* @return TP_SMSDOResult
* @since 2012��2��12��
*/
TP_SMSDOResult TP_ListMsgIndex(TP_SMSBox Box,
                                      uint16 *SmsIndex,
                                      uint16 MaxSmsCount,
                                      uint16 *SmsCount);



/**
* ���ݶ��ű���λ��Ԥ������
* @author ���Ķ�
* @param Box ��������<BR>
         index ������<BR>
         *Phone ����ָ��<BR>
         *Sms ��������ָ��<BR>
         PreviewLength Ԥ�����ų���<BR>
         *SmsState ����״̬<BR>
         *DateTime ����ʱ��
* @return TP_SMSDOResult
* @since 2012��2��12��
*/
TP_SMSDOResult TP_PreviewMsg(TP_SMSBox Box,
                                  uint16 index,
                                  uint8 *Phone,
                                  uint8 *Sms,
                                  uint16 PreviewLength,
                                  TP_SMSRecordState *SmsState,
                                  TP_DateTime *DateTime);


/**
* ���ݶ��ű���λ�ö�ȡ����
* @author ���Ķ�
* @param Box ��������<BR>
         index ������<BR>
         *Phone ����ָ��<BR>
         *Sms ��������ָ��<BR>
         *SmsLength ���ų���<BR>
         *SmsState ����״̬<BR>
         *DateTime ����ʱ��
* @return TP_SMSDOResult
* @since 2012��2��12��
*/
TP_SMSDOResult TP_ReadMsg(TP_SMSBox Box,
                              uint16 index,
                              uint8 *Phone,
                              uint8 *Sms,
                              uint16 *SmsLength,
                              TP_SMSRecordState *SmsState,
                              TP_DateTime *DateTime);


/**
* ���ݶ��ű���λ��ɾ������
* @author ���Ķ�
* @param idnex ������<BR>
* @return TP_SMSDOResult
* @since 2012��2��12��
*/
TP_SMSDOResult TP_DelMsg(TP_SMSBox Box,uint16 index);

/**
* ɾ�������ȫ������
* @author ���Ķ�
* @param Box Ҫɾ��������<BR>
* @return TP_SMSDOResult
* @since 2012��4��17��
* @note: ���ɾ�������г������˳�
*/
TP_SMSDOResult TP_DelBoxMsg(TP_SMSBox Box);

/**
* ɾ�����ж���
* @author ���Ķ�
* @return TP_SMSDOResult
* @since 2012��4��17��
* @note: ���ɾ�������г������˳�
*/
TP_SMSDOResult TP_DelAllMsg(void);

/**
* ���ݶ��ű���λ�ñ������
* @author ���Ķ�
* @param Box ��������<BR>
         *StoreIndex ����������<BR>
         *Phone ����ָ��<BR>
         *Sms ��������ָ��<BR>
         *SmsLength ���ų��ȣ�Ԥ��ʱΪԤ���Ķ������ݳ���<BR>
*         SmsState ����״̬<BR>
*         DateTime ����ʱ��
* @return TP_SMSDOResult
* @since 2012��2��12��
*/
TP_SMSDOResult TP_StoreMsg(TP_SMSBox Box,
                                  uint16 *StoreIndex,
                                  uint8 *Phone,
                                  uint8 *Sms,
                                  uint16 SmsLength,
                                  TP_SMSRecordState SmsSate,
                                  TP_DateTime *DateTime);


/**
* ���ݶ��ű���λ�ñ������(����洢��������ɾ����ɵ��ռ������)
* @author ���Ķ�
* @param Box ��������<BR>
         *StoreIndex ����������<BR>
         *Phone ����ָ��<BR>
         *Sms ��������ָ��<BR>
         *SmsLength ���ų��ȣ�Ԥ��ʱΪԤ���Ķ������ݳ���<BR>
*         SmsState ����״̬<BR>
*         DateTime ����ʱ��
* @return TP_SMSDOResult
* @since 2012��2��12��
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



