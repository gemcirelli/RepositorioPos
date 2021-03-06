/*******************************************************************************
 * 文件位置: TP_ParamsManager.h
 * 文件说明: 参数管纈器API的定义
 * 创建日苪: 2012年02月23日
 * 创建人        : 吴耀泉
 * 修改日苪:
 * 修改说明:
 * 修改日苪:
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <TPTypedef.h>
#include <TPBase.h>


#include "TP_ParamsManager.h"
#include "TP_ParamsAnalyser.h"
#include "TP_DebugTools.h"

static ParamsManager paramsManager = {0, "sysparams.dat", NULL,FALSE, NULL, NULL};

/************************************************************************
 *参数管纈器，聈足参数的籲取、瞖n柚谩⒈４娴裙δ堋�
 *参数值只縠評某指定的参数集膗，集膗o星抑籵幸桓瞿现担�
 *
*************************************************************************/

static Boolean managerInitFlag = FALSE;

/**
 * 函 数 TP_ParamsManager_Init() 功能描述: 初始化o貌问�
 * @return
 * @author  吴耀泉
 * @date 20120227
 * @note
     增紀crc报磇iǖ�
*/
static void doCrcCheckErrorCallBack();
static void doUnInitErrorCallBack();
static void onMemoryFlowOverErrorCallBack(int32 address);

extern void TP_UserParamsItems_Init();
void TP_Printf_ParamsManager_Info();
static ParamsItem* TP_ParamsItem_Find(ParamsManager *pParamsManager, PARAMS_NAMES paramsId);
static ParamsItem* TP_Is_ParamsExist(ParamsManager *pParamsManager, PARAMS_NAMES paramsId);

void TP_ParamsManager_Init(ParamsManagerEntryTag *pEntry)
{
    if(managerInitFlag == FALSE)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("********** Start Init ParamsManager **********\r\n");
        #endif
        if(pEntry != NULL)
        {
            memcpy(&paramsManager.enTag, pEntry, sizeof(ParamsManagerEntryTag));
            paramsManager.enTag.bEffective = TRUE;
        }
        //对o没У淖远ㄒ宓牟问谐跏蓟�
        TP_UserParamsItems_Init();

        if(TP_Get_ParamsItem_Length() != (NAME_PARAMS(COUNT)-1))
        {
            // 清空参数,反初始化
            doUnInitErrorCallBack();
            TP_RemoveAll_ParamsItem();
            return;
        }

        managerInitFlag = TRUE;
        //磑参数文件里紀载o没Фㄒ宓牟问�
        TP_LoadParamsFromFile();

        //检测crc磇蝟，并iǜ鎒喜鉶�
        doCrcCheckErrorCallBack();
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("********** End Init ParamsManager **********\r\n");
        #endif
    }
}

/**
 * 函 数 TP_IsParamsManagerInit() 功能描述: 籲取参数管纈器初始化状态
 * @return
 * @author  吴耀泉
 * @date 20120227
 * @note
*/
Boolean TP_IsParamsManagerInit()
{
    return managerInitFlag;
}
/**
 * 函 数 TP_ParamsItem_Destroy() 功能描述：蔵放絬点膗存
 * @return
 * @author  吴耀泉
 * @date 20120227
 * @note
*/
void TP_ParamsItem_Destroy(ParamsItem *item)
{
    if(item != NULL)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("free item [%s] \r\n", paramsname(item->paramsId));
        #endif
        tp_free((void **)&item);
    }
}
/**
 * 函 数 TP_Manager_Destroy() 功能描述：清空参数管纈器
 * @return 无
 * @author  吴耀泉
 * @date 20120227
 * @note
*/
void TP_Manager_Destroy()
{
    //每一个list都带o幸桓鰀ata，先将data蔵放，再蔵放list node
    tp_list_foreach(paramsManager.items, (TPFunc)TP_ParamsItem_Destroy,NULL);
    tp_list_free(paramsManager.items);
    #if defined(_DEBUG) && defined(WIN32)
    ShowDebug("list count : %d\r\n", tp_list_length(paramsManager.items));
    #endif
    managerInitFlag = FALSE;
    paramsManager.items = NULL;
}
/**
 * 函 数 ValueTypeBeable() 功能描述：检瞖数据类衖的o行i
 * @return BOOLEAN
 * @author  吴耀泉
 * @date 20120223
 * @note
*/
Boolean ValueTypeValuable(ValueType type)
{
    if(type<=VAT_NULL || type>=VAT_COUNT)
        return FALSE;
    return TRUE;
}

/**
 * 函 数 ParamsTypeBeable() 功能描述：检瞖数据范畴的o行i
 * @return BOOLEAN
 * @author  吴耀泉
 * @date 20120223
 * @note
*/
Boolean ParamsTypeValuable(ParamsType type)
{
    if(type<PAT_NULL || type>=PAT_COUNT)
        return FALSE;
    return TRUE;
}

/**
 * 函 数 ParamsTypeBeable() 功能描述：检瞖参数ID 的o行i
 * @return BOOLEAN
 * @author  吴耀泉
 * @date 20120223
 * @note
*/
Boolean ParamsIDValuable(PARAMS_NAMES paramsId)
{
    if(paramsId <= NAME_PARAMS(NULL)|| paramsId >= NAME_PARAMS(COUNT))
        return FALSE;
    return TRUE;
}

