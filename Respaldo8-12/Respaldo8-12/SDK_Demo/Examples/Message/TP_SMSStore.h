/**
 *
 * @file ����Ϣ���洦��ģ��
 *  @author   ���Ķ�
 *  @date    2012-2-4
 * @brief  SMS���ű���
 * @version V1.0.1
 * @note
 */


#ifndef TP_SMSSTORE_H
#define TP_SMSSTORE_H

#ifdef __cplusplus
extern "C" {
#endif



#define SMSINDEX_FILE_NAME  "SMSINDEX"
#define SMSSTORE_FILE_NAME  "SMSSTORE"//SMSSTORE1,SMSSTORE2,SMSSTORE3....
#define SMSSTORE_MAX_EACH_FILE  10//ÿ���ļ��洢�Ķ�������


/*
���Ŵ洢���:
������:1~N (N=MAX_SMS_STORE_ALL)
���ɴ洢(�����ϴ洢)
�ռ���洢���ȼ���ߣ�

*/
#define MAX_SMS_STORE       200 //ϵͳ���������������������ֵҪ��������ע��,��Ȼ��Ҫ�޸�TP_MSGMenu.c
                                //������漸���궼����Ϊ0����Ϊ����������
                                //�����Ϊ0����Ϊ��������������
#define MAX_SMS_OUTBOX      10  //����������������壬��Ϊ0�����ر�����
#define MAX_SMS_SENTBOX     0   //�ѷ���Ϣ�������壬��Ϊ0�����ر�����
#define MAX_SMS_DRAFTBOX    10  //�ݸ����������壬��Ϊ0�����ر�����
#define MAX_SMS_STORE_ALL   (MAX_SMS_STORE+MAX_SMS_OUTBOX+MAX_SMS_SENTBOX+MAX_SMS_DRAFTBOX)

#define SMS_MAX_ADDR_LEN    20  //�ֻ����볤�ȶ���

typedef enum SMSBoxEnum
{
    SMS_BOX_INBOX = 1,
    SMS_BOX_OUTBOX,
    SMS_BOX_SENTBOX,
    SMS_BOX_DRAFT,

    //SMS_BOX_ALL,
    SMS_BOX_MAX
}TP_SMSBox;

typedef enum SMSRecordStateEnum
{
    SMS_RECORD_RECEIVED_UNREAD = 0,
    SMS_RECORD_RECEIVED_READ,
    SMS_RECORD_SENT,
    SMS_RECORD_NOT_SENT,
    SMS_RECORD_DRAFT,
    SMS_RECORD_MAX
}TP_SMSRecordState;


//���Ŵ洢�ṹ
typedef struct SMSStoreOnFlashStruct
{
    uint8               recordNumber;           //������ 1~~MAX_SMS_STORE
    uint8               Phone[SMS_MAX_ADDR_LEN];//�ֻ�����
    uint8               MsgData[SMS_SHORT_MSG_LEN+1];           //��������
    uint8               MsgLen;                 //���ų���
    uint8               ConcatMax;              //�����Ͷ�������
    uint8               ConcatCurrent;          //������Ŀǰ�ڼ�������
    uint8               ConcatFirsIndex;        //�����͵�һ������������
    uint8               ConcatNextIndex;        //��������һ������������
    TP_SMSRecordState   SmsSate;                //����״̬(�Ѷ���δ�����ѷ��ͣ�δ����)
    TP_DateTime         DateTime;               //����ʱ��
    TP_SMSBox           Box;                    //��������(�ռ��䣬�����䣬�ѷ���Ϣ���ݸ���)
}TP_SMSStore;

typedef struct SMSStoreOnFlash2Struct
{
    uint16               recordNumber;           //������ 1~~MAX_SMS_STORE
    uint8               Phone[SMS_MAX_ADDR_LEN];//�ֻ�����
    uint8               MsgData[SMS_SHORT_MSG_LEN+1];           //��������
    uint8               MsgLen;                 //���ų���
    uint8               ConcatMax;              //�����Ͷ�������
    uint8               ConcatCurrent;          //������Ŀǰ�ڼ�������
    uint16               ConcatFirsIndex;        //�����͵�һ������������
    uint16               ConcatNextIndex;        //��������һ������������
    TP_SMSRecordState   SmsSate;                //����״̬(�Ѷ���δ�����ѷ��ͣ�δ����)
    TP_DateTime         DateTime;               //����ʱ��
    TP_SMSBox           Box;                    //��������(�ռ��䣬�����䣬�ѷ���Ϣ���ݸ���)
}TP_SMSStore2;


//���������ļ��ṹ
typedef struct TP_SMSInfoStruct
{
    //uint8               recordNumber;
    TP_DateTime         DateTime;
    uint8               FirstIndex;
    //TP_SMSRecordState   IsReadOrSend;
    TP_SMSBox           Box;
    //uint8               Phone[SMS_MAX_ADDR_LEN];//
    //uint8 PrewView[MAX_SMS_PREVIEW_LEN + 1];
}TP_SMSInfo;

typedef struct SMSIndexStruct
{
    TP_SMSInfo          SMSInfo[MAX_SMS_STORE_ALL];         //���ż�����Ϣ�������䣬�����ţ�ʱ��
    uint8               IsRecordUsed[MAX_SMS_STORE_ALL];    //��¼���Ƿ��Ѿ���ʹ��
    uint16              UnreadRecords;                  //�ռ����δ����������
    //uint8               SMSCount;
}TP_SMSIndex;


Boolean TP_FileWrite(const char *filename,uint8 *data_p,uint32 filesize);
Boolean TP_FileRead(const char *filename,uint8 *data_p,uint32 filesize);

void TP_SMSStoreInit(void);
TP_SMSDOResult TP_SMSSave(uint16 *index,
                                uint8 *Phone,
                                uint8 *Sms,
                                uint16 SmsLength,
                                TP_SMSBox Box,
                                TP_SMSRecordState SmsSate,
                                TP_DateTime *DateTime);
TP_SMSDOResult TP_SMSRead(uint16 recordNumber,
                                uint8 *Phone,
                                uint8 *Sms,
                                uint16 *SmsLength,
                                TP_SMSRecordState *smsstate,
                                TP_DateTime *DateTime);
TP_SMSDOResult TP_SMSPreview(uint16 recordNumber,
                                  uint8 *Phone,
                                  uint8 *Sms,
                                  uint16 PreviewLength,
                                  TP_SMSRecordState *smsstate,
                                  TP_DateTime *DateTime);
TP_SMSDOResult TP_SMSDel(uint16 recordNumber);
TP_SMSDOResult TP_ListIndexByBox(TP_SMSBox Box,
                                       uint16 *IndexArray,
                                       uint16 IndexMaxCount,
                                       uint16 *SmsCount);
uint16 TP_GetFlashSMSAllCount(TP_SMSBox Box);
uint16 TP_GetFlashSMSCount(TP_SMSBox Box);
uint16 TP_GetFlashUnReadSMSCount(void);
uint16 TP_GetFlashSMSMemorySize(void);
uint8 TP_GetBoxFreeSpace(TP_SMSBox Box);
int16 TP_CmpDataTime(TP_DateTime *dt1_p,TP_DateTime *dt2_p);


#ifdef __cplusplus
}
#endif

#endif//TP_SMSSTORE_H


