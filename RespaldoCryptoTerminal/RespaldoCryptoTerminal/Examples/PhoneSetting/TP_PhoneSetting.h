#ifndef TP_PHONE_SETTING_H
#define TP_PHONE_SETTING_H
#include "TP_ParamsManager.h"



#ifdef PHONESETTING_CHINESE

/**********************************************************************
 *String define
 **********************************************************************/
#define TEXT_SYSSETTING                     "��������"

#define TEXT_SYSSETTING_VOICESETTING        "��������"
#define TEXT_SYSSETTING_RING                "����"
#define TEXT_SYSSETTING_VOLUMN              "����"
#define TEXT_SYSSETTING_ANNOUNCE            "��ʾ��"
#define TEXT_SYSSETTING_KEYPADVOLUMN        "��������"
#define TEXT_SYSSETTING_RINGVOLUMN          "��������"
#define TEXT_SYSSETTING_MICROPHONEVOLUMN    "�ֱ�����"
#define TEXT_SYSSETTING_SPEECHVOLUMN        "��������"
#define TEXT_SYSSETTING_DISPLAYSETTING      "��ʾ����"
#define TEXT_SYSSETTING_BACKLIGHT           "�����"
#define TEXT_SYSSETTING_CONTRAST            "�Աȶ�"
#define TEXT_SYSSETTING_TIME                "ʱ��"
#define TEXT_SYSSETTING_ON                  "��"
#define TEXT_SYSSETTING_OFF                 "��"
#define TEXT_SYSSETTING_ONONEMIN            "��һ����"
#define TEXT_SYSSETTING_SECURITYSETTING     "��ȫ����"
#define TEXT_SYSSETTING_PIN                 "Pin��"
#define TEXT_SYSSETTING_CHANGEPASSWORD      "�޸�����"
#define TEXT_SYSSETTING_RESTORESETTING      "�ָ���������"
#define TEXT_SYSSETTING_PINON               "Pin����"
#define TEXT_SYSSETTING_PINOFF              "Pin�ر�"
#define TEXT_SYSSETTING_PINCHANGE           "Pin�޸�"
#define TEXT_SYSSETTING_PHONEINFO           "������Ϣ"
#define TEXT_SYSSETTING_PHONENO             "��������"
#define TEXT_SYSSETTING_SOFTVERSION         "����汾"
#define TEXT_SYSSTETTING_IMEI               "IMEI"

#define TEXT_SYSSETTING_SELECTEXIT          "ѡ��            �˳�"
#define TEXT_SYSSETTING_PARAMSABNORMAL      "�����쳣��"
#define TEXT_SYSSETTING_CONFIRM             "ȷ��"
#define TEXT_SYSSETTING_RETURN              "����"
#define TEXT_SYSSETTING_SETUPSUCCESS        "���óɹ���"
#define TEXT_SYSSETTING_SETUPFAIL           "����ʧ�ܣ�"
#define TEXT_SYSSETTING_OUTRANGE            "������Χ"
#define TEXT_SYSSETTING_INPUTPASSWORD       "���밲ȫ����:"
#define TEXT_SYSSETTING_INPUTOLDPASSWORD    "��������룺"
#define TEXT_SYSSETTING_INPUTNEWPASSWORD    "���������룺"
#define TEXT_SYSSETTING_INPUTCFMPASSWORD    "ȷ�������룺"
#define TEXT_SYSSETTING_OLDPWDERROR         "���������!"
#define TEXT_SYSSETTING_PWDNOTEQUAL         "�������벻��ȣ�"
#define TEXT_SYSSETTING_PWDERROR            "�������"
#define TEXT_SYSSETTING_PINALREADYON        "Pin���ѿ���"
#define TEXT_SYSSETTING_PINNUMBER           "Pin�룺"
#define TEXT_SYSSETTING_OLDPINNUMMER        "��Pin��:"
#define TEXT_SYSSETTING_INPUTNEWPIN         "������Pin��:"
#define TEXT_SYSSETTING_REINPUTNEWPIN       "ȷ����Pin��:"
#define TEXT_SYSSETTING_PINONSUCCESS        "Pin�뿪���ɹ�!"
#define TEXT_SYSSETTING_PINERROR            "Pin�����"
#define TEXT_SYSSETTING_PINONFAIL           "Pin�뿪��ʧ�ܣ�"
#define TEXT_SYSSETTING_PINALREADYOFF       "Pin��δ����!"
#define TEXT_SYSSETTING_PINOFFSUCCESS       "Pin��رճɹ�!"
#define TEXT_SYSSETTING_PINOFFFAIL          "Pin��ر�ʧ�ܣ�"
#define TEXT_SYSSETTING_INPUERROR           "�������� \r���������� "
#define TEXT_SYSSETTING_PINCHANGESUCCESS    "Pin���޸ĳɹ�!"
#define TEXT_SYSSETTING_PINCHANGEFAIL       "Pin���޸�ʧ��!"
#define TEXT_SYSSETTING_SETDATETIME         "����ʱ������"
#define TEXT_SYSSETTING_YEARERROR           "��ݳ���"
#define TEXT_SYSSETTING_MONTHERROR          "�·ݳ���"
#define TEXT_SYSSETTING_DATEERROR           "�ճ���"
#define TEXT_SYSSETTING_HOURERROR           "Сʱ����"
#define TEXT_SYSSETTING_MINUTEERROR         "���ӳ���"
#define TEXT_SYSSETTING_SECONDERROR         "�����"
#define TEXT_SYSSETTING_INVALID_DATA        "��Ч����"
#define TEXT_SYSSETTING_IMEINUMBER          "IMEI��:"
#define TEXT_SYSSETTING_SOFTWAREVERSION     "����汾��:"
#define TEXT_SYSSETTING_FACTORYSETTINGNOTE  "�ָ�Ĭ�ϲ�������??"
#define TEXT_SYSSETTING_YES                 "��"
#define TEXT_SYSSETTING_NO                  "��"
#define TEXT_SYSSETTING_PLEASEWAIT          "���Ժ�..."
#define TEXT_SYSSETTING_RESTORESUCCESS      "�ָ��������óɹ�!"
#define TEXT_SYSSETTING_RESTOREFAIL         "�ָ���������ʧ��!"
#define TEXT_SYSSETTING_SETOWNNUMBER        "���ñ�������:"
#define TEXT_SYSSETTING_KEYPADLOCK          "��������"
#define TEXT_SYSSETTING_ONOFF               "��/��"
#define TEXT_SYSSETTING_TIMEOUT             "��ʱʱ��"
#define TEXT_SYSSETTING_ONEMIN              "1����"
#define TEXT_SYSSETTING_FIVEMIN             "5����"
#define TEXT_SYSSETTING_TENMIN             "10����"
#else

