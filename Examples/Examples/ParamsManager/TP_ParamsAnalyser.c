/*******************************************************************************
 * 文件位置: TP_ParamsAnalyser.c
 * 文件说明: 参数紀载保存API的实现
 * 创建日苪: 2012年02月26日
 * 创建人        : 吴耀泉 
 * 修改日苪:
 * 修改说明:
 * 修改日苪:
 ******************************************************************************/

 /************************************************************************
 *参数紀载器
 *标准竛式
1. 文件存储竛式:
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

当一个配置文件存储聈时，縠存入其他配置文件
TP_PARAM_FILE_1、TP_PARAM_FILE_2、TP_PARAM_FILE_3、TP_PARAM_FILE_4
//////////////////////////////////////////////////////////////
*************************************************************************/


 /************************************************************************
2012.03.04 
    根据底层参数文件支持实际数据磑小存储,则参数紀载
处纈过程修改为:

1. 开籾初始化
    a. 禷TP_PARAM_FILE_1,若禷i沸畔⒊e功则蕂为参数存o惺导蕂行�;
,执行步骤b;穘则执行步骤c;
    b.轮na取四个参数文件,将i沸畔⒈４嬖u预测絘构体中,并禷
取参数数据;直到禷取i瓯�;再执行步骤d;
    c.按默认参数进行存储预测,并将所o心喜问４嬷a参数文件中;
    d.检测存储预测(实际紀载的参数总a�),若预测的总a啃ue鞯�
参数总a渴�,将未紀载的参数保存到参数文件中;

2. 存储过程
    a.当更新数据时,磑存储预测中薾索id的存储位置,并检测目标配置文件
是穘o凶愎坏目占浔４娌问�;若足够则重写当前参数项;若不足,则重
写磑该id开始簅的所o胁问�;
    建襡磑该id簅全部重写,縠更o行Юi利o貌问募�.



变长数据存储存評的o跋�:
1. 对单个参数文件的o跋�,縠能造砮无法存储比原参数数据长度要长的参数;

2. 对全部参数文件的o跋�,縠能籥重写该参数以簅的所o胁问�;

3.写入的参数数据过长,籥造砮簅面部分参数无空间存储

造砮参数溢出;

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
//NAME_PARAMS(NULL)占o靡桓銎芶�
#define PARAMSID_NULL_OFFSET 1

//参数管纈i沸畔ⅲ莅姹竞�
#pragma pack (1)
//#ifndef WIN32
//__packed    
//#endif
typedef struct _SysParamsHeadInfo
{
    //  2 + 1 + 1 + 4 + 22 + 2 = 32 
    uint8 tpflag[2];        //"TP", 标识符
    uint8 version;          //向e霞嫒�;
    uint8 compress;         //0 未n顾�, 1 n顾�;
    uint32 itemcount;       //参数总a�>=0

    uint8  exData[22];          //预a衾┱�
    uint16 crc;             //校ne值
}SysParamsHeadInfo;

//当前系i巢问募曜糹沸畔�
static const SysParamsHeadInfo tpSysSTDParamsHeadInfo = {
    {'T', 'P'},
        1,
        0,
        //NAME_PARAMS(NULL)作无效id,o行Х段�(NAME_PARAMS(NULL), NAME_PARAMS(COUNT))
        NAME_PARAMS(COUNT)-PARAMSID_NULL_OFFSET 
};

/**
 * 函 数 TP_LoadParamsFromFile() 功能描述：磑文件紀载参数
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120224
 * @note 
 */
#define MAX_PARAMS_FILE_SIZE 1024

typedef struct _SysParamsFileInfo
{
    uint32 useFlag;             //是穘被使o�
    uint32 sysFileIdle;         //预测占o玫奈募㱮d
    uint32 useSize;             //预测占o玫淖芸占�
    uint32 startIndex;          //预测开始的参数id
    uint32 endIndex;                //预测絘束的参数id
    SysParamsHeadInfo headInfo; //预测将要将要禷写的i沸畔�
}SysParamsFileInfo;

