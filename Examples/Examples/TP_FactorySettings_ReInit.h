
/* �ڴ˴����ӻָ��������ú�ϵͳ��Ҫ���½��г�ʼ���Ĳ���*/
//��Ҫ��ʼ���Ĳ������绰��������ʼ��������ģ���ʼ��������ȫ�ֱ����ó�ʼֵ
/*Add some reinitialization here after restore factory setting*/

#ifdef INCLUDE_PHONE_SETTING
    TP_Set_AllParamsItem_Default();
#endif

#ifdef INCLUDE_MESSAGE_BOX
    TP_SMSStoreInit();
#endif

#ifdef INCLUDE_CALL_RECORD
    TP_CallRecordInit();
#endif
/*******************************************************/
