/*******************************************************************************
 * 文件位置: TP_ParamsBackUp.c
 * 文件说明: 参数管纈器数据备份API的实现
 * 创建日苪: 2012年03月16日
 * 创建人	     : 吴耀泉 
 * 修改日苪:
 * 修改说明:
 * 修改日苪:
 * 备注	     :
	1. 将参数管纈器膗部所o惺菀允輆鞣绞浇籵ee喜阋滴n备份
	2. e喜阋滴n输入i暾牟问輆鳎问芾i器iü治鯽魇萁屑o载参数。
******************************************************************************/
#include "TPTypedef.h"
#include "TP_ParamsBackUp.h"
#include "TP_ParamsManager.h"
#include "TP_Public.h"


extern int32 TP_ParamsItems_ToBuffer(uint8* dataBuffer, uint32 length);
extern int32 TP_LoadParams_FromBuffer(const uint8* dataStream, uint32 length);

/**
 * 函 数 TP_BakeUp_ParamsItem() 功能描述：将参数管纈器膗部所o胁问詀鞣绞绞涑�
 * @param dataBuffer, e喜阋滴n蘟供的写缓冲区;
 * @param length, e喜阋滴n蘟供的写缓冲长度;
 * @return >0 数据长度
	<=0,缓冲区不足
 * @author  吴耀泉
 * @date 20120315
 * @note 
 */
int32 TP_BakeUp_ParamsItem(uint8* dataBuffer, uint32 length)
{
	return TP_ParamsItems_ToBuffer(dataBuffer, length);
}

/**
 * 函 数 TP_ParamsItem_LoadFrom_BakeUp() 功能描述：将参数管纈器膗部所o胁问詀鞣绞绞涑�
 * @param dataStream, e喜阋滴n蘟供的参数数据a�;
 * @param length, e喜阋滴n蘟供的参数数据a鞯氖莩ざ�;
 * @return >0 紀载的参数个数
	<=0,数据异常
 * @author  吴耀泉
 * @date 20120315
 * @note 
 	仅作i獠縜骷o载,不保存到flash文件,若要保存则需給着调o肨P_Save_AllParamsItem_To_File();
 */
int32 TP_ParamsItem_LoadFrom_BakeUp(const uint8* dataStream, uint32 length)
{
	return TP_LoadParams_FromBuffer(dataStream, length);
}

