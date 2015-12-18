/*******************************************************************************
 * 文件位置: TP_ParamsManager.h
 * 文件说明: 参数管理器API的定义
 * 创建日期: 2012年02月23日
 * 创建人	     : 吴耀泉 
 * 修改日期:
 * 修改说明:
 * 修改日期:
 * 备注	     :
 			约定:对于产品使用而言，参数管理器内部定义的参数集在另一产品中
 			未必有效。
 				因参数文件的特性,要注意以下几点:
 				1. 某一参数数据区大小或数据类型发生变化时,重新格式化flash
 				2. 参数枚举集发生变化时除增加参数外,需重新格式化flash
 				3. 鉴于应用程序升级后,数据兼容,仅支持新程序参数追加.
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

//参数数据类型
typedef enum _ValueType{
	VAT_NULL = 0,
	VAT_Int32,
	VAT_String,
	VAT_Stream,
	VAT_COUNT
}ValueType;

//参数范畴
typedef enum _ParamsType{
	PAT_NULL = 0,
	PAT_Normal,
	PAT_General,
	PAT_Security,
	PAT_COUNT
}ParamsType;
#define MAX_STR_PARAM_LEN 128
//参数数据缓冲区
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
	//注意根据数据类型定义长度int32:4, string:64;影响到参数文件的存储
	//定义长度后，不得再次改变
	DataBuffer dBuffer;
}Data;

//数据状态
typedef enum {
	Status_NULL = 0, 
	//正常数据
	Status_Default, 		//默认参数
	Status_User,			//用户参数
	Status_Backup,		//源自用户备份
	//异常数据		[使用默认值]
	Status_CrcErr,		//CRC校验出错
	Status_FileErr,		//从文件读入的参数失败
	Status_Count
}DataStatus;

//参数结构
typedef struct _ParamsItem{
//	PARAMS_NAMES	paramsId;		//参数id
//	ParamsType	paramsType;
	uint32	paramsId;		//参数id
	uint32	paramsType;
//	uint8	itemName[32];
	uint32 dStatus;
	uint32 maxSize;
	Data data;
	Data defaultData;
}ParamsItem;

/*参数管理器回调函数*/
//参数加载crc校验失败回调函数
typedef void (*OnCrcErrorFuncP)(PARAMS_NAMES paramsId[], uint32 length);

//参数未初始化上报回调函数
typedef void (*OnUnInitErrorFuncP)(PARAMS_NAMES paramsId[], uint32 length);

//简单的内存溢出上报回调函数
typedef void (*OnMemoryOverflowErrorFuncP)(int32 address);


//参数管理器初始化参数
typedef struct 
{
	uint32 bEffective;
	OnCrcErrorFuncP crcErrFunc;
	OnUnInitErrorFuncP unInitErrFunc;
	OnMemoryOverflowErrorFuncP MOFErrorFunc;
}ParamsManagerEntryTag;

//参数管理器
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
 *用户操作层
 *
*********************************************/

/**
 * 函 数 TP_ParamsManager_Init() 功能描述：初始化参数表
 * @return 无
 * @author  吴耀泉
 * @date 20120228
 * @note 
 *参数命名枚举集NAME_PARAMS 内已声明的参数id都必须全部初始化
 */
void TP_ParamsManager_Init(ParamsManagerEntryTag *pEntry);

/**
 * 函 数 TP_Manager_Destroy() 功能描述：清空参数管理器
 *@return 无
 * @author  吴耀泉
 * @date 20120227
 * @note 
*/
void TP_Manager_Destroy(void);

/**
 * 函 数 TP_IsParamsManagerInit() 功能描述: 获取参数管理器初始化状态
 *@return 
 * @author  吴耀泉
 * @date 20120227
 * @note 
*/
Boolean TP_IsParamsManagerInit(void);

/**
 * 函 数 paramsname() 功能描述: 根据参数id获取字符串命名
 * @param type, 参数id
 * @return uint8* 字符命名
 * @author  吴耀泉
 * @date 20120227
 * @note 
*/
const uint8 *paramsname(PARAMS_NAMES type);

/**
 * 函 数 TP_AddValueParams() 功能描述：添加数值参数项
 *@param paramsType 参数类型
 *@param paramsId 参数id
 *@param defaultVal 默认参数值
 *@return 成功返回0,失败返回-1
 * @author  吴耀泉
 * @date 20120223
 * @note 
 */
 int32 TP_AddValueParams(ParamsType paramsType,
				   PARAMS_NAMES paramsId,
				   int32 defaultVal);

/**
 * 函 数 TP_AddStringParams() 功能描述：添加字符参数项
 *@param paramsType, 参数类型
 *@param paramsId, 参数id
 *@param defaultBuf 默认参数值
 *@return 成功返回0,失败返回-1
 * @author  吴耀泉
 * @date 20120223
 * @note 
	字符串defaultBuf长度必须大于等于0，小于64个字节
	允许空字符串
 */
int32 TP_AddStringParams(ParamsType paramsType,
				   PARAMS_NAMES paramsId,
				   const uint8* defaultBuf);

/**
 * 函 数 TP_AddStreamParams() 功能描述：添加流参数项
 *@param paramsType, 参数类型
 *@param paramsId, 参数id
 *@param dataSize, defaultDataBuf的数据长度
 *@param defaultDataBuf 默认参数值
 *@return 成功返回0,失败返回-1
 * @author  吴耀泉
 * @date 20120223
 * @note 
 */
int32 TP_AddStreamParams(ParamsType paramsType,
				   PARAMS_NAMES paramsId,
				   uint32 dataSize,
				   const void* defaultDataBuf);
	
