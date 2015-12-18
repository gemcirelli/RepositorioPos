/*******************************************************************************
 * �ļ�λ��: TP_ParamsManager.h
 * �ļ�˵��: �������i��API�Ķ���
 * �������u: 2012��02��23��
 * ������        : ��ҫȪ
 * �޸����u:
 * �޸�˵��:
 * �޸����u:
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
 *�������i�����u������Ļnȡ���en���e��á�����ȹ��ܡ�
 *����ֵֻ�e�uĳָ���Ĳ������u�����uo���ֻo�һ��Ĭ��ֵ��
 *
*************************************************************************/

static Boolean managerInitFlag = FALSE;

/**
 * �� �� TP_ParamsManager_Init() ��������: ��ʼ��o�oò���
 * @return
 * @author  ��ҫȪ
 * @date 20120227
 * @note
     ���ocrc���ii���
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
        //��oû����Զ���Ĳ������г�ʼ��
        TP_UserParamsItems_Init();

        if(TP_Get_ParamsItem_Length() != (NAME_PARAMS(COUNT)-1))
        {
            // ��ղ���,����ʼ��
            doUnInitErrorCallBack();
            TP_RemoveAll_ParamsItem();
            return;
        }

        managerInitFlag = TRUE;
        //�o�����ļ���o��oû�����Ĳ���
        TP_LoadParamsFromFile();

        //���crc�i�o����i���eϲ�o�o�
        doCrcCheckErrorCallBack();
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("********** End Init ParamsManager **********\r\n");
        #endif
    }
}

/**
 * �� �� TP_IsParamsManagerInit() ��������: �nȡ�������i����ʼ��״̬
 * @return
 * @author  ��ҫȪ
 * @date 20120227
 * @note
*/
Boolean TP_IsParamsManagerInit()
{
    return managerInitFlag;
}
/**
 * �� �� TP_ParamsItem_Destroy() �����������i�Žu���u��
 * @return
 * @author  ��ҫȪ
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
 * �� �� TP_Manager_Destroy() ������������ղ������i��
 * @return ��
 * @author  ��ҫȪ
 * @date 20120227
 * @note
*/
void TP_Manager_Destroy()
{
    //ÿһ��list����o�һ��data���Ƚ�data�i�ţ����i��list node
    tp_list_foreach(paramsManager.items, (TPFunc)TP_ParamsItem_Destroy,NULL);
    tp_list_free(paramsManager.items);
    #if defined(_DEBUG) && defined(WIN32)
    ShowDebug("list count : %d\r\n", tp_list_length(paramsManager.items));
    #endif
    managerInitFlag = FALSE;
    paramsManager.items = NULL;
}
/**
 * �� �� ValueTypeBeable() ������������e�������i��o�Ч�i
 * @return BOOLEAN
 * @author  ��ҫȪ
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
 * �� �� ParamsTypeBeable() ������������e���ݷ����o�Ч�i
 * @return BOOLEAN
 * @author  ��ҫȪ
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
 * �� �� ParamsTypeBeable() ������������e����ID ��o�Ч�i
 * @return BOOLEAN
 * @author  ��ҫȪ
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
 * �� �� DataStatusValuable() ������������e����״̬�Ƿno�Ч
 * @return BOOLEAN
 * @author  ��ҫȪ
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
        //���ݹ���
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
 * �� �� TP_AddValueParams() �����������i�o��ֵ������
 * @param paramsType �������i
 * @param paramsId ����id
 * @param defaultVal Ĭ�ϲ���ֵ
 * @return �e������0,ʧ�ܷ���-1
 * @author  ��ҫȪ
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
 * �� �� TP_AddStringParams() �����������i�o�ַ�������
 * @param paramsType, �������i
 * @param paramsId, ����id
 * @param defaultBuf Ĭ�ϲ���ֵ
 * @return �e������0,ʧ�ܷ���-1
 * @author  ��ҫȪ
 * @date 20120223
 * @note
    �ַ���defaultBuf���ȱ���oou��ou0��Сou64���ֽu;
    ���i���ַ���
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
 * �� �� TP_AddStreamParams() �����������i�oa�������
 * @param paramsType, �������i
 * @param paramsId, ����id
 * @param dataSize, defaultDataBuf�����ݳ���
 * @param defaultDataBuf Ĭ�ϲ���ֵ
 * @return �e������0,ʧ�ܷ���-1
 * @author  ��ҫȪ
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
 * �� �� TP_ParamsItem_Compare_ByParamsID() ����������������id�oС���o����
 * @param itema
 * @param itemb
 * @return itema <= itemb ����0���n�򷵻�1
 * @author  ��ҫȪ
 * @date 20120228
 * @note
 ��������o�Ч,�n���޷��Ƚ�
 */
