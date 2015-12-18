/*******************************************************************************
 * ÎÄ¼þÎ»ÖÃ: TP_ParamsAnalyser.c
 * ÎÄ¼þËµÃ÷: ²ÎÊý¼oÔØ±£´æAPIµÄÊµÏÖ
 * ´´½¨ÈÕÆu: 2012Äê02ÔÂ26ÈÕ
 * ´´½¨ÈË        : ÎâÒ«Èª 
 * ÐÞ¸ÄÈÕÆu:
 * ÐÞ¸ÄËµÃ÷:
 * ÐÞ¸ÄÈÕÆu:
 ******************************************************************************/

 /************************************************************************
 *²ÎÊý¼oÔØÆ÷
 *±ê×¼¸nÊ½
1. ÎÄ¼þ´æ´¢¸nÊ½:
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

µ±Ò»¸öÅäÖÃÎÄ¼þ´æ´¢ÂuÊ±£¬¿e´æÈëÆäËûÅäÖÃÎÄ¼þ
TP_PARAM_FILE_1¡¢TP_PARAM_FILE_2¡¢TP_PARAM_FILE_3¡¢TP_PARAM_FILE_4
//////////////////////////////////////////////////////////////
*************************************************************************/


 /************************************************************************
2012.03.04 
    ¸ù¾Ýµ×²ã²ÎÊýÎÄ¼þÖ§³ÖÊµ¼ÊÊý¾Ý´oÐ¡´æ´¢,Ôò²ÎÊý¼oÔØ
´¦Ài¹ý³ÌÐÞ¸ÄÎª:

1. ¿ª»u³õÊ¼»¯
    a. ¶aTP_PARAM_FILE_1,Èô¶ai·ÐÅÏ¢³e¹¦ÔòÊoÎª²ÎÊý´æoÐÊµ¼ÊoÐÐ§Êý¾Ý;
,Ö´ÐÐ²½Öèb;·nÔòÖ´ÐÐ²½Öèc;
    b.ÂÖn¯¶aÈ¡ËÄ¸ö²ÎÊýÎÄ¼þ,½«i·ÐÅÏ¢±£´æÔuÔ¤²â½a¹¹ÌåÖÐ,²¢¶a
È¡²ÎÊýÊý¾Ý;Ö±µ½¶aÈ¡iê±Ï;ÔÙÖ´ÐÐ²½Öèd;
    c.°´Ä¬ÈÏ²ÎÊý½øÐÐ´æ´¢Ô¤²â,²¢½«ËùoÐÄ¬ÈÏ²ÎÊý±£´æÖa²ÎÊýÎÄ¼þÖÐ;
    d.¼ì²â´æ´¢Ô¤²â(Êµ¼Ê¼oÔØµÄ²ÎÊý×Üa¿),ÈôÔ¤²âµÄ×Üa¿Ð¡oueùÃ÷µÄ
²ÎÊý×Üa¿Ê±,½«Î´¼oÔØµÄ²ÎÊý±£´æµ½²ÎÊýÎÄ¼þÖÐ;

2. ´æ´¢¹ý³Ì
    a.µ±¸üÐÂÊý¾ÝÊ±,´o´æ´¢Ô¤²âÖÐËnË÷idµÄ´æ´¢Î»ÖÃ,²¢¼ì²âÄ¿±êÅäÖÃÎÄ¼þ
ÊÇ·noÐ×ã¹»µÄ¿Õ¼ä±£´æ²ÎÊýÊý¾Ý;Èô×ã¹»ÔòÖØÐ´µ±Ç°²ÎÊýÏî;Èô²»×ã,ÔòÖØ
Ð´´o¸Ãid¿ªÊ¼ºoµÄËùoÐ²ÎÊýÊý¾Ý;
    ½¨Òe´o¸ÃidºoÈ«²¿ÖØÐ´,¿e¸üoÐÐ§ÀiÀûoÃ²ÎÊýÎÄ¼þ.



±ä³¤Êý¾Ý´æ´¢´æÔuµÄo°Ïì:
1. ¶Ôµ¥¸ö²ÎÊýÎÄ¼þµÄo°Ïì,¿eÄÜÔì³eÎÞ·¨´æ´¢±ÈÔ­²ÎÊýÊý¾Ý³¤¶ÈÒª³¤µÄ²ÎÊý;

2. ¶ÔÈ«²¿²ÎÊýÎÄ¼þµÄo°Ïì,¿eÄÜ»aÖØÐ´¸Ã²ÎÊýÒÔºoµÄËùoÐ²ÎÊý;

3.Ð´ÈëµÄ²ÎÊýÊý¾Ý¹ý³¤,»aÔì³eºoÃæ²¿·Ö²ÎÊýÎÞ¿Õ¼ä´æ´¢

Ôì³e²ÎÊýÒç³ö;

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
//NAME_PARAMS(NULL)Õ¼oÃÒ»¸öÆ«ÒÆa¿
#define PARAMSID_NULL_OFFSET 1

//²ÎÊý¹ÜÀii·ÐÅÏ¢£¬¸ù¾Ý°æ±¾ºÅ
#pragma pack (1)
//#ifndef WIN32
//__packed    
//#endif
typedef struct _SysParamsHeadInfo
{
    //  2 + 1 + 1 + 4 + 22 + 2 = 32 
    uint8 tpflag[2];        //"TP", ±êÊ¶·û
    uint8 version;          //ÏòeÏ¼æÈÝ;
    uint8 compress;         //0 Î´n¹Ëõ, 1 n¹Ëõ;
    uint32 itemcount;       //²ÎÊý×Üa¿>=0

    uint8  exData[22];          //Ô¤aôÀ©Õ¹
    uint16 crc;             //Ð£neÖµ
}SysParamsHeadInfo;

//µ±Ç°Ïµi³²ÎÊýÎÄ¼þ±ê×¼i·ÐÅÏ¢
static const SysParamsHeadInfo tpSysSTDParamsHeadInfo = {
    {'T', 'P'},
        1,
        0,
        //NAME_PARAMS(NULL)×÷ÎÞÐ§id,oÐÐ§·¶Î§(NAME_PARAMS(NULL), NAME_PARAMS(COUNT))
        NAME_PARAMS(COUNT)-PARAMSID_NULL_OFFSET 
};

/**
 * º¯ Êý TP_LoadParamsFromFile() ¹¦ÄÜÃèÊö£º´oÎÄ¼þ¼oÔØ²ÎÊý
 * @return³e¹¦·µ»Ø0
 * @author  ÎâÒ«Èª
 * @date 20120224
 * @note 
 */
