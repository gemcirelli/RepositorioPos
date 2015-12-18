/*******************************************************************************
 * �ļ�λ��: TP_ParamsAnalyser.c
 * �ļ�˵��: �����o�ر���API��ʵ��
 * �������u: 2012��02��26��
 * ������        : ��ҫȪ 
 * �޸����u:
 * �޸�˵��:
 * �޸����u:
 ******************************************************************************/

 /************************************************************************
 *�����o����
 *��׼�nʽ
1. �ļ��洢�nʽ:
______________________________________________________
|SysParamsHeadInfo|Data+Crc|Data+Crc|Data+Crc|Data+Crc|
______________________________________________________
|Data+Crc|.............                               |
|_____________________________________________________|

2. SysParamsHeadInfo
______________________________________________________
|'T'|'P'|Ver|Cmp|Enc|Count|     23byte       |Crc     |
|_____________________________________________________|

3.Data
______________________________________________________
|type|ssize|DataBuffer                       |Crc     |
|_____________________________________________________|

��һ�������ļ��洢�uʱ���e�������������ļ�
TP_PARAM_FILE_1��TP_PARAM_FILE_2��TP_PARAM_FILE_3��TP_PARAM_FILE_4
//////////////////////////////////////////////////////////////
*************************************************************************/


 /************************************************************************
2012.03.04 
    ���ݵײ�����ļ�֧��ʵ�����ݴoС�洢,������o��
���i�����޸�Ϊ:

1. ���u��ʼ��
    a. �aTP_PARAM_FILE_1,���ai���Ϣ�e�����oΪ������o�ʵ��o�Ч����;
,ִ�в���b;�n��ִ�в���c;
    b.��n��aȡ�ĸ������ļ�,��i���Ϣ�����uԤ��a������,���a
ȡ��������;ֱ���aȡi��;��ִ�в���d;
    c.��Ĭ�ϲ������д洢Ԥ��,������o�Ĭ�ϲ��������a�����ļ���;
    d.���洢Ԥ��(ʵ�ʼo�صĲ�����a�),��Ԥ�����a�Сoue�����
������a�ʱ,��δ�o�صĲ������浽�����ļ���;

2. �洢����
    a.����������ʱ,�o�洢Ԥ�����n��id�Ĵ洢λ��,�����Ŀ�������ļ�
�Ƿno��㹻�Ŀռ䱣���������;���㹻����д��ǰ������;������,����
д�o��id��ʼ�o����oв�������;
    ���e�o��id�oȫ����д,�e��o�Ч�i��oò����ļ�.



�䳤���ݴ洢���u��o���:
1. �Ե��������ļ���o���,�e����e�޷��洢��ԭ�������ݳ���Ҫ���Ĳ���;

2. ��ȫ�������ļ���o���,�e�ܻa��д�ò����Ժo����oв���;

3.д��Ĳ������ݹ���,�a��e�o�沿�ֲ����޿ռ�洢

��e�������;

*************************************************************************/
//#include "TPTypedef.h"
#include "TP_ParamsManager.h"
#include "TP_ParamsAnalyser.h"
#include "TP_Public.h"
#include "TPBase.h"
#include "TPFile.h"
#include "TPLib.h"
#include "TPList.h"
#include "TPSerial.h"
#include "TP_DebugTools.h"

void TP_DbgSerialPrnHexEx(uint8 dbLev,uint8* inBuf, uint16 inLen);
//NAME_PARAMS(NULL)ռo�һ��ƫ��a�
#define PARAMSID_NULL_OFFSET 1

//�������ii���Ϣ�����ݰ汾��
#pragma pack (1)
//#ifndef WIN32
//__packed    
//#endif
typedef struct _SysParamsHeadInfo
{
    //  2 + 1 + 1 + 4 + 22 + 2 = 32 
    uint8 tpflag[2];        //"TP", ��ʶ��
    uint8 version;          //��eϼ���;
    uint8 compress;         //0 δn���, 1 n���;
    uint32 itemcount;       //������a�>=0

    uint8  exData[22];          //Ԥa���չ
    uint16 crc;             //Уneֵ
}SysParamsHeadInfo;

//��ǰϵi������ļ���׼i���Ϣ
static const SysParamsHeadInfo tpSysSTDParamsHeadInfo = {
    {'T', 'P'},
        1,
        0,
        //NAME_PARAMS(NULL)����Чid,o�Ч��Χ(NAME_PARAMS(NULL), NAME_PARAMS(COUNT))
        NAME_PARAMS(COUNT)-PARAMSID_NULL_OFFSET 
};

/**
 * �� �� TP_LoadParamsFromFile() �����������o�ļ��o�ز���
 * @return�e������0
 * @author  ��ҫȪ
 * @date 20120224
 * @note 
 */
#define MAX_PARAMS_FILE_SIZE 1024

typedef struct _SysParamsFileInfo
{
    uint32 useFlag;             //�Ƿn��ʹo�
    uint32 sysFileIdle;         //Ԥ��ռoõ��ļ�id
    uint32 useSize;             //Ԥ��ռoõ��ܿռ�
    uint32 startIndex;          //Ԥ�⿪ʼ�Ĳ���id
    uint32 endIndex;                //Ԥ��a���Ĳ���id
    SysParamsHeadInfo headInfo; //Ԥ�⽫Ҫ��Ҫ�aд��i���Ϣ
}SysParamsFileInfo;

#pragma pack ()

/*
��n��aȡ��o��Ĳ����ļ�i���Ϣ
*/
static SysParamsFileInfo tpSysParamsFileInfos[4]  = {
    {FALSE, TP_PARAM_FILE_1},
    {FALSE, TP_PARAM_FILE_2},
    {FALSE, TP_PARAM_FILE_3},
    {FALSE, TP_PARAM_FILE_4}
};

#ifdef WIN32
#define TPWriteParam TP_WriteParam
#else
#define TPWriteParam TP_WriteParamEx
#endif

extern void TP_Printf_ParamsManager_Info();
extern void TP_Check_ParamsManager_Info();

extern uint32 Get_Data_Write_Size(uint32 dataBuf_maxSize);

extern int32 TP_Set_ParamsItem_Status(PARAMS_NAMES paramsId, DataStatus dStatus);

extern int32 TP_Update_ParamsItem_Val(PARAMS_NAMES paramsID,
                                        Data* pVal, DataStatus dStatus);

static int32 TP_Load_Params_HeadInfo(uint32 fileIndex,
                                        SysParamsHeadInfo *headInfo);
static int32 TP_Load_ParamsItem_From_File(uint32 fileIndex,
                                        SysParamsHeadInfo *headInfo,
                                        PARAMS_NAMES baseParamsId);
static int32 TP_Check_And_Save_Params_HeadInfo(uint32 fileIndex, 
                                                        SysParamsHeadInfo *headInfo);

int32 ParamsItem_Data_To_Buf(Data *data, uint8* writebuf, uint32 buflen);

static int32 TP_Save_Nth_SysParamsFile(uint32 nfileIndex);

