#ifndef TP_SECRET_CODE_H
#define TP_SECRET_CODE_H


#define CMD_SECRETCODE_SIGNAL           "*#301#"
#define CMD_SECRETCODE_IMSISHOW         "*#302#"
#define CMD_SECRETCODE_RATEVER          "*#303#"
#define CMD_SECRETCODE_SCRIPTVER        "*#304#"
#define CMD_SECRETCODE_AUDIOFILE        "*#305#"
#define CMD_SECRETCODE_CELL             "*#306#"
#define CMD_SECRETCODE_TIMESHOW         "*#307#"

#define CMD_SECRETCODE_IMEISHOW         "*#06#"

#define CMD_SECRETCODE_SYSTEST          "*#55#"
#define CMD_SECRETCODE_KEYPTEST         "*#56#"
#define CMD_SECRETCODE_BURNIN           "*66*"
#define CMD_SECRETCODE_SDKVERSION       "*#7898#"

/*************************************************************************
 *String to display define here
 ************************************************************************/
#define ENGLISH

#ifdef CHINESE

#define TEXT_SECRETCODE_ERROR           "出错！"
#define TEXT_SECRETCODE_NOTSUPPORT      "暂不支持！"

#else

#define TEXT_SECRETCODE_ERROR           "Error!"
#define TEXT_SECRETCODE_NOTSUPPORT      "Not support yet!"

#endif

extern
Boolean TP_SecretCodeParse(char* str);








#endif