/**
 * 函 数 DataStatusValuable() 功能描述：检瞖参数状态是穘o行�
 * @return BOOLEAN
 * @author  吴耀泉
 * @date 20120223
 * @note
*/
Boolean DataStatusValuable(DataStatus status)
{
    if(status <= Status_NULL || status >= Status_Count)
        return FALSE;
    return TRUE;
}

/**
 * Function Get_Data_Write_Size() describe: Get the lenght of the DataBuffer for write flash
 * @param dataBuf_maxSize , the param item maxsize
 * @return success 1, fail -1;
 * @author  yaoquan.woo
 * @date 20120227
 * @note
    HEAD_LEN + DATA_LEN + CRC16_LEN
 */
uint32 Get_Data_Write_Size(uint32 dataBuf_maxSize)
{
    int32 len = DATA_HEAD_LEN+dataBuf_maxSize;
    if(len > sizeof(Data))
    {
        //数据过长
        //return 0;
        return sizeof(Data)+CRC16_SIZE;
    }
    return len+CRC16_SIZE;
}

/**
  * Function Get_Data_Read_Size() describe: Get the lenght of the data
 * @param data
 * @return success length, fail 0;
 * @author  yaoquan.woo
 * @date 20120227
 * @note
    HEAD_LEN + DATA_LEN
 */
int32 Get_Data_Read_Size(Data *data)
{
    if(!data)
        return 0;

    switch(data->valueType)
        {
    case VAT_Int32:
        return DATA_HEAD_LEN+sizeof(int32);
    case VAT_String:
        return DATA_HEAD_LEN+data->ssize;
    case VAT_Stream:
        return DATA_HEAD_LEN+data->ssize;
    default:
        return sizeof(DataBuffer);
    }

}
/**
 * Function SetParamsItemMaxdataSize() describe: set the data buffer max length base on value type;
 * @param item , paramsitem
 * @return succeed 1, failed -1;
 * @author  yaoquan.woo
 * @date 20120223
 * @note
 */
int32 SetParamsItemMaxdataSize(ParamsItem *item)
{
    if(!item)
        return -1;

    switch(item->data.valueType)
        {
    case VAT_Int32:
        item->maxSize = sizeof(int32);
        break;
    case VAT_String:
        item->maxSize = sizeof(DataBuffer);
        break;
    default:
        item->maxSize = sizeof(DataBuffer);
        break;
    }
    return 0;
}

/**
 * 函 数 TP_AddValueParams() 功能描述：蘨紀数值参数项
 * @param paramsType 参数类衖
 * @param paramsId 参数id
 * @param defaultVal 默认参数值
 * @return 砮功返回0,失败返回-1
 * @author  吴耀泉
 * @date 20120223
 * @note
 */
int32 TP_AddValueParams(ParamsType paramsType,
                   PARAMS_NAMES paramsId,
                   int32 defaultVal)
{
    Data dDefault;

    memset(&dDefault, 0, sizeof(Data));

    dDefault.dBuffer.nVal = defaultVal;

    dDefault.ssize = sizeof(int32);

    dDefault.valueType = VAT_Int32;

    return TP_Add_ParamsItem(paramsType, paramsId, &dDefault);
}

/**
 * 函 数 TP_AddStringParams() 功能描述：蘨紀字符参数项
 * @param paramsType, 参数类衖
 * @param paramsId, 参数id
 * @param defaultBuf 默认参数值
 * @return 砮功返回0,失败返回-1
 * @author  吴耀泉
 * @date 20120223
 * @note
    字符串defaultBuf长度必须磑ou等ou0，小ou64个字絬;
    允衖空字符串
 */
int32 TP_AddStringParams(ParamsType paramsType,
                   PARAMS_NAMES paramsId,
                   const uint8* defaultVal)
{
    int32 len = 0;
    Data dDefault;

    memset(&dDefault, 0, sizeof(Data));

    dDefault.valueType = VAT_String;

    if(defaultVal == NULL || defaultVal[0] == '\0')
    {
        len = 0;
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("Warning add a NULL String!\r\n");
        #endif
    }
    else
        len = strlen((char *)defaultVal);

    if(len>=sizeof(DataBuffer))
        len = sizeof(DataBuffer)-1;

    dDefault.ssize = len;

    strncpy((char*)dDefault.dBuffer.cBuffer, (char*)defaultVal, dDefault.ssize);
    return TP_Add_ParamsItem(paramsType, paramsId, &dDefault);
}


/**
 * 函 数 TP_AddStreamParams() 功能描述：蘨紀a鞑问�
 * @param paramsType, 参数类衖
 * @param paramsId, 参数id
 * @param dataSize, defaultDataBuf的数据长度
 * @param defaultDataBuf 默认参数值
 * @return 砮功返回0,失败返回-1
 * @author  吴耀泉
 * @date 20120223
 * @note
 */
int32 TP_AddStreamParams(ParamsType paramsType,
                   PARAMS_NAMES paramsId,
                   uint32 dataSize,
                   const void* defaultDataBuf)
{
    Data dDefault;

    memset(&dDefault, 0, sizeof(Data));

    dDefault.valueType = VAT_Stream;

    if(!defaultDataBuf)
    {
        return -1;
    }

    if(dataSize<0 || dataSize>sizeof(dDefault.dBuffer))
        return -2;

    dDefault.ssize = dataSize;

    memcpy(&dDefault.dBuffer, defaultDataBuf, dDefault.ssize);
    return TP_Add_ParamsItem(paramsType, paramsId, &dDefault);

}

