/*******************************************************************************
 * �ļ�λ��: TP_ParamsManager.h
 * �ļ�˵��: ����������API�Ķ���
 * ��������: 2012��02��23��
 * ������	     : ��ҫȪ 
 * �޸�����:
 * �޸�˵��:
 * �޸�����:
 * ��ע	     :
 			Լ��:���ڲ�Ʒʹ�ö��ԣ������������ڲ�����Ĳ���������һ��Ʒ��
 			δ����Ч��
 				������ļ�������,Ҫע�����¼���:
 				1. ĳһ������������С���������ͷ����仯ʱ,���¸�ʽ��flash
 				2. ����ö�ټ������仯ʱ�����Ӳ�����,�����¸�ʽ��flash
 				3. ����Ӧ�ó���������,���ݼ���,��֧���³������׷��.
 ******************************************************************************/
 
#ifndef TP_PARAMSMANAGER_H
#define TP_PARAMSMANAGER_H
#ifdef __cplusplus
extern "C" {
#endif
#include "TPTypedef.h"
#include "TPBase.h"
#include "TPLib.h"
#include "TPList.h"
#include "TP_ParamsItemID.h"

#ifdef __cplusplus
}
#endif

//������������
typedef enum _ValueType{
	VAT_NULL = 0,
	VAT_Int32,
	VAT_String,
	VAT_Stream,
	VAT_COUNT
}ValueType;

//��������
typedef enum _ParamsType{
	PAT_NULL = 0,
	PAT_Normal,
	PAT_General,
	PAT_Security,
	PAT_COUNT
}ParamsType;
#define MAX_STR_PARAM_LEN 128
//�������ݻ�����
typedef union _DataBuffer
{
    int32 nVal;
    uint8 cBuffer[MAX_STR_PARAM_LEN];
}DataBuffer;


//4+4+64
typedef struct _Data
{
	//ValueType valueType;
	uint32 valueType;
	uint32 ssize;				//
	//ע������������Ͷ��峤��int32:4, string:64;Ӱ�쵽�����ļ��Ĵ洢
	//���峤�Ⱥ󣬲����ٴθı�
	DataBuffer dBuffer;
}Data;

//����״̬
typedef enum {
	Status_NULL = 0, 
	//��������
	Status_Default, 		//Ĭ�ϲ���
	Status_User,			//�û�����
	Status_Backup,		//Դ���û�����
	//�쳣����		[ʹ��Ĭ��ֵ]
	Status_CrcErr,		//CRCУ�����
	Status_FileErr,		//���ļ�����Ĳ���ʧ��
	Status_Count
}DataStatus;

//�����ṹ
typedef struct _ParamsItem{
//	PARAMS_NAMES	paramsId;		//����id
//	ParamsType	paramsType;
	uint32	paramsId;		//����id
	uint32	paramsType;
//	uint8	itemName[32];
	uint32 dStatus;
	uint32 maxSize;
	Data data;
	Data defaultData;
}ParamsItem;

/*�����������ص�����*/
//��������crcУ��ʧ�ܻص�����
typedef void (*OnCrcErrorFuncP)(PARAMS_NAMES paramsId[], uint32 length);

//����δ��ʼ���ϱ��ص�����
typedef void (*OnUnInitErrorFuncP)(PARAMS_NAMES paramsId[], uint32 length);

//�򵥵��ڴ�����ϱ��ص�����
typedef void (*OnMemoryOverflowErrorFuncP)(int32 address);


//������������ʼ������
typedef struct 
{
	uint32 bEffective;
	OnCrcErrorFuncP crcErrFunc;
	OnUnInitErrorFuncP unInitErrFunc;
	OnMemoryOverflowErrorFuncP MOFErrorFunc;
}ParamsManagerEntryTag;

//����������
typedef struct _ParamsManager{
	int32 hFile;
	uint8 fileName[32];
	TPList *head;
	TPList *items;
	TPList *tail;
	ParamsManagerEntryTag enTag;
}ParamsManager;

#define CRC16_SIZE			sizeof(uint16)
						//ValueType		ssize
#define DATA_HEAD_LEN (sizeof(uint32)+sizeof(uint32))

#define SYSPARAMSHEADINFO_LEN (sizeof(SysParamsHeadInfo))