#define MAX_PARAMS_FILE_SIZE 1024

typedef struct _SysParamsFileInfo
{
    uint32 useFlag;             //ÊÇ·n±»Ê¹oÃ
    uint32 sysFileIdle;         //Ô¤²âÕ¼oÃµÄÎÄ¼þid
    uint32 useSize;             //Ô¤²âÕ¼oÃµÄ×Ü¿Õ¼ä
    uint32 startIndex;          //Ô¤²â¿ªÊ¼µÄ²ÎÊýid
    uint32 endIndex;                //Ô¤²â½aÊøµÄ²ÎÊýid
    SysParamsHeadInfo headInfo; //Ô¤²â½«Òª½«Òª¶aÐ´µÄi·ÐÅÏ¢
}SysParamsFileInfo;

#pragma pack ()

/*
ÂÖn¯¶aÈ¡¶Ôo¦µÄ²ÎÊýÎÄ¼þi·ÐÅÏ¢
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
 * º¯ Êý TP_ScanParamsItem() ¹¦ÄÜÃèÊö£ºÔ¤²â²ÎÊý´æ´¢Î»ÖÃ
 * @return³e¹¦·µ»Ø0
 * @author  ÎâÒ«Èª
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
                //µ±Ð´ÈëµÄÄuÈÝ³¬³öÅäÖÃÎÄ¼þsizeÊ±£¬Ð´ÏÂÒ»¸öÅäÖÃÎÄ¼þ
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
 * º¯ Êý TP_ScanParamsItem() ¹¦ÄÜÃèÊö£ºÐ´Ä¬ÈÏÖµÊ±,¸ù¾ÝÄ¬ÈÏÖµÔ¤²â²ÎÊý´æ´¢Î»ÖÃ
 * @return³e¹¦·µ»Ø0
 * @author  ÎâÒ«Èª
 * @date 20120305
 * @note 
 */
