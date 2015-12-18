#ifndef TP_SYSTEMPARAMSINIT_CC
#define TP_SYSTEMPARAMSINIT_CC
//该文件无需加入到makefile编译
/*******************************************步骤二************************************************/
//用户在此宏定义追加用户参数的命名
/*****************************************
            测试参数，使用前删除 START
******************************************/
#define NAME_SYSTEM_PARAMS  \
    CASE(ID_PARAMS_SMSSaveOn);  \
    CASE(ID_PARAMS_SMSOverwrite);   \
    CASE(ID_PARAMS_RingType);   \
    CASE(ID_PARAMS_KeypadVolumn);   \
    CASE(ID_PARAMS_RingVolumn); \
    CASE(ID_PARAMS_MicrophoneVolumn); \
    CASE(ID_PARAMS_SpeakerVolumn);  \
    CASE(ID_PARAMS_Backlight);  \
    CASE(ID_PARAMS_Contrast);   \
    CASE(ID_PARAMS_SecurityPassword);   \
    CASE(ID_PARAMS_OwnNumber);
#if 0
#ifdef TPS300
#define NAME_SYSTEM_PARAMS  \
    CASE(ID_PARAMS_AAAAA); \
    CASE(ID_PARAMS_BBBBB); \
    CASE(ID_PARAMS_CCCCC); \
    CASE(ID_PARAMS_DDDDD);
#endif


#ifdef TPS320
#define NAME_SYSTEM_PARAMS \
    CASE(ID_PARAMS_AAAAA); \
    CASE(ID_PARAMS_BBBBB); \
    CASE(ID_PARAMS_CCCCC); \
    CASE(ID_PARAMS_DDDDD);
#endif


#ifdef TP_JINDUN_G
#define NAME_SYSTEM_PARAMS \
    CASE(ID_PARAMS_AAAAA); \
    CASE(ID_PARAMS_BBBBB); \
    CASE(ID_PARAMS_CCCCC); \
    CASE(ID_PARAMS_DDDDD);
#endif
#endif
/*****************************************
            测试参数，使用前删除 END
******************************************/

#include <TPFlashParam.h>
#include "tpcore_params.h"

/**
 * 函 数 SYSTEM_PARAMS_INIT_LIST() 功能描述: 初始化系统参数
 * @return
 * @author  吴耀泉
 * @date 20120323
 * @note
    用户在此函数进行系统参数的初始化
*/
static void SYSTEM_PARAMS_INIT_LIST()
{
    uint8 defaultContrast;

    /*************************************************
    几种参数初始化的方法:
    1. int32:
        TP_AddValueParams;
    2. string:
        TP_AddStringParams;
    3. stream:
        TP_AddStreamParams;
    4:  先格式数据后加入
        TP_Format_Data;
        TP_Add_ParamsItem;
    **************************************************/

/*****************************************
            测试参数，使用前删除 END
******************************************/
    TP_AddValueParams(PAT_NULL, NAME_PARAMS(SMSSaveOn),0);
    TP_AddValueParams(PAT_NULL, NAME_PARAMS(SMSOverwrite),0);

    TP_AddValueParams(PAT_NULL, NAME_PARAMS(RingType),3);
    TP_AddValueParams(PAT_NULL, NAME_PARAMS(KeypadVolumn),3);
    TP_AddValueParams(PAT_NULL, NAME_PARAMS(RingVolumn),3);
    TP_AddValueParams(PAT_NULL, NAME_PARAMS(MicrophoneVolumn),3);
    TP_AddValueParams(PAT_NULL, NAME_PARAMS(SpeakerVolumn),3);

    TP_AddValueParams(PAT_NULL, NAME_PARAMS(KeypadLock),0);
    TP_AddValueParams(PAT_NULL, NAME_PARAMS(KeypadLockTimeout),60000);
    TP_AddValueParams(PAT_NULL, NAME_PARAMS(Backlight),1);
    if (TP_FlashParam_Read("LcdContrastLevel", VAL_INTERGER,
                           &defaultContrast, sizeof(defaultContrast)) > 0)
    {
        TP_AddValueParams(PAT_NULL, NAME_PARAMS(Contrast), defaultContrast);
    }
    else
    {
        TP_AddValueParams(PAT_NULL, NAME_PARAMS(Contrast), TP_LCD_DefaultContrastLevel);
    }
    TP_AddStringParams(PAT_NULL,NAME_PARAMS(SecurityPassword),(uint8*)"1234");
    TP_AddStringParams(PAT_NULL,NAME_PARAMS(OwnNumber),(uint8*)"");
/*****************************************
            测试参数，使用前删除 END
******************************************/

}


#endif

