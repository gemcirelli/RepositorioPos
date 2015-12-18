#ifndef TP_USERPARAMSINIT_CC
#define TP_USERPARAMSINIT_CC
//该文件无需加入到makefile编译

/*******************************************步骤二************************************************/
//用户在此宏定义追加用户参数的命名
#define NAME_USER_PARAMS
//  CASE(ID_PARAMS_EEEEE);\
//  CASE(ID_PARAMS_FFFFF);\
//  CASE(ID_PARAMS_GGGGG);
/*****************************************
			测试参数，使用前删除 START
******************************************/

/*****************************************
			测试参数，使用前删除 END
******************************************/

//
/**
 * 函 数 USER_PARAMS_INIT_LIST() 功能描述: 初始化业务应用参数
 * @return 
 * @author  吴耀泉
 * @date 20120323
 * @note 
	用户在此函数进行用户参数的初始化
*/
static void USER_PARAMS_INIT_LIST()
{
	/*************************************************
	几种参数初始化的方法:
	1. int32:
		TP_AddValueParams;
	2. string:
		TP_AddStringParams;
	3. stream:
		TP_AddStreamParams;
	4:	先格式数据后加入
		TP_Format_Data;
		TP_Add_ParamsItem;
	**************************************************/
	//二次开发加入自定义的参数初始值

/*****************************************
			测试参数，使用前删除 START
******************************************/

    #if 0
    TP_AddValueParams(PAT_NULL, NAME_PARAMS(EEEEE),0);
    TP_AddStringParams(PAT_NULL, NAME_PARAMS(FFFFF),"1234567890");
    TP_AddValueParams(PAT_NULL, NAME_PARAMS(GGGGG), 2);
    #endif


/*****************************************
			测试参数，使用前删除 END
******************************************/



}	

#endif