static int32 TP_SysParamsFilesInfo_Init()
{
    return TP_ScanParamsItem(TRUE);
}

/**
 * º¯ Êý TP_SysParamsFilesInfo_Remap() ¹¦ÄÜÃèÊö£ºoÐ¸üÐÂÊ±,ÖØÐÂÔ¤²â²ÎÊý´æ´¢Î»ÖÃ
 * @return³e¹¦·µ»Ø0
 * @author  ÎâÒ«Èª
 * @date 20120305
 * @note 
 */
static int32 TP_SysParamsFilesInfo_Remap()
{
    return TP_ScanParamsItem(FALSE);
}
    
/**
 * º¯ Êý TP_LoadParamsFromFile() ¹¦ÄÜÃèÊö£º
 ´o²ÎÊýÎÄ¼þÈºÖÐ¼oÔØ²ÎÊý;Èô¼oÔØÊ§°Ü£¬Ôò×Ô¶¯»Ö¸´Ä¬ÈÏ²ÎÊý
 * @return³e¹¦·µ»Ø0
 * @author  ÎâÒ«Èª
 * @date 20120227
 * @note 
    Ôu¶aÈ¡¹ý³ÌÖÐ,·¢euÊý¾ÝÏî²»Æ¥ÅäµÄÇe¿ö,o¦½«ËùoÐÊý¾ÝÖØÐ´Ò»±e.
    ²»Æ¥ÅäµÄÇe¿öÈçÏÂ:
    1.²ÎÊýÎÄ¼þi·ÐÅÏ¢¶aÈ¡Ê§°Ü;
    2.ÐèÒª¶aÈ¡µÄ²ÎÊý¸öÊýÐ¡oui·ÐÅÏ¢°üº¬µÄ¸öÊýÊ±,±¨¾¯¸æ. (´æÔuÒþ»¼,o¦ÖØÐÂ¸nÊ½»¯²ÎÊýÎÄ¼þ)
    3.ÐèÒª¶aÈ¡µÄ²ÎÊý¸öÊý´ooui·ÐÅÏ¢°üº¬µÄ¸öÊýÊ±,ÖØÐÂ¸Ã²ÎÊýÎÄ¼þ.
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
            //Ôö¼o»ò¼õeÙaË²ÎÊýºo,oë²ÎÊýÎÄ¼þ²»Ò»ÖÂ
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

    //Ã»oÐÈÎºÎÅäÖÃÎÄ¼þ£¬Ð´ÈëÄ¬ÈÏÅäÖÃÏî

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
            //È±Ïî¼ì²â
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
 * º¯ Êý TP_Read_HeadInfo() ¹¦ÄÜÃèÊö£º´oÎÄ¼þ¶aÈ¡0 ²ÎÊý(i·ÐÅÏ¢)
 * @return³e¹¦·µ»Ø0
 * @author  ÎâÒ«Èª
 * @date 20120227
 * @note 
    ¼oÔØi·ÐÅÏ¢³e¹¦Ê±£¬²Å¼oÔØ²ÎÊýÐÅÏ¢
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
        //ÒÔÊý¾ÝÎÞ·¨Ê¶±ð´¦Ài: ¼oÔØÄ¬ÈÏÊý¾Ý£¬²¢Ð´Èëflash
        #if defined(_DEBUG) 
        ShowDebug("read [%d] params head infomation failed\r\n", fileIndex);
        #endif
        
        return -3;
    }
    return 0;
}
/**
 * º¯ Êý TP_Load_Params_HeadInfo() ¹¦ÄÜÃèÊö£º´oÎÄ¼þ¼oÔØ²ÎÊýi·ÐÅÏ¢
 * @return³e¹¦·µ»Ø0
 * @author  ÎâÒ«Èª
 * @date 20120227
 * @note 
    ¼oÔØi·ÐÅÏ¢³e¹¦Ê±£¬²Å¼oÔØ²ÎÊýÐÅÏ¢
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
 * º¯ Êý TP_Check_Params_HeadInfo() ¹¦ÄÜÃèÊö£º¼ì²âi·ÐÅÏ¢ÊÇ·nÕý³£
 * @return³e¹¦·µ»Ø0
 * @author  ÎâÒ«Èª
 * @date 20120229
 * @note 
    ¼ì²ei·ÐÅÏ¢µÄºÏ·¨ÐÔ,ÇÒ×ö¼æÈÝ´¦Ài
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
        //Êý¾Ý°æ±¾¹ý¸ß£¬µ±Ç°³ÌÐòÎÞ·¨´¦Ài
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("headInfo version error [%d, %d]\r\n", headInfo->version, tpSysSTDParamsHeadInfo.version);
        #endif
        return -4;
    }
    
    if(headInfo->compress > tpSysSTDParamsHeadInfo.compress)
    {
        //´æÔuÎÞ·¨´¦ÀiµÄn¹ËõÊý¾Ý
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
    
    //¼ÇÂ¼ÎÄ¼þ´æ´¢ÐÅÏ¢
    memcpy(&tpSysParamsFileInfos[fileIndex].headInfo, &tpSysSTDParamsHeadInfo, sizeof(tpSysSTDParamsHeadInfo));

    tpSysParamsFileInfos[fileIndex].useFlag = TRUE;
    tpSysParamsFileInfos[fileIndex].sysFileIdle= fileIndex;
    tpSysParamsFileInfos[fileIndex].headInfo.compress = headInfo->compress;
    tpSysParamsFileInfos[fileIndex].headInfo.itemcount = headInfo->itemcount;

    if((headInfo->itemcount + allItemCount)> tpSysSTDParamsHeadInfo.itemcount)
    {
        //ÎÄ¼þ´æÔu¶à³öÎÞ·¨´¦ÀiµÄ²ÎÊýÌõÄ¿
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("warning headInfo itemcount [%d, %d]\r\n", headInfo->itemcount+allItemCount, tpSysSTDParamsHeadInfo.itemcount);
        #endif
        
        tpSysParamsFileInfos[fileIndex].headInfo.itemcount = tpSysSTDParamsHeadInfo.itemcount-allItemCount;
    }

    if((headInfo->itemcount + allItemCount) < tpSysSTDParamsHeadInfo.itemcount)
    {
        //²ÎÊý×Üa¿¶àouÎÄ¼þ´æ´¢×Üa¿
        //°´Êµ¼Ê´æ´¢×Üa¿¼oÔØ£¬±ÜÃâÎÛÈ¾Ä¬ÈÏÊý¾Ý
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
 * º¯ Êý TP_Load_ParamsItem_From_File() ¹¦ÄÜÃèÊö£º´oÖ¸¶¨µÄ²ÎÊýÎÄ¼þÖÐ¼oÔØ²ÎÊý
 * @param fileIndex,Òª¶aÈ¡µÄ²ÎÊýÎÄ¼þË÷Òý
 * @param headInfo , ²ÎÊýÎÄ¼þi·ÐÅÏ¢
 * @param baseParamsId, ±£´æµ½¹ÜÀiµÄ»ù²ÎÊýid,o³eä»ØÕæÊµµÄ²ÎÊýid
 * @return ·µ»ØoÐÐ§µÄ²ÎÊýa¿
 * @author  ÎâÒ«Èª
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
    //index = 0 Òn±»i·ÐÅÏ¢Õ¼oÃ
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
 * º¯ Êý TP_Write_ParamsFileHeadInfo() ¹¦ÄÜÃèÊö£ºÐ´Èë²ÎÊýÎÄ¼þi·ÐÅÏ¢
 * @return > 0 ³e¹¦·µ»ØÐ´Èë³¤¶È,
        <=0 Ê§°Ü·µ»Ø;
 * @author  ÎâÒ«Èª
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
 * º¯ Êý TP_Write_ParamsItem() ¹¦ÄÜÃèÊö£ºÐ´Èë²ÎÊýÊý¾Ý
 * @return > 0 ³e¹¦·µ»ØÐ´Èë³¤¶È,
        <=0 Ê§°Ü·µ»Ø;
 * @author  ÎâÒ«Èª
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
 * º¯ Êý TP_Clear_ParamsItem() ¹¦ÄÜÃèÊö£ºÇå³ý²ÎÊýÊý¾Ý
 * @return³e¹¦·µ»Ø0,
        Ê§°Ü·µ»Ø-1;
 * @author  ÎâÒ«Èª
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
 * º¯ Êý TP_Save_AllParamsItem_To_File() ¹¦ÄÜÃèÊö£º½«²ÎÊý±£´æµ½ÎÄ¼þ
 * @return³e¹¦·µ»Ø0;
        -1: ²ÎÊýÔu²ÎÊýÎÄ¼þ¶¨Î»Ê§°Ü;;
        -2: ²ÎÊýÔu²ÎÊýÎÄ¼þºi²ÎÊý±i²»´æÔu(²ÎÊýÎÄ¼þÊý¾Ý³¤¶ÈÐ¡ou0,²ÎÊý²»Ôu²ÎÊýa´±iÄu);
        -3: Ð´²ÎÊýµ½flash»òË¢ÐÂflash¿eÊ§°Ü;
        -4: Ô¤²â´æ´¢²ÎÊýÊ§°Ü(Ð´Èë¿Õ¼ä²»×ã);
 * @author  ÎâÒ«Èª
 * @date 20120327
 * @note 
    ÏÈ¶¨Î»ºoÐ´Èë
    oeouµ×²ã²eoÃ¿eÐ´Èë,ÈôÐ´ÈëÊ§°Ü,Ôò¿eÄÜÔì³eeÏ²ãÊý¾Ýoëµ×²ãÊý¾Ý²»i¬²½
 */
