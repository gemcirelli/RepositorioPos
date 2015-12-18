/*******************************************************************************
 * �ļ�λ��: TP_UserParamsInit.c
 * �ļ�˵��: �������i��API�Ķ���
 * �������u: 2012��03��23��
 * ������        : ��ҫȪ
 * �޸����u:
 * �޸�˵��:
 * �޸����u:
    ��ע��
    1. ϵi��������弰��ʼ����
        IDe���: <TP_SystemParamsId.h>
        ��ʼ��: <TP_SystemParamsInit.cc>
    2. oû��������弰��ʼ����
        IDe���: <TP_UserParamsId.h>
        ��ʼ��: <TP_UserParamsInit.cc>
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "TPTypedef.h"
#include "TP_ParamsManager.h"
//ϵi�������ʼ��
#include "TP_SystemParamsInit.h"
//oû�������ʼ��
#include "TP_UserParamsInit.h"

#define CASE(type)  case type: return ((uint8*)#type)

/**
 * �� �� paramsname() ��������: ���ݲ���id�nȡ�ַ�������
 * @param type, ����id
 * @return uint8* �ַ�����
 * @author  ��ҫȪ
 * @date 20120227
 * @note
*/
const uint8 *paramsname(PARAMS_NAMES type)
{
    switch(type)
    {
    /*
    �����o�ID�������u�����o��o���ID
    */
    //ϵi���������
#ifdef NAME_SYSTEM_PARAMS
    NAME_SYSTEM_PARAMS
#undef NAME_SYSTEM_PARAMS
#endif
    //oû���������
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
 * �� �� TP_UserParamsItems_Init() ��������: ���ο������Զ��������ʼ��
 * @return void
 * @author  ��ҫȪ
 * @date 20120227
 * @note
*/
void TP_UserParamsItems_Init()
{
    //����ʵ���e��ʵ��ϵi�TP_SystemParamsInit.cc �ļ�
    SYSTEM_PARAMS_INIT_LIST();
    //oû���ʵ��TP_UserParamsInit.cc �ļ�
    USER_PARAMS_INIT_LIST();
}