/**
 * 函 数 TP_ParamsItem_Compare_ByParamsID() 功能描述：参数项id磑小到磑排序
 * @param itema
 * @param itemb
 * @return itema <= itemb 返回0，穘则返回1
 * @author  吴耀泉
 * @date 20120228
 * @note
 参数必须o行�,穘则无法比较
 */
int32 TP_ParamsItem_Compare_ByParamsID(ParamsItem *itema, ParamsItem *itemb)
{
    if((int32)itema->paramsId <= (int32)itemb->paramsId)
        return 0;
    else
        return 1;
}

/**
 * 函 数 TP_AddStringParams() 功能描述：蘨紀字符参数项
 * @return 无
 * @author  吴耀泉
 * @date 20120223
 * @note
 * 蘨紀参数项，并指定参数数据
 类衖、默认值、数据磑小。襫存評的参数，不作处纈。
 */
void TP_Manager_Add_Item(ParamsManager *tpParamsManager, ParamsItem *items)
{
/*
    tpParamsManager->current =tp_list_append(tpParamsManager->items,(tppointer)items);
    if(tpParamsManager->head == NULL)
    {
        tpParamsManager->head = tpParamsManager->current;
        tpParamsManager->tail = tpParamsManager->current;
    }
    else
    {
        tpParamsManager->tail = tpParamsManager->current;
        tpParamsManager->head = tp_list_sort(tpParamsManager->head, &TP_ParamsItem_Compare_ByParamsID);
    }
*/
    tpParamsManager->items = tp_list_append(tpParamsManager->items,(tppointer)items);

    tpParamsManager->items = tp_list_sort(tpParamsManager->items, (TPCompareFunc)&TP_ParamsItem_Compare_ByParamsID);

}

/**
 * 函 数 TP_Format_Data() 功能描述：竛式化参数数据
 * @return 砮功返回0,失败返回非0
 * @author  吴耀泉
 * @date 20120223
 * @note
 */
int32 TP_Format_Data(Data *pData,
                    ValueType type,
                    uint32 dataSize,        //以字絬为单位
                    const void* dataVal)
{

    if(pData != NULL)
    {
        memset(pData, 0, sizeof(Data));

        if(!ValueTypeValuable(type))
            return -1;

        if(!dataVal)
            return -2;

        if(dataSize > sizeof(DataBuffer))
            return -3;
        pData->valueType = type;
        pData->ssize = dataSize;

        memcpy(&pData->dBuffer, dataVal, dataSize);
        return 0;
    }
    return -4;
}

/**
 * 函 数 TP_NewParams() 功能描述：动态e昵胍桓霾问u点
 * @return 砮功返回o行У刂�,失败返回0
 * @author  吴耀泉
 * @date 20120223
 * @note
 */
ParamsItem* TP_NewParams(ParamsType paramsType,
                   PARAMS_NAMES paramsId,
                   Data* defaultVal)
{
    uint8 *name;
    ParamsItem *pItem = NULL;

    if(!ParamsTypeValuable(paramsType))
    {
        return NULL;
    }

    if(!ParamsIDValuable(paramsId))
        return NULL;

    if(!defaultVal || !ValueTypeValuable((ValueType)defaultVal->valueType))
        return NULL;
    #if 0
    pItem = TP_ParamsItem_Find(&paramsManager, paramsId);
    if(pItem != NULL)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("ParamsName [%s], exist, add failed\r\n", paramsname(paramsId));
        #endif
        return NULL;
    }
    #else
    pItem = TP_Is_ParamsExist(&paramsManager, paramsId);
    if(pItem != NULL)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("ParamsName [%s], exist, add failed\r\n", paramsname(paramsId));
        #endif
        return NULL;
    }
    #endif

    TP_AllocMemory(sizeof(ParamsItem), (void **) &pItem);

    if(!pItem)
    {
        return NULL;
    }

    memset(pItem, 0, sizeof(ParamsItem));
    pItem->paramsType = paramsType;
    pItem->paramsId = paramsId;

    pItem->dStatus = Status_Default;

    pItem->maxSize = sizeof(DataBuffer);

    //strncpy(pItem->itemName, paramsname(paramsId), sizeof(pItem->itemName)-1);

    memcpy(&pItem->data, defaultVal, sizeof(Data));
    memcpy(&pItem->defaultData, defaultVal, sizeof(Data));

    #if defined(_DEBUG) && defined(WIN32)
    switch(pItem->data.valueType )
    {
    case VAT_Int32:
        ShowDebug("add params : %s, value : %d, list count %d\r\n",
            paramsname(paramsId),
            pItem->data.dBuffer.nVal,
            tp_list_length(paramsManager.items));
        break;
    case VAT_String:
        ShowDebug("add params : %s, value : %s, list count %d\r\n",
            paramsname(paramsId),
            pItem->data.dBuffer.cBuffer,
            tp_list_length(paramsManager.items));
        break;
    default:
        ShowDebug("add params : %s, dataSize : %d, list count %d\r\n",
            paramsname(paramsId),
            pItem->data.ssize,
            tp_list_length(paramsManager.items));
        break;
    }
    #endif

    return pItem;
}

/**
 * 函 数 TP_Is_ParamsExist() 功能描述：检测参数是穘存評
 * @return 絬点地址
 * @author  吴耀泉
 * @date 20120223
 * @note
 */
 void TP_DbgSerialPrnHex(uint8* inBuf, uint16 inLen);

