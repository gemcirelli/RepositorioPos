/*******************************************************************************
 * 文件位置: TP_ParamsItemId.h
 * 文件说明: 参数ID申请的定义
 * 创建日期: 2012年03月23日
 * 创建人	     : 吴耀泉 
 * 修改日期:
 * 修改说明:
 * 修改日期:
 * 备注	     : 所有参数ID的申请包括,系统参数、用户参数:
 ******************************************************************************/
#ifndef TP_PARAMSITEMID_H
#define TP_PARAMSITEMID_H

/*---------------------------------------------------------------------------------------
					参数 ID 命名与使用准则

1. 增删ID
	用户自行维护枚举集PARAMS_NAMES;
	其中:ID_PARAMS_BASE 为基参数值,ID_PARAMS_COUNT为参数总量,
	作边界检测,不可删改

2. 参数命名
   在const char *paramsname(PARAMS_NAMES type)函数内,增加与枚举集PARAMS_NAMES
一致的CASE(ID)即可.

3.
	由于参数ID与Flash存储索引一一对应,参数ID申请的次序发生了变化,如原aaaaa、bbbbb、ccccc的声明次序
变为bbbbb、ccccc、aaaaa,导致数据偏移与Flash存储不一致，则会三者的数据都会受到影响;
    所以,在项目开发过程中,不建议对原有的参数进行删改或插入新的条目,若增加参数则在尾部插入。

4. 通过以上步骤,管理器即可自动加载和保存枚举集内的参数项.
   对于在动态删除的节点,管理器不作保存;
   对于非参数集合内的节点,管理器不作加载;

备注:参数管理器存储空间仅4k,不做大量信息存储.
---------------------------------------------------------------------------------------*/
//基ID,不可删改
#define PARAMS_BASE_ID 0x00

#define NAME_PARAMS(Name) ID_PARAMS_##Name
//用户请在此增加删减自行定义的参数
typedef enum {
	NAME_PARAMS(NULL) = PARAMS_BASE_ID,	//不可删除,外部作为边界检测
	/*---------------------------------
			自行管理区 : Start
	---------------------------------*/	
	//系统参数ID
	#include "TP_SystemParamsId.h"
	//用户参数ID
	#include "TP_UserParamsId.h"
	/*---------------------------------
			自行管理区 : End
	---------------------------------*/
	//ID总量,总在最后
	NAME_PARAMS(COUNT)				//不可删除,外部作为边界检测
}PARAMS_NAMES;

#endif

