/*******************************************************************************
 * �ļ�λ��: TP_ParamsAnalyser.h
 * �ļ�˵��: �������ر���API�Ķ���
 * ��������: 2012��02��26��
 * ������	     : ��ҫȪ 
 * �޸�����:
 * �޸�˵��:
 * �޸�����:
 * ��ע	     : ������ļ�������,Ҫע�����¼���:
 				1. ĳһ������������С���������ͷ����仯ʱ,���¸�ʽ��flash
 				2. ����ö�ټ������仯ʱ�����Ӳ�����,�����¸�ʽ��flash
 				3. ����Ӧ�ó���������,���ݼ���,��֧���²���׷��.
 ******************************************************************************/
#ifndef TP_PARAMSANALYSER_H
#define TP_PARAMSANALYSER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * �� �� TP_LoadParamsFromFile() ����������
 �Ӳ����ļ�Ⱥ�м��ز���;������ʧ�ܣ����Զ��ָ�Ĭ�ϲ���
 *@return�ɹ�����0,ʧ�ܷ��ط���
 * @author  ��ҫȪ
 * @date 20120227
 * @note 
 	���ڲ���ʧ�ܵĲ���������
 */
int32 TP_LoadParamsFromFile(void);

/**
 * �� �� TP_Save_AllParamsItem_To_File() �������������������浽�ļ�
 * @params paramsID, Ҫ����Ĳ���ID
 * @return �ɹ�����0,ʧ�ܷ��ط�0
 * @author  ��ҫȪ
 * @date 20120229
 * @note 
 */
int32 TP_Save_ParamsItem_To_File(PARAMS_NAMES paramsId);

/**
 * �� �� TP_Save_AllParamsItem_To_File() ���������������в������浽�ļ�
 * @return�ɹ�����0
 		-1: �����ڲ����ļ���λʧ��;;
 		-2: �����ڲ����ļ��Ͳ���������(�����ļ����ݳ���С��0,�������ڲ���������);
 		-3: д������flash��ˢ��flash��ʧ��;
 		-4: Ԥ��洢����ʧ��(д��ռ䲻��);
 * @author  ��ҫȪ
 * @date 20120327
 * @note 
 */
int32 TP_Save_AllParamsItem_To_File(void);

#ifdef __cplusplus
}
#endif

#endif