static ParamsItem* TP_Is_ParamsExist(ParamsManager *pParamsManager, PARAMS_NAMES paramsId)
{
       int32 count = 0;
    TPList *elem;
    ParamsItem *item = NULL;
    if(pParamsManager == NULL)
        return NULL;
    if(ParamsIDValuable(paramsId) == FALSE)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("unvaluable paramsid : %d\r\n", paramsId);
        #endif
        return NULL;
    }
    //若IDo行Ф问齛幢i瞖找不到则膗存溢出,数据被i獠啃薷�
    for(elem=pParamsManager->items; elem!=NULL;elem=tp_list_next(elem))
    {
        item=(ParamsItem*)elem->data;
        if (item->paramsId == paramsId)
        {
            return item;
        }
              count++;
              if(count>NAME_PARAMS(COUNT))
              {
                //n反问躠浚蚴o之为膗存溢出
                TP_DbgSerialPrnHex((uint8*)"参数膗存异常\r\n", strlen((char *)"参数膗存异常\r\n"));
                break;
              }
    }

    return NULL;
}

/**
 * 函 数 TP_ParamsItem_Find() 功能描述：籲取参数项絬点
 * @return 絬点地址
 * @author  吴耀泉
 * @date 20120223
 * @note
 */
 void TP_Check_Mem_Info(ParamsItem *item)
{
    static uint32 lastId = 0;
    if(lastId == NAME_PARAMS(COUNT)-1)
        lastId = 0;
    if(item->paramsId != (lastId+1))
    {
        if(paramsManager.enTag.bEffective == TRUE && paramsManager.enTag.MOFErrorFunc != NULL)
        {
            paramsManager.enTag.MOFErrorFunc((int32)item);
        }
    }
    lastId = item->paramsId;
}

static ParamsItem* TP_ParamsItem_Find(ParamsManager *pParamsManager, PARAMS_NAMES paramsId)
{
    ParamsItem *item = NULL;

    item = TP_Is_ParamsExist(pParamsManager, paramsId);
    if(item == NULL)
    {
        if(pParamsManager->enTag.bEffective == TRUE && pParamsManager->enTag.MOFErrorFunc != NULL)
        {
        #if defined(_DEBUG) && defined(WIN32)
                ShowDebug("Error check params mem : %d\r\n", paramsId);
        #endif
            tp_list_foreach(pParamsManager->items, (TPFunc)TP_Check_Mem_Info, NULL);
        }
        return NULL;
    }
    else
        return item;
}

/**
 * 函 数 TP_Add_ParamsItem() 功能描述：蘨紀参数项絬点
 * @param paramsType 参数类衖
 * @param paramsId 参数id
 * @param maxSize 改参数数据区最磑长度,
    <=0 或>sizeof(DataBuffer)时，使o媚洗i;
 * @return 砮功返回0,失败返回-1
 * @author  吴耀泉
 * @date 20120227
 * @note
 */
int32 TP_Add_ParamsItem(ParamsType paramsType,
                   PARAMS_NAMES paramsId,
                   Data* defaultVal)
{
    ParamsItem *pItem = NULL;
    pItem = TP_NewParams(paramsType, paramsId, defaultVal);
    if(pItem != NULL)
    {
        TP_Manager_Add_Item(&paramsManager, pItem);
        return 0;
    }
    else
        return -1;
}

/**
 * 函 数 TP_Manager_Remove_item() 功能描述：参数参数絬点
 * @return
 * @author  吴耀泉
 * @date 20120227
 * @note
 */
void TP_Manager_Remove_item(ParamsManager *tpParamsManager, ParamsItem *item)
{
    if(tpParamsManager != NULL)
    {
        tpParamsManager->items = tp_list_remove(tpParamsManager->items,(tppointer)item);
        TP_ParamsItem_Destroy(item);
    }
}

/**
 * 函 数 TP_Remove_ParamsItem() 功能描述：e境问�
 * @param paramsId 参数id
 * @return 无
 * @author  吴耀泉
 * @date 20120223
 * @note
 */
int32 TP_Remove_ParamsItem(PARAMS_NAMES paramsId)
{
    ParamsItem *pTemp = TP_ParamsItem_Find(&paramsManager, paramsId);
    if(pTemp != NULL)
    {
        TP_Manager_Remove_item(&paramsManager, pTemp);
        return 0;
    }
    return -1;
}

/**
 * 函 数 TP_RemoveAll_ParamsItem() 功能描述：e境鵲胁问�
 * @return 无
 * @author  吴耀泉
 * @date 20120225
 * @note
 */
void TP_RemoveAll_ParamsItem()
{
    #if defined(_DEBUG) && defined(WIN32)
    ShowDebug("params items count : %d\r\n", tp_list_length(paramsManager.items));
    #endif
    //每一个list都带o幸桓鰀ata，先将data蔵放，再蔵放list node
    tp_list_foreach(paramsManager.items, (TPFunc)TP_ParamsItem_Destroy, NULL);
    tp_list_free(paramsManager.items);

    managerInitFlag = FALSE;

    paramsManager.items = NULL;
}


/**
 * 函 数 TP_Set_ParamsItem_Status() 功能描述：e柚貌问刺�
 * @return 无
 * @author  吴耀泉
 * @date 20120401
 * @note
 */