static int32 TP_SaveDefaultParamsToFile();
/**
 * �� �� TP_ScanParamsItem() ����������Ԥ������洢λ��
 * @return�e������0
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 */
static int32 TP_ScanParamsItem(Boolean saveDefault)
{
    int32 ret;
    Data data;
    int32 data_len, fileIndex = 0;

    PARAMS_NAMES paramsId = NAME_PARAMS(NULL);
    uint32 count = TP_Get_ParamsItem_Length();
    
    if(count <= 0)
        return 0;
    
    for(fileIndex = TP_PARAM_FILE_1; fileIndex <= TP_PARAM_FILE_4; fileIndex++)
    {
        tpSysParamsFileInfos[fileIndex].useFlag = FALSE;

        //tpSysParamsFileInfos[fileIndex].useSize = 0;
        tpSysParamsFileInfos[fileIndex].useSize = SYSPARAMSHEADINFO_LEN;
        
        tpSysParamsFileInfos[fileIndex].startIndex = 0;
        tpSysParamsFileInfos[fileIndex].endIndex = 0;
        
        memset(&tpSysParamsFileInfos[fileIndex].headInfo, 0, sizeof(SysParamsHeadInfo));
        memcpy(&tpSysParamsFileInfos[fileIndex].headInfo, &tpSysSTDParamsHeadInfo, sizeof(SysParamsHeadInfo));
        tpSysParamsFileInfos[fileIndex].headInfo.itemcount = 0;
    }

    fileIndex = 0;
    tpSysParamsFileInfos[fileIndex].startIndex = NAME_PARAMS(NULL)+PARAMSID_NULL_OFFSET;

    for(paramsId=(PARAMS_NAMES)(NAME_PARAMS(NULL)+PARAMSID_NULL_OFFSET);
        paramsId<(PARAMS_NAMES)NAME_PARAMS(COUNT); paramsId++)
    {
        if(saveDefault == TRUE)
            ret = TP_Get_ParamsItem_DefaultVal(paramsId, &data);
        else
            ret = TP_Get_ParamsItem_Val(paramsId, &data);
            
        if(ret == 0)
        {
            data_len =(int32) Get_Data_Write_Size(data.ssize);
            
            //if((tpSysParamsFileInfos[fileIndex].useSize + data_len) <= (MAX_PARAMS_FILE_SIZE-SYSPARAMSHEADINFO_LEN))
            if((tpSysParamsFileInfos[fileIndex].useSize + data_len) <= (MAX_PARAMS_FILE_SIZE))
            {
                tpSysParamsFileInfos[fileIndex].useFlag = TRUE;
                tpSysParamsFileInfos[fileIndex].useSize += data_len;
                tpSysParamsFileInfos[fileIndex].endIndex = paramsId;
                tpSysParamsFileInfos[fileIndex].headInfo.itemcount++;
            }
            else
            {   
                //��д����u�ݳ��������ļ�sizeʱ��д��һ�������ļ�
                fileIndex++;
                //if(fileIndex<=TP_PARAM_FILE_4 && data_len <= (MAX_PARAMS_FILE_SIZE-SYSPARAMSHEADINFO_LEN))
                if(fileIndex<=TP_PARAM_FILE_4 && data_len <= (MAX_PARAMS_FILE_SIZE))
                {
                    tpSysParamsFileInfos[fileIndex].useFlag = TRUE;
                    tpSysParamsFileInfos[fileIndex].useSize = data_len;
                    tpSysParamsFileInfos[fileIndex].startIndex = paramsId;
                    tpSysParamsFileInfos[fileIndex].endIndex = paramsId;
                    tpSysParamsFileInfos[fileIndex].headInfo.itemcount++;
                    
                    #if defined(_DEBUG) && defined(WIN32)
                    ShowDebug("New Paramsfile index : %d, size : %d, datalen : %d, start : %d, end : %d, count : %d\r\n",
                            fileIndex,
                            tpSysParamsFileInfos[fileIndex].useSize, data_len,
                            tpSysParamsFileInfos[fileIndex].startIndex,
                            tpSysParamsFileInfos[fileIndex].endIndex,
                            tpSysParamsFileInfos[fileIndex].headInfo.itemcount);
                    #endif
                    
                }
                else
                {
                    #if defined(_DEBUG) && defined(WIN32) 
                    if(data_len > (MAX_PARAMS_FILE_SIZE-SYSPARAMSHEADINFO_LEN))
                        ShowDebug("Not enough databuf : %d, %d, %d\r\n", MAX_PARAMS_FILE_SIZE, SYSPARAMSHEADINFO_LEN, data_len);
                    ShowDebug("Too much params : %d, left : %d\r\n", count, count-paramsId);
                    #endif
                    
                    return -1;
                }
            }
        }
        else
        {
            #if defined(_DEBUG) && defined(WIN32) 
            ShowDebug("Params Error get ret : %d, curid: %d, idcount : %d\r\n",
                    ret,
                    paramsId,
                    NAME_PARAMS(COUNT)-PARAMSID_NULL_OFFSET);
            #endif
            return -2;
        }
    }

    for(fileIndex = TP_PARAM_FILE_1; fileIndex <= TP_PARAM_FILE_4; fileIndex++)
    {
        uint16 crc = usCRC16((uint8 *)&tpSysParamsFileInfos[fileIndex].headInfo, sizeof(SysParamsHeadInfo)-CRC16_SIZE);
        tpSysParamsFileInfos[fileIndex].headInfo.crc= crc;
    }
        
    return 0;
}

/**
 * �� �� TP_ScanParamsItem() ����������дĬ��ֵʱ,����Ĭ��ֵԤ������洢λ��
 * @return�e������0
 * @author  ��ҫȪ
 * @date 20120305
 * @note 
 */
static int32 TP_SysParamsFilesInfo_Init()
{
    return TP_ScanParamsItem(TRUE);
}

/**
 * �� �� TP_SysParamsFilesInfo_Remap() ����������oи���ʱ,����Ԥ������洢λ��
 * @return�e������0
 * @author  ��ҫȪ
 * @date 20120305
 * @note 
 */
static int32 TP_SysParamsFilesInfo_Remap()
{
    return TP_ScanParamsItem(FALSE);
}
    
/**
 * �� �� TP_LoadParamsFromFile() ����������
 �o�����ļ�Ⱥ�мo�ز���;���o��ʧ�ܣ����Զ��ָ�Ĭ�ϲ���
 * @return�e������0
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
    �u�aȡ������,��eu�����ƥ����e��,o�����o�������дһ�e.
    ��ƥ����e������:
    1.�����ļ�i���Ϣ�aȡʧ��;
    2.��Ҫ�aȡ�Ĳ�������Сoui���Ϣ�����ĸ���ʱ,������. (���u����,o����¸nʽ�������ļ�)
    3.��Ҫ�aȡ�Ĳ��������ooui���Ϣ�����ĸ���ʱ,���¸ò����ļ�.
 */
int32 TP_LoadParamsFromFile(void)
{

    int32 ret = 0;
    uint32 fileIndex = 0, loadCount = 0, allCount = 0;

    SysParamsHeadInfo headInfo;
    PARAMS_NAMES baseParamsId=NAME_PARAMS(NULL);

    #if defined(_DEBUG)
    ShowDebug("TP_LoadParamsFromFile Start : %d\r\n", TP_Get_ParamsItem_Length());
    #endif
    
    for(fileIndex= TP_PARAM_FILE_1; fileIndex<=TP_PARAM_FILE_4; fileIndex++)
    {
        memset(&headInfo, 0, sizeof(headInfo));
        
        ret = TP_Load_Params_HeadInfo(fileIndex, &headInfo);
        if(ret == 0)
        {
            loadCount = TP_Load_ParamsItem_From_File(fileIndex,
                                                    &tpSysParamsFileInfos[fileIndex].headInfo,
                                                    baseParamsId);
            //���o���e�a˲����o,o�����ļ���һ��
            if(loadCount != tpSysParamsFileInfos[fileIndex].headInfo.itemcount)
            {
            #if defined(_DEBUG)
                ShowDebug("load file[%d] count error[load %d, count:%d]\r\n", fileIndex, loadCount, tpSysParamsFileInfos[fileIndex].headInfo.itemcount);
            #endif
            }

            allCount += loadCount;
            
            baseParamsId += (PARAMS_NAMES)tpSysParamsFileInfos[fileIndex].headInfo.itemcount;
            
            if(baseParamsId == tpSysSTDParamsHeadInfo.itemcount)
            {
            #if defined(_DEBUG)
                ShowDebug("ParamsAnalyser load all params over, load count [%d]!\r\n", baseParamsId);
            #endif
                break;
            }
            continue;
        }
        else
        {
            if(fileIndex == TP_PARAM_FILE_1)
                break;
        }
    }

    //ûo��κ������ļ���д��Ĭ��������

    #if defined(_DEBUG) && defined(WIN32)
    TP_Printf_ParamsManager_Info();
    #endif

    if(allCount == 0 && tpSysSTDParamsHeadInfo.itemcount > 0)
    {
        ret = TP_SaveDefaultParamsToFile();
        return ret;
    }
    else
    {
        if(allCount != tpSysSTDParamsHeadInfo.itemcount)
        {
            baseParamsId = (PARAMS_NAMES)(NAME_PARAMS(NULL)+1);
            if(TP_SysParamsFilesInfo_Remap() != 0)
            {
                return -2;
            }
            //ȱ����
            #if defined(_DEBUG)
            ShowDebug("ParamsAnalyser lost item [%d:%d]!\r\n", allCount, tpSysSTDParamsHeadInfo.itemcount);
            #endif
            
            for(; baseParamsId<NAME_PARAMS(COUNT); baseParamsId++)
            {
                if(TP_Is_ParamsItem_StatusException(baseParamsId) == FALSE)
                {
                    ret = TP_Save_ParamsItem_To_File(baseParamsId);
                    #if defined(_DEBUG)
                    ShowDebug("ParamsAnalyser item Status Exception [%d], rewrite to file, ret %d!\r\n",
                                    baseParamsId, ret);
                    #endif
                }
            }
        }
        return ret;
    }
}