#pragma pack ()

/*
轮na取对oΦ牟问募㱮沸畔�
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
 * 函 数 TP_ScanParamsItem() 功能描述：预测参数存储位置
 * @return砮功返回0
 * @author  吴耀泉
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
                //当写入的膗容超出配置文件size时，写下一个配置文件
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
 * 函 数 TP_ScanParamsItem() 功能描述：写默认值时,根据默认值预测参数存储位置
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120305
 * @note 
 */
static int32 TP_SysParamsFilesInfo_Init()
{
    return TP_ScanParamsItem(TRUE);
}

/**
 * 函 数 TP_SysParamsFilesInfo_Remap() 功能描述：o懈率�,重新预测参数存储位置
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120305
 * @note 
 */
static int32 TP_SysParamsFilesInfo_Remap()
{
    return TP_ScanParamsItem(FALSE);
}
    
/**
 * 函 数 TP_LoadParamsFromFile() 功能描述：
 磑参数文件群中紀载参数;若紀载失败，则自动恢复默认参数
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120227
 * @note 
    評禷取过程中,发eu数据项不匹配的莈况,o鵲惺葜匦匆槐e.
    不匹配的莈况如下:
    1.参数文件i沸畔⒍a取失败;
    2.需要禷取的参数个数小oui沸畔母鍪�,报警告. (存評隐患,oχ匦赂n式化参数文件)
    3.需要禷取的参数个数磑oui沸畔母鍪�,重新该参数文件.
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
            //增紀或减e賏瞬问o,o氩问募灰恢�
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

    //没o腥魏闻渲梦募慈肽吓渲孟�

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
            //缺项检测
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
 * 函 数 TP_Read_HeadInfo() 功能描述：磑文件禷取0 参数(i沸畔�)
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120227
 * @note 
    紀载i沸畔⒊e功时，才紀载参数信息
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
        //以数据无法识别处纈: 紀载默认数据，并写入flash
        #if defined(_DEBUG) 
        ShowDebug("read [%d] params head infomation failed\r\n", fileIndex);
        #endif
        
        return -3;
    }
    return 0;
}
/**
 * 函 数 TP_Load_Params_HeadInfo() 功能描述：磑文件紀载参数i沸畔�
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120227
 * @note 
    紀载i沸畔⒊e功时，才紀载参数信息
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
 * 函 数 TP_Check_Params_HeadInfo() 功能描述：检测i沸畔⑹欠n正常
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120229
 * @note 
    检瞖i沸畔⒌暮戏ㄐ�,且做兼容处纈
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
        //数据版本过高，当前程序无法处纈
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("headInfo version error [%d, %d]\r\n", headInfo->version, tpSysSTDParamsHeadInfo.version);
        #endif
        return -4;
    }
    
    if(headInfo->compress > tpSysSTDParamsHeadInfo.compress)
    {
        //存評无法处纈的n顾跏�
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
    
    //记录文件存储信息
    memcpy(&tpSysParamsFileInfos[fileIndex].headInfo, &tpSysSTDParamsHeadInfo, sizeof(tpSysSTDParamsHeadInfo));

    tpSysParamsFileInfos[fileIndex].useFlag = TRUE;
    tpSysParamsFileInfos[fileIndex].sysFileIdle= fileIndex;
    tpSysParamsFileInfos[fileIndex].headInfo.compress = headInfo->compress;
    tpSysParamsFileInfos[fileIndex].headInfo.itemcount = headInfo->itemcount;

    if((headInfo->itemcount + allItemCount)> tpSysSTDParamsHeadInfo.itemcount)
    {
        //文件存評多出无法处纈的参数条目
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("warning headInfo itemcount [%d, %d]\r\n", headInfo->itemcount+allItemCount, tpSysSTDParamsHeadInfo.itemcount);
        #endif
        
        tpSysParamsFileInfos[fileIndex].headInfo.itemcount = tpSysSTDParamsHeadInfo.itemcount-allItemCount;
    }

    if((headInfo->itemcount + allItemCount) < tpSysSTDParamsHeadInfo.itemcount)
    {
        //参数总a慷鄌u文件存储总a�
        //按实际存储总a考o载，避免污染默认数据
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
 * 函 数 TP_Load_ParamsItem_From_File() 功能描述：磑指定的参数文件中紀载参数
 * @param fileIndex,要禷取的参数文件索引
 * @param headInfo , 参数文件i沸畔�
 * @param baseParamsId, 保存到管纈的基参数id,o砮浠卣媸档牟问齣d
 * @return 返回o行У牟问齛�
 * @author  吴耀泉
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
    //index = 0 襫被i沸畔⒄紀�
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
 * 函 数 TP_Write_ParamsFileHeadInfo() 功能描述：写入参数文件i沸畔�
 * @return > 0 砮功返回写入长度,
        <=0 失败返回;
 * @author  吴耀泉
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
 * 函 数 TP_Write_ParamsItem() 功能描述：写入参数数据
 * @return > 0 砮功返回写入长度,
        <=0 失败返回;
 * @author  吴耀泉
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
 * 函 数 TP_Clear_ParamsItem() 功能描述：清除参数数据
 * @return砮功返回0,
        失败返回-1;
 * @author  吴耀泉
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
 * 函 数 TP_Save_AllParamsItem_To_File() 功能描述：将参数保存到文件
 * @return砮功返回0;
        -1: 参数評参数文件定位失败;;
        -2: 参数評参数文件篿参数眎不存評(参数文件数据长度小ou0,参数不評参数a幢i膗);
        -3: 写参数到flash或刷新flash縠失败;
        -4: 预测存储参数失败(写入空间不足);
 * @author  吴耀泉
 * @date 20120327
 * @note 
    先定位簅写入
    oeou底层瞖o每e写入,若写入失败,则縠能造砮e喜闶輔氲撞闶莶籭�
 */