int32 TP_Set_ParamsItem_Status(PARAMS_NAMES paramsId, DataStatus dStatus)
{
    ParamsItem *pTemp = NULL;
    if(managerInitFlag == FALSE)
        return -1;

    if(ParamsIDValuable(paramsId) == FALSE)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("unvaluable paramsid : %d\r\n", paramsId);
        #endif
        return -2;
    }
    if(DataStatusValuable(dStatus) == FALSE)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("unvaluable data status : %d\r\n", dStatus);
        #endif
        return -3;
    }

    pTemp = TP_ParamsItem_Find(&paramsManager, paramsId);
    if(pTemp != NULL)
    {
        pTemp->dStatus = dStatus;
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("set paramitem [%d, %s] status : !!!\r\n", paramsId,  paramsname(paramsId), dStatus);
        #endif
        //showLcdDebug("set paramitem [%d, %s] status : %d!!!\r\n", paramsId, paramsname(paramsId), dStatus);
        return 0;
    }
    return -4;

}
/**
 * 函 数 TP_Set_ParamsItem_CrcError() 功能描述：e柚貌问e出磇状态
 * @param paramsId 参数id
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120223
 * @note
 */
int32 TP_Set_ParamsItem_CrcError(PARAMS_NAMES paramsId)
{
    ParamsItem *pTemp = NULL;
    if(managerInitFlag == FALSE)
        return -1;

    if(ParamsIDValuable(paramsId) == FALSE)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("unvaluable paramsid : %d\r\n", paramsId);
        #endif
        return -2;
    }

    pTemp = TP_ParamsItem_Find(&paramsManager, paramsId);
    if(pTemp != NULL)
    {
        pTemp->dStatus = Status_CrcErr;
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("set paramitem [%s] crcError !!!\r\n", paramsname(paramsId));
        #endif
        return 0;
    }
    return -3;
}

/**
 * 函 数 TP_SetParamsVal() 功能描述：e柚貌问�
 * @param paramsId 参数id
 * @param pVal 数据源
 * @param dStatus 参数数据状态
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120223
 * @note
 */
int32 TP_Update_ParamsItem_Val(PARAMS_NAMES paramsId, Data* pVal, DataStatus dStatus)
{
    ParamsItem *pTemp = NULL;
    if(pVal == NULL || managerInitFlag == FALSE)
        return -1;

    if(ValueTypeValuable((ValueType)pVal->valueType) == FALSE)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("unvaluable valueType : %d\r\n", pVal->valueType);
        #endif

        return -2;
    }

    if(ParamsIDValuable(paramsId) == FALSE)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("unvaluable paramsid : %d\r\n", paramsId);
        #endif
        return -3;
    }

    if(DataStatusValuable(dStatus) == FALSE)
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("unvaluable data status : %d\r\n", dStatus);
        #endif
        return -4;
    }
    if(pVal->ssize>sizeof(DataBuffer))
    {
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("unvaluable data size: %d\r\n", pVal->ssize);
        #endif
        return -5;
    }

    pTemp = TP_ParamsItem_Find(&paramsManager, paramsId);
    if(pTemp != NULL)
    {
        pTemp->dStatus = dStatus;
        memset(&pTemp->data, 0, sizeof(Data));
        memcpy(&pTemp->data, (uint8*)pVal, DATA_HEAD_LEN+pVal->ssize);

        return 0;
    }
    return -6;
}

/**
 * 函 数 TP_Set_ParamsItem_Val() 功能描述：e柚貌问�
 * @param paramsId 参数id
 * @param pVal 数据源
 * @return 砮功返回0,失败返回-1
 * @author  吴耀泉
 * @date 20120223
 * @note
    e柚贸e功并保存謅flash
 */
int32 TP_Set_ParamsItem_Val(PARAMS_NAMES paramsId, Data* pVal)
{
    int32 ret;
    DataStatus status;
    Data tmp;

    ret = TP_Get_ParamsItem_Val(paramsId, &tmp);
    if(ret != 0)
        return -1;

    status = TP_Get_ParamsItem_Status(paramsId);

    ret = TP_Update_ParamsItem_Val(paramsId, pVal, Status_User);
    if(ret == 0)
    {
        ret = TP_Save_ParamsItem_To_File(paramsId);
        if(ret == 0)
            return 0;
        //保存失败,则反更新
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("set paramitem val failed : %d;   reset paramsitem\r\n", ret);
        #endif
        if(DataStatusValuable(status) == TRUE)
            ret = TP_Update_ParamsItem_Val(paramsId, &tmp, status);
        else
            ret = TP_Update_ParamsItem_Val(paramsId, &tmp, Status_Default);
    }

    return ret;
    //i北４�
}

/**
 * 函 数 TP_Get_ParamsItem_Status() 功能描述：籲取参数的状态
 * @param paramsId 参数id
 * @return 砮功返回合法的状态{Status_Default, Status_CrcErr, Status_User},
    失败返回Status_NULL
 * @author  吴耀泉
 * @date 20120315
 * @note
 */
DataStatus TP_Get_ParamsItem_Status(PARAMS_NAMES paramsId)
{
    ParamsItem *pTemp= NULL;
    if(managerInitFlag == FALSE)
    {
        return Status_NULL;
    }

    pTemp = TP_ParamsItem_Find(&paramsManager, paramsId);
    if(pTemp != NULL)
    {
        return ((DataStatus)pTemp->dStatus);
    }

    return Status_NULL;
}