/**
 * �� �� TP_Read_HeadInfo() �����������o�ļ��aȡ0 ����(i���Ϣ)
 * @return�e������0
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
    �o��i���Ϣ�e��ʱ���żo�ز�����Ϣ
 */
static int32 TP_Read_HeadInfo(uint32 fileIndex, SysParamsHeadInfo *headInfo)
{
    const int32 headlen = sizeof(SysParamsHeadInfo);
    
    if(fileIndex<TP_PARAM_FILE_1 || fileIndex>TP_PARAM_FILE_4 || headInfo == NULL)
        return -1;

    if(TP_GetItemSize(fileIndex, NAME_PARAMS(NULL)) != headlen)
    {
        return -2;
    }
    if(TP_ReadParam(fileIndex, NAME_PARAMS(NULL), (uint8*)headInfo, headlen) == FALSE)
    {       
        //�������޷�ʶ���i: �o��Ĭ�����ݣ���д��flash
        #if defined(_DEBUG) 
        ShowDebug("read [%d] params head infomation failed\r\n", fileIndex);
        #endif
        
        return -3;
    }
    return 0;
}
/**
 * �� �� TP_Load_Params_HeadInfo() �����������o�ļ��o�ز���i���Ϣ
 * @return�e������0
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
    �o��i���Ϣ�e��ʱ���żo�ز�����Ϣ
 */
static int32 TP_Load_Params_HeadInfo(uint32 fileIndex, SysParamsHeadInfo *headInfo)
{
    int32 ret;
    
    ret = TP_Read_HeadInfo(fileIndex, headInfo);

    if(ret < 0)
        return ret;
    
    if(TP_Check_And_Save_Params_HeadInfo(fileIndex, headInfo) < 0)
    {
        return (ret-1);
    }
    else
        return 0;
}

/**
 * �� �� TP_Check_Params_HeadInfo() �������������i���Ϣ�Ƿn����
 * @return�e������0
 * @author  ��ҫȪ
 * @date 20120229
 * @note 
    ��ei���Ϣ�ĺϷ���,�������ݴ��i
 */
static int32 TP_Check_And_Save_Params_HeadInfo(uint32 fileIndex, 
                                                        SysParamsHeadInfo *headInfo)
{
    uint16 crc;
    int32 i = 0, allItemCount = 0;
#if defined(_DEBUG) && defined(WIN32)
    uint8 buf1[3], buf2[3];
#endif
    if(fileIndex<TP_PARAM_FILE_1 || fileIndex>TP_PARAM_FILE_4)
    {
        return -1;
    }

    //check dataset infomation
    crc = usCRC16((uint8*)headInfo, sizeof(SysParamsHeadInfo)-CRC16_SIZE);
    if(crc != headInfo->crc)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("read params head infomation crc16 check failed[%04x:%04x]\r\n", crc, headInfo->crc);
        #endif
        return -2;
    }
    
    if(memcmp(&headInfo->tpflag,
            &tpSysSTDParamsHeadInfo.tpflag,
            sizeof(tpSysSTDParamsHeadInfo.tpflag)) !=0 )
    {
        #if defined(_DEBUG) && defined(WIN32)
        memset(buf1, 0, 3);
        memset(buf2, 0, 3);
        memcpy(buf1, &headInfo->tpflag, 2);
        memcpy(buf2, &tpSysSTDParamsHeadInfo.tpflag, 2);
        ShowDebug("headInfo flag error [%s, %s]\r\n", buf1, buf2);
        #endif
        return -3;
    }

    if(headInfo->version > tpSysSTDParamsHeadInfo.version)
    {
        //���ݰ汾���ߣ���ǰ�����޷����i
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("headInfo version error [%d, %d]\r\n", headInfo->version, tpSysSTDParamsHeadInfo.version);
        #endif
        return -4;
    }
    
    if(headInfo->compress > tpSysSTDParamsHeadInfo.compress)
    {
        //���u�޷����i��n�������
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("headInfo compress error [%d, %d]\r\n", headInfo->compress, tpSysSTDParamsHeadInfo.compress);
        #endif
        return -5;
    }

    if(headInfo->itemcount<=0)
        return -6;
    
    allItemCount = 0;
    for(i = 0; i <fileIndex; i++)
    {
        allItemCount+= tpSysParamsFileInfos[i].headInfo.itemcount;
    }
    
    //��¼�ļ��洢��Ϣ
    memcpy(&tpSysParamsFileInfos[fileIndex].headInfo, &tpSysSTDParamsHeadInfo, sizeof(tpSysSTDParamsHeadInfo));

    tpSysParamsFileInfos[fileIndex].useFlag = TRUE;
    tpSysParamsFileInfos[fileIndex].sysFileIdle= fileIndex;
    tpSysParamsFileInfos[fileIndex].headInfo.compress = headInfo->compress;
    tpSysParamsFileInfos[fileIndex].headInfo.itemcount = headInfo->itemcount;

    if((headInfo->itemcount + allItemCount)> tpSysSTDParamsHeadInfo.itemcount)
    {
        //�ļ����u����޷����i�Ĳ�����Ŀ
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("warning headInfo itemcount [%d, %d]\r\n", headInfo->itemcount+allItemCount, tpSysSTDParamsHeadInfo.itemcount);
        #endif
        
        tpSysParamsFileInfos[fileIndex].headInfo.itemcount = tpSysSTDParamsHeadInfo.itemcount-allItemCount;
    }

    if((headInfo->itemcount + allItemCount) < tpSysSTDParamsHeadInfo.itemcount)
    {
        //������a���ou�ļ��洢��a�
        //��ʵ�ʴ洢��a��o�أ�������ȾĬ������
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("headInfo itemcount warning [%d, %d]\r\n", headInfo->itemcount+allItemCount, tpSysSTDParamsHeadInfo.itemcount);
        #endif
        tpSysParamsFileInfos[fileIndex].headInfo.itemcount = headInfo->itemcount;
    }

    if(fileIndex == 0)
    {
        tpSysParamsFileInfos[fileIndex].startIndex = 1;
        tpSysParamsFileInfos[fileIndex].endIndex = tpSysParamsFileInfos[fileIndex].startIndex + tpSysParamsFileInfos[fileIndex].headInfo.itemcount- 1;
    }
    else
    {
        tpSysParamsFileInfos[fileIndex].startIndex = tpSysParamsFileInfos[fileIndex-1].endIndex + 1;
        tpSysParamsFileInfos[fileIndex].endIndex = tpSysParamsFileInfos[fileIndex].startIndex + tpSysParamsFileInfos[fileIndex].headInfo.itemcount - 1;
    }
    
    #if defined(_DEBUG) && defined(WIN32)
    memset(buf1, 0, 3);
    memcpy(buf1, &headInfo->tpflag, 2);
    
    ShowDebug("headInfo INFO :[tpflag:%s, ver:%d, cmp:%d, count:%d, start:%d, end:%d]\r\n",
        buf1,
        headInfo->version,
        headInfo->compress,
        headInfo->itemcount,
        tpSysParamsFileInfos[fileIndex].startIndex,
        tpSysParamsFileInfos[fileIndex].endIndex);
    #endif

    return 0;
}