int32 TP_ParamsItem_Compare_ByParamsID(ParamsItem *itema, ParamsItem *itemb)
{
    if((int32)itema->paramsId <= (int32)itemb->paramsId)
        return 0;
    else
        return 1;
}

/**
 * �� �� TP_AddStringParams() �����������i�o�ַ�������
 * @return ��
 * @author  ��ҫȪ
 * @date 20120223
 * @note
 * �i�o�������ָ����������
 ���i��Ĭ��ֵ�����ݴoС���n���u�Ĳ������������i��
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
 * �� �� TP_Format_Data() �����������nʽ����������
 * @return �e������0,ʧ�ܷ��ط�0
 * @author  ��ҫȪ
 * @date 20120223
 * @note
 */
int32 TP_Format_Data(Data *pData,
                    ValueType type,
                    uint32 dataSize,        //���ֽuΪ��λ
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
 * �� �� TP_NewParams() ������������̬e���һ�������u��
 * @return �e������o�Ч��ַ,ʧ�ܷ���0
 * @author  ��ҫȪ
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
 * �� �� TP_Is_ParamsExist() �����������������Ƿn���u
 * @return �u���ַ
 * @author  ��ҫȪ
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
    //��IDo�Ч������a��i�e�Ҳ������u�����,���ݱ�iⲿ�޸�
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
                //n�������������a������o֮Ϊ�u�����
                TP_DbgSerialPrnHex((uint8*)"�����u���쳣\r\n", strlen((char *)"�����u���쳣\r\n"));
                break;
              }
    }

    return NULL;
}

/**
 * �� �� TP_ParamsItem_Find() �����������nȡ������u��
 * @return �u���ַ
 * @author  ��ҫȪ
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
 * �� �� TP_Add_ParamsItem() �����������i�o������u��
 * @param paramsType �������i
 * @param paramsId ����id
 * @param maxSize �Ĳ�����������o����,
    <=0 ��>sizeof(DataBuffer)ʱ��ʹo�Ĭ�ϴ��i;
 * @return �e������0,ʧ�ܷ���-1
 * @author  ��ҫȪ
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
 * �� �� TP_Manager_Remove_item() �������������������u��
 * @return
 * @author  ��ҫȪ
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
 * �� �� TP_Remove_ParamsItem() ����������e���������
 * @param paramsId ����id
 * @return ��
 * @author  ��ҫȪ
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
 * �� �� TP_RemoveAll_ParamsItem() ����������e�����oв�����
 * @return ��
 * @author  ��ҫȪ
 * @date 20120225
 * @note
 */
void TP_RemoveAll_ParamsItem()
{
    #if defined(_DEBUG) && defined(WIN32)
    ShowDebug("params items count : %d\r\n", tp_list_length(paramsManager.items));
    #endif
    //ÿһ��list����o�һ��data���Ƚ�data�i�ţ����i��list node
    tp_list_foreach(paramsManager.items, (TPFunc)TP_ParamsItem_Destroy, NULL);
    tp_list_free(paramsManager.items);

    managerInitFlag = FALSE;

    paramsManager.items = NULL;
}


