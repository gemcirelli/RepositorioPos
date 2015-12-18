/*******************************************************************************
 * ÎÄ¼şÎ»ÖÃ: TP_UserParamsInit.c
 * ÎÄ¼şËµÃ÷: ²ÎÊı¹ÜÀiÆ÷APIµÄ¶¨Òå
 * ´´½¨ÈÕÆu: 2012Äê03ÔÂ23ÈÕ
 * ´´½¨ÈË        : ÎâÒ«Èª
 * ĞŞ¸ÄÈÕÆu:
 * ĞŞ¸ÄËµÃ÷:
 * ĞŞ¸ÄÈÕÆu:
    ±¸×¢£¬
    1. Ïµi³²ÎÊı¶¨Òå¼°³õÊ¼»¯£º
        IDeêÇë: <TP_SystemParamsId.h>
        ³õÊ¼»¯: <TP_SystemParamsInit.cc>
    2. oÃ»§²ÎÊı¶¨Òå¼°³õÊ¼»¯£º
        IDeêÇë: <TP_UserParamsId.h>
        ³õÊ¼»¯: <TP_UserParamsInit.cc>
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "TPTypedef.h"
#include "TP_ParamsManager.h"
//Ïµi³²ÎÊı³õÊ¼»¯
#include "TP_SystemParamsInit.h"
//oÃ»§²ÎÊı³õÊ¼»¯
#include "TP_UserParamsInit.h"

#define CASE(type)  case type: return ((uint8*)#type)

/**
 * º¯ Êı paramsname() ¹¦ÄÜÃèÊö: ¸ù¾İ²ÎÊıid»nÈ¡×Ö·û´®ÃüÃû
 * @param type, ²ÎÊıid
 * @return uint8* ×Ö·ûÃüÃû
 * @author  ÎâÒ«Èª
 * @date 20120227
 * @note
*/
const uint8 *paramsname(PARAMS_NAMES type)
{
    switch(type)
    {
    /*
    ÈçĞè´òo¡IDÃû£¬ĞèÔu´ËÔö¼o¶Ôo¦µÄID
    */
    //Ïµi³²ÎÊıÃüÃû
#ifdef NAME_SYSTEM_PARAMS
    NAME_SYSTEM_PARAMS
#undef NAME_SYSTEM_PARAMS
#endif
    //oÃ»§²ÎÊıÃüÃû
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
 * º¯ Êı TP_UserParamsItems_Init() ¹¦ÄÜÃèÊö: ¶ş´Î¿ª·¢Õß×Ô¶¨Òå²ÎÊı³õÊ¼»¯
 * @return void
 * @author  ÎâÒ«Èª
 * @date 20120227
 * @note
*/
void TP_UserParamsItems_Init()
{
    //¸ù¾İÊµ¼ÊÇe¿öÊµÏÖÏµi³TP_SystemParamsInit.cc ÎÄ¼ş
    SYSTEM_PARAMS_INIT_LIST();
    //oÃ»§×ÔÊµÏÖTP_UserParamsInit.cc ÎÄ¼ş
    USER_PARAMS_INIT_LIST();
}