/**
 * 函 数 TP_Is_ParamsItem_StatusError() 功能描述：检测参数状态是穘异常
 * @param paramsId 参数id
 * @return
    砮功返回TRUE;
    失败返回FALSE
 * @author  吴耀泉
 * @date 20120405
 * @note
*/
Boolean TP_Is_ParamsItem_StatusException(PARAMS_NAMES paramsId)
{
    Boolean flag = TRUE;
    ParamsItem *pTemp= NULL;
    if(managerInitFlag == FALSE)
    {
        return flag;
    }

    pTemp = TP_ParamsItem_Find(&paramsManager, paramsId);
    if(pTemp != NULL)
    {
        switch( pTemp->dStatus)
        {
        case Status_User:
        case Status_Backup:
            flag = TRUE;
            break;
        default:
            flag = FALSE;
            break;
        }
    }
    return flag;
}

/**
 * 函 数 TP_GetParamsVal() 功能描述：禷取参数值
 * @param paramsId 参数id
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120223
 * @note
 */
int32 TP_Get_ParamsItem_Val(PARAMS_NAMES paramsId, Data* writeBuf)
{
    ParamsItem *pTemp= NULL;
    if(!writeBuf || managerInitFlag == FALSE)
    {
        return -1;
    }

    pTemp = TP_ParamsItem_Find(&paramsManager, paramsId);
    if(pTemp != NULL)
    {
        memset(writeBuf, 0, sizeof(Data));
        memcpy(writeBuf, &pTemp->data, sizeof(Data));
        return 0;
    }
    return -2;
}

/**
 * 函 数 TP_ParamsResetDefault() 功能描述：参数恢复为默认值
 * @param paramsId 参数id
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120223
 * @note
 */
int32 TP_Set_ParamsItem_Default(PARAMS_NAMES paramsId)
{
    int32 ret;
    Data tmp;
    ParamsItem *pTemp= NULL;

    if(managerInitFlag == FALSE)
        return -1;

    pTemp = TP_ParamsItem_Find(&paramsManager, paramsId);
    if(pTemp != NULL)
    {
        #if defined(_DEBUG) && defined(WIN32)
        if(pTemp->data.valueType == VAT_Int32)
            ShowDebug("[data : %d, def : %d]\r\n",
                pTemp->data.dBuffer.nVal,
                pTemp->defaultData.dBuffer.nVal);
        else
            ShowDebug("[data : %s, def : %s]\r\n",
                pTemp->data.dBuffer.cBuffer,
                pTemp->defaultData.dBuffer.cBuffer);
        #endif
        pTemp->dStatus = Status_Default;
        memcpy(&tmp, &pTemp->data, sizeof(Data));
        memcpy(&pTemp->data, &pTemp->defaultData, sizeof(Data));

        ret = TP_Save_ParamsItem_To_File(paramsId);
        if(ret == 0)
            return 0;
        else
            memcpy(&pTemp->data, &tmp, sizeof(Data));
        return ret;
    }
    return -2;
}
/**
 * 函 数 TP_ParamsResetDefault() 功能描述：参数恢复为默认值
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120224
 * @note
 */
void TP_Default_ParamsItem(ParamsItem *item)
{
    if(item != NULL)
    {
        #if defined(_DEBUG) && defined(WIN32)
        if(item->data.valueType == VAT_Int32)
            ShowDebug("[data : %d, def : %d]\r\n",
                item->data.dBuffer.nVal,
                item->defaultData.dBuffer.nVal);
        else
            ShowDebug("[data : %s, def : %s]\r\n",
                item->data.dBuffer.cBuffer,
                item->defaultData.dBuffer.cBuffer);
        #endif
        item->dStatus = Status_Default;
        memcpy(&item->data, &item->defaultData, sizeof(item->data));
    }
}
/**
 * 函 数 TP_AllParamsResetDefault() 功能描述：所o胁问指次现�
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120223
 * @note
    被动态e境南畈蛔龌指�
 */
int32 TP_Set_AllParamsItem_Default()
{
    if(managerInitFlag == FALSE)
        return -1;

    tp_list_foreach(paramsManager.items, (TPFunc)TP_Default_ParamsItem, NULL);


    return TP_Save_AllParamsItem_To_File();
}

/**
 * 函 数 TP_Get_ParamsItem_Length() 功能描述：籲得参数总数a�
 * @return砮功返回总a�
 * @author  吴耀泉
 * @date 20120228
 * @note
 */
uint32 TP_Get_ParamsItem_Length()
{
    return tp_list_length(paramsManager.items);
}

/**
 * 函 数 TP_Get_Nth_ParamsItem_Data() 功能描述：籲得指定索引的参数
 * @param index 索引
 * @param data 数据写缓冲
 * @return砮功返回参数o行d，NAME_PARAMS(NULL), NAME_PARAMS(COUNT)除i�
 * @author  吴耀泉
 * @date 20120228
 * @note
 */
PARAMS_NAMES TP_Get_Nth_ParamsItem_Data(uint32 index, Data *data)
{
    ParamsItem *tmp;
    if(data == NULL || managerInitFlag == FALSE)
        return NAME_PARAMS(NULL);

    tmp = tp_list_nth_data(paramsManager.items, index);
    if(tmp == NULL)
        return NAME_PARAMS(NULL);
    else
    {
        memset(data, 0, sizeof(Data));
        memcpy(data, &tmp->data, sizeof(Data));
        return (PARAMS_NAMES)tmp->paramsId;
    }
}