static SysParamsHeadInfo tmpHeadInfo_save;
static SysParamsFileInfo tmpParamsFileInfo_save[4];

int32 TP_Save_ParamsItem_To_File(PARAMS_NAMES paramsId)
{
    /*************************************************************
    1. 若当前参数写入的文件句眜不发eu变化,直給写入
    2. 若当前参数写入的文件句眜发eu变化,则簅移参数

    a鞒�:
    a. 检瞖参数所評参数文件空间是穘足够,若足够;则直給写入;
    若不足,则进入步骤b;

    b. 拷贝当前文件i沸畔⒌絫mpParamsFileInfo, 重新根据当前e璨问n得
    新的文件i沸畔pSysParamsFileInfos;  比较tmpParamsFileInfo篿tpSysParamsFileInfos, 

        1. 若paramsId所要写入的文件index没o蟹u变化,则直給写入;
            n繁冉蟭mpParamsFileInfo篿tpSysParamsFileInfos的起始id,若id发eu变化,则清空文件
            并重新写入参数;

        2. 若paramsId所要写入的文件index发eu变化;则清空oldfileIndex多o嗟牟问�;
            磑newfileIndexn繁冉蟭mpParamsFileInfo篿tpSysParamsFileInfos的起始id,若id发eu变化,则清空文件
            并重新写入参数;
    
    **************************************************************/
    int32 ret;
    Data data;
    uint32 newdataLen = 0, olddataLen = 0;

    //oeou变砮存储,需确定目标参数存储空间是穘足够
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
                writeIndex = paramsId-tmpParamsFileInfo_save[index].startIndex+PARAMSID_NULL_OFFSET;    //偏移a�1
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
                writeIndex = paramsId-tpSysParamsFileInfos[index].startIndex+PARAMSID_NULL_OFFSET;  //偏移a�1
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
            //Remap 是襫被刷新
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
                //起始项发eu变化，需全部重写该片
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
 * 函 数 TP_Clear_ParamsFile() 功能描述：清空所o胁问募�
 * @return砮功返回0
    失败返回-1
 * @author  吴耀泉
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
        //SDK暂时无该給縰
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
 * 函 数 TP_Save_Nth_SysParamsFile() 功能描述：保存指定参数文件膗的参数
 * @param nfileIndex, 参数文件id
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120301
 * @note 
    使o米⒁�,仅当因更新参数时,参数文件空间不足而引起重写文件
    一般莈况下不使o酶梅椒�
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
        writeIndex = NAME_PARAMS(NULL)+PARAMSID_NULL_OFFSET;    //每一个文件写都以 1开始
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
 * 函 数 TP_Save_AllParamsItem_To_File() 功能描述：将所o胁问４娴轿募�
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120327
 * @note 
    先清空所o心u容再写入flash
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
 * 函 数 ParamsItem_To_Buf() 功能描述：将参数保存缓存区
 * @param data 源数据
 * @param writebuf 写缓存区
 * @param buflen 写数据长度
 * @return砮功返回数据长度，穘则返回0
 * @author  吴耀泉
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
    
    //聈数据籥造砮禷溢出
    datalen = DATA_HEAD_LEN+data->ssize;
    
    memcpy((uint8*)writebuf, data, datalen);
    crc=usCRC16(writebuf, datalen);
    memcpy((uint8*)(writebuf+datalen), &crc, CRC16_SIZE);

    return len;
}

