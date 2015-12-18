/*******************************************************************************
 * 文件位置: TP_ParamsAnalyser.h
 * 文件说明: 参数加载保存API的定义
 * 创建日期: 2012年02月26日
 * 创建人	     : 吴耀泉 
 * 修改日期:
 * 修改说明:
 * 修改日期:
 * 备注	     : 因参数文件的特性,要注意以下几点:
 				1. 某一参数数据区大小或数据类型发生变化时,重新格式化flash
 				2. 参数枚举集发生变化时除增加参数外,需重新格式化flash
 				3. 鉴于应用程序升级后,数据兼容,仅支持新参数追加.
 ******************************************************************************/
#ifndef TP_PARAMSANALYSER_H
#define TP_PARAMSANALYSER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 函 数 TP_LoadParamsFromFile() 功能描述：
 从参数文件群中加载参数;若加载失败，则自动恢复默认参数
 *@return成功返回0,失败返回非零
 * @author  吴耀泉
 * @date 20120227
 * @note 
 	对于不能失败的参数不加载
 */
int32 TP_LoadParamsFromFile(void);

/**
 * 函 数 TP_Save_AllParamsItem_To_File() 功能描述：将参数保存到文件
 * @params paramsID, 要保存的参数ID
 * @return 成功返回0,失败返回非0
 * @author  吴耀泉
 * @date 20120229
 * @note 
 */
int32 TP_Save_ParamsItem_To_File(PARAMS_NAMES paramsId);

/**
 * 函 数 TP_Save_AllParamsItem_To_File() 功能描述：将所有参数保存到文件
 * @return成功返回0
 		-1: 参数在参数文件定位失败;;
 		-2: 参数在参数文件和参数表不存在(参数文件数据长度小于0,参数不在参数链表内);
 		-3: 写参数到flash或刷新flash块失败;
 		-4: 预测存储参数失败(写入空间不足);
 * @author  吴耀泉
 * @date 20120327
 * @note 
 */
int32 TP_Save_AllParamsItem_To_File(void);

#ifdef __cplusplus
}
#endif

#endif