/**
 * 函 数 TP_Add_ParamsItem() 功能描述：添加参数项
 *@param paramsType 参数类型
 *@param paramsId 参数id
 *@param defaultVal 默认参数值
 *@return 成功返回0,失败返回-1
 * @author  吴耀泉
 * @date 20120223
 * @note 
 */
int32 TP_Add_ParamsItem(ParamsType paramsType,
				   PARAMS_NAMES paramsId,
				   Data* defaultVal);

/**
 * 函 数 TP_Format_Data() 功能描述：格式化参数数据
 *@return 成功返回0,失败返回非0
 * @author  吴耀泉
 * @date 20120223
 * @note 
 */
int32 TP_Format_Data(Data *pData,
					ValueType type,
					uint32 dataSize,		//以字节为单位
					const void* dataVal);
/**
 * 函 数 TP_Remove_ParamsItem() 功能描述：删除参数项
 *@param paramsId 参数id
 *@return 成功返回0，失败返回-1
 * @author  吴耀泉
 * @date 20120223
 * @note 
	不建议使用,否则影响底层参数文件的存储,导致数据异常
 */
int32 TP_Remove_ParamsItem(PARAMS_NAMES paramsId);

/**
 * 函 数 TP_RemoveAll_ParamsItem() 功能描述：删除所有参数项
 * @return 无
 * @author  吴耀泉
 * @date 20120225
 * @note 
	不建议使用,否则影响底层参数文件的存储,导致flash配置文件异常
 */
void TP_RemoveAll_ParamsItem();

/**
 * 函 数 TP_Get_ParamsItem_Status() 功能描述：获取参数的状态
 * @param paramsId 参数id
 * @return 成功返回合法的状态{Status_Default, Status_CrcErr, Status_User},
 	失败返回Status_NULL
 * @author  吴耀泉
 * @date 20120223
 * @note 
  */
DataStatus TP_Get_ParamsItem_Status(PARAMS_NAMES paramsId);

/**
 * 函 数 TP_Is_ParamsItem_StatusError() 功能描述：检测参数状态是否异常
 * @param paramsId 参数id
 * @return 
 	成功返回TRUE;
 	失败返回FALSE
 * @author  吴耀泉
 * @date 20120405
 * @note 
 	正常状态: Status_User Status_Backup;
 	异常状态: Status_Default Status_CrcErr Status_FileErr
*/
Boolean TP_Is_ParamsItem_StatusException(PARAMS_NAMES paramsId);

/**
 * 函 数 TP_Set_ParamsItem_Val() 功能描述：设置参数值
 * @param paramsId 参数id
 * @param pVal 数据源
 * @return 成功返回0,失败返回-1
 * @author  吴耀泉
 * @date 20120223
 * @note 
 	设置成功并保存至flash
 */
int32 TP_Set_ParamsItem_Val(PARAMS_NAMES paramsId, Data* pVal);

/**
 * 函 数 TP_Get_ParamsItem_Val() 功能描述：读取参数值
 * @param paramsId 参数id
 * @param pVal 数据写缓冲
 * @return成功返回0,失败返回-1
 * @author  吴耀泉
 * @date 20120223
 * @note 
 */
int32 TP_Get_ParamsItem_Val(PARAMS_NAMES paramsId, Data* writeBuf);

/**
 * 函 数 TP_Set_ParamsItem_Default() 功能描述：参数恢复为默认值
 * @param paramsId 参数id
 * @return成功返回0,失败返回-1
 * @author  吴耀泉
 * @date 20120223
 * @note 
 */
int32 TP_Set_ParamsItem_Default(PARAMS_NAMES paramsId);

/**
 * 函 数 TP_Set_AllParamsItem_Default() 功能描述：所有参数恢复为默认值
 * @return成功返回0
 * @author  吴耀泉
 * @date 20120223
 * @note 
 	被动态删除的项不做恢复
 */
int32 TP_Set_AllParamsItem_Default(void);

/**
 * 函 数 TP_Get_ParamsItem_Length() 功能描述：获得参数总数量
 * @return成功返回总量
 * @author  吴耀泉
 * @date 20120228
 * @note 
 */
uint32 TP_Get_ParamsItem_Length(void);

/**
 * 函 数 TP_Get_Nth_ParamsItem_Data() 功能描述：获得指定索引的参数
 * @param index 索引
 * @param data 数据写缓冲
 * @return成功返回参数有效id，NAME_PARAMS(NULL), NAME_PARAMS(COUNT)除外
 * @author  吴耀泉
 * @date 20120228
 * @note 
 */
PARAMS_NAMES TP_Get_Nth_ParamsItem_Data(uint32 index, Data *data);

/**
 * 函 数 TP_Get_Nth_ParamsItem_Defaultdata() 功能描述：获得指定索引的默认参数
 * @param index 索引
 * @param data 数据写缓冲
 * @return成功返回参数有效id，NAME_PARAMS(NULL), NAME_PARAMS(COUNT)除外
 * @author  吴耀泉
 * @date 20120228
 * @note 
 */
PARAMS_NAMES TP_Get_Nth_ParamsItem_Defaultdata(uint32 index, Data *data);


/**
 * 函 数 TP_Get_IDth_ParamsItem_Defaultdata() 功能描述：获得指定ID的默认参数
 * @param paramsId 参数id
 * @param data 数据写缓冲
 * @return成功返回0
 * @author  吴耀泉
 * @date 20120228
 * @note 
 */
uint32 TP_Get_ParamsItem_DefaultVal(PARAMS_NAMES paramsId, Data *data);

#ifdef __cplusplus
}
#endif

#endif