/**
 * �� �� TP_Set_ParamsItem_Status() ����������e��ò���״̬
 * @return ��
 * @author  ��ҫȪ
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
 * �� �� TP_Set_ParamsItem_CrcError() ����������e��ò���Уne���i״̬
 * @param paramsId ����id
 * @return�e������0
 * @author  ��ҫȪ
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
 * �� �� TP_SetParamsVal() ����������e��ò���ֵ
 * @param paramsId ����id
 * @param pVal ����Դ
 * @param dStatus ��������״̬
 * @return�e������0
 * @author  ��ҫȪ
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
 * �� �� TP_Set_ParamsItem_Val() ����������e��ò���ֵ
 * @param paramsId ����id
 * @param pVal ����Դ
 * @return �e������0,ʧ�ܷ���-1
 * @author  ��ҫȪ
 * @date 20120223
 * @note
    e��óe���������aflash
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
        //����ʧ��,�򷴸���
        #if defined(_DEBUG) && defined(WIN32)
        ShowDebug("set paramitem val failed : %d;   reset paramsitem\r\n", ret);
        #endif
        if(DataStatusValuable(status) == TRUE)
            ret = TP_Update_ParamsItem_Val(paramsId, &tmp, status);
        else
            ret = TP_Update_ParamsItem_Val(paramsId, &tmp, Status_Default);
    }

    return ret;
    //i�ʱ����
}

/**
 * �� �� TP_Get_ParamsItem_Status() �����������nȡ������״̬
 * @param paramsId ����id
 * @return �e�����غϷ���״̬{Status_Default, Status_CrcErr, Status_User},
    ʧ�ܷ���Status_NULL
 * @author  ��ҫȪ
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
 * �� �� TP_Is_ParamsItem_StatusError() ����������������״̬�Ƿn�쳣
 * @param paramsId ����id
 * @return
    �e������TRUE;
    ʧ�ܷ���FALSE
 * @author  ��ҫȪ
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
 * �� �� TP_GetParamsVal() �����������aȡ����ֵ
 * @param paramsId ����id
 * @return�e������0
 * @author  ��ҫȪ
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
 * �� �� TP_ParamsResetDefault() ���������������ָ�ΪĬ��ֵ
 * @param paramsId ����id
 * @return�e������0
 * @author  ��ҫȪ
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
 * �� �� TP_ParamsResetDefault() ���������������ָ�ΪĬ��ֵ
 * @return�e������0
 * @author  ��ҫȪ
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
 * �� �� TP_AllParamsResetDefault() ������������oв����ָ�ΪĬ��ֵ
 * @return�e������0
 * @author  ��ҫȪ
 * @date 20120223
 * @note
    ����̬e���������ָ�
 */
int32 TP_Set_AllParamsItem_Default()
{
    if(managerInitFlag == FALSE)
        return -1;

    tp_list_foreach(paramsManager.items, (TPFunc)TP_Default_ParamsItem, NULL);


    return TP_Save_AllParamsItem_To_File();
}

/**
 * �� �� TP_Get_ParamsItem_Length() �����������n�ò�������a�
 * @return�e��������a�
 * @author  ��ҫȪ
 * @date 20120228
 * @note
 */
uint32 TP_Get_ParamsItem_Length()
{
    return tp_list_length(paramsManager.items);
}

/**
 * �� �� TP_Get_Nth_ParamsItem_Data() �����������n��ָ�������Ĳ���
 * @param index ����
 * @param data ����д����
 * @return�e�����ز���o�Чid��NAME_PARAMS(NULL), NAME_PARAMS(COUNT)��i�
 * @author  ��ҫȪ
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
 * �� �� TP_Get_Nth_ParamsItem_Defaultdata() �����������n��ָ��������Ĭ�ϲ���
 * @param index ����
 * @param data ����д����
 * @return�e�����ز���o�Чid��NAME_PARAMS(NULL), NAME_PARAMS(COUNT)��i�
 * @author  ��ҫȪ
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
 * �� �� TP_Get_ParamsItem_DefaultVal() �����������n��ָ��ID��Ĭ�ϲ���
 * @param paramsId ����id
 * @param data ����д����
 * @return�e������0
 * @author  ��ҫȪ
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
 * �� �� TP_Printf_ParamsItem_Info() ������������o�������Ϣ
 * @param item
 * @return��
 * @author  ��ҫȪ
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
 * �� �� TP_Printf_ParamsManager_Info() ������������o���oв�����Ϣ
 * @return��
 * @author  ��ҫȪ
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
 * �� �� doCrcCheckErrorCallBack() ����������crc���ieϱ�
 * @return��
 * @author  ��ҫȪ
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
 * �� �� doUnInitErrorCallBack() ��������������δ��ʼ�����ieϱ�
 * @return��
 * @author  ��ҫȪ
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
 * �� �� onMemoryFlowOverErrorCallBack() ����������������̬a��i���ݱ���д
 * @return��
 * @author  ��ҫȪ
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