static SysParamsHeadInfo tmpHeadInfo_save;
static SysParamsFileInfo tmpParamsFileInfo_save[4];

int32 TP_Save_ParamsItem_To_File(PARAMS_NAMES paramsId)
{
    /*************************************************************
    1. Èôµ±Ç°²ÎÊýÐ´ÈëµÄÎÄ¼þ¾ä±u²»·¢eu±ä»¯,Ö±½oÐ´Èë
    2. Èôµ±Ç°²ÎÊýÐ´ÈëµÄÎÄ¼þ¾ä±u·¢eu±ä»¯,ÔòºoÒÆ²ÎÊý

    a÷³Ì:
    a. ¼ì²e²ÎÊýËùÔu²ÎÊýÎÄ¼þ¿Õ¼äÊÇ·n×ã¹»,Èô×ã¹»;ÔòÖ±½oÐ´Èë;
    Èô²»×ã,Ôò½øÈë²½Öèb;

    b. ¿½±´µ±Ç°ÎÄ¼þi·ÐÅÏ¢µ½tmpParamsFileInfo, ÖØÐÂ¸ù¾Ýµ±Ç°e¨Ãè²ÎÊý»nµÃ
    ÐÂµÄÎÄ¼þi·ÐÅÏ¢tpSysParamsFileInfos;  ±È½ÏtmpParamsFileInfoºitpSysParamsFileInfos, 

        1. ÈôparamsIdËùÒªÐ´ÈëµÄÎÄ¼þindexÃ»oÐ·¢eu±ä»¯,ÔòÖ±½oÐ´Èë;
            n­»·±È½ÏtmpParamsFileInfoºitpSysParamsFileInfosµÄÆðÊ¼id,Èôid·¢eu±ä»¯,ÔòÇå¿ÕÎÄ¼þ
            ²¢ÖØÐÂÐ´Èë²ÎÊý;

        2. ÈôparamsIdËùÒªÐ´ÈëµÄÎÄ¼þindex·¢eu±ä»¯;ÔòÇå¿ÕoldfileIndex¶àoàµÄ²ÎÊý;
            ´onewfileIndexn­»·±È½ÏtmpParamsFileInfoºitpSysParamsFileInfosµÄÆðÊ¼id,Èôid·¢eu±ä»¯,ÔòÇå¿ÕÎÄ¼þ
            ²¢ÖØÐÂÐ´Èë²ÎÊý;
    
    **************************************************************/
    int32 ret;
    Data data;
    uint32 newdataLen = 0, olddataLen = 0;

    //oeou±ä³e´æ´¢,ÐèÈ·¶¨Ä¿±ê²ÎÊý´æ´¢¿Õ¼äÊÇ·n×ã¹»
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
                writeIndex = paramsId-tmpParamsFileInfo_save[index].startIndex+PARAMSID_NULL_OFFSET;    //Æ«ÒÆa¿1
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
                writeIndex = paramsId-tpSysParamsFileInfos[index].startIndex+PARAMSID_NULL_OFFSET;  //Æ«ÒÆa¿1
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
            //Remap ÊÇÒn±»Ë¢ÐÂ
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
                //ÆðÊ¼Ïî·¢eu±ä»¯£¬ÐèÈ«²¿ÖØÐ´¸ÃÆ¬
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
 * º¯ Êý TP_Clear_ParamsFile() ¹¦ÄÜÃèÊö£ºÇå¿ÕËùoÐ²ÎÊýÎÄ¼þ
 * @return³e¹¦·µ»Ø0
    Ê§°Ü·µ»Ø-1
 * @author  ÎâÒ«Èª
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
        //SDKÔÝÊ±ÎÞ¸Ã½o¿u
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
 * º¯ Êý TP_Save_Nth_SysParamsFile() ¹¦ÄÜÃèÊö£º±£´æÖ¸¶¨²ÎÊýÎÄ¼þÄuµÄ²ÎÊý
 * @param nfileIndex, ²ÎÊýÎÄ¼þid
 * @return³e¹¦·µ»Ø0
 * @author  ÎâÒ«Èª
 * @date 20120301
 * @note 
    Ê¹oÃ×¢Òâ,½öµ±Òò¸üÐÂ²ÎÊýÊ±,²ÎÊýÎÄ¼þ¿Õ¼ä²»×ã¶øÒýÆðÖØÐ´ÎÄ¼þ
    Ò»°ãÇe¿öÏÂ²»Ê¹oÃ¸Ã·½·¨
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
        writeIndex = NAME_PARAMS(NULL)+PARAMSID_NULL_OFFSET;    //Ã¿Ò»¸öÎÄ¼þÐ´¶¼ÒÔ 1¿ªÊ¼
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
 * º¯ Êý TP_Save_AllParamsItem_To_File() ¹¦ÄÜÃèÊö£º½«ËùoÐ²ÎÊý±£´æµ½ÎÄ¼þ
 * @return³e¹¦·µ»Ø0
 * @author  ÎâÒ«Èª
 * @date 20120327
 * @note 
    ÏÈÇå¿ÕËùoÐÄuÈÝÔÙÐ´Èëflash
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
 * º¯ Êý ParamsItem_To_Buf() ¹¦ÄÜÃèÊö£º½«²ÎÊý±£´æ»º´æÇø
 * @param data Ô´Êý¾Ý
 * @param writebuf Ð´»º´æÇø
 * @param buflen Ð´Êý¾Ý³¤¶È
 * @return³e¹¦·µ»ØÊý¾Ý³¤¶È£¬·nÔò·µ»Ø0
 * @author  ÎâÒ«Èª
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
    
    //ÂuÊý¾Ý»aÔì³e¶aÒç³ö
    datalen = DATA_HEAD_LEN+data->ssize;
    
    memcpy((uint8*)writebuf, data, datalen);
    crc=usCRC16(writebuf, datalen);
    memcpy((uint8*)(writebuf+datalen), &crc, CRC16_SIZE);

    return len;
}

