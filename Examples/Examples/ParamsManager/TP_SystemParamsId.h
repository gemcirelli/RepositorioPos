/*******************************************************************************
 * �ļ�λ��: TP_SystemParamsId.h
 * �ļ�˵��: ϵͳ�̶�����ID
 * ��������: 2012��03��23��
 * ������        : ��ҫȪ 
 * �޸�����:
 * �޸�˵��:
 * �޸�����:
 * ��ע      :���ܸ��ݸ�ƽ̨���ж������ID:
 ******************************************************************************/
#ifndef TP_SYSTEMPARAMSID_H
#define TP_SYSTEMPARAMSID_H

/*---------------------------------------------------------------------------------------
                    ���� ID ������ʹ��׼��

1. ��ɾID
    �û�����ά��ö�ټ�PARAMS_NAMES;
    ����:ID_PARAMS_BASE Ϊ������ֵ,ID_PARAMS_COUNTΪ��������,
    ���߽���,����ɾ��

2. ��������
   ��const char *paramsname(PARAMS_NAMES type)������,������ö�ټ�PARAMS_NAMES
һ�µ�CASE(ID)����.

3.
    ���ڲ���ID��Flash�洢����һһ��Ӧ,����ID����Ĵ������˱仯,��ԭaaaaa��bbbbb��ccccc����������
��Ϊbbbbb��ccccc��aaaaa,��������ƫ����Flash�洢��һ�£�������ߵ����ݶ����ܵ�Ӱ��;
    ����,����Ŀ����������,�������ԭ�еĲ�������ɾ�Ļ�����µ���Ŀ,�����Ӳ�������β�����롣

4. ͨ�����ϲ���,�����������Զ����غͱ���ö�ټ��ڵĲ�����.
   �����ڶ�̬ɾ���Ľڵ�,��������������;
   ���ڷǲ��������ڵĽڵ�,��������������;

ϵͳ������:
    NAME_PARAMS(BackLight),                  //��ʾ����--�����
    NAME_PARAMS(Constrast),           //��ʾ����--�Աȶ�
    NAME_PARAMS(Hyphen),           //��ʾ����--���ַ�
    NAME_PARAMS(LockNumber),             // ��ȫ��������

��ע:�����������洢�ռ��4k,����������Ϣ�洢.
---------------------------------------------------------------------------------------*/

/*******************************************����һ************************************************/

/*****************************************
            ���Բ�����ʹ��ǰɾ�� START
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
    NAME_PARAMS(AAAAA),           //����ID
    NAME_PARAMS(BBBBB),           //����ID
    NAME_PARAMS(CCCCC),           //����ID
    NAME_PARAMS(DDDDD),           //����ID
#endif


#ifdef TPS320
    NAME_PARAMS(AAAAA),           //����ID
    NAME_PARAMS(BBBBB),           //����ID
    NAME_PARAMS(CCCCC),           //����ID
    NAME_PARAMS(DDDDD),           //����ID
#endif


#ifdef TP_JINDUN_G
    NAME_PARAMS(AAAAA),           //����ID
    NAME_PARAMS(BBBBB),           //����ID
    NAME_PARAMS(CCCCC),           //����ID
    NAME_PARAMS(DDDDD),           //����ID
#endif
#endif
/*****************************************
            ���Բ�����ʹ��ǰɾ�� END
******************************************/

#endif