/*---------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************
 *
 *�û�������
 *
*********************************************/

/**
 * �� �� TP_ParamsManager_Init() ������������ʼ��������
 * @return ��
 * @author  ��ҫȪ
 * @date 20120228
 * @note 
 *��������ö�ټ�NAME_PARAMS ���������Ĳ���id������ȫ����ʼ��
 */
void TP_ParamsManager_Init(ParamsManagerEntryTag *pEntry);

/**
 * �� �� TP_Manager_Destroy() ������������ղ���������
 *@return ��
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
*/
void TP_Manager_Destroy(void);

/**
 * �� �� TP_IsParamsManagerInit() ��������: ��ȡ������������ʼ��״̬
 *@return 
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
*/
Boolean TP_IsParamsManagerInit(void);

/**
 * �� �� paramsname() ��������: ���ݲ���id��ȡ�ַ�������
 * @param type, ����id
 * @return uint8* �ַ�����
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
*/
const uint8 *paramsname(PARAMS_NAMES type);

/**
 * �� �� TP_AddValueParams() ���������������ֵ������
 *@param paramsType ��������
 *@param paramsId ����id
 *@param defaultVal Ĭ�ϲ���ֵ
 *@return �ɹ�����0,ʧ�ܷ���-1
 * @author  ��ҫȪ
 * @date 20120223
 * @note 
 */
 int32 TP_AddValueParams(ParamsType paramsType,
				   PARAMS_NAMES paramsId,
				   int32 defaultVal);

/**
 * �� �� TP_AddStringParams() ��������������ַ�������
 *@param paramsType, ��������
 *@param paramsId, ����id
 *@param defaultBuf Ĭ�ϲ���ֵ
 *@return �ɹ�����0,ʧ�ܷ���-1
 * @author  ��ҫȪ
 * @date 20120223
 * @note 
	�ַ���defaultBuf���ȱ�����ڵ���0��С��64���ֽ�
	������ַ���
 */
int32 TP_AddStringParams(ParamsType paramsType,
				   PARAMS_NAMES paramsId,
				   const uint8* defaultBuf);

/**
 * �� �� TP_AddStreamParams() ���������������������
 *@param paramsType, ��������
 *@param paramsId, ����id
 *@param dataSize, defaultDataBuf�����ݳ���
 *@param defaultDataBuf Ĭ�ϲ���ֵ
 *@return �ɹ�����0,ʧ�ܷ���-1
 * @author  ��ҫȪ
 * @date 20120223
 * @note 
 */
int32 TP_AddStreamParams(ParamsType paramsType,
				   PARAMS_NAMES paramsId,
				   uint32 dataSize,
				   const void* defaultDataBuf);
	
/**
 * �� �� TP_Add_ParamsItem() ������������Ӳ�����
 *@param paramsType ��������
 *@param paramsId ����id
 *@param defaultVal Ĭ�ϲ���ֵ
 *@return �ɹ�����0,ʧ�ܷ���-1
 * @author  ��ҫȪ
 * @date 20120223
 * @note 
 */
int32 TP_Add_ParamsItem(ParamsType paramsType,
				   PARAMS_NAMES paramsId,
				   Data* defaultVal);

/**
 * �� �� TP_Format_Data() ������������ʽ����������
 *@return �ɹ�����0,ʧ�ܷ��ط�0
 * @author  ��ҫȪ
 * @date 20120223
 * @note 
 */
int32 TP_Format_Data(Data *pData,
					ValueType type,
					uint32 dataSize,		//���ֽ�Ϊ��λ
					const void* dataVal);
/**
 * �� �� TP_Remove_ParamsItem() ����������ɾ��������
 *@param paramsId ����id
 *@return �ɹ�����0��ʧ�ܷ���-1
 * @author  ��ҫȪ
 * @date 20120223
 * @note 
	������ʹ��,����Ӱ��ײ�����ļ��Ĵ洢,���������쳣
 */
int32 TP_Remove_ParamsItem(PARAMS_NAMES paramsId);

/**
 * �� �� TP_RemoveAll_ParamsItem() ����������ɾ�����в�����
 * @return ��
 * @author  ��ҫȪ
 * @date 20120225
 * @note 
	������ʹ��,����Ӱ��ײ�����ļ��Ĵ洢,����flash�����ļ��쳣
 */