/**
 * 函 数 TP_Get_Nth_ParamsItem_Defaultdata() 功能描述：籲得指定索引的默认参数
 * @param index 索引
 * @param data 数据写缓冲
 * @return砮功返回参数o行d，NAME_PARAMS(NULL), NAME_PARAMS(COUNT)除i�
 * @author  吴耀泉
 * @date 20120228
 * @note
 */
PARAMS_NAMES TP_Get_Nth_ParamsItem_Defaultdata(uint32 index, Data *data)
{
    ParamsItem *tmp;
    if(data == NULL || managerInitFlag == FALSE)
        return NAME_PARAMS(NULL);

    tmp = tp_list_nth_data(paramsManager.items, index);
    if(tmp == NULL)
        return NAME_PARAMS(NULL);
    else
    {
        memset(data, 0, sizeof(Data));
        memcpy(data, &tmp->defaultData, sizeof(Data));
        return (PARAMS_NAMES)tmp->paramsId;
    }
}

/**
 * 函 数 TP_Get_ParamsItem_DefaultVal() 功能描述：籲得指定ID的默认参数
 * @param paramsId 参数id
 * @param data 数据写缓冲
 * @return砮功返回0
 * @author  吴耀泉
 * @date 20120228
 * @note
 */
uint32 TP_Get_ParamsItem_DefaultVal(PARAMS_NAMES paramsId, Data *data)
{
    TPList *elem;
    ParamsItem *item = NULL;
    if(data== NULL ||managerInitFlag == FALSE)
        return -1;
    for(elem=paramsManager.items; elem!=NULL;elem=tp_list_next(elem))
    {
        item=(ParamsItem*)elem->data;
        if (item->paramsId == paramsId)
        {
            memset(data, 0, sizeof(Data));
            memcpy(data, &item->defaultData, sizeof(Data));
            return 0;
        }
    }
    return -2;

}

/**
 * 函 数 TP_Printf_ParamsItem_Info() 功能描述：打o〔问畔�
 * @param item
 * @return无
 * @author  吴耀泉
 * @date 20120228
 * @note
 */
void TP_Printf_ParamsItem_Info(ParamsItem *item)
{
    static uint32 lastId = 0;
    #if defined(_DEBUG) && defined(WIN32)
    ShowDebug("ParamsItem Name      : %s\r\n", paramsname(item->paramsId));
    ShowDebug("ParamsItem ID        : %d\r\n", item->paramsId);
    switch(item->data.valueType)
    {
    case VAT_Int32:
        ShowDebug("ParamsItem Data      : CurVal [%d]",
                item->data.dBuffer.nVal);
        break;
    case VAT_String:
        ShowDebug("ParamsItem Data      : CurVal [%s]",
                item->data.dBuffer.cBuffer);
        break;
    case VAT_Stream:
        ShowDebug("ParamsItem Data      : CurVal [%d]",
                item->data.dBuffer.nVal);
        break;
    default:
        ShowDebug("ParamsItem Data      : CurVal [unkonwType]");
        break;
    }
    switch(item->defaultData.valueType)
    {
    case VAT_Int32:
        ShowDebug(", DefaultVal [%d]\r\n",
                item->defaultData.dBuffer.nVal);
        break;
    case VAT_String:
        ShowDebug(", DefaultVal [%s]\r\n",
                item->defaultData.dBuffer.cBuffer);
        break;
    case VAT_Stream:
        ShowDebug(", DefaultVal [%d]\r\n",
                item->defaultData.dBuffer.nVal);
        break;
    default:
        ShowDebug(", DefaultVal [unkonwType]\r\n");
        break;
    }

    #else
    if(lastId == NAME_PARAMS(COUNT)-1)
        lastId = 0;
    if(item->paramsId != (lastId+1))
    {
        //showLcdDebug("1 Error : [%d, %d]\r\n", lastId, item->paramsId);
        //showLcdDebug("2 Error : [%d, %d]\r\n", lastId, item->paramsId);
        //showLcdDebug("3 Error : [%d, %d]\r\n", lastId, item->paramsId);
        //showLcdDebug("4 Error : [%d, %d]\r\n", lastId, item->paramsId);
    }
    lastId = item->paramsId;

    //showLcdDebug("Name : %s\r\n", paramsname(item->paramsId));
    //showLcdDebug("ID   : %d\r\n", item->paramsId);
    /*
    switch(item->data.valueType)
    {
    case VAT_Int32:
        showLcdDebug("Data      : CurVal [%d]",
                item->data.dBuffer.nVal);
        break;
    case VAT_String:
        showLcdDebug("Data      : CurVal [%s]",
                item->data.dBuffer.cBuffer);
        break;
    case VAT_Stream:
        showLcdDebug("Data      : CurVal [%d]",
                item->data.dBuffer.nVal);
        break;
    default:
        showLcdDebug("Data      : CurVal [unkonwType]");
        break;
    }
    switch(item->defaultData.valueType)
    {
    case VAT_Int32:
        showLcdDebug(", DefaultVal [%d]\r\n",
                item->defaultData.dBuffer.nVal);
        break;
    case VAT_String:
        showLcdDebug(", DefaultVal [%s]\r\n",
                item->defaultData.dBuffer.cBuffer);
        break;
    case VAT_Stream:
        showLcdDebug(", DefaultVal [%d]\r\n",
                item->defaultData.dBuffer.nVal);
        break;
    default:
        showLcdDebug(", DefaultVal [unkonwType]\r\n");
        break;
    }
    */
    #endif

}


