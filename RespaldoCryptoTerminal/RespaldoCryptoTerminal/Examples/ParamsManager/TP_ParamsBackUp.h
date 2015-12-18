/*******************************************************************************
 * �ļ�λ��: TP_ParamsBackUp.h
 * �ļ�˵��: �������������ݱ���API�Ķ���
 * ��������: 2012��03��16��
 * ������	     : ��ҫȪ 
 * �޸�����:
 * �޸�˵��:
 * �޸�����:
 * ��ע	     :
	1. �������������ڲ�������������������ʽ�����ϲ�ҵ�񱸷�
	2. �ϲ�ҵ�����������Ĳ���������������������ͨ�����������ݽ��м��ز�����
******************************************************************************/
#ifndef TP_PARAMSBACKUP_H
#define TP_PARAMSBACKUP_H
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * �� �� TP_BakeUp_ParamsItem() �����������������������ڲ����в���������ʽ���
 * @param dataBuffer, �ϲ�ҵ���ṩ��д������;
 * @param length, �ϲ�ҵ���ṩ��д���峤��;
 * @return >0 ���ݳ���
	<=0,����������
 * @author  ��ҫȪ
 * @date 20120315
 * @note 
 */
int32 TP_BakeUp_ParamsItem(uint8* buffer, uint32 length);

/**
 * �� �� TP_ParamsItem_LoadFrom_BakeUp() �����������������������ڲ����в���������ʽ���
 * @param dataStream, �ϲ�ҵ���ṩ�Ĳ���������;
 * @param length, �ϲ�ҵ���ṩ�Ĳ��������������ݳ���;
 * @return >0 ���صĲ�������
	<=0,�����쳣
 * @author  ��ҫȪ
 * @date 20120315
 * @note 
 */
int32 TP_ParamsItem_LoadFrom_BakeUp(const uint8* buffer, uint32 length);

#ifdef __cplusplus
}
#endif

#endif