void TP_RemoveAll_ParamsItem();

/**
 * �� �� TP_Get_ParamsItem_Status() ������������ȡ������״̬
 * @param paramsId ����id
 * @return �ɹ����غϷ���״̬{Status_Default, Status_CrcErr, Status_User},
 	ʧ�ܷ���Status_NULL
 * @author  ��ҫȪ
 * @date 20120223
 * @note 
  */
DataStatus TP_Get_ParamsItem_Status(PARAMS_NAMES paramsId);

/**
 * �� �� TP_Is_ParamsItem_StatusError() ����������������״̬�Ƿ��쳣
 * @param paramsId ����id
 * @return 
 	�ɹ�����TRUE;
 	ʧ�ܷ���FALSE
 * @author  ��ҫȪ
 * @date 20120405
 * @note 
 	����״̬: Status_User Status_Backup;
 	�쳣״̬: Status_Default Status_CrcErr Status_FileErr
*/
Boolean TP_Is_ParamsItem_StatusException(PARAMS_NAMES paramsId);

/**
 * �� �� TP_Set_ParamsItem_Val() �������������ò���ֵ
 * @param paramsId ����id
 * @param pVal ����Դ
 * @return �ɹ�����0,ʧ�ܷ���-1
 * @author  ��ҫȪ
 * @date 20120223
 * @note 
 	���óɹ���������flash
 */
int32 TP_Set_ParamsItem_Val(PARAMS_NAMES paramsId, Data* pVal);

/**
 * �� �� TP_Get_ParamsItem_Val() ������������ȡ����ֵ
 * @param paramsId ����id
 * @param pVal ����д����
 * @return�ɹ�����0,ʧ�ܷ���-1
 * @author  ��ҫȪ
 * @date 20120223
 * @note 
 */
int32 TP_Get_ParamsItem_Val(PARAMS_NAMES paramsId, Data* writeBuf);

/**
 * �� �� TP_Set_ParamsItem_Default() ���������������ָ�ΪĬ��ֵ
 * @param paramsId ����id
 * @return�ɹ�����0,ʧ�ܷ���-1
 * @author  ��ҫȪ
 * @date 20120223
 * @note 
 */
int32 TP_Set_ParamsItem_Default(PARAMS_NAMES paramsId);

/**
 * �� �� TP_Set_AllParamsItem_Default() �������������в����ָ�ΪĬ��ֵ
 * @return�ɹ�����0
 * @author  ��ҫȪ
 * @date 20120223
 * @note 
 	����̬ɾ��������ָ�
 */
int32 TP_Set_AllParamsItem_Default(void);

/**
 * �� �� TP_Get_ParamsItem_Length() ������������ò���������
 * @return�ɹ���������
 * @author  ��ҫȪ
 * @date 20120228
 * @note 
 */
uint32 TP_Get_ParamsItem_Length(void);

/**
 * �� �� TP_Get_Nth_ParamsItem_Data() �������������ָ�������Ĳ���
 * @param index ����
 * @param data ����д����
 * @return�ɹ����ز�����Чid��NAME_PARAMS(NULL), NAME_PARAMS(COUNT)����
 * @author  ��ҫȪ
 * @date 20120228
 * @note 
 */
PARAMS_NAMES TP_Get_Nth_ParamsItem_Data(uint32 index, Data *data);

/**
 * �� �� TP_Get_Nth_ParamsItem_Defaultdata() �������������ָ��������Ĭ�ϲ���
 * @param index ����
 * @param data ����д����
 * @return�ɹ����ز�����Чid��NAME_PARAMS(NULL), NAME_PARAMS(COUNT)����
 * @author  ��ҫȪ
 * @date 20120228
 * @note 
 */
PARAMS_NAMES TP_Get_Nth_ParamsItem_Defaultdata(uint32 index, Data *data);


/**
 * �� �� TP_Get_IDth_ParamsItem_Defaultdata() �������������ָ��ID��Ĭ�ϲ���
 * @param paramsId ����id
 * @param data ����д����
 * @return�ɹ�����0
 * @author  ��ҫȪ
 * @date 20120228
 * @note 
 */
uint32 TP_Get_ParamsItem_DefaultVal(PARAMS_NAMES paramsId, Data *data);

#ifdef __cplusplus
}
#endif

#endif


