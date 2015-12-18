
/* 在此处增加恢复出厂设置后系统需要重新进行初始化的操作*/
//需要初始化的操作比如话机参数初始化、公共模块初始化、部分全局变量置初始值
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