/**
 * �� �� TP_Load_ParamsItem_From_File() �����������oָ���Ĳ����ļ��мo�ز���
 * @param fileIndex,Ҫ�aȡ�Ĳ����ļ�����
 * @param headInfo , �����ļ�i���Ϣ
 * @param baseParamsId, ���浽���i�Ļ�����id,o�e����ʵ�Ĳ���id
 * @return ����o�Ч�Ĳ���a�
 * @author  ��ҫȪ
 * @date 20120229
 * @note 
*/
static int32 TP_Load_ParamsItem_From_File(uint32 fileIndex, SysParamsHeadInfo *headInfo, PARAMS_NAMES baseParamsId)
{
    Data *pData;
    int32 count = 0;
    int32 itemNum = 0, dataSize = 0, itemLen = 0;

    uint8 readBuf[256];
    const int32 BUF_LEN=sizeof(readBuf);
    
    pData = (Data*)readBuf;

    if(!headInfo || fileIndex<TP_PARAM_FILE_1  || fileIndex>TP_PARAM_FILE_4)
        return -1;

    #if defined(_DEBUG) && defined(WIN32)
    ShowDebug("Start load paramsitem from paramsfile : %d\r\n", fileIndex);
    #endif
    //load params items
    //index = 0 �n��i���Ϣռo�
    tpSysParamsFileInfos[fileIndex].useSize = SYSPARAMSHEADINFO_LEN;
    for(itemNum=1 ; itemNum<=headInfo->itemcount; itemNum++)
    {
        itemLen = TP_GetItemSize(fileIndex, itemNum);
        if(itemLen <= DATA_HEAD_LEN)
        {
            #ifdef _DEBUG
            ShowDebug("Error itemsize error index : %d, size [%d, %d]\r\n",
                            itemNum, itemLen, DATA_HEAD_LEN);
            #endif
            
            TP_Set_ParamsItem_Status((PARAMS_NAMES)(baseParamsId+itemNum), Status_FileErr);
            continue;
        }
        memset(readBuf, 0, BUF_LEN);
        if(TP_ReadParam(fileIndex, itemNum, readBuf, itemLen) == FALSE)
        {
            #ifdef _DEBUG
            ShowDebug("Error read item [%d] error, read return false \r\n", itemNum);
            #endif
            
            TP_Set_ParamsItem_Status((PARAMS_NAMES)(baseParamsId+itemNum), Status_FileErr);
            continue;
        }
        else
        {
            
            dataSize = Get_Data_Write_Size(pData->ssize);
            if(dataSize != itemLen)
            {
                #ifdef _DEBUG
                ShowDebug("Error item [%d] check lenght error [%d, %d] \r\n", itemNum, itemLen, dataSize);
                #endif
                
                TP_Set_ParamsItem_Status((PARAMS_NAMES)(baseParamsId+itemNum), Status_FileErr);
                continue;
            }
            else
            {
                //crc check
                uint16 crcChcek, crcVal;

                tpSysParamsFileInfos[fileIndex].useSize += dataSize;                

                crcChcek = usCRC16(readBuf, dataSize-CRC16_SIZE);
                
                memcpy(&crcVal, readBuf+(dataSize-CRC16_SIZE), CRC16_SIZE);
                                
                if(crcChcek != crcVal)
                {
                    #ifdef _DEBUG
                    ShowDebug("Error item [%d] crcChcek error [%04x:%04x] \r\n",
                            itemNum,
                            crcChcek,
                            crcVal);
                    #endif
                    
                    TP_Set_ParamsItem_Status((PARAMS_NAMES)(baseParamsId+itemNum), Status_CrcErr);

                    
                    continue;
                }
                readBuf[dataSize-CRC16_SIZE] = '\0';
                readBuf[dataSize-1] = '\0';

                switch(pData->valueType)
                {
                case VAT_Int32:
                    count++;
                    #if defined(_DEBUG) && defined(WIN32)
                    ShowDebug("read item [baseParamsId : %d, readlen : %d, type: %d, ssize : %d,  data : %d, file use : %d byte] \r\n",
                            baseParamsId+itemNum,
                            dataSize,
                            pData->valueType,
                            pData->ssize,
                            pData->dBuffer.nVal,
                            tpSysParamsFileInfos[fileIndex].useSize);
                    #endif
                    TP_Update_ParamsItem_Val((PARAMS_NAMES)(baseParamsId+itemNum), pData, Status_User);
                    break;
                case VAT_String:
                    count++;
                    #if defined(_DEBUG) && defined(WIN32)
                    ShowDebug("read item [baseParamsId : %d, readlen : %d, type: %d, ssize : %d,  data : %s,  file use : %d byte] \r\n",
                            baseParamsId+itemNum,
                            dataSize,
                            pData->valueType,
                            pData->ssize,
                            pData->dBuffer.cBuffer,
                            tpSysParamsFileInfos[fileIndex].useSize);
                    #endif
                    TP_Update_ParamsItem_Val((PARAMS_NAMES)(baseParamsId+itemNum), pData, Status_User);
                    break;
                case VAT_Stream:
                    count++;
                    #if defined(_DEBUG) && defined(WIN32)
                    ShowDebug("read item [baseParamsId : %d, readlen : %d, type: %d, ssize : %d, file use : %d byte] \r\n",
                            baseParamsId+itemNum,
                            dataSize,
                            pData->valueType,
                            pData->ssize,
                            tpSysParamsFileInfos[fileIndex].useSize);
                    #endif
                    TP_Update_ParamsItem_Val((PARAMS_NAMES)(baseParamsId+itemNum), pData, Status_User);
                    break;
                default:
                    #if defined(_DEBUG) && defined(WIN32)
                    ShowDebug("read item unknow valueType [type : %d, len : %d] \r\n", pData->valueType, pData->ssize);
                    #endif
                    break;
                }
            }
        }
    }
    return count;
}

/**
 * �� �� TP_Write_ParamsFileHeadInfo() ����������д������ļ�i���Ϣ
 * @return > 0 �e������д�볤��,
        <=0 ʧ�ܷ���;
 * @author  ��ҫȪ
 * @date 20120326
 * @note 

 */
static int32 TP_Write_ParamsFileHeadInfo(int32 paramFileIndex, SysParamsHeadInfo *headInfo)
{
    uint32 dataLen = 0;

    if(paramFileIndex<TP_PARAM_FILE_1 || paramFileIndex>TP_PARAM_FILE_4)
        return -1;
    if(!headInfo)
        return -2;

    //rewrite fileHeadInfo
    dataLen = sizeof(SysParamsHeadInfo);

    if(TPWriteParam(paramFileIndex, NAME_PARAMS(NULL),
          (uint8*)headInfo, dataLen) == FALSE)
    {
        int32 ret = TP_GetLastError();
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("TP_Write_ParamsFileHeadInfo [file %d, count : %d], error : %d\r\n",
                paramFileIndex,
                headInfo->itemcount,
                ret);
        #endif
        return -3;
    }
    else
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("TP_Write_ParamsFileHeadInfo succeed : [file %d, count : %d]\r\n",
                paramFileIndex,
                headInfo->itemcount);
        #endif
    }

    return dataLen;
}