#define TEXT_SYSSETTING                     "Phone Setting"

#define TEXT_SYSSETTING_VOICESETTING        "Voice Setting"
#define TEXT_SYSSETTING_RING                "Ring"
#define TEXT_SYSSETTING_VOLUMN              "Volumn"
#define TEXT_SYSSETTING_ANNOUNCE            "Announce"
#define TEXT_SYSSETTING_KEYPADVOLUMN        "Keypad Volumn"
#define TEXT_SYSSETTING_RINGVOLUMN          "Ring Volumn"
#define TEXT_SYSSETTING_MICROPHONEVOLUMN    "Microphone Volumn"
#define TEXT_SYSSETTING_SPEECHVOLUMN        "Speaker Volumn"
#define TEXT_SYSSETTING_DISPLAYSETTING      "Display Setting"
#define TEXT_SYSSETTING_BACKLIGHT           "Backlight"
#define TEXT_SYSSETTING_CONTRAST            "Contrast"
#define TEXT_SYSSETTING_TIME                "Time"
#define TEXT_SYSSETTING_ON                  "On"
#define TEXT_SYSSETTING_OFF                 "Off"
#define TEXT_SYSSETTING_ONONEMIN            "Backlight on 1 minute"
#define TEXT_SYSSETTING_SECURITYSETTING     "Security Setting"
#define TEXT_SYSSETTING_PIN                 "Pin"
#define TEXT_SYSSETTING_CHANGEPASSWORD      "Change Password"
#define TEXT_SYSSETTING_RESTORESETTING      "Restore Factory Setting"
#define TEXT_SYSSETTING_PINON               "Pin On"
#define TEXT_SYSSETTING_PINOFF              "Pin Off"
#define TEXT_SYSSETTING_PINCHANGE           "Pin Change"
#define TEXT_SYSSETTING_PHONEINFO           "Phone Information"
#define TEXT_SYSSETTING_PHONENO             "Own Number"
#define TEXT_SYSSETTING_SOFTVERSION         "Software Version"
#define TEXT_SYSTETTING_IMEI                "IMEI"

