/*******************************************************************************
 * 文件位置: TP_SystemParamsId.h
 * 文件说明: 系统固定参数ID
 * 创建日期: 2012年03月23日
 * 创建人        : 吴耀泉 
 * 修改日期:
 * 修改说明:
 * 修改日期:
 * 备注      :仅能根据各平台进行定义参数ID:
 ******************************************************************************/
#ifndef TP_SYSTEMPARAMSID_H
#define TP_SYSTEMPARAMSID_H

/*---------------------------------------------------------------------------------------
                    参数 ID 命名与使用准则

1. 增删ID
    用户自行维护枚举集PARAMS_NAMES;
    其中:ID_PARAMS_BASE 为基参数值,ID_PARAMS_COUNT为参数总量,
    作边界检测,不可删改

2. 参数命名
   在const char *paramsname(PARAMS_NAMES type)函数内,增加与枚举集PARAMS_NAMES
一致的CASE(ID)即可.

3.
    由于参数ID与Flash存储索引一一对应,参数ID申请的次序发生了变化,如原aaaaa、bbbbb、ccccc的声明次序
变为bbbbb、ccccc、aaaaa,导致数据偏移与Flash存储不一致，则会三者的数据都会受到影响;
    所以,在项目开发过程中,不建议对原有的参数进行删改或插入新的条目,若增加参数则在尾部插入。

4. 通过以上步骤,管理器即可自动加载和保存枚举集内的参数项.
   对于在动态删除的节点,管理器不作保存;
   对于非参数集合内的节点,管理器不作加载;

系统参数如:
    NAME_PARAMS(BackLight),                  //显示设置--背光灯
    NAME_PARAMS(Constrast),           //显示设置--对比度
    NAME_PARAMS(Hyphen),           //显示设置--连字符
    NAME_PARAMS(LockNumber),             // 安全设置密码

备注:参数管理器存储空间仅4k,不做大量信息存储.
---------------------------------------------------------------------------------------*/

/*******************************************步骤一************************************************/

/*****************************************
            测试参数，使用前删除 START
******************************************/
        NAME_PARAMS(SMSSaveOn),
        NAME_PARAMS(SMSOverwrite),

        NAME_PARAMS(RingType),
        NAME_PARAMS(KeypadVolumn),
        NAME_PARAMS(RingVolumn),
        NAME_PARAMS(MicrophoneVolumn),
        NAME_PARAMS(SpeakerVolumn),

        NAME_PARAMS(KeypadLock),
        NAME_PARAMS(KeypadLockTimeout),
        NAME_PARAMS(Backlight),
        NAME_PARAMS(Contrast),
        NAME_PARAMS(SecurityPassword),
        NAME_PARAMS(OwnNumber),

#if 0
#ifdef TPS300
    NAME_PARAMS(AAAAA),           //测试ID
    NAME_PARAMS(BBBBB),           //测试ID
    NAME_PARAMS(CCCCC),           //测试ID
    NAME_PARAMS(DDDDD),           //测试ID
#endif


#ifdef TPS320
    NAME_PARAMS(AAAAA),           //测试ID
    NAME_PARAMS(BBBBB),           //测试ID
    NAME_PARAMS(CCCCC),           //测试ID
    NAME_PARAMS(DDDDD),           //测试ID
#endif


#ifdef TP_JINDUN_G
    NAME_PARAMS(AAAAA),           //测试ID
    NAME_PARAMS(BBBBB),           //测试ID
    NAME_PARAMS(CCCCC),           //测试ID
    NAME_PARAMS(DDDDD),           //测试ID
#endif
#endif
/*****************************************
            测试参数，使用前删除 END
******************************************/

#endif

