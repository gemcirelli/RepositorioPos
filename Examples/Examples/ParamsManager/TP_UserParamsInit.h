#ifndef TP_USERPARAMSINIT_CC
#define TP_USERPARAMSINIT_CC
//���ļ�������뵽makefile����

/*******************************************�����************************************************/
//�û��ڴ˺궨��׷���û�����������
#define NAME_USER_PARAMS
//  CASE(ID_PARAMS_EEEEE);\
//  CASE(ID_PARAMS_FFFFF);\
//  CASE(ID_PARAMS_GGGGG);
/*****************************************
			���Բ�����ʹ��ǰɾ�� START
******************************************/

/*****************************************
			���Բ�����ʹ��ǰɾ�� END
******************************************/

//
/**
 * �� �� USER_PARAMS_INIT_LIST() ��������: ��ʼ��ҵ��Ӧ�ò���
 * @return 
 * @author  ��ҫȪ
 * @date 20120323
 * @note 
	�û��ڴ˺��������û������ĳ�ʼ��
*/
static void USER_PARAMS_INIT_LIST()
{
	/*************************************************
	���ֲ�����ʼ���ķ���:
	1. int32:
		TP_AddValueParams;
	2. string:
		TP_AddStringParams;
	3. stream:
		TP_AddStreamParams;
	4:	�ȸ�ʽ���ݺ����
		TP_Format_Data;
		TP_Add_ParamsItem;
	**************************************************/
	//���ο��������Զ���Ĳ�����ʼֵ

/*****************************************
			���Բ�����ʹ��ǰɾ�� START
******************************************/

    #if 0
    TP_AddValueParams(PAT_NULL, NAME_PARAMS(EEEEE),0);
    TP_AddStringParams(PAT_NULL, NAME_PARAMS(FFFFF),"1234567890");
    TP_AddValueParams(PAT_NULL, NAME_PARAMS(GGGGG), 2);
    #endif


/*****************************************
			���Բ�����ʹ��ǰɾ�� END
******************************************/



}	

#endif

