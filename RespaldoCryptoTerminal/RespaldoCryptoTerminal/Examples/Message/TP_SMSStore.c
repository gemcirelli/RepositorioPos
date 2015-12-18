/*******************************************************************************
 * TELPO Software Copyright (c) 1997-2011 TELPO Ltd
 * Licensed to Simcom Ltd
 ******************************************************************************/

/*******************************************************************************
 * ÎÄ¼þÎ»ÖÃ:.TPSDK\TPSMS
 * ÎÄ¼þËµÃ÷:¶ÌÐÅ´æ´¢
 * ´´½¨ÈÕÆu:2012.2.4
 *   ´´½¨ÈË:ºÎÎÄ¶«
 * ÐÞ¸ÄÈÕÆu:2012.2.4
 * ÐÞ¸ÄËµÃ÷:
 * ÐÞ¸ÄÈÕÆu:
 ******************************************************************************/

#include <TPTypedef.h>
#include <TPBase.h>
#include <TPSMS.h>
#include <TPFile.h>
#include <TPSerial.h>
#include "TP_SMSStore.h"
#include "TP_MSGMenu.h"//

static Boolean IsPreview = FALSE;

TP_SMSIndex SMSIndex;

//Boolean TP_FileWrite(const char *filename,uint8 *data_p,uint32 filesize);
//Boolean TP_FileRead(const char *filename,uint8 *data_p,uint32 filesize);

//¼ÆËãË÷ÒýºÅÔuÎÄ¼þÖÐµÄÎ»ÖÃ£¬±ÈÈçÒ»¸öÎÄ¼þ´æ10Ìõ¶ÌÐÅ£¬Ë÷ÒýºÅ³ý10£¬ÄÜ³ý¾¡Ê±±iÊ¾Ôu×îºoÒ»¸öÎ»ÖÃeÏ£¬Èç¹û²»ÄÜ³ý¾¡ÔòÔuoàÊý¼õÒ»µÄÎ»ÖÃeÏ
#define SMS_POS_ON_FILE(recordNumber) ( (recordNumber%SMSSTORE_MAX_EACH_FILE!=0)?((recordNumber%SMSSTORE_MAX_EACH_FILE)-1):(SMSSTORE_MAX_EACH_FILE-1))

/**
* ±È½ÏÁ½¸öÊý×ÖµÄ´oÐ¡
* @author ºÎÎÄ¶«
* @version 1.0
* @param x Êý×Ö1<BR>
         y Êý×Ö2
* @return 1 Êý×Ö1>Êý×Ö2<BR>
          0 Êý×Ö1=Êý×Ö2<BR>
          -1 Êý×Ö1<Êý×Ö2
* @since 2012Äê2ÔÂ3ÈÕ
*/
#define CMP(x,y) (((x)>(y))?1:(((x)==(y))?0:(-1)))

void TP_itoa(int32 num,uint8 *str,uint8 radix)
{
    /* Ë÷Òý±i */
    uint8 index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    uint32 unum; /* ÖÐ¼ä±äÁ¿ */
    int32 i=0,j=0,k=0;
    /* È·¶¨unumµÄÖµ */
    if(radix==10&&num<0) /* Ê®½øÖÆ¸ºÊý */
    {
        unum=(unsigned)-num;
        str[i++]='-';
    }
    else
    {
        unum=(unsigned)num; /* ÆäËûÇe¿ö */
    }
    /* ÄæÐò */
    do
    {
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;
    }while(unum);
    str[i]='\0';
    /* ×ª»» */
    if(str[0]=='-')
    {
        k=1; /* Ê®½øÖÆ¸ºÊý */
    }
    else
    {
        k=0;
    }
    /* ½«Ô­À´µÄ¡°/2¡±¸ÄÎª¡°/2.0¡±£¬±£Ö¤µ±numÔu16~255Ö®¼ä£¬radixµÈou16Ê±£¬Ò²ÄÜµÃµ½ÕýÈ·½á¹û */
    for(j=k;j<=(i-1)/2.0+k;j++)
    {
        num=str[j];
        str[j]=str[i-j-1+k];
        str[i-j-1+k]=num;
    }

}

//»¹ÐèÒª×öSIM¿¨µÄ³õÊ¼»¯
void TP_SMSStoreInit()
{
    uint8 tempi = 0;
    char filename[17];
    uint8 filecount =0;
    TP_SMSStore smsdata[SMSSTORE_MAX_EACH_FILE];
    //uint8 trycnt = 0;
    //TP_DbgSerialPrn("\n SMSStore size is %d   %d  ",sizeof(TP_SMSStore),sizeof(TP_SMSStore2));
    memset(&SMSIndex,0,sizeof(TP_SMSIndex));
    //¶ÁÈ¡Ë÷ÒýÎÄ¼þ
    if(TP_FExist(SMSINDEX_FILE_NAME)==0)
    {
        //Èç¹ûË÷ÒýÎÄ¼þ²»´æÔu£¬´´½¨
        if(TP_Create(SMSINDEX_FILE_NAME, sizeof(TP_SMSIndex))!=0)
        {
            TP_FileWrite(SMSINDEX_FILE_NAME,(uint8 *)&SMSIndex,sizeof(TP_SMSIndex));
        }
        else
        {
            TP_DbgSerialPrn("create file fail!");
        }
    }
    tempi = TP_FileRead(SMSINDEX_FILE_NAME,(uint8*)&SMSIndex,sizeof(TP_SMSIndex));
    #ifdef SDK_PC_SIMULATION
    _cprintf("\nread result %d",tempi);
    #endif
    /*
    while(trycnt<3)
    {
        if(FALSE == TP_FileRead(SMSINDEX_FILE_NAME,&SMSIndex,sizeof(TP_SMSIndex)))
        {
            trycnt++;
        }
        else
        {
            trycnt = 3;
        }
    }
    */
    //Èç¹û¶ÌÐÅÎÄ¼þ²»´æÔu£¬Ôò´´½¨
    memset(smsdata,0,sizeof(smsdata));
    filecount = MAX_SMS_STORE_ALL/SMSSTORE_MAX_EACH_FILE;
    if( (MAX_SMS_STORE_ALL%SMSSTORE_MAX_EACH_FILE) != 0 )
    {
        filecount++;
    }
    for(tempi=1;tempi<=filecount;tempi++)
    {
        memset(filename,0,sizeof(filename));
        sprintf((char*)filename,"%s%d",SMSSTORE_FILE_NAME,tempi);

        if(TP_FExist(filename)==0)
        {
            if(TP_Create(filename, sizeof(TP_SMSStore)*SMSSTORE_MAX_EACH_FILE)==0)
            {
                TP_DbgSerialPrn("\r\n Create %s Err:%d ",filename,TP_GetLastError());
            }
            else
            {
                TP_FileWrite(filename, (uint8*)smsdata, sizeof(smsdata));
            }
        }

    }
    #ifdef MSG_SIM800
    if(TP_GetSMSSaveOn() == 0)
    {
        TP_SetNewSMSStoreOnSim(FALSE);
    }
    else
    {
        TP_SetNewSMSStoreOnSim(TRUE);
    }
    #endif
}

