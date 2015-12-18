/*******************************************************************************
 * 文件位置: TP_ParamsBackUp.h
 * 文件说明: 参数管理器数据备份API的定义
 * 创建日期: 2012年03月16日
 * 创建人	     : 吴耀泉 
 * 修改日期:
 * 修改说明:
 * 修改日期:
 * 备注	     :
	1. 将参数管理器内部所有数据以数据流方式交由上层业务备份
	2. 上层业务输入完整的参数数据流，参数管理器通过分析流数据进行加载参数。
******************************************************************************/
#ifndef TP_PARAMSBACKUP_H
#define TP_PARAMSBACKUP_H
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 函 数 TP_BakeUp_ParamsItem() 功能描述：将参数管理器内部所有参数以流方式输出
 * @param dataBuffer, 上层业务提供的写缓冲区;
 * @param length, 上层业务提供的写缓冲长度;
 * @return >0 数据长度
	<=0,缓冲区不足
 * @author  吴耀泉
 * @date 20120315
 * @note 
 */
int32 TP_BakeUp_ParamsItem(uint8* buffer, uint32 length);

/**
 * 函 数 TP_ParamsItem_LoadFrom_BakeUp() 功能描述：将参数管理器内部所有参数以流方式输出
 * @param dataStream, 上层业务提供的参数数据流;
 * @param length, 上层业务提供的参数数据流的数据长度;
 * @return >0 加载的参数个数
	<=0,数据异常
 * @author  吴耀泉
 * @date 20120315
 * @note 
 */
int32 TP_ParamsItem_LoadFrom_BakeUp(const uint8* buffer, uint32 length);

#ifdef __cplusplus
}
#endif

#endif