void TP_Check_ParamsItem_Info(ParamsItem *item)
{
    static uint32 lastId = 0;
    if(lastId == NAME_PARAMS(COUNT)-1)
        lastId = 0;
    if(item->paramsId != (lastId+1))
    {
        //TP_ScrBackLight(1);
        //showLcdDebug("1 Error : [%d, %d]\r\n", lastId, item->paramsId);
        //showLcdDebug("2 Error : [%d, %d]\r\n", lastId, item->paramsId);
        //showLcdDebug("3 Error : [%d, %d]\r\n", lastId, item->paramsId);
        //showLcdDebug("4 Error : [%d, %d]\r\n", lastId, item->paramsId);
        //TP_ScrBackLight(0);
    }
    lastId = item->paramsId;

}
/**
 * 函 数 TP_Printf_ParamsManager_Info() 功能描述：打o∷鵲胁问畔�
 * @return无
 * @author  吴耀泉
 * @date 20120228
 * @note
 */
void TP_Printf_ParamsManager_Info()
{
    #if defined(_DEBUG) && defined(WIN32)
    ShowDebug("ParamsCount : %d\r\n", tp_list_length(paramsManager.items));
    #endif
    if(managerInitFlag != NULL)
        tp_list_foreach(paramsManager.items, (TPFunc)TP_Printf_ParamsItem_Info, NULL);
}


void TP_Check_ParamsManager_Info()
{
    #if defined(_DEBUG) && defined(WIN32)
    ShowDebug("ParamsCount : %d\r\n", tp_list_length(paramsManager.items));
    #endif
    if(managerInitFlag != NULL)
        tp_list_foreach(paramsManager.items, (TPFunc)TP_Check_ParamsItem_Info, NULL);

    //showLcdDebug("TP_Check_ParamsManager_Info over\r\n");
}

/**
 * 函 数 doCrcCheckErrorCallBack() 功能描述：crc出磇e媳�
 * @return无
 * @author  吴耀泉
 * @date 20120316
 * @note
 */
static void doCrcCheckErrorCallBack()
{
    int32 i=0;
    TPList *elem;
    ParamsItem *item = NULL;
    PARAMS_NAMES errId[NAME_PARAMS(COUNT)];

    for(elem=paramsManager.items; elem!=NULL;elem=tp_list_next(elem))
    {
        item=(ParamsItem*)elem->data;
        if (item->dStatus == Status_CrcErr)
        {
            errId[i++] = (PARAMS_NAMES)item->paramsId;
        }
    }
    if(paramsManager.enTag.bEffective == TRUE && paramsManager.enTag.crcErrFunc != NULL)
    {
    #if defined(_DEBUG) && defined(WIN32)
            ShowDebug("checkCrcError : %d\r\n", i);
    #endif
        paramsManager.enTag.crcErrFunc((PARAMS_NAMES*)&errId, i);
    }
}

/**
 * 函 数 doUnInitErrorCallBack() 功能描述：参数未初始化出磇e媳�
 * @return无
 * @author  吴耀泉
 * @date 20120316
 * @note
 */
static void doUnInitErrorCallBack()
{
    int32 i = 0;
    ParamsItem *item;
    PARAMS_NAMES id;
    PARAMS_NAMES errId[NAME_PARAMS(COUNT)];

    #if defined(_DEBUG) && defined(WIN32)
    ShowDebug("\r\nWarning : the count of params_link not equal with NAME_PARAMS(COUNT) : [%d, %d]\r\n",
        TP_Get_ParamsItem_Length(), (NAME_PARAMS(COUNT)-1));
    #endif

    for(id = (PARAMS_NAMES)(NAME_PARAMS(NULL)+1); id<NAME_PARAMS(COUNT); id++)
    {
        item = TP_ParamsItem_Find(&paramsManager, id);
        if(item == NULL)
        {
            errId[i++] = id;
            #if defined(_DEBUG) && defined(WIN32)
            ShowDebug("Warning : You need add ParamsItem      [%s]\r\n",
                paramsname(id));
            #endif
        }
    }
    #if defined(_DEBUG) && defined(WIN32)
    ShowDebug("\r\n");
    #endif

    if(paramsManager.enTag.bEffective == TRUE && paramsManager.enTag.unInitErrFunc != NULL)
    {
    #if defined(_DEBUG) && defined(WIN32)
            ShowDebug("Error unInit check count : %d\r\n", i);
    #endif
        paramsManager.enTag.unInitErrFunc((PARAMS_NAMES*)&errId, i);
    }
}


/**
 * 函 数 onMemoryFlowOverErrorCallBack() 功能描述：参数动态a幢i数据被改写
 * @return无
 * @author  吴耀泉
 * @date 20120316
 * @note
 */
static void onMemoryFlowOverErrorCallBack(int32 address)
{
    if(paramsManager.enTag.bEffective == TRUE && paramsManager.enTag.MOFErrorFunc != NULL)
    {
    #if defined(_DEBUG) && defined(WIN32)
            ShowDebug("Error check memory overflow\r\n");
    #endif
        paramsManager.enTag.MOFErrorFunc(address);
    }
}