#define TEXT_SYSSETTING_SELECTEXIT          "Select          Exit"
#define TEXT_SYSSETTING_PARAMSABNORMAL      "Parameter Abnormal!"
#define TEXT_SYSSETTING_CONFIRM             "Confirm"
#define TEXT_SYSSETTING_RETURN              "Return"
#define TEXT_SYSSETTING_SETUPSUCCESS        "Setup Success!"
#define TEXT_SYSSETTING_SETUPFAIL           "Setup Fail!"
#define TEXT_SYSSETTING_OUTRANGE            "Out of range"
#define TEXT_SYSSETTING_INPUTPASSWORD       "Security Password:"
#define TEXT_SYSSETTING_INPUTOLDPASSWORD    "Old Password:"
#define TEXT_SYSSETTING_INPUTNEWPASSWORD    "New Password:"
#define TEXT_SYSSETTING_INPUTCFMPASSWORD    "Confirm Password��"
#define TEXT_SYSSETTING_OLDPWDERROR         "Old password error!"
#define TEXT_SYSSETTING_PWDNOTEQUAL         "Password not equal"
#define TEXT_SYSSETTING_PWDERROR            "Password Error!"
#define TEXT_SYSSETTING_PINALREADYON        "Pin Alreay On!"
#define TEXT_SYSSETTING_PINNUMBER           "Pin Number:"
#define TEXT_SYSSETTING_OLDPINNUMMER        "Old Pin Number:"
#define TEXT_SYSSETTING_INPUTNEWPIN         "Input new pin:"
#define TEXT_SYSSETTING_REINPUTNEWPIN       "Reinput new pin:"
#define TEXT_SYSSETTING_PINONSUCCESS        "Pin On success!"
#define TEXT_SYSSETTING_PINERROR            "Pin No. Error!"
#define TEXT_SYSSETTING_PINONFAIL           "Pin On Fail!"
#define TEXT_SYSSETTING_PINALREADYOFF       "Pin Already Off!"
#define TEXT_SYSSETTING_PINOFFSUCCESS       "Pin Off success!"
#define TEXT_SYSSETTING_PINOFFFAIL          "Pin Off Fail!"
#define TEXT_SYSSETTING_INPUERROR           "Input Error\rPlease Reinput "
#define TEXT_SYSSETTING_PINCHANGESUCCESS    "Pin Change Success!"
#define TEXT_SYSSETTING_PINCHANGEFAIL       "Pin Change Fail!"
#define TEXT_SYSSETTING_SETDATETIME         "Set Date Time"
#define TEXT_SYSSETTING_YEARERROR           "year format error"
#define TEXT_SYSSETTING_MONTHERROR          "month format error"
#define TEXT_SYSSETTING_DATEERROR           "day format error"
#define TEXT_SYSSETTING_HOURERROR           "hour format error"
#define TEXT_SYSSETTING_MINUTEERROR         "min format error"
#define TEXT_SYSSETTING_SECONDERROR         "second format error"
#define TEXT_SYSSETTING_INVALID_DATA        "invalid data"
#define TEXT_SYSSETTING_IMEINUMBER          "IMEI Number:"
#define TEXT_SYSSETTING_SOFTWAREVERSION     "Software Version:"
#define TEXT_SYSSETTING_FACTORYSETTINGNOTE  "Restore default setting??"
#define TEXT_SYSSETTING_YES                 "Yes"
#define TEXT_SYSSETTING_NO                  "No"
#define TEXT_SYSSETTING_PLEASEWAIT          "please wait..."
#define TEXT_SYSSETTING_RESTORESUCCESS      "Restore Factory Success!"
#define TEXT_SYSSETTING_RESTOREFAIL         "Restore Factory Fail!"
#define TEXT_SYSSETTING_SETOWNNUMBER        "Set own phone No."
#define TEXT_SYSSETTING_KEYPADLOCK          "Keypad Lock"
#define TEXT_SYSSETTING_ONOFF               "On/Off"
#define TEXT_SYSSETTING_TIMEOUT             "Timeout"
#define TEXT_SYSSETTING_ONEMIN              "1 minute"
#define TEXT_SYSSETTING_FIVEMIN             "5 minutes"
#define TEXT_SYSSETTING_TENMIN             "10 minutes"
#endif

/***********************************************************************
* Key Define
***********************************************************************/
#ifdef TPS300
#define KEY_SYSSETTING_CONFIRM1     TP_KEY_MENU
#else
#define KEY_SYSSETTING_CONFIRM1     TP_KEY_CONFIRM
#endif
#define KEY_SYSSETTING_CONFIRM2     TP_KEY_OK
#define KEY_SYSSETTING_CANCEL1      TP_KEY_CANCEL
#define KEY_SYSSETTING_CANCEL2      TP_KEY_NONE
#ifdef TPS300
#define KEY_SYSSETTING_UP           TP_KEY_UP           //ʱ�����������л�
#define KEY_SYSSETTING_DOWN         TP_KEY_DOWN        //ʱ�����������л�
#define KEY_SYSSETTING_IMCHANGEKEY  TP_KEY_FUNC
#else
#define KEY_SYSSETTING_UP           TP_KEY_LEFT         //ʱ�����������л�
#define KEY_SYSSETTING_DOWN         TP_KEY_RIGHT        //ʱ�����������л�
#define KEY_SYSSETTING_IMCHANGEKEY  TP_KEY_POUND
#endif

extern
Boolean TP_GetParamItem(PARAMS_NAMES paramsId,ValueType valueType,void* value);

extern
Boolean TP_SetParamItem(PARAMS_NAMES paramsId,ValueType valueType,void* value);

extern
void TP_PhoneSetting(void);


extern
void TP_PhoneSettingInitForPowerOn(void);

extern
uint32 TP_GetKeypadTimeout(void);

extern
Boolean TP_UnlockKeypad(void);













#endif