/**
 * º¯ Êý TP_SaveParamsToFile() ¹¦ÄÜÃèÊö£º½«Ä¬ÈÏ²ÎÊý±£´æµ½ÎÄ¼þ
 * @return³e¹¦·µ»Ø0
 * @author  ÎâÒ«Èª
 * @date 20120229
 * @note 
    ¸ù¾Ý²ÎÊýÎÄ¼þÌØÐÔ£¬²ÎÊýÏîÖ»ÄÜ³õÊ¼»¯Ò»´Î¡¢ÇÒÎÞÈ±ÏîoÐÐò¶aÐ´
    ÏÈÇå¿ÕËùoÐÄuÈÝÔÙÐ´Èëflash
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
 * º¯ Êý TP_Buffer_SysParamsHeadInfo() ¹¦ÄÜÃèÊö£º½«²ÎÊýi·ÐÅÏ¢Ð´µ½»º³åÇøÄu
 * @param buffer, iâ²¿Ìa¹©µÄÐ´»º³å;
 * @param length, iâ²¿Ìa¹©µÄ»º³åÇøµÄ´oÐ¡
 * @return >0 Êý¾Ý³¤¶È
    0,»º³åÇø²»×ã
 * @author  ÎâÒ«Èª
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
 * º¯ Êý TP_ParamsItems_ToBuffer() ¹¦ÄÜÃèÊö£º½«ËùoÐ²ÎÊýÐ´µ½»º³åÇøÄu
 * @param dataBuffer, iâ²¿Ìa¹©µÄÐ´»º³å;
 * @param length, iâ²¿Ìa¹©µÄ»º³åÇøµÄ´oÐ¡
 * @return >0 Êý¾Ý³¤¶È
    <=0,»º³åÇø²»×ã
 * @author  ÎâÒ«Èª
 * @date 20120316
 * @note 
 */
