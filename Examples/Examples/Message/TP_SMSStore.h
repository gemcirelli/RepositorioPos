/**
 *
 * @file 短信息保存处理模块
 *  @author   何文东
 *  @date    2012-2-4
 * @brief  SMS短信保存
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
#define SMSSTORE_MAX_EACH_FILE  10//每个文件存储的短信条数


/*
短信存储简介:
索引号:1~N (N=MAX_SMS_STORE_ALL)
自由存储(信箱混合存储)
收件箱存储优先级最高，

*/
#define MAX_SMS_STORE       200 //系统保存短信最多条数，这个数值要比其它大，注意,不然需要修改TP_MSGMenu.c
                                //如果下面几个宏都定义为0，则为短信总条数
                                //如果不为0，则为其它信箱总条数
#define MAX_SMS_OUTBOX      10  //发件箱短信条数定义，如为0，则不特别限制
#define MAX_SMS_SENTBOX     0   //已发信息条数定义，如为0，则不特别限制
#define MAX_SMS_DRAFTBOX    10  //草稿箱条数定义，如为0，则不特别限制
#define MAX_SMS_STORE_ALL   (MAX_SMS_STORE+MAX_SMS_OUTBOX+MAX_SMS_SENTBOX+MAX_SMS_DRAFTBOX)

#define SMS_MAX_ADDR_LEN    20  //手机号码长度定义

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


//短信存储结构
typedef struct SMSStoreOnFlashStruct
{
    uint8               recordNumber;           //索引号 1~~MAX_SMS_STORE
    uint8               Phone[SMS_MAX_ADDR_LEN];//手机号码
    uint8               MsgData[SMS_SHORT_MSG_LEN+1];           //短信内容
    uint8               MsgLen;                 //短信长度
    uint8               ConcatMax;              //串接型短信条数
    uint8               ConcatCurrent;          //串接型目前第几条短信
    uint8               ConcatFirsIndex;        //串接型第一条短信索引号
    uint8               ConcatNextIndex;        //串接型下一条短信索引号
    TP_SMSRecordState   SmsSate;                //短信状态(已读，未读，已发送，未发送)
    TP_DateTime         DateTime;               //日期时间
    TP_SMSBox           Box;                    //所在信箱(收件箱，发件箱，已发信息，草稿箱)
}TP_SMSStore;

typedef struct SMSStoreOnFlash2Struct
{
    uint16               recordNumber;           //索引号 1~~MAX_SMS_STORE
    uint8               Phone[SMS_MAX_ADDR_LEN];//手机号码
    uint8               MsgData[SMS_SHORT_MSG_LEN+1];           //短信内容
    uint8               MsgLen;                 //短信长度
    uint8               ConcatMax;              //串接型短信条数
    uint8               ConcatCurrent;          //串接型目前第几条短信
    uint16               ConcatFirsIndex;        //串接型第一条短信索引号
    uint16               ConcatNextIndex;        //串接型下一条短信索引号
    TP_SMSRecordState   SmsSate;                //短信状态(已读，未读，已发送，未发送)
    TP_DateTime         DateTime;               //日期时间
    TP_SMSBox           Box;                    //所在信箱(收件箱，发件箱，已发信息，草稿箱)
}TP_SMSStore2;


//短信索引文件结构
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
    TP_SMSInfo          SMSInfo[MAX_SMS_STORE_ALL];         //短信简略信息，如信箱，索引号，时间
    uint8               IsRecordUsed[MAX_SMS_STORE_ALL];    //记录号是否已经被使用
    uint16              UnreadRecords;                  //收件箱的未读短信数量
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