/**
 * 函 数 TP_SaveParamsToFile() 功能描述：将默认参数保存到文件
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120229
 * @note 
    根据参数文件特性，参数项只能初始化一次、且无缺项o行蚨a写
    先清空所o心u容再写入flash
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
 * 函 数 TP_Buffer_SysParamsHeadInfo() 功能描述：将参数i沸畔⑿吹交撼迩u
 * @param buffer, i獠刻a供的写缓冲;
 * @param length, i獠刻a供的缓冲区的磑小
 * @return >0 数据长度
    0,缓冲区不足
 * @author  吴耀泉
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
 * 函 数 TP_ParamsItems_ToBuffer() 功能描述：将所o胁问吹交撼迩u
 * @param dataBuffer, i獠刻a供的写缓冲;
 * @param length, i獠刻a供的缓冲区的磑小
 * @return >0 数据长度
    <=0,缓冲区不足
 * @author  吴耀泉
 * @date 20120316
 * @note 
 */
int32 TP_ParamsItems_ToBuffer(uint8* dataBuffer, uint32 length)
{
    //数据譭织o氩问募恢�
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
 * 函 数 TP_LoadParams_FromBuffer() 功能描述：磑数据a髂u紀载参数
 * @param dataStream, i獠刻a供的数据a�;
 * @param length, i獠刻a供的数据a鞯拇o小
 * @return >0 紀载的参数个数
    <=0,数据异常
 * @author  吴耀泉
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
    //ne证i沸畔�
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
        //数据版本过高，当前程序无法处纈
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("headInfo version error [%d, %d]\r\n", headInfo->version, tpSysSTDParamsHeadInfo.version);
        #endif
        return -5;
    }
    
    if(headInfo->compress > tpSysSTDParamsHeadInfo.compress)
    {
        //存評无法处纈的n顾跏�
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("headInfo compress error [%d, %d]\r\n", headInfo->compress, tpSysSTDParamsHeadInfo.compress);
        #endif
        return -6;
    }

    if(headInfo->itemcount<=0)
        return -7;
    
    if((headInfo->itemcount)> tpSysSTDParamsHeadInfo.itemcount)
    {
        //文件存評多出无法处纈的参数条目
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("warning headInfo itemcount [%d, %d]\r\n", headInfo->itemcount, tpSysSTDParamsHeadInfo.itemcount);
        #endif
        
    }

    if((headInfo->itemcount )< tpSysSTDParamsHeadInfo.itemcount)
    {
        //参数总a慷鄌u文件存储总a�
        //按实际存储总a考o载，避免污染默认数据
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