Boolean TP_FileWrite(const char *filename,uint8 *data_p,uint32 filesize)
{
    int32 fd;
    char FileName[17];
    memset(FileName,0,sizeof(FileName));
    sprintf((char *)FileName,"%s" ,(char *)filename);
    if(TP_FExist(FileName)==0)
    {
        if(TP_Create(FileName, filesize)==0)
        {
            return FALSE;
        }
    }
    fd = TP_Open(FileName);
    if(fd < 0)
    {
        return FALSE;
    }
    TP_Seek(fd, 0, SEEK_FOREWARD, SEEK_SET);
    TP_Write(fd, data_p, filesize);
    if (TP_GetLastError() < 0)
    {
        TP_Close(fd);
        return FALSE;
    }
    TP_Close(fd);
    return TRUE;

}

Boolean TP_FileWriteSingle(const char *filename,uint32 offset,uint8 *data_p,uint32 filesize)
{
    int32 fd;
    char FileName[17];
    uint32 result = 0;
    memset(FileName,0,sizeof(FileName));
    sprintf((char *)FileName,"%s" ,(char *)filename);
    if(TP_FExist(FileName)==0)
    {
        if(TP_Create(FileName, filesize)==0)
        {
            return FALSE;
        }
    }
    fd = TP_Open(FileName);
    if(fd < 0)
    {
        return FALSE;
    }
    TP_Seek(fd, offset, SEEK_FOREWARD, SEEK_SET);
    result = TP_Write(fd, data_p, filesize);
    //TP_DbgSerialPrn("\n\r WriteSingle=%d",result);
    if (TP_GetLastError() < 0)
    {
        //TP_DbgSerialPrn("\n\r LastError=%d",TP_GetLastError());
        TP_Close(fd);
        return FALSE;
    }
    TP_Close(fd);
    return TRUE;

}


Boolean TP_FileRead(const char *filename,uint8 *data_p,uint32 filesize)
{
    int32 fd;
    char FileName[17];
    memset(FileName,0,sizeof(FileName));
    sprintf((char *)FileName,"%s" ,(char *)filename);
    if(TP_FExist(FileName)==FALSE)
    {
        TP_DbgSerialPrn("\r\n no exist!");
        return FALSE;
    }
    fd = TP_Open(FileName);
    if(fd < 0)
    {
        TP_DbgSerialPrn("\r\nopen fail:%d",TP_GetLastError());
        return FALSE;
    }
    if(TP_Seek(fd, 0, SEEK_FOREWARD, SEEK_SET)==FALSE)
    {
        TP_DbgSerialPrn("\r\nSeek fail:%d",TP_GetLastError());
        TP_Close(fd);
    }
    TP_Read(fd, data_p, filesize);
    if (TP_GetLastError() < 0)
    {
        TP_DbgSerialPrn("\r\nRead fail:%d",TP_GetLastError());
        TP_Close(fd);
        return FALSE;
    }
    TP_Close(fd);
    return TRUE;

}


Boolean TP_FileReadSingle(const char *filename,uint32 offset,uint8 *data_p,uint32 filesize)
{
    int32 fd;
    char FileName[17];
    uint32 result=0;
    memset(FileName,0,sizeof(FileName));
    sprintf((char *)FileName,"%s" ,(char *)filename);
    if(TP_FExist(FileName)==0)
    {
        return FALSE;
    }
    fd = TP_Open(FileName);
    if(fd < 0)
    {
        return FALSE;
    }
    TP_Seek(fd, offset, SEEK_FOREWARD, SEEK_SET);
    result = TP_Read(fd, data_p, filesize);
    //TP_DbgSerialPrn("\n\r ReadSingle=%d",result);
    if (TP_GetLastError() < 0)
    {
        //TP_DbgSerialPrn("\n\r LastError=%d",TP_GetLastError());
        TP_Close(fd);
        return FALSE;
    }
    TP_Close(fd);
    return TRUE;

}

/**
* ±È½ÏÈÕÆuÊ±¼ä´oÐ¡
* @author ºÎÎÄ¶«
* @version 1.0
* @param *dt1_p ÈÕÆuÊ±¼ä1Ö¸Õë<BR>
         *dt1_p ÈÕÆuÊ±¼ä2Ö¸Õë
* @return >0 dt1_p>dt2_p<BR>
          =0 dt1_p=dt2_p<BR>
          <0 dt1_p<dt2_p
* @since 2012Äê2ÔÂ3ÈÕ
*/
int16 TP_CmpDataTime(TP_DateTime *dt1_p,TP_DateTime *dt2_p)
{
    int16 result =0;
    result = CMP(dt1_p->Date.Year,dt2_p->Date.Year);
    result = (result!=0)?result:CMP(dt1_p->Date.Month,dt2_p->Date.Month);
    result = (result!=0)?result:CMP(dt1_p->Date.Day,  dt2_p->Date.Day);

    result = (result!=0)?result:CMP(dt1_p->Time.Hours,  dt2_p->Time.Hours);
    result = (result!=0)?result:CMP(dt1_p->Time.Minutes,dt2_p->Time.Minutes);
    result = (result!=0)?result:CMP(dt1_p->Time.Seconds,dt2_p->Time.Seconds);
    return result;
}