int32 TP_ParamsItems_ToBuffer(uint8* dataBuffer, uint32 length)
{
    //Êý¾Ý×eÖ¯oë²ÎÊýÎÄ¼þÒ»ÖÂ
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
 * º¯ Êý TP_LoadParams_FromBuffer() ¹¦ÄÜÃèÊö£º´oÊý¾Ýa÷Äu¼oÔØ²ÎÊý
 * @param dataStream, iâ²¿Ìa¹©µÄÊý¾Ýa÷;
 * @param length, iâ²¿Ìa¹©µÄÊý¾Ýa÷µÄ´oÐ¡
 * @return >0 ¼oÔØµÄ²ÎÊý¸öÊý
    <=0,Êý¾ÝÒì³£
 * @author  ÎâÒ«Èª
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
    //neÖ¤i·ÐÅÏ¢
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
        //Êý¾Ý°æ±¾¹ý¸ß£¬µ±Ç°³ÌÐòÎÞ·¨´¦Ài
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("headInfo version error [%d, %d]\r\n", headInfo->version, tpSysSTDParamsHeadInfo.version);
        #endif
        return -5;
    }
    
    if(headInfo->compress > tpSysSTDParamsHeadInfo.compress)
    {
        //´æÔuÎÞ·¨´¦ÀiµÄn¹ËõÊý¾Ý
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("headInfo compress error [%d, %d]\r\n", headInfo->compress, tpSysSTDParamsHeadInfo.compress);
        #endif
        return -6;
    }

    if(headInfo->itemcount<=0)
        return -7;
    
    if((headInfo->itemcount)> tpSysSTDParamsHeadInfo.itemcount)
    {
        //ÎÄ¼þ´æÔu¶à³öÎÞ·¨´¦ÀiµÄ²ÎÊýÌõÄ¿
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("warning headInfo itemcount [%d, %d]\r\n", headInfo->itemcount, tpSysSTDParamsHeadInfo.itemcount);
        #endif
        
    }

    if((headInfo->itemcount )< tpSysSTDParamsHeadInfo.itemcount)
    {
        //²ÎÊý×Üa¿¶àouÎÄ¼þ´æ´¢×Üa¿
        //°´Êµ¼Ê´æ´¢×Üa¿¼oÔØ£¬±ÜÃâÎÛÈ¾Ä¬ÈÏÊý¾Ý
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