/**
 * �� �� TP_Write_ParamsItem() ����������д���������
 * @return > 0 �e������д�볤��,
        <=0 ʧ�ܷ���;
 * @author  ��ҫȪ
 * @date 20120326
 * @note 

 */
static int32 TP_Write_ParamsItem(PARAMS_NAMES paramsId, int32 paramFileIndex, int32 itemIndex)
{
    int32 ret;
    Data data;
    uint32 dataLen = 0;
    uint8 writeBuf[256];
    const uint32 BUF_LEN = sizeof(writeBuf);
    if(paramFileIndex<TP_PARAM_FILE_1 || paramFileIndex>TP_PARAM_FILE_4)
        return -1;

    ret = TP_Get_ParamsItem_Val(paramsId, &data);
    if(ret != 0)
    {
        #ifdef _DEBUG
        ShowDebug("TP_Write_ParamsItem : get item data error  [%d, %d]\r\n ", 
                paramsId, ret);
        #endif
        return -2;
    }

    memset(writeBuf, 0, sizeof(writeBuf));
    dataLen = ParamsItem_Data_To_Buf(&data, writeBuf, BUF_LEN);
    if(dataLen<=0)
        return 0;

    if(TPWriteParam(paramFileIndex, itemIndex,
                (const uint8*)writeBuf, dataLen) == FALSE)
    {
        int32 ret = TP_GetLastError();
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("TP_Write_ParamsItem       witer file [%d], item [%d] error : %d\r\n",
                paramFileIndex, paramsId, ret);
        #endif
        return -3;
    }
    //#ifdef WIN32
    #if defined(_DEBUG) && defined(WIN32)
    ShowDebug("TP_Write_ParamsItem       witer file [%d], [%d] on item [%d] SUCCEED \r\n",
            paramFileIndex, paramsId, itemIndex);
    #endif
    return dataLen;
}


/**
 * �� �� TP_Clear_ParamsItem() ���������������������
 * @return�e������0,
        ʧ�ܷ���-1;
 * @author  ��ҫȪ
 * @date 20120326
 * @note 

 */
static int32 TP_Clear_ParamsFileItem(int32 paramFileIndex, int32 itemIndex)
{
    if(TPWriteParam(paramFileIndex, itemIndex,
                    (const uint8*)"", 0) == FALSE)
    {
        /*
            #if defined(_DEBUG) && defined(WIN32)
        int32 ret = TP_GetLastError();
        ShowDebug("TP_Clear_ParamsItem       clear file [%d], item [%d] error : %d\r\n",
                paramFileIndex, itemIndex, ret);
        #endif
        */
        #ifdef WIN32
            return 0;
        #else
            return -1;
        #endif
    }
    return 0;
}
/**
 * �� �� TP_Save_AllParamsItem_To_File() �������������������浽�ļ�
 * @return�e������0;
        -1: �����u�����ļ���λʧ��;;
        -2: �����u�����ļ��i�����i�����u(�����ļ����ݳ���Сou0,�������u����a��i�u);
        -3: д������flash��ˢ��flash�eʧ��;
        -4: Ԥ��洢����ʧ��(д��ռ䲻��);
 * @author  ��ҫȪ
 * @date 20120327
 * @note 
    �ȶ�λ�oд��
    oeou�ײ�eoÿeд��,��д��ʧ��,��e����eeϲ�����o�ײ����ݲ�i���
 */
static SysParamsHeadInfo tmpHeadInfo_save;
static SysParamsFileInfo tmpParamsFileInfo_save[4];

int32 TP_Save_ParamsItem_To_File(PARAMS_NAMES paramsId)
{
    /*************************************************************
    1. ����ǰ����д����ļ���u����eu�仯,ֱ�oд��
    2. ����ǰ����д����ļ���u��eu�仯,��o�Ʋ���

    a���:
    a. ��e�������u�����ļ��ռ��Ƿn�㹻,���㹻;��ֱ�oд��;
    ������,����벽��b;

    b. ������ǰ�ļ�i���Ϣ��tmpParamsFileInfo, ���¸��ݵ�ǰe�������n��
    �µ��ļ�i���ϢtpSysParamsFileInfos;  �Ƚ�tmpParamsFileInfo�itpSysParamsFileInfos, 

        1. ��paramsId��Ҫд����ļ�indexûoз�eu�仯,��ֱ�oд��;
            n����Ƚ�tmpParamsFileInfo�itpSysParamsFileInfos����ʼid,��id��eu�仯,������ļ�
            ������д�����;

        2. ��paramsId��Ҫд����ļ�index��eu�仯;�����oldfileIndex��o�Ĳ���;
            �onewfileIndexn����Ƚ�tmpParamsFileInfo�itpSysParamsFileInfos����ʼid,��id��eu�仯,������ļ�
            ������д�����;
    
    **************************************************************/
    int32 ret;
    Data data;
    uint32 newdataLen = 0, olddataLen = 0;

    //oeou��e�洢,��ȷ��Ŀ������洢�ռ��Ƿn�㹻
    int32 index = 0, itemIndex = 0,  writeIndex = -1;
    int32 fileIndex = 0, newfileIndex = -1, oldfileIndex = -1;

    //save paramsfileInfo
    memcpy(&tmpParamsFileInfo_save, &tpSysParamsFileInfos, sizeof(tpSysParamsFileInfos));
     TP_DbgSerialPrnHexEx(3,(uint8*)"TP_Save_ParamsItem_To_File 0\r\n",
     strlen((char *)"TP_Save_ParamsItem_To_File 0\r\n"));

STEP_A:
    //check file index been chack;
    for(index=TP_PARAM_FILE_1; index<=TP_PARAM_FILE_4; index++)
    {
        if(paramsId>=tmpParamsFileInfo_save[index].startIndex)
        {
            if(paramsId<=tmpParamsFileInfo_save[index].endIndex)
            {               
                writeIndex = paramsId-tmpParamsFileInfo_save[index].startIndex+PARAMSID_NULL_OFFSET;    //ƫ��a�1
                oldfileIndex = index;
                break;
            }
            else
                continue;
        }
        else
        {
            #if defined(_DEBUG) && defined(WIN32)
            ShowDebug("TP_Write_ParamsItem : find index OVERFLOW [%d, %d], [%d]\r\n ", 
                    tmpParamsFileInfo_save[index].startIndex,
                    tmpParamsFileInfo_save[index].endIndex,
                    paramsId);
            #endif
            return -1;
        }
    }
    if(oldfileIndex < 0 || oldfileIndex > 3)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("TP_Write_ParamsItem : find oldfileIndex error [%d]\r\n ", 
                oldfileIndex);
        #endif
        return -1;
    }
    olddataLen = TP_GetItemSize(oldfileIndex, writeIndex);
    if(olddataLen < 0)
    {   
        #ifdef _DEBUG
        ShowDebug("TP_Write_ParamsItem : get itemsize error [%d, %d]\r\n ", 
                oldfileIndex, writeIndex);
        #endif
        olddataLen = 0;
    }
    
    ret = TP_Get_ParamsItem_Val(paramsId, &data);
    if(ret != 0)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("TP_Write_ParamsItem : get item data error  [%d, %d]\r\n ", 
                paramsId, ret);
        #endif
        return -2;
    }
    if((MAX_PARAMS_FILE_SIZE-(tmpParamsFileInfo_save[oldfileIndex].useSize -olddataLen))
        < Get_Data_Write_Size(data.ssize))
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("ParamsFile : %d not enought space[%d, %d] for ParamsIt [%d]!\r\n",
                oldfileIndex,
                MAX_PARAMS_FILE_SIZE-(tmpParamsFileInfo_save[oldfileIndex].useSize -olddataLen),
                Get_Data_Write_Size(data.ssize),
                paramsId);
        #endif
        goto STEP_B;
    }
    else
    {
        newdataLen = Get_Data_Write_Size(data.ssize);

        ret = TP_Read_HeadInfo(tpSysParamsFileInfos[oldfileIndex].sysFileIdle, &tmpHeadInfo_save);

        if(ret != 0 || tmpHeadInfo_save.itemcount != tpSysParamsFileInfos[oldfileIndex].headInfo.itemcount)
        {
        
            ret = TP_Write_ParamsFileHeadInfo(tpSysParamsFileInfos[oldfileIndex].sysFileIdle, &tpSysParamsFileInfos[oldfileIndex].headInfo);
            if(ret <= 0)
                return -3;
        }
        
        if(TP_Write_ParamsItem(paramsId, tpSysParamsFileInfos[oldfileIndex].sysFileIdle, writeIndex) == newdataLen)
        {
            tpSysParamsFileInfos[oldfileIndex].useSize += (newdataLen -olddataLen);

            #ifndef WIN32
                     TP_DbgSerialPrnHexEx(3,(uint8*)"flashParams start 0\r\n",
                     strlen((char *)"flashParams start 0\r\n"));
            //write flash in target broad;
            if(TP_FlushParam(tpSysParamsFileInfos[oldfileIndex].sysFileIdle) == FALSE)
            {
                return -3;
            }
                     TP_DbgSerialPrnHexEx(3,(uint8*)"flashParams Succeed 0\r\n",
                     strlen((char *)"flashParams Succeed 0\r\n"));
            #endif
            
            return 0;
        }
        else
            return -3;
    }
    /****************************************************************
                                step B
    ****************************************************************/
