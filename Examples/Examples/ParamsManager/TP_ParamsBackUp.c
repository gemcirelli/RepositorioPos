/*******************************************************************************
 * �ļ�λ��: TP_ParamsBackUp.c
 * �ļ�˵��: �������i�����ݱ���API��ʵ��
 * �������u: 2012��03��16��
 * ������	     : ��ҫȪ 
 * �޸����u:
 * �޸�˵��:
 * �޸����u:
 * ��ע	     :
	1. ���������i���u����o�����������a���ʽ��oeeϲ�ҵ�n����
	2. eϲ�ҵ�n����i����Ĳ�������a����������i��i�������a����ݽ��мo�ز�����
******************************************************************************/
#include "TPTypedef.h"
#include "TP_ParamsBackUp.h"
#include "TP_ParamsManager.h"
#include "TP_Public.h"


extern int32 TP_ParamsItems_ToBuffer(uint8* dataBuffer, uint32 length);
extern int32 TP_LoadParams_FromBuffer(const uint8* dataStream, uint32 length);

/**
 * �� �� TP_BakeUp_ParamsItem() �������������������i���u����oв�����a���ʽ���
 * @param dataBuffer, eϲ�ҵ�n�a����д������;
 * @param length, eϲ�ҵ�n�a����д���峤��;
 * @return >0 ���ݳ���
	<=0,����������
 * @author  ��ҫȪ
 * @date 20120315
 * @note 
 */
int32 TP_BakeUp_ParamsItem(uint8* dataBuffer, uint32 length)
{
	return TP_ParamsItems_ToBuffer(dataBuffer, length);
}

/**
 * �� �� TP_ParamsItem_LoadFrom_BakeUp() �������������������i���u����oв�����a���ʽ���
 * @param dataStream, eϲ�ҵ�n�a���Ĳ�������a�;
 * @param length, eϲ�ҵ�n�a���Ĳ�������a������ݳ���;
 * @return >0 �o�صĲ�������
	<=0,�����쳣
 * @author  ��ҫȪ
 * @date 20120315
 * @note 
 	����iⲿa��o��,�����浽flash�ļ�,��Ҫ��������o�ŵ�o�TP_Save_AllParamsItem_To_File();
 */
int32 TP_ParamsItem_LoadFrom_BakeUp(const uint8* dataStream, uint32 length)
{
	return TP_LoadParams_FromBuffer(dataStream, length);
}