//ÅÐ¶ÏË÷ÒýºÅÊÇ·nÒn¾­±»Ê¹oÃ
Boolean TP_IsRecordNumberUsed(uint8 recordnumber)
{
    if(recordnumber>MAX_SMS_STORE_ALL)
    {
        return TRUE;//³¬³öË÷ÒýºÅ·¶Î§£¬ÔòÈÏÎªÒn±»Ê¹oÃ
    }
    if(SMSIndex.IsRecordUsed[recordnumber-1]>0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#if 0
void TP_SMSInfoWriteToIndex(TP_SMSInfo *SMSInfo_p,Boolean ForceWrite)
{
    uint8 recordNumber;
    TP_SMSIndex IndexBack;
    //sendTcpStringToSerialPort("==TP_SMSInfoWriteToIndex==",strlen((char *)"==TP_SMSInfoWriteToIndex=="), 0);
    memset(&IndexBack,0,sizeof(TP_SMSIndex));
    memcpy(&IndexBack,&SMSIndex,sizeof(TP_SMSIndex));

    recordNumber = SMSInfo_p->recordNumber;
    if(TP_IsRecordNumberUsed(recordNumber))
    {
        if(ForceWrite == FALSE)
        {
            return;
        }
    }
    else
    {
        if(SMSIndex.SMSCount<MAX_SMS_STORE_ALL)
        {
            SMSIndex.SMSCount++;
        }
    }
    //sendTcpStringToSerialPort("==2==",strlen((char *)"==2=="), 0);
    SMSIndex.IsRecordUsed[recordNumber-1] = TRUE;
    memcpy(&(SMSIndex.SMSInfo[recordNumber-1]),SMSInfo_p,sizeof(TP_SMSInfo));
    //if(TP_SMSIndexWrite(&SMSIndex)==FALSE)
    if(TP_FileWrite(SMSINDEX_FILE_NAME,&SMSIndex,sizeof(TP_SMSIndex))==FALSE)
    {
        //sendTcpStringToSerialPort("==write fail==",strlen((char *)"==write fail=="), 0);
        memcpy(&SMSIndex,&IndexBack,sizeof(TP_SMSIndex));
    }
}

void TP_SMSInfoReadFromIndex(TP_SMSInfo *SMSInfo_p)
{
    uint8 tempi;
    TP_SMSIndex IndexBack;
    //sendTcpStringToSerialPort("==TP_SMSInfoReadFromIndex==",strlen((char *)"==TP_SMSInfoReadFromIndex=="), 0);
    memset(&IndexBack,0,sizeof(TP_SMSIndex));
    memcpy(&IndexBack,&SMSIndex,sizeof(TP_SMSIndex));
    //if(TP_SMSIndexRead(&SMSIndex)==FALSE)
    if(TP_FileRead(SMSINDEX_FILE_NAME,&SMSIndex,sizeof(TP_SMSIndex))==FALSE)
    {
        //sendTcpStringToSerialPort("==read fail==",strlen((char *)"==read fail=="), 0);
        memcpy(&SMSIndex,&IndexBack,sizeof(TP_SMSIndex));
        return;
    }
    //if(TP_IsRecordNumberUsed(SMSInfo_p->recordNumber)==FALSE)
    //{
    //    return;
    //}
    for(tempi=0;tempi<MAX_SMS_STORE_ALL;tempi++)
    {
        if(SMSIndex.SMSInfo[tempi].recordNumber == SMSInfo_p->recordNumber)
        {
            memcpy(SMSInfo_p,&(SMSIndex.SMSInfo[tempi]),sizeof(TP_SMSInfo));
        }
    }
}


void TP_SMSStoreWriteToIndex(TP_SMSStore *SMSStore_p)
{
    uint8 recordNumber;
    TP_SMSIndex IndexBack;
    //sendTcpStringToSerialPort("==TP_SMSInfoWriteToIndex==",strlen((char *)"==TP_SMSInfoWriteToIndex=="), 0);
    memset(&IndexBack,0,sizeof(TP_SMSIndex));
    memcpy(&IndexBack,&SMSIndex,sizeof(TP_SMSIndex));
    recordNumber = SMSStore_p->recordNumber;
    if(TP_IsRecordNumberUsed(recordNumber) == FALSE)
    {
        if(SMSIndex.SMSCount<MAX_SMS_STORE_ALL)
        {
            SMSIndex.SMSCount++;
        }
        if(SMSStore_p->SmsSate == SMS_RECORD_RECEIVED_UNREAD)
        {
            SMSIndex.unreadRecords++;
        }
    }

    SMSIndex.SMSInfo[recordNumber-1].Box = SMSStore_p->Box;
    SMSIndex.SMSInfo[recordNumber-1].recordNumber = SMSStore_p->ConcatFirsIndex;
    SMSIndex.SMSInfo[recordNumber-1].SmsSate = SMSStore_p->SmsSate;
    SMSIndex.SMSInfo[recordNumber-1].DateTime = SMSStore_p->DateTime;
    memcpy(SMSIndex.SMSInfo[recordNumber-1].Phone,SMSStore_p->Phone,strlen((char *)SMSStore_p->Phone));

    //sendTcpStringToSerialPort("==2==",strlen((char *)"==2=="), 0);
    SMSIndex.IsRecordUsed[recordNumber-1] = TRUE;

    if(TP_FileWrite(SMSINDEX_FILE_NAME,&SMSIndex,sizeof(TP_SMSIndex))==FALSE)
    {
        //sendTcpStringToSerialPort("==write fail==",strlen((char *)"==write fail=="), 0);
        memcpy(&SMSIndex,&IndexBack,sizeof(TP_SMSIndex));
    }
}

#endif

//¿¼ÂÇ¶ÌÐÅ·ÖÎÄ¼þ´æ´¢Çe¿ö£¬¼oÁËÕâ¸öº¯Êý
// 1~10-->1  11~20-->2  21~30-->3
Boolean TP_SMSGetStoreFileName(uint16 recordNumber,char *StoreFileName)
{
    uint16 filenum = 0;
    if(recordNumber>MAX_SMS_STORE_ALL)
    {
        return FALSE;
    }
    filenum = recordNumber/SMSSTORE_MAX_EACH_FILE;
    if(recordNumber%SMSSTORE_MAX_EACH_FILE != 0)
    {
        filenum++;
    }
    sprintf(StoreFileName,"%s%d",SMSSTORE_FILE_NAME,filenum);
    return TRUE;
}

//·µ»Ø0£¬Ã»oÐ¿ÕÏÐÎ»ÖÃ£¬·Ç0
//Èôrecordnumber == 0£¬µuÒ»¸ö¿ÕÏÐÎ»ÖÃ
//Èôrecordnumber != 0,³ýrecordnumberiâµÄ¿ÕÏÐÎ»ÖÃ£¬¿eÒÔoÃÀ´n°ÕÒµu¶þ¸ö¿ÕÏÐÎ»ÖÃ
uint8 TP_SMSGetFreeRec(TP_SMSBox Box,TP_SMSIndex *SMSIndex_p, uint16 recordnumber)
{
    uint8 tempi;
    if( Box < SMS_BOX_INBOX
        || Box >= SMS_BOX_MAX )
    {
        return 0;
    }
    switch(Box)
    {
        case SMS_BOX_INBOX:
            break;
        case SMS_BOX_OUTBOX:
            if( MAX_SMS_OUTBOX > 0)
            {
                if(TP_GetFlashSMSAllCount(Box) >= MAX_SMS_OUTBOX )
                {
                    return 0;
                }
            }
            break;
        case SMS_BOX_SENTBOX:
            if( MAX_SMS_SENTBOX > 0)
            {
                if(TP_GetFlashSMSAllCount(Box) >= MAX_SMS_SENTBOX)
                {
                    return 0;
                }
            }
            break;
        case SMS_BOX_DRAFT:
            if( MAX_SMS_DRAFTBOX > 0)
            {
                if(TP_GetFlashSMSAllCount(Box) >= MAX_SMS_DRAFTBOX)
                {
                    return 0;
                }
            }
            break;
    }
    for(tempi = 0; tempi < MAX_SMS_STORE_ALL; tempi++ )
    {
        if(SMSIndex_p->IsRecordUsed[tempi]==0 && recordnumber!=(tempi+1))
        {
            return (tempi+1);
        }
    }
    return 0;
}

//Èç¹ûÐÅÏäoÐÏÞÖÆ£¬Ôò·µ»ØÊ£oàµÄ¿Õ¼ä
//Èç¹ûÃ»oÐÏÞÖÆ£¬Ôò·µ»Ø×ÜµÃÊ£oà¿Õ¼ä
uint8 TP_GetBoxFreeSpace(TP_SMSBox Box)
{
    uint16 tempi;
    uint16 BoxCount = 0;
    uint16 FreeCount = MAX_SMS_STORE_ALL;
    if( Box < SMS_BOX_INBOX
        || Box >= SMS_BOX_MAX )
    {
        return 0;
    }
    //i³¼Æ
    BoxCount = TP_GetFlashSMSAllCount(Box);
    switch(Box)
    {
        case SMS_BOX_INBOX:
            FreeCount -= BoxCount;
            if(FreeCount == 0)
            {
                return 0;
            }

            //¼õÈ¥OUTBOXµÄÈÝÁ¿
            if(MAX_SMS_OUTBOX > 0)
            {
                if(FreeCount >= MAX_SMS_OUTBOX)
                {
                    FreeCount -= MAX_SMS_OUTBOX;
                }
                else
                {
                    FreeCount = 0;
                }
            }
            else
            {
                BoxCount = TP_GetFlashSMSAllCount(SMS_BOX_OUTBOX);
                if(FreeCount >= BoxCount)
                {
                    FreeCount -= BoxCount;
                }
                else
                {
                    FreeCount = 0;
                }
            }
            if(FreeCount == 0)
            {
                return 0;
            }

            //¼õÈ¥SENTBOXµÄÈÝÁ¿
            if(MAX_SMS_SENTBOX > 0)
            {
                if(FreeCount >= MAX_SMS_SENTBOX )
                {
                    FreeCount -= MAX_SMS_SENTBOX;
                }
                else
                {
                    FreeCount = 0;
                }
            }
            else
            {
                BoxCount = TP_GetFlashSMSAllCount(SMS_BOX_SENTBOX);
                if(FreeCount >= BoxCount)
                {
                    FreeCount -= BoxCount;
                }
                else
                {
                    FreeCount = 0;
                }
            }
            if(FreeCount == 0)
            {
                return 0;
            }
            
            //¼õÈ¥DRAFTBOXµÄÈÝÁ¿
            if(MAX_SMS_DRAFTBOX > 0)
            {
                if(FreeCount >= MAX_SMS_DRAFTBOX)
                {
                    FreeCount -= MAX_SMS_DRAFTBOX;
                }
                else
                {
                    FreeCount = 0;
                }
            }
            else
            {
                BoxCount = TP_GetFlashSMSAllCount(SMS_BOX_DRAFT);
                if(FreeCount >= BoxCount)
                {
                    FreeCount -= BoxCount;
                }
                else
                {
                    FreeCount = 0;
                }
            }
            if(FreeCount == 0)
            {
                return 0;
            }
            
            break;
            
        case SMS_BOX_OUTBOX:
            if(MAX_SMS_OUTBOX > 0)
            {
                if(BoxCount >= MAX_SMS_OUTBOX)
                {
                    return 0;
                }
                else
                {
                    return ( MAX_SMS_OUTBOX - BoxCount);
                }
            }
            else
            {
                FreeCount -= BoxCount;
                if(FreeCount == 0)
                {
                    return 0;
                }
                
                BoxCount = TP_GetFlashSMSAllCount(SMS_BOX_INBOX);
                if(FreeCount >= BoxCount)
                {
                    FreeCount -= BoxCount;
                }
                else
                {
                    FreeCount = 0;
                }
                if(FreeCount == 0)
                {
                    return 0;
                }
                
                //¼õÈ¥SENTBOXµÄÈÝÁ¿
                if(MAX_SMS_SENTBOX> 0)
                {
                    if(FreeCount >= MAX_SMS_SENTBOX)
                    {
                        FreeCount -= MAX_SMS_SENTBOX;
                    }
                    else
                    {
                        FreeCount = 0;
                    }
                }
                else
                {
                    BoxCount = TP_GetFlashSMSAllCount(SMS_BOX_SENTBOX);
                    if(FreeCount >= BoxCount)
                    {
                        FreeCount -= BoxCount;
                    }
                    else
                    {
                        FreeCount = 0;
                    }
                }
                if(FreeCount == 0)
                {
                    return 0;
                }
                
                //¼õÈ¥DRAFTBOXµÄÈÝÁ¿
                if(MAX_SMS_DRAFTBOX > 0)
                {
                    if(FreeCount >= MAX_SMS_DRAFTBOX)
                    {
                        FreeCount -= MAX_SMS_DRAFTBOX;
                    }
                    else
                    {
                        FreeCount = 0;
                    }
                }
                else
                {
                    BoxCount = TP_GetFlashSMSAllCount(SMS_BOX_DRAFT);
                    if(FreeCount >= BoxCount)
                    {
                        FreeCount -= BoxCount;
                    }
                    else
                    {
                        FreeCount = 0;
                    }
                }
                if(FreeCount == 0)
                {
                    return 0;
                }
                
            }
            break;
        case SMS_BOX_SENTBOX:
            if(MAX_SMS_SENTBOX > 0)
            {
                if(BoxCount >= MAX_SMS_SENTBOX)
                {
                    return 0;
                }
                else
                {
                    return ( MAX_SMS_SENTBOX - BoxCount);
                }
            }
            else
            {
                FreeCount -= BoxCount;
                if(FreeCount == 0)
                {
                    return 0;
                }
                
                BoxCount = TP_GetFlashSMSAllCount(SMS_BOX_INBOX);
                if(FreeCount >= BoxCount)
                {
                    FreeCount -= BoxCount;
                }
                else
                {
                    FreeCount = 0;
                }
                if(FreeCount == 0)
                {
                    return 0;
                }

                //¼õÈ¥OUTBOXµÄÈÝÁ¿
                if(MAX_SMS_OUTBOX > 0)
                {
                    if(FreeCount >= MAX_SMS_OUTBOX)
                    {
                        FreeCount -= MAX_SMS_OUTBOX;
                    }
                    else
                    {
                        FreeCount = 0;
                    }
                }
                else
                {
                    BoxCount = TP_GetFlashSMSAllCount(SMS_BOX_OUTBOX);
                    if(FreeCount >= BoxCount)
                    {
                        FreeCount -= BoxCount;
                    }
                    else
                    {
                        FreeCount = 0;
                    }
                }
                if(FreeCount == 0)
                {
                    return 0;
                }
                
                //¼õÈ¥DRAFTBOXµÄÈÝÁ¿
                if(MAX_SMS_DRAFTBOX > 0)
                {
                    if(FreeCount >= MAX_SMS_DRAFTBOX)
                    {
                        FreeCount -= MAX_SMS_DRAFTBOX;
                    }
                    else
                    {
                        FreeCount = 0;
                    }
                }
                else
                {
                    BoxCount = TP_GetFlashSMSAllCount(SMS_BOX_DRAFT);
                    if(FreeCount >= BoxCount)
                    {
                        FreeCount -= BoxCount;
                    }
                    else
                    {
                        FreeCount = 0;
                    }
                }
                if(FreeCount == 0)
                {
                    return 0;
                }
            }
            break;
        case SMS_BOX_DRAFT:
            if(MAX_SMS_DRAFTBOX > 0)
            {
                if(BoxCount >= MAX_SMS_DRAFTBOX)
                {
                    return 0;
                }
                else
                {
                    return ( MAX_SMS_DRAFTBOX - BoxCount);
                }
            }
            else
            {
                FreeCount -= BoxCount;
                if(FreeCount == 0)
                {
                    return 0;
                }
                
                BoxCount = TP_GetFlashSMSAllCount(SMS_BOX_INBOX);
                if(FreeCount >= BoxCount)
                {
                    FreeCount -= BoxCount;
                }
                else
                {
                    FreeCount = 0;
                }
                if(FreeCount == 0)
                {
                    return 0;
                }

                //¼õÈ¥OUTBOXµÄÈÝÁ¿
                if(MAX_SMS_OUTBOX > 0)
                {
                    if(FreeCount >= MAX_SMS_OUTBOX)
                    {
                        FreeCount -= MAX_SMS_OUTBOX;
                    }
                    else
                    {
                        FreeCount = 0;
                    }
                }
                else
                {
                    BoxCount = TP_GetFlashSMSAllCount(SMS_BOX_OUTBOX);
                    if(FreeCount >= BoxCount)
                    {
                        FreeCount -= BoxCount;
                    }
                    else
                    {
                        FreeCount = 0;
                    }
                }
                if(FreeCount == 0)
                {
                    return 0;
                }

                //¼õÈ¥SENTBOXµÄÈÝÁ¿
                if(MAX_SMS_SENTBOX > 0)
                {
                    if(FreeCount >= MAX_SMS_SENTBOX )
                    {
                        FreeCount -= MAX_SMS_SENTBOX;
                    }
                    else
                    {
                        FreeCount = 0;
                    }
                }
                else
                {
                    BoxCount = TP_GetFlashSMSAllCount(SMS_BOX_SENTBOX);
                    if(FreeCount >= BoxCount)
                    {
                        FreeCount -= BoxCount;
                    }
                    else
                    {
                        FreeCount = 0;
                    }
                }
                if(FreeCount == 0)
                {
                    return 0;
                }
                
            }
            break;
    }
    return FreeCount;

}

//e¾³ý×î¾eµÄÒ»Ìõ¶ÌÐÅ
uint8 TP_DeleteLastSMS(TP_SMSBox Box)
{
    uint8 tempi;
    TP_DateTime DateTime;
    uint8 lastindex = 0;
    Boolean FirstSMS = TRUE;
    int16 cmpresult = 0;
    uint8 FreeSpace = 0;//Êi·Å³öÀ´µÄ¿Õ¼ä

    memset(&DateTime,0,sizeof(TP_DateTime));

    if( Box < SMS_BOX_INBOX
        || Box >= SMS_BOX_MAX )
    {
        return;
    }
    //²eÕÒ×î¾eµÄ¶ÌÐÅ
    for(tempi=0;tempi<MAX_SMS_STORE_ALL;tempi++)
    {
        if( SMSIndex.IsRecordUsed[tempi] >0
            && SMSIndex.SMSInfo[tempi].Box == Box
            && (SMSIndex.SMSInfo[tempi].FirstIndex == (tempi+1)))
        {
            if(FirstSMS == TRUE)
            {
                FirstSMS = FALSE;
                DateTime = SMSIndex.SMSInfo[tempi].DateTime;
                lastindex = tempi+1;
            }
            else
            {
                cmpresult = TP_CmpDataTime(&DateTime, &(SMSIndex.SMSInfo[tempi].DateTime));
                if(cmpresult > 0)
                {
                    DateTime = SMSIndex.SMSInfo[tempi].DateTime;
                    lastindex = tempi+1;
                }
            }
        }
    }
    if(lastindex != 0)
    {
        TP_SMSDel(lastindex);
    }

}


//Ôu±£´æÇ°Îª´æ´¢Êi·Å³ö×ã¹»¿Õ¼ä
//·µ»ØFALSE£¬±iÊ¾¿Õ¼äÒnÂu
//·µ»ØTRUE,ÒnoÐ×ã¹»¿Õ¼ä(»òÕßÒn¾­Êi·Å×ã¹»¿Õ¼ä)
Boolean TP_FreeEnoughSpaceForSave(TP_SMSBox Box,uint8 FreeCount)
{
    TP_DateTime DateTime;
    uint8 tempi;
    uint8 maxcount =0;
    uint8 FreeSpace = 0;
    if( Box < SMS_BOX_INBOX
        || Box >= SMS_BOX_MAX )
    {
        return FALSE;
    }
    FreeSpace = TP_GetBoxFreeSpace(Box);
    //ÊÕ¼þÏä»òÕßÆäËüÐÅÏä²»×öÏÞÖÆ
    if( Box == SMS_BOX_INBOX
        || (Box == SMS_BOX_OUTBOX && MAX_SMS_OUTBOX  == 0)
        || (Box == SMS_BOX_SENTBOX&& MAX_SMS_SENTBOX == 0)
        || (Box == SMS_BOX_DRAFT  && MAX_SMS_DRAFTBOX== 0)
        )
    {
        if( FreeSpace < FreeCount)
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    
    //oÐ×öÏÞÖÆ£¬e¾³ý×î¾eµÄ¶ÌÐÅ£¬ÒÔÌu³ö¿Õ¼ä
    
    switch(Box)
    {
        case SMS_BOX_OUTBOX:
            maxcount = MAX_SMS_OUTBOX;
            if(FreeCount > MAX_SMS_OUTBOX)
            {
                return FALSE;
            }
            break;
        case SMS_BOX_SENTBOX:
            maxcount = MAX_SMS_SENTBOX;
            if(FreeCount > MAX_SMS_SENTBOX)
            {
                return FALSE;
            }
            break;
        case SMS_BOX_DRAFT:
            maxcount = MAX_SMS_DRAFTBOX;
            if(FreeCount > MAX_SMS_DRAFTBOX)
            {
                return FALSE;
            }
            break;
    }

    while(maxcount>0 && FreeSpace < FreeCount)
    {
        TP_DeleteLastSMS(Box);
        FreeSpace = TP_GetBoxFreeSpace(Box);
    }
    return TRUE;


}


TP_SMSDOResult TP_SMSSave(uint16 *index,
                                uint8 *Phone,
                                uint8 *Sms,
                                uint16 SmsLength,
                                TP_SMSBox Box,
                                TP_SMSRecordState SmsSate,
                                TP_DateTime *DateTime)
{
    char filename[17];
    TP_SMSStore SMSStore;
    uint16 savedlen = 0;
    TP_SMSStore readdata[SMSSTORE_MAX_EACH_FILE];
    TP_SMSIndex *SMSIndexBack;

    memset(&SMSStore,0,sizeof(TP_SMSStore));
    memset(readdata,0,sizeof(readdata));

    //Ïà¹ØÐÅÏ¢µÄ±£´æ
    if(SmsLength > SMS_SHORT_MSG_LEN)
    {
        SMSStore.ConcatMax = SmsLength/SMS_SHORT_MSG_LEN;
        if(SmsLength%SMS_SHORT_MSG_LEN >0)
        {
            SMSStore.ConcatMax++;
        }

    }
    else
    {
        SMSStore.ConcatMax = 1;

    }
    if(TP_FreeEnoughSpaceForSave(Box,SMSStore.ConcatMax)==FALSE)
    {
        return SMSDO_FULL;
    }


    TP_AllocMemory(sizeof(TP_SMSIndex), (void**)&SMSIndexBack);
    memset(SMSIndexBack,0,sizeof(TP_SMSIndex));
    memcpy(SMSIndexBack,&SMSIndex,sizeof(TP_SMSIndex));

    //TP_GetDateTime(&SMSStore.DateTime);
    memcpy(&SMSStore.DateTime, DateTime, sizeof(TP_DateTime));
    SMSStore.Box = Box;
    SMSStore.SmsSate = SmsSate;
    SMSStore.ConcatCurrent = 0;//ÕâÀïeè0£¬±£´æÊ±´o1¿ªÊ¼
    if(strlen((char *)Phone)>= SMS_MAX_ADDR_LEN)
    {
        memcpy(SMSStore.Phone,Phone,SMS_MAX_ADDR_LEN);
    }
    else
    {
        memcpy(SMSStore.Phone,Phone,strlen((char *)Phone));
    }

    SMSStore.ConcatNextIndex = 0;
    SMSStore.ConcatFirsIndex = TP_SMSGetFreeRec(Box,SMSIndexBack,0);
    if(SMSStore.ConcatFirsIndex == 0)
    {
        //sendTcpStringToSerialPort("==full==",strlen((char *)"==full=="), 0);
        //sendTcpStringToSerialPort(&SMSStore.ConcatFirsIndex,1, 0);
        TP_FreeMemory((void**)&SMSIndexBack);
        return SMSDO_FULL;
    }
    SMSStore.recordNumber = SMSStore.ConcatFirsIndex;
    SMSIndexBack->IsRecordUsed[SMSStore.recordNumber-1] = TRUE;//Õ¼oÃÕâ¸öÎ»ÖÃ
    //ÏÂÃæµÄÏîÄ¿±£´æÊ±¶¼ÐèÒª¸ü¸Ä
    //SMSStore.recordNumber
    //SMSStore.ConcatCurrent
    //SMSStore.ConcatNextIndex
    //SMSStore.MsgData
    //SMSStore.MsgLen

    /*
    if(SMSIndex.SMSCount+SMSStore.ConcatMax >= MAX_SMS_STORE)
    {
        sendTcpStringToSerialPort("==3==",strlen((char *)"==3=="), 0);
        return;//´æ´¢ÒnÂu
    }
    */
    //TP_DbgSerialPrn("\n SmsLength = %d ",SmsLength);
    do
    {
        SMSStore.ConcatCurrent++;
        if(SMSStore.ConcatMax == 1)
        {
            //¶ÌÐÅÏ¢Ê±µÄÏÂÒ»ÌõË÷ÒýºÅ¾iÊÇµuÒ»ÌõË÷ÒýºÅ
            SMSStore.ConcatNextIndex = SMSStore.ConcatFirsIndex;
        }
        else
        {
            SMSStore.ConcatNextIndex = TP_SMSGetFreeRec(Box,SMSIndexBack,SMSStore.recordNumber);
            //³¤¶ÌÐÅÊ±±£´æ×îºoÒ»Ìõ¶ÌÐÅÊ±£¬ÏÂÒ»ÌõË÷ÒýºÅ¾iÊÇµ±Ç°µÄË÷ÒýºÅ
            if(SMSStore.ConcatCurrent==SMSStore.ConcatMax)
            {
                SMSStore.ConcatNextIndex = SMSStore.recordNumber;
            }
            if(SMSStore.ConcatNextIndex == 0)
            {
                //sendTcpStringToSerialPort("==full==",strlen((char *)"==full=="), 0);
                //ÎÞÎ»ÖÃ´æ´¢¶ÌÐÅ
                return SMSDO_FULL;
            }
            SMSIndexBack->IsRecordUsed[SMSStore.ConcatNextIndex-1]=TRUE;//
        }
        //TP_DbgSerialPrn("\n now index=%d,next index =%d\n",SMSStore.recordNumber,SMSStore.ConcatNextIndex);

        //´Ë´Î±£´æµÄ¶ÌÐÅ³¤¶È¼ÆËã
        SMSStore.MsgLen =  (SmsLength - savedlen)>SMS_SHORT_MSG_LEN?SMS_SHORT_MSG_LEN:(SmsLength - savedlen);
        /*
        if( SMSStore.MsgLen > SMS_SHORT_MSG_LEN)
        {
            SMSStore.MsgLen = SMS_SHORT_MSG_LEN;
        }
        */

        memset(SMSStore.MsgData,0,sizeof(SMSStore.MsgData));
        memcpy(SMSStore.MsgData,&(Sms[savedlen]),SMSStore.MsgLen);
        savedlen += SMSStore.MsgLen;
        //TP_DbgSerialPrn("\n storelen = %d  %d \n",SMSStore.MsgLen,savedlen);
        //TP_DbgSerialPrnHex(SMSStore.MsgData, SMSStore.MsgLen);
        //¶ÁÈ¡¶Ôo¦ÎÄ¼þ²¢±£´æ
        TP_SMSGetStoreFileName(SMSStore.recordNumber, filename);

        //sendTcpStringToSerialPort("==file name==",strlen((char *)"==file name=="), 0);
        //sendTcpStringToSerialPort(filename,strlen((char *)filename), 0);

        memset(readdata,0,sizeof(readdata));
        if(TP_FExist(filename)==FALSE)
        {
            TP_Create(filename, sizeof(readdata));
            //TP_FileWrite(filename, readdata, sizeof(readdata));
        }
        if(TP_FileRead(filename, (uint8*)readdata, sizeof(readdata))==TRUE)
        {
            /*
            if(SMSStore.recordNumber%SMSSTORE_MAX_EACH_FILE != 0)
            {
                memcpy(&(readdata[]),&SMSStore,sizeof(TP_SMSStore));
            }
            else
            {
                memcpy(&(readdata[SMSSTORE_MAX_EACH_FILE]),&SMSStore,sizeof(TP_SMSStore));
            }
            */
            memcpy(&(readdata[SMS_POS_ON_FILE(SMSStore.recordNumber)]),&SMSStore,sizeof(TP_SMSStore));
            TP_FileWrite(filename, (uint8*)readdata, sizeof(readdata));
            //¸üÐÂË÷ÒýÎÄ¼þ
            SMSIndexBack->SMSInfo[SMSStore.recordNumber-1].Box = Box;
            //SMSIndex.SMSInfo[SMSStore.recordNumber-1].recordNumber = SMSStore.recordNumber;
            SMSIndexBack->SMSInfo[SMSStore.recordNumber-1].FirstIndex = SMSStore.ConcatFirsIndex;
            //SMSIndex.SMSInfo[SMSStore.recordNumber-1].SmsSate = SmsSate;
            memcpy(&(SMSIndexBack->SMSInfo[SMSStore.recordNumber-1].DateTime),&(SMSStore.DateTime),sizeof(TP_DateTime));
            SMSStore.recordNumber = SMSStore.ConcatNextIndex;
        }
        else
        {
            TP_FreeMemory((void**)&SMSIndexBack);
            TP_DbgSerialPrn("\n File Read Err");
            return SMSDO_FAIL;
        }
    }
    while(SMSStore.ConcatCurrent != SMSStore.ConcatMax);
    memcpy(&SMSIndex,SMSIndexBack,sizeof(TP_SMSIndex));
    if(Box == SMS_BOX_INBOX && SmsSate == SMS_RECORD_RECEIVED_UNREAD )
    {
        SMSIndex.UnreadRecords++;
    }
    TP_FileWrite(SMSINDEX_FILE_NAME,(uint8*)&SMSIndex,sizeof(TP_SMSIndex));//±£´æË÷ÒýÎÄ¼þ
    TP_FreeMemory((void**)&SMSIndexBack);
    *index = SMSStore.ConcatFirsIndex;
    return SMSDO_SUCCESS;
}


TP_SMSDOResult TP_SMSRead(uint16 recordNumber,
                                uint8 *Phone,
                                uint8 *Sms,
                                uint16 *SmsLength,
                                TP_SMSRecordState *smsstate,
                                TP_DateTime *DateTime)
{
    char filename[17];
    TP_SMSStore readdata[SMSSTORE_MAX_EACH_FILE];
    TP_SMSStore *SMSStore_p;
    uint16 index = recordNumber;
    uint16 msglen = 0;
    uint8 msgdata[SMS_SHORT_MSG_LEN*MAX_SMS_CONT_NUM];
    uint8 IsUnReadSMS = FALSE;
    uint16 tempSmsLength = *SmsLength;

    index = recordNumber;
    if(index>MAX_SMS_STORE_ALL)
    {
        IsPreview = FALSE;
        //TP_DbgSerialPrn("\n read fail:out of rang:%d",index);
        return SMSDO_NONEXIST;
    }
    memset(msgdata,0,sizeof(msgdata));

    do
    {
        //sendTcpStringToSerialPort("==read index==",strlen((char *)"==read index=="), 0);
        //sendTcpStringToSerialPort(&index,1, 0);

        memset(readdata,0,sizeof(readdata));
        memset(filename,0,sizeof(filename));
        TP_SMSGetStoreFileName(index, filename);
        //sendTcpStringToSerialPort("==file name==",strlen((char *)"==file name=="), 0);
        //sendTcpStringToSerialPort(filename,strlen((char *)filename), 0);
        if(TP_FExist(filename)==FALSE)
        {
            TP_DbgSerialPrn("\n read fail:file no exit");
            return SMSDO_FAIL;
        }

        if(TP_FileRead(filename, (uint8*)readdata, sizeof(readdata))==FALSE)
        {
            return SMSDO_FAIL;
        }
        SMSStore_p = &(readdata[SMS_POS_ON_FILE(index)]);
        if(SMSStore_p->recordNumber!= index)
        {
            //TP_DbgSerialPrn("\n read fail:recordNumber!= index");
            //·ÀÖ¹¶Á´i
            return SMSDO_FAIL;
        }
        if((msglen+SMSStore_p->MsgLen)>=SMS_SHORT_MSG_LEN*MAX_SMS_CONT_NUM)
        {
            SMSStore_p->MsgLen = SMS_SHORT_MSG_LEN*MAX_SMS_CONT_NUM - msglen -1;
        }
        memcpy(&(msgdata[msglen]),SMSStore_p->MsgData,SMSStore_p->MsgLen);
        msglen += SMSStore_p->MsgLen;
        if( SMSStore_p->recordNumber == recordNumber)
        {
            *smsstate = SMSStore_p->SmsSate;
        }
        if(IsPreview == FALSE)
        {
            if(SMSStore_p->SmsSate == SMS_RECORD_RECEIVED_UNREAD)
            {
                //eèÎªÒn¶Á£¬²¢±£´æ¼°¸üÐÂË÷Òý
                SMSStore_p->SmsSate = SMS_RECORD_RECEIVED_READ;
                TP_FileWrite(filename, (uint8*)readdata, sizeof(readdata));
                if(SMSStore_p->Box == SMS_BOX_INBOX )
                {
                    IsUnReadSMS = TRUE;
                }
                //Ë÷ÒýeÐÎ´Ð´
                //SMSIndex.SMSInfo[SMSStore_p->recordNumber-1].SmsSate = SMSStore_p->SmsSate;
            }
        }
        else
        {
            if(tempSmsLength >= SMSStore_p->MsgLen)
            {
                tempSmsLength -= SMSStore_p->MsgLen;
            }
            else
            {
                tempSmsLength = 0;
            }
            if(tempSmsLength == 0)
            {
                break;
            }
        }
        //Èç¹û¶ÌÐÅÎ´¶Áiê£¬¼ÌÐø¶ÁÏÂÒ»Ìõ
        if(SMSStore_p->ConcatNextIndex == index)
        {
            break;
        }
        else
        {
            index = SMSStore_p->ConcatNextIndex;
        }
    }
    while(SMSStore_p->ConcatCurrent != SMSStore_p->ConcatMax);

    if(IsPreview == FALSE)
    {
        *SmsLength = msglen;
        memcpy(Sms,msgdata,msglen);
        if( IsUnReadSMS==TRUE &&  SMSIndex.UnreadRecords > 0)
        {
            SMSIndex.UnreadRecords--;
            TP_FileWrite(SMSINDEX_FILE_NAME,(uint8*)&SMSIndex,sizeof(TP_SMSIndex));//±£´æË÷ÒýÎÄ¼þ
        }
    }
    else
    {
        if(*SmsLength < msglen)
        {
            //msglen = *SmsLength;
            memcpy(Sms,msgdata,*SmsLength);
        }
        else
        {
            memcpy(Sms,msgdata,msglen);
        }
    }
    if(strlen((char *)SMSStore_p->Phone)>= SMS_MAX_ADDR_LEN)
    {
        memcpy(Phone,SMSStore_p->Phone,SMS_MAX_ADDR_LEN);
    }
    else
    {
        memcpy(Phone,SMSStore_p->Phone,strlen((char *)SMSStore_p->Phone));
    }
    memcpy(DateTime,&(SMSStore_p->DateTime),sizeof(TP_DateTime));
    IsPreview = FALSE;
    return SMSDO_SUCCESS;
}

TP_SMSDOResult TP_SMSPreview(uint16 recordNumber,
                                  uint8 *Phone,
                                  uint8 *Sms,
                                  uint16 PreviewLength,
                                  TP_SMSRecordState *smsstate,
                                  TP_DateTime *DateTime)
{
    uint16 PreviewLen = PreviewLength;
    IsPreview = TRUE;
    return TP_SMSRead(recordNumber,Phone,Sms,&PreviewLen,smsstate,DateTime);
}

TP_SMSDOResult TP_SMSDel(uint16 recordNumber)
{
    char filename[17];
    TP_SMSStore readdata[SMSSTORE_MAX_EACH_FILE];
    TP_SMSStore *SMSStore_p;
    uint16 index = recordNumber;
    uint16 ConcatCurrent=0,ConcatMax=0;

    if(index>MAX_SMS_STORE_ALL)
    {
        return SMSDO_NONEXIST;
    }

    do
    {
        //sendTcpStringToSerialPort("==del index==",strlen((char *)"==del index=="), 0);
        //sendTcpStringToSerialPort(&index,1, 0);

        memset(readdata,0,sizeof(readdata));
        memset(filename,0,sizeof(filename));
        TP_SMSGetStoreFileName(index, filename);
        //sendTcpStringToSerialPort("==file name==",strlen((char *)"==file name=="), 0);
        //sendTcpStringToSerialPort(filename,strlen((char *)filename), 0);
        if(TP_FExist(filename)==0)
        {
            //sendTcpStringToSerialPort("==6==",strlen((char *)"==6=="), 0);
            return SMSDO_FAIL;
        }

        TP_FileRead(filename, (uint8*)readdata, sizeof(readdata));
        SMSStore_p = &(readdata[SMS_POS_ON_FILE(index)]);
        if(SMSStore_p->recordNumber != index)
        {
            //sendTcpStringToSerialPort("==7==",strlen((char *)"==7=="), 0);
            //·À´i
            return SMSDO_FAIL;
        }
        if(SMSStore_p->ConcatFirsIndex != recordNumber)
        {
            //sendTcpStringToSerialPort("==8==",strlen((char *)"==8=="), 0);
            //·À´i
            return SMSDO_FAIL;
        }
        ConcatCurrent = SMSStore_p->ConcatCurrent;
        ConcatMax = SMSStore_p->ConcatMax;
        //¸üÐÂË÷Òý
        memset(&(SMSIndex.SMSInfo[index-1]),0,sizeof(TP_SMSInfo));
        SMSIndex.IsRecordUsed[index-1] = FALSE;
        //¼ÌÐø¶ÁÏÂÒ»Ìõ
        if(SMSStore_p->ConcatNextIndex == index)
        {
            if(SMSStore_p->Box == SMS_BOX_INBOX
                && SMSStore_p->SmsSate == SMS_RECORD_RECEIVED_UNREAD
                && SMSIndex.UnreadRecords>0)
            {
                SMSIndex.UnreadRecords--;
            }
            memset(SMSStore_p,0,sizeof(TP_SMSStore));
            TP_FileWrite(filename, (uint8*)readdata, sizeof(readdata));
            break;
        }
        else
        {
            index = SMSStore_p->ConcatNextIndex;
            memset(SMSStore_p,0,sizeof(TP_SMSStore));
            TP_FileWrite(filename, (uint8*)readdata, sizeof(readdata));
        }
    }
    while(ConcatCurrent != ConcatMax);

    TP_FileWrite(SMSINDEX_FILE_NAME,(uint8*)&SMSIndex,sizeof(TP_SMSIndex));//±£´æË÷ÒýÎÄ¼þ
    return SMSDO_SUCCESS;

}

//¸üÐÂË÷ÒýÎÄ¼þ
//void TP_UpdateSMSIndexFile()
//{
//}

//¸ù¾ÝÐÅÏäÁÐ³ö¶ÌÐÅË÷ÒýºÅ
TP_SMSDOResult TP_ListIndexByBox(TP_SMSBox Box,
                                       uint16 *IndexArray,
                                       uint16 IndexMaxCount,
                                       uint16 *SmsCount)
{
    uint16 tempi,tempj;
    //uint16 indexTemp[MAX_SMS_STORE];
    uint16 count = 0;
    //TP_DateTime DateTime[MAX_SMS_STORE];
    TP_DateTime *DateTime;
    TP_DateTime dateTime1,dateTime2;
    uint16 index1,index2;
    int16 result = 0;

    TP_AllocMemory(sizeof(TP_DateTime)*IndexMaxCount, (void**)&DateTime);
    //memset(indexTemp,0,sizeof(indexTemp));
    memset(DateTime,0,sizeof(TP_DateTime)*IndexMaxCount);
    //²en¯
    for(tempi=0;tempi<MAX_SMS_STORE_ALL;tempi++)
    {
        //if(SMS_BOX_ALL != Box)
        {
            if(SMSIndex.SMSInfo[tempi].Box != Box)
            {
                continue;
            }
        }
        if( SMSIndex.IsRecordUsed[tempi] >0
            && (SMSIndex.SMSInfo[tempi].FirstIndex == (tempi+1)))
        {
            IndexArray[count] = SMSIndex.SMSInfo[tempi].FirstIndex;
            memcpy(&(DateTime[count]),&(SMSIndex.SMSInfo[tempi].DateTime),sizeof(TP_DateTime));
            count++;
            if(count >= IndexMaxCount)
            {
                break;
            }
        }
    }
    //ÅÅÐò
    for(tempj=0;tempj<(count - 1);tempj++)
    {
        for(tempi=0;tempi<count-tempj-1;tempi++)
        {
            dateTime1 = DateTime[tempi];
            dateTime2 = DateTime[tempi+1];
            index1 = IndexArray[tempi];
            index2 = IndexArray[tempi+1];

            result = TP_CmpDataTime(&dateTime1, &dateTime2);

            if(result < 0 )
            {
                //¶Ôµ÷
                DateTime[tempi] = dateTime2;
                DateTime[tempi+1] = dateTime1;
                IndexArray[tempi] = index2;
                IndexArray[tempi+1] = index1;
            }
        }
    }
    //memcpy(IndexArray,indexTemp,count*sizeof(uint16));
    *SmsCount = count;
    TP_FreeMemory((void**)&DateTime);
    return SMSDO_SUCCESS;
}

//²»·Ö³¤¶ÌÐÅ
uint16 TP_GetFlashSMSAllCount(TP_SMSBox Box)
{
    uint16 tempi;
    uint16 count = 0;
    for(tempi=0;tempi<MAX_SMS_STORE_ALL;tempi++)
    {
        if( SMSIndex.IsRecordUsed[tempi] >0
            && SMSIndex.SMSInfo[tempi].Box == Box)
        {
            count++;
        }
    }
    return count;
}

//·Ö³¤¶ÌÐÅ
uint16 TP_GetFlashSMSCount(TP_SMSBox Box)
{
    uint16 tempi;
    uint16 count = 0;
    for(tempi=0;tempi<MAX_SMS_STORE_ALL;tempi++)
    {
        if( SMSIndex.IsRecordUsed[tempi] >0
            && SMSIndex.SMSInfo[tempi].Box == Box
            && (SMSIndex.SMSInfo[tempi].FirstIndex == (tempi+1)))
        {
            count++;
        }
    }
    return count;

}


//¶ÁÈ¡ÊÕ¼þÏäµÄÎ´¶Á¶ÌÐÅÌõÊý
uint16 TP_GetFlashUnReadSMSCount()
{
    return SMSIndex.UnreadRecords;
}

//¶ÁÈ¡flash¿e´æ¶ÌÐÅÌõÊý(ÈÝÁ¿)
uint16 TP_GetFlashSMSMemorySize()
{
    return MAX_SMS_STORE_ALL;
}