STEP_B: 
    if(TP_SysParamsFilesInfo_Remap() != 0)
    {
        memcpy(&tpSysParamsFileInfos, &tmpParamsFileInfo_save, sizeof(tpSysParamsFileInfos));
        return -4;
    }
    for(index=TP_PARAM_FILE_1; index<=TP_PARAM_FILE_4; index++)
    {
        if(paramsId>=tpSysParamsFileInfos[index].startIndex)
        {
            if(paramsId<=tpSysParamsFileInfos[index].endIndex)
            {               
                writeIndex = paramsId-tpSysParamsFileInfos[index].startIndex+PARAMSID_NULL_OFFSET;  //ƫ��a�1
                newfileIndex = index;

                #if defined(_DEBUG) && defined(WIN32)
                ShowDebug("TP_Write_ParamsItem       STEP B       : find index [file %d, index %d], [%d]\r\n ", 
                        newfileIndex,
                        writeIndex,
                        paramsId);
                #endif

                break;
            }
            else
                continue;
        }
        else
        {
            #if defined(_DEBUG) && defined(WIN32)
            ShowDebug("TP_Write_ParamsItem       STEP B       : find index OVERFLOW [%d, %d], [%d]\r\n ", 
                    tpSysParamsFileInfos[index].startIndex,
                    tpSysParamsFileInfos[index].endIndex,
                    paramsId);
            #endif
            return -1;
        }
    }
    if(newfileIndex < 0 || newfileIndex > 3)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("TP_Write_ParamsItem : find newfileIndex error [%d]\r\n ", 
                newfileIndex);
        #endif
        return -1;
    }

    if(newfileIndex == oldfileIndex)
    {
        PARAMS_NAMES tmpId;
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("TP_Write_ParamsItem       STEP B       clear  [%d, %d]\r\n", 
                tmpParamsFileInfo_save[newfileIndex].endIndex,
                tpSysParamsFileInfos[newfileIndex].endIndex);
        #endif
        
        if(tmpParamsFileInfo_save[newfileIndex].endIndex > tpSysParamsFileInfos[newfileIndex].endIndex)
        {
            if(TP_Write_ParamsFileHeadInfo(newfileIndex, &tpSysParamsFileInfos[newfileIndex].headInfo) <=0 )
                return -3;
            
            //clear items;
            for(index=writeIndex+1; index<= tmpParamsFileInfo_save[newfileIndex].headInfo.itemcount; index++)
            {
                TP_Clear_ParamsFileItem(newfileIndex, index);
            }
        }
        
        for(tmpId=paramsId; tmpId<= tpSysParamsFileInfos[newfileIndex].endIndex; tmpId++)
        {
            newdataLen = TP_Write_ParamsItem(tmpId, tpSysParamsFileInfos[newfileIndex].sysFileIdle, writeIndex++);
            if(newdataLen <= 0)
            {
                return -3;
            }
            //Remap ���n��ˢ��
            //tpSysParamsFileInfos[newfileIndex].useSize += (newdataLen -olddataLen);
        }
        fileIndex = oldfileIndex;

        #ifndef WIN32
             TP_DbgSerialPrnHexEx(3,(uint8*)"flashParams start 1\r\n",
             strlen((char *)"flashParams start 1\r\n"));

        //write flash in target broad;
        if(TP_FlushParam(tpSysParamsFileInfos[newfileIndex].sysFileIdle) == FALSE)
        {
            return -3;
        }
             TP_DbgSerialPrnHexEx(3,(uint8*)"flashParams Succeed 1\r\n",
             strlen((char *)"flashParams Succeed 1\r\n"));
        
        #endif
        
    }
    else
        fileIndex = newfileIndex;
    
    for(index=fileIndex; index<=TP_PARAM_FILE_4; index++)
    {
        if(index != oldfileIndex)
        {
            if(tmpParamsFileInfo_save[index].startIndex != tpSysParamsFileInfos[index].startIndex ||
                tmpParamsFileInfo_save[index].endIndex != tpSysParamsFileInfos[index].endIndex)
            {
                int32 j = 0;
                //clear items
                if(tmpParamsFileInfo_save[index].useFlag == 1 &&
                    tmpParamsFileInfo_save[index].headInfo.itemcount>tpSysParamsFileInfos[index].headInfo.itemcount)
                {
                    int32 j=tpSysParamsFileInfos[index].headInfo.itemcount;
                    for(; j<=tmpParamsFileInfo_save[index].headInfo.itemcount; j++)
                    {
                        TP_Clear_ParamsFileItem(index, j);
                    }
                    #ifndef WIN32
                    //write flash in target broad;
                                 TP_DbgSerialPrnHexEx(3,(uint8*)"flashParams start 2\r\n",
                                 strlen((char *)"flashParams start 2\r\n"));
                    if(TP_FlushParam(tpSysParamsFileInfos[index].sysFileIdle) == FALSE)
                    {
                        return -3;
                    }
                                 TP_DbgSerialPrnHexEx(3,(uint8*)"flashParams Succeed 2\r\n",
                                 strlen((char *)"flashParams Succeed 2\r\n"));
                    #endif
                    
                }
                //��ʼ�eu�仯����ȫ����д��Ƭ
                if(tmpParamsFileInfo_save[index].useFlag == 1 || tpSysParamsFileInfos[index].useFlag == 1)
                if(tmpParamsFileInfo_save[index].startIndex != tpSysParamsFileInfos[index].startIndex)
                    if(TP_Save_Nth_SysParamsFile(index) != 0)
                        return -3;
            }
        }
    }
    return 0;
}

/**
 * �� �� TP_Clear_ParamsFile() ���������������oв����ļ�
 * @return�e������0
    ʧ�ܷ���-1
 * @author  ��ҫȪ
 * @date 20120326
 * @note 
 */
