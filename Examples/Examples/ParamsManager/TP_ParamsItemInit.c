/*******************************************************************************
 * 文件位置: TP_UserParamsInit.c
 * 文件说明: 参数管纈器API的定义
 * 创建日苪: 2012年03月23日
 * 创建人        : 吴耀泉
 * 修改日苪:
 * 修改说明:
 * 修改日苪:
    备注，
    1. 系i巢问ㄒ寮俺跏蓟�
        IDe昵�: <TP_SystemParamsId.h>
        初始化: <TP_SystemParamsInit.cc>
    2. o没Р问ㄒ寮俺跏蓟�
        IDe昵�: <TP_UserParamsId.h>
        初始化: <TP_UserParamsInit.cc>
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "TPTypedef.h"
#include "TP_ParamsManager.h"
//系i巢问跏蓟�
#include "TP_SystemParamsInit.h"
//o没Р问跏蓟�
#include "TP_UserParamsInit.h"

#define CASE(type)  case type: return ((uint8*)#type)

/**
 * 函 数 paramsname() 功能描述: 根据参数id籲取字符串命名
 * @param type, 参数id
 * @return uint8* 字符命名
 * @author  吴耀泉
 * @date 20120227
 * @note
*/
const uint8 *paramsname(PARAMS_NAMES type)
{
    switch(type)
    {
    /*
    如需打oD名，需評此增紀对oΦ腎D
    */
    //系i巢问�
#ifdef NAME_SYSTEM_PARAMS
    NAME_SYSTEM_PARAMS
#undef NAME_SYSTEM_PARAMS
#endif
    //o没Р问�
#ifdef NAME_USER_PARAMS
    NAME_USER_PARAMS;
#undef NAME_USER_PARAMS
#endif
    default:
        return (uint8*)"INVALID NAME";
    }
    return NULL;
}


/**
 * 函 数 TP_UserParamsItems_Init() 功能描述: 二次开发者自定义参数初始化
 * @return void
 * @author  吴耀泉
 * @date 20120227
 * @note
*/
void TP_UserParamsItems_Init()
{
    //根据实际莈况实现系i砊P_SystemParamsInit.cc 文件
    SYSTEM_PARAMS_INIT_LIST();
    //o没ё允迪諸P_UserParamsInit.cc 文件
    USER_PARAMS_INIT_LIST();
}


