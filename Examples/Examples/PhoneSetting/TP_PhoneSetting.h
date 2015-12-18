#ifndef TP_PHONE_SETTING_H
#define TP_PHONE_SETTING_H
#include "TP_ParamsManager.h"



#ifdef PHONESETTING_CHINESE

/**********************************************************************
 *String define
 **********************************************************************/
#define TEXT_SYSSETTING                     "话机设置"

#define TEXT_SYSSETTING_VOICESETTING        "声音设置"
#define TEXT_SYSSETTING_RING                "铃声"
#define TEXT_SYSSETTING_VOLUMN              "音量"
#define TEXT_SYSSETTING_ANNOUNCE            "提示音"
#define TEXT_SYSSETTING_KEYPADVOLUMN        "按键音量"
#define TEXT_SYSSETTING_RINGVOLUMN          "铃声音量"
#define TEXT_SYSSETTING_MICROPHONEVOLUMN    "手柄音量"
#define TEXT_SYSSETTING_SPEECHVOLUMN        "免提音量"
#define TEXT_SYSSETTING_DISPLAYSETTING      "显示设置"
#define TEXT_SYSSETTING_BACKLIGHT           "背光灯"
#define TEXT_SYSSETTING_CONTRAST            "对比度"
#define TEXT_SYSSETTING_TIME                "时间"
#define TEXT_SYSSETTING_ON                  "开"
#define TEXT_SYSSETTING_OFF                 "关"
#define TEXT_SYSSETTING_ONONEMIN            "开一分钟"
#define TEXT_SYSSETTING_SECURITYSETTING     "安全设置"
#define TEXT_SYSSETTING_PIN                 "Pin码"
#define TEXT_SYSSETTING_CHANGEPASSWORD      "修改密码"
#define TEXT_SYSSETTING_RESTORESETTING      "恢复出厂设置"
#define TEXT_SYSSETTING_PINON               "Pin开启"
#define TEXT_SYSSETTING_PINOFF              "Pin关闭"
#define TEXT_SYSSETTING_PINCHANGE           "Pin修改"
#define TEXT_SYSSETTING_PHONEINFO           "话机信息"
#define TEXT_SYSSETTING_PHONENO             "本机号码"
#define TEXT_SYSSETTING_SOFTVERSION         "软件版本"
#define TEXT_SYSSTETTING_IMEI               "IMEI"

#define TEXT_SYSSETTING_SELECTEXIT          "选择            退出"
#define TEXT_SYSSETTING_PARAMSABNORMAL      "参数异常！"
#define TEXT_SYSSETTING_CONFIRM             "确定"
#define TEXT_SYSSETTING_RETURN              "返回"
#define TEXT_SYSSETTING_SETUPSUCCESS        "设置成功！"
#define TEXT_SYSSETTING_SETUPFAIL           "设置失败！"
#define TEXT_SYSSETTING_OUTRANGE            "超出范围"
#define TEXT_SYSSETTING_INPUTPASSWORD       "输入安全密码:"
#define TEXT_SYSSETTING_INPUTOLDPASSWORD    "输入旧密码："
#define TEXT_SYSSETTING_INPUTNEWPASSWORD    "输入新密码："
#define TEXT_SYSSETTING_INPUTCFMPASSWORD    "确认新密码："
#define TEXT_SYSSETTING_OLDPWDERROR         "旧密码错误!"
#define TEXT_SYSSETTING_PWDNOTEQUAL         "密码输入不相等！"
#define TEXT_SYSSETTING_PWDERROR            "密码错误"
#define TEXT_SYSSETTING_PINALREADYON        "Pin码已开启"
#define TEXT_SYSSETTING_PINNUMBER           "Pin码："
#define TEXT_SYSSETTING_OLDPINNUMMER        "旧Pin码:"
#define TEXT_SYSSETTING_INPUTNEWPIN         "输入新Pin码:"
#define TEXT_SYSSETTING_REINPUTNEWPIN       "确认新Pin码:"
#define TEXT_SYSSETTING_PINONSUCCESS        "Pin码开启成功!"
#define TEXT_SYSSETTING_PINERROR            "Pin码错误！"
#define TEXT_SYSSETTING_PINONFAIL           "Pin码开启失败！"
#define TEXT_SYSSETTING_PINALREADYOFF       "Pin码未开启!"
#define TEXT_SYSSETTING_PINOFFSUCCESS       "Pin码关闭成功!"
#define TEXT_SYSSETTING_PINOFFFAIL          "Pin码关闭失败！"
#define TEXT_SYSSETTING_INPUERROR           "输入有误 \r请重新输入 "
#define TEXT_SYSSETTING_PINCHANGESUCCESS    "Pin码修改成功!"
#define TEXT_SYSSETTING_PINCHANGEFAIL       "Pin码修改失败!"
#define TEXT_SYSSETTING_SETDATETIME         "设置时间日期"
#define TEXT_SYSSETTING_YEARERROR           "年份出错"
#define TEXT_SYSSETTING_MONTHERROR          "月份出错"
#define TEXT_SYSSETTING_DATEERROR           "日出错"
#define TEXT_SYSSETTING_HOURERROR           "小时出错"
#define TEXT_SYSSETTING_MINUTEERROR         "分钟出错"
#define TEXT_SYSSETTING_SECONDERROR         "秒出错"
#define TEXT_SYSSETTING_INVALID_DATA        "无效数据"
#define TEXT_SYSSETTING_IMEINUMBER          "IMEI码:"
#define TEXT_SYSSETTING_SOFTWAREVERSION     "软件版本号:"
#define TEXT_SYSSETTING_FACTORYSETTINGNOTE  "恢复默认参数设置??"
#define TEXT_SYSSETTING_YES                 "是"
#define TEXT_SYSSETTING_NO                  "否"
#define TEXT_SYSSETTING_PLEASEWAIT          "请稍候..."
#define TEXT_SYSSETTING_RESTORESUCCESS      "恢复出厂设置成功!"
#define TEXT_SYSSETTING_RESTOREFAIL         "恢复出厂设置失败!"
#define TEXT_SYSSETTING_SETOWNNUMBER        "设置本机号码:"
#define TEXT_SYSSETTING_KEYPADLOCK          "键盘锁定"
#define TEXT_SYSSETTING_ONOFF               "开/关"
#define TEXT_SYSSETTING_TIMEOUT             "超时时间"
#define TEXT_SYSSETTING_ONEMIN              "1分钟"
#define TEXT_SYSSETTING_FIVEMIN             "5分钟"
#define TEXT_SYSSETTING_TENMIN             "10分钟"
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
#define TEXT_SYSSETTING_INPUTCFMPASSWORD    "Confirm Password："
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
#define KEY_SYSSETTING_UP           TP_KEY_UP           //时间设置左右切换
#define KEY_SYSSETTING_DOWN         TP_KEY_DOWN        //时间设置左右切换
#define KEY_SYSSETTING_IMCHANGEKEY  TP_KEY_FUNC
#else
#define KEY_SYSSETTING_UP           TP_KEY_LEFT         //时间设置左右切换
#define KEY_SYSSETTING_DOWN         TP_KEY_RIGHT        //时间设置左右切换
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