#define MAX_PARAMS_ITEM 128
int32 TP_Clear_ParamsFile()
{
    int8 message[128];
    int32 nfileIndex, itemIndex, maxItem;

    for(nfileIndex=TP_PARAM_FILE_1; nfileIndex<=TP_PARAM_FILE_4; nfileIndex++)
    {
        #ifndef WIN32
        //SDK��ʱ�޸ýo�u
        maxItem = TP_GetItemNum(nfileIndex);
        if(maxItem<=0)
        {
            continue;
        }
        #else
        maxItem = MAX_PARAMS_ITEM;
        #endif
                
        for(itemIndex=0; itemIndex<maxItem; itemIndex++)
        {
            if(TP_Clear_ParamsFileItem(nfileIndex, itemIndex) != 0)
                break;
        }
    }
    return 0;
}
/**
 * �� �� TP_Save_Nth_SysParamsFile() ��������������ָ�������ļ��u�Ĳ���
 * @param nfileIndex, �����ļ�id
 * @return�e������0
 * @author  ��ҫȪ
 * @date 20120301
 * @note 
    ʹo�ע��,��������²���ʱ,�����ļ��ռ䲻���������д�ļ�
    һ���e���²�ʹoø÷���
 */
static int32 TP_Save_Nth_SysParamsFile(uint32 nfileIndex)
{
    int32 ret;
    Data data;
    PARAMS_NAMES paramsId;
    uint32 datalen = 0, writeIndex = 0;

    if(nfileIndex<TP_PARAM_FILE_1 || nfileIndex>TP_PARAM_FILE_4)
    {
        return -1;
    }
    
    if(tpSysParamsFileInfos[nfileIndex].useFlag == TRUE)
    {       
        // 1 write params head
        ret = TP_Write_ParamsFileHeadInfo(tpSysParamsFileInfos[nfileIndex].sysFileIdle,
                                        &tpSysParamsFileInfos[nfileIndex].headInfo);
        if(ret <= 0)
        {
            return -2;
        }
    
        // 2 write items;
        writeIndex = NAME_PARAMS(NULL)+PARAMSID_NULL_OFFSET;    //ÿһ���ļ�д���� 1��ʼ
        for(paramsId=(PARAMS_NAMES)tpSysParamsFileInfos[nfileIndex].startIndex; 
            paramsId<=tpSysParamsFileInfos[nfileIndex].endIndex;
            paramsId++)
        {
            ret = TP_Write_ParamsItem(paramsId, tpSysParamsFileInfos[nfileIndex].sysFileIdle, writeIndex++);
            if(ret <= 0 )
            {
                return ret*paramsId;
            }
        }
        #ifndef WIN32
        //write flash in target broad;
             TP_DbgSerialPrnHexEx(3,(uint8*)"flashParams start 3n\r\n",
             strlen((char *)"flashParams start 3n\r\n"));
        if(TP_FlushParam(tpSysParamsFileInfos[nfileIndex].sysFileIdle) == FALSE)
        {
            //showLcdDebug("TP_FlushParam error :%d \r\n", nfileIndex);
            return -4 * (tpSysParamsFileInfos[nfileIndex].sysFileIdle+1);
        }
             TP_DbgSerialPrnHexEx(3,(uint8*)"flashParams Succeed n3\r\n",
             strlen((char *)"flashParams Succeed n3\r\n"));
        
        #endif
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("TP_Save_Nth_SysParamsFile witer file [%d] from [%d, %d] over\r\n",
                    nfileIndex,
                    tpSysParamsFileInfos[nfileIndex].startIndex,
                    tpSysParamsFileInfos[nfileIndex].endIndex);
        #endif

    }
    return 0;
}

/**
 * �� �� TP_Save_AllParamsItem_To_File() ��������������oв������浽�ļ�
 * @return�e������0
 * @author  ��ҫȪ
 * @date 20120327
 * @note 
    �������o��u����д��flash
 */
int32 TP_Save_AllParamsItem_To_File(void)
{   
    int32 ret = 0;
    uint32 nfileIndex = 0;
    //clear items

    TP_Clear_ParamsFile();

    ret = TP_SysParamsFilesInfo_Remap();
    if(ret != 0)
    {
        //showLcdDebug("TP_SysParamsFilesInfo_Remap error :%d \r\n", ret);
        //TP_Check_ParamsManager_Info();
        return -1;
    }
    
    for( nfileIndex=TP_PARAM_FILE_1;  nfileIndex<=TP_PARAM_FILE_4; nfileIndex++)
    {
        ret = TP_Save_Nth_SysParamsFile(nfileIndex);
        if(ret != 0)
            break;
    }
    return ret;
}


/**
 * �� �� ParamsItem_To_Buf() �������������������滺����
 * @param data Դ����
 * @param writebuf д������
 * @param buflen д���ݳ���
 * @return�e���������ݳ��ȣ��n�򷵻�0
 * @author  ��ҫȪ
 * @date 20120229
 * @note 
 */
int32 ParamsItem_Data_To_Buf(Data *data, uint8* writebuf, uint32 buflen)
{
    uint16 crc;
    int32 len, datalen;
    
    if(!data)
        return -1;

    len = Get_Data_Write_Size(data->ssize);
    
    if(buflen < len)
        return -2;
    
    //�u���ݻa��e�a���
    datalen = DATA_HEAD_LEN+data->ssize;
    
    memcpy((uint8*)writebuf, data, datalen);
    crc=usCRC16(writebuf, datalen);
    memcpy((uint8*)(writebuf+datalen), &crc, CRC16_SIZE);

    return len;
}

/**
 * �� �� TP_SaveParamsToFile() ������������Ĭ�ϲ������浽�ļ�
 * @return�e������0
 * @author  ��ҫȪ
 * @date 20120229
 * @note 
    ���ݲ����ļ����ԣ�������ֻ�ܳ�ʼ��һ�Ρ�����ȱ��o���aд
    �������o��u����д��flash
 */
static int32 TP_SaveDefaultParamsToFile()
{
    Data data, *ptmp;
    PARAMS_NAMES paramsId;
    int32 ret = 0, fileIndex = 0;

    TP_Clear_ParamsFile();
    
    TP_SysParamsFilesInfo_Init();

    for(fileIndex = TP_PARAM_FILE_1; fileIndex<=TP_PARAM_FILE_4; fileIndex++)
    {
        ret = TP_Save_Nth_SysParamsFile(fileIndex);
        if(ret != 0)
            return ret;
    }

    return 0;
}

/**
 * �� �� TP_Buffer_SysParamsHeadInfo() ����������������i���Ϣд���������u
 * @param buffer, iⲿ�a����д����;
 * @param length, iⲿ�a���Ļ������ĴoС
 * @return >0 ���ݳ���
    0,����������
 * @author  ��ҫȪ
 * @date 20120316
 * @note 
 */
static int32 TP_Buffer_SysParamsHeadInfo(uint8* buffer, uint32 length)
{
    uint16 crcVal;
    int32 dszie = sizeof(tpSysSTDParamsHeadInfo);
    
    if(buffer == NULL || length<dszie)
        return 0;
    crcVal = usCRC16((uint8*)&tpSysSTDParamsHeadInfo, dszie-CRC16_SIZE);
    memcpy(buffer, &tpSysSTDParamsHeadInfo, dszie);
    memcpy(buffer+(dszie-CRC16_SIZE), &crcVal, CRC16_SIZE);

    return dszie;
}

/**
 * �� �� TP_ParamsItems_ToBuffer() ��������������oв���д���������u
 * @param dataBuffer, iⲿ�a����д����;
 * @param length, iⲿ�a���Ļ������ĴoС
 * @return >0 ���ݳ���
    <=0,����������
 * @author  ��ҫȪ
 * @date 20120316
 * @note 
 */
int32 TP_ParamsItems_ToBuffer(uint8* dataBuffer, uint32 length)
{
    //�����e֯o�����ļ�һ��
    Data data;
    int16 crcVal;
    const uint32 dataSize = sizeof(Data);
    int32 offset = 0, ret = 0, i = 0;
    memset(dataBuffer, 0, length);
    
    ret = TP_Buffer_SysParamsHeadInfo(dataBuffer, length);
    if(ret<=0)
        return 0;
    offset += ret;
    
    for(i=NAME_PARAMS(NULL)+1; i<NAME_PARAMS(COUNT); i++)
    {
        if(TP_Get_ParamsItem_Val((PARAMS_NAMES)i, &data)==0)
        {
            BUFFER_LENGTH_CHECK(length-offset, dataSize)

            ret = ParamsItem_Data_To_Buf(&data, dataBuffer+offset, length-offset);
            if(ret<=0)
                return ret;
            offset += ret;
        }
    }
    crcVal = usCRC16(dataBuffer, offset);
    memcpy(dataBuffer+offset, &crcVal, CRC16_SIZE);
    offset += CRC16_SIZE;
        
    return offset;

}

/**
 * �� �� TP_LoadParams_FromBuffer() �����������o����a��u�o�ز���
 * @param dataStream, iⲿ�a��������a�;
 * @param length, iⲿ�a��������a��ĴoС
 * @return >0 �o�صĲ�������
    <=0,�����쳣
 * @author  ��ҫȪ
 * @date 20120316
 * @note 
 */
int32 TP_LoadParams_FromBuffer(const uint8* dataStream, uint32 length)
{
    Data *data;
    uint16 crcVal, checkVal;
    int32 i = 0, loadCount = 0, offset = 0, readlen;
#if defined(_DEBUG) && defined(WIN32)
    uint8 buf1[3], buf2[3];
#endif
    uint8 dataBuf[256];
    const int32 headSzie = sizeof(tpSysSTDParamsHeadInfo);
    const int32 dataHeadSize = DATA_HEAD_LEN;
    SysParamsHeadInfo *headInfo = (SysParamsHeadInfo*)dataStream;

    if(dataStream == NULL||length<headSzie)
        return -1;

    checkVal = usCRC16((uint8 *)dataStream, length-CRC16_SIZE);
    memcpy(&crcVal, (uint8*)(dataStream+length-CRC16_SIZE), CRC16_SIZE);
    if(crcVal != checkVal)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("on TP_LoadParams_FromBuffer, check stream data crc error : %02x, %02x\r\n", crcVal, checkVal);
        #endif
        return -2;
    }

    
    crcVal = usCRC16((uint8*)headInfo, headSzie-CRC16_SIZE);
    if(crcVal != headInfo->crc)
    {
        #ifdef _DEBUF
        ShowDebug("on TP_LoadParams_FromBuffer, check SysHead crc error : %02x, %02x\r\n", crcVal, headInfo->crc);
        #endif
        return -3;
    }
    //ne֤i���Ϣ
    if(memcmp(&headInfo->tpflag,
            &tpSysSTDParamsHeadInfo.tpflag,
            sizeof(tpSysSTDParamsHeadInfo.tpflag)) !=0 )
    {
        #if defined(_DEBUG) && defined(WIN32)
        memset(buf1, 0, 3);
        memset(buf2, 0, 3);
        memcpy(buf1, &headInfo->tpflag, 2);
        memcpy(buf2, &tpSysSTDParamsHeadInfo.tpflag, 2);
        ShowDebug("headInfo flag error [%s, %s]\r\n", buf1, buf2);
        #endif
        return -4;
    }

    if(headInfo->version > tpSysSTDParamsHeadInfo.version)
    {
        //���ݰ汾���ߣ���ǰ�����޷����i
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("headInfo version error [%d, %d]\r\n", headInfo->version, tpSysSTDParamsHeadInfo.version);
        #endif
        return -5;
    }
    
    if(headInfo->compress > tpSysSTDParamsHeadInfo.compress)
    {
        //���u�޷����i��n�������
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("headInfo compress error [%d, %d]\r\n", headInfo->compress, tpSysSTDParamsHeadInfo.compress);
        #endif
        return -6;
    }

    if(headInfo->itemcount<=0)
        return -7;
    
    if((headInfo->itemcount)> tpSysSTDParamsHeadInfo.itemcount)
    {
        //�ļ����u����޷����i�Ĳ�����Ŀ
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("warning headInfo itemcount [%d, %d]\r\n", headInfo->itemcount, tpSysSTDParamsHeadInfo.itemcount);
        #endif
        
    }

    if((headInfo->itemcount )< tpSysSTDParamsHeadInfo.itemcount)
    {
        //������a���ou�ļ��洢��a�
        //��ʵ�ʴ洢��a��o�أ�������ȾĬ������
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("headInfo itemcount error [%d, %d]\r\n", headInfo->itemcount, tpSysSTDParamsHeadInfo.itemcount);
        #endif
    }

    #if defined(_DEBUG) && defined(WIN32)
    memset(buf1, 0, 3);
    memcpy(buf1, &headInfo->tpflag, 2);
    
    ShowDebug("headInfo INFO :[tpflag:%s, ver:%d, cmp:%d, count:%d]\r\n",
        buf1,
        headInfo->version,
        headInfo->compress,
        headInfo->itemcount);
    
    #endif

    offset+=headSzie;
    
    for(i=0; i<headInfo->itemcount; i++)
    {
        BUFFER_LENGTH_CHECK(length-offset, dataHeadSize)
        memcpy(dataBuf, dataStream+offset, sizeof(Data)+CRC16_SIZE);
        data = (Data*)(dataBuf);
        
        checkVal = usCRC16((uint8*)(dataBuf), dataHeadSize+data->ssize);
        memcpy(&crcVal, (uint8*)(dataBuf+dataHeadSize+data->ssize), CRC16_SIZE);
        if(crcVal != checkVal)
        {
            #if defined(_DEBUG) && defined(WIN32)
            ShowDebug("on TP_LoadParams_FromBuffer, check data crc error : %02x, %02x\r\n", crcVal, checkVal);
            #endif
            TP_Set_ParamsItem_CrcError(i+1);
            return -8;
        }
        memset((uint8*)dataBuf+dataHeadSize+data->ssize, 0, CRC16_SIZE);

        readlen = Get_Data_Write_Size(data->ssize);
        offset+=readlen;

        switch(data->valueType)
        {
        case VAT_Int32:
            loadCount++;
            #if defined(_DEBUG) && defined(WIN32)
            ShowDebug("read item from buffer [baseParamsId : %d, readlen : %d, type: %d, ssize : %d,  data : %d] \r\n",
                    i+1,
                    readlen,
                    data->valueType,
                    data->ssize,
                    data->dBuffer.nVal);
            #endif
            TP_Update_ParamsItem_Val((PARAMS_NAMES)(i+1), data, Status_Backup);
            break;
        case VAT_String:
            loadCount++;
            #if defined(_DEBUG) && defined(WIN32)
            ShowDebug("read item from buffer [baseParamsId : %d, readlen : %d, type: %d, ssize : %d,  data : %s] \r\n",
                    i+1,
                    readlen,
                    data->valueType,
                    data->ssize,
                    data->dBuffer.cBuffer);
            #endif
            TP_Update_ParamsItem_Val((PARAMS_NAMES)(i+1), data, Status_Backup);
            break;
        case VAT_Stream:
            loadCount++;
            #if defined(_DEBUG) && defined(WIN32)
            ShowDebug("read item [baseParamsId : %d, readlen : %d, type: %d, ssize : %d] \r\n",
                    i+1,
                    readlen,
                    data->valueType,
                    data->ssize);
            #endif
            TP_Update_ParamsItem_Val((PARAMS_NAMES)(i+1), data, Status_Backup);
            break;
        default:
            #if defined(_DEBUG) && defined(WIN32)
            ShowDebug("read item unknow valueType [type : %d, len : %d] \r\n", data->valueType, data->ssize);
            #endif
            break;
        }
    }

    if(loadCount != headInfo->itemcount)    
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("load count [: %d, headcount : %d] \r\n", loadCount, headInfo->itemcount);
        #endif
    }

    #if defined(_DEBUG) && defined(WIN32)
    ShowDebug("load paramsitem from buffer over count : %d \r\n", loadCount);
    #endif
    return loadCount;
}


