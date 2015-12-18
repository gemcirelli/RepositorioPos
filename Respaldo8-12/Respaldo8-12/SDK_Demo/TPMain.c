//#include <TPTypedef.h>
//#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TPSerial.h>
#include <TPUpgrade.h>
#include <TPPrint.h>
#include <TPExtPic.h>
#include <TPSMS.h>
#include <TPMobileCard.h>
#include <TPNet.h>
#include <TPFile.h>

#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"
#ifdef INCLUDE_MESSAGE_BOX
#include "TP_Smsstore.h"
#endif
#ifdef INCLUDE_PHONE_SETTING
#include "TP_PhoneSetting.h"
#endif

#ifdef INCLUDE_PARAM_MANAGER
#include "TP_Paramsmanager.h"
#endif
#include "TP_UpgradeTest.h"
#include "TP_IdleScreen.h"
#include "TP_Phone.h"
#include "TP_SimCard.h"

#include "TP_SerialDataProc.h"
#include "TP_SimCard.h"
#include "TP_StartPic.h"
#include "TP_CallRecord.h"
#include "TPFonts.h"

#include "tb_upgrade.h"

#include "Examples\Scolopendra\CT_GlobalData.h"
#include "Examples\Scolopendra\CT_File.h"
#include "Examples\Scolopendra\CT_Input.h"

#include "tb_upgrade.h"

extern USER_DATA g_userData;
extern int32 total_txn_to_confirm;
extern int16 user_type;

/******************************************************************************
* Constants
*******************************************************************************/

const char SoftwareVersion[10] = {"V84121229\0"};


void TP_GetProductInfo(char* pInfoBuf)
{
    sprintf(pInfoBuf, "%s.%s.%s",
            TP_Data_APPInfo.productName,
            TP_Data_APPInfo.productTerminal,
            TP_Data_APPInfo.productVersion);
}

static void myRestoreSettings(void)
{
    if (TRUE == TP_FSFormat())
    {
        #include "./TP_FactorySettings_ReInit.h"
    }
}

/*
 *  Deal with the received data from the default serial port
 *
 *  @author Zhangl
 *
 *  @version 1.0
 *
 *  @date 2012-08-01
 */
void TP_OnDefaultSerialData(const uint8* data_p, uint16 dataLen)
{
    char imeiBuf[20];
    char sendBuf[60];
    int32 cmdLen;

    cmdLen = strlen("AT+SIMEI?");

    if (dataLen >= cmdLen)
    {
        if ((memcmp(data_p, "at+simei?", cmdLen) == 0) ||
            (memcmp(data_p, "AT+SIMEI?", cmdLen) == 0))
        {
            memset(imeiBuf, 0x00, sizeof(imeiBuf));
            if (TP_GetModalIMEI(imeiBuf, sizeof(imeiBuf)) > 0)
            {
                sprintf(sendBuf,
                        "at+simei?\r\r\n%s\r\n\r\nOK\r\n",
                        imeiBuf);

                TP_DefaultPortSends(sendBuf, strlen(sendBuf));
            }

            return;
        }
    }

    TP_DefaultSerialDataParse((uint8*)data_p,dataLen);
}


/**
 * To notify a incoming call
 *
 * @author Zuo Jun
 *
 * @version 1.0
 *
 * @date 2012-08-01
 */

void TP_OnIncomingCallNotify(void)
{
    //eèÖÃÒ»ÏÂ×Ö·û¼¯£¬·ÀÖ¹°¢À­²®ÊäÈë·¨Ê±µ¯³ö½çÃæÏÔÊ¾²»Õý³£
    TP_CharSetT tempCharSet = {0};
    TP_GetCurrentCharSet(&tempCharSet);
    TP_SetCurrentCharSet(CHARSET_LATIN0);

    TP_ShowIncomingDilog();

    TP_SetCurrentCharSet(tempCharSet.name);
}


/**
 * Dispaly current software version
 *
 * @author Zuo Jun
 *
 * @version 1.0
 *
 * @date 2012-08-01
 */

void TP_DisplaySofterVersion()
{
    char versionInfo[256];
	sprintf(TP_Data_APPInfo.productName,"ScoloTerminal");
    sprintf(TP_Data_APPInfo.productTerminal,"G50.0");
    sprintf(TP_Data_APPInfo.productVersion, "%s", TPDEMO_VERSION);
    sprintf(versionInfo, "Terminal GRE-5 \nVersion: %s", TPDEMO_VERSION);

    TP_PopupMessageTimed((uchar*)versionInfo, 3 * 1000);
}


#ifdef INCLUDE_PARAM_MANAGER
/**
 * º¯ Êý CrcErrorFunc£¨£© ¹¦ÄÜÃèÊö£ºCRCÐ£ne³ö´i´¦Àiº¯Êý
 * @return ÎÞ
 * @author
 * @date
 * @note
 */
void CrcErrorFunc(PARAMS_NAMES paramsId[], uint32 length)
{
    int32 i=0;
    uint8 buffer[256];
    if(paramsId == NULL || length<=0)
    {
        return;
    }

    TP_SetDisplayArea(0,0,127,63);
    TP_ScrClsDisplayArea();

    TP_ScrCls();
    TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
    TP_ScrGotoxyEx(0, 24);
    sprintf((char *)buffer, "CrcError Accout %d\r\n", length);    TP_LcdPrintf(buffer);

    TP_TimerSet(1, 2000);
    while (TP_TimerCheck(1));

    for(i; i<length; i++)
    {
        TP_ScrClsDisplayArea();
        TP_ScrGotoxyEx(0, 24);
        sprintf((char*)buffer, "µu%d  ¸ö²ÎÊýCRC´iÎoID : %s\r\n", i+1, paramsname(paramsId[i]));        TP_LcdPrintf(buffer);
              TP_DbgSerialPrnHex(buffer, strlen((char *)buffer));
        TP_TimerSet(1, 1000);
        while (TP_TimerCheck(1));
    }
}

//²ÎÊýÊý¾Ý±»Òì³£¸ÄÐ´eÏ±¨i¨µÀ
void OMOFErrorFunc(int32 address)
{
    TP_ScrRestore(0);

    TP_ScrCls();
    TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
    TP_ScrGotoxyEx(0, 24);

    TP_LcdPrintf((uint8*)"²ÎÊýÊý¾Ý±»Òì³£¸ÄÐ´address : 0X%08X", address);

    TP_TimerSet(1, 5000);
    while (TP_TimerCheck(1));

    TP_SetDisplayArea(0, 0, 127, 63);
    TP_ScrRestore(1);

}
#endif

 
static void myAPPMain(void)
{
    uint8 result;

	char information[INF_LENGHT];
	uint16 cTemp;

#ifdef INCLUDE_PARAM_MANAGER
    ParamsManagerEntryTag enTag =
    {
        TRUE,
        &CrcErrorFunc,
        NULL,
        &OMOFErrorFunc
    };
#endif
	/*Initializing Global Data*/

	if( !TP_FExist("CFG") ){
		CT_InitCFGFile();
	}
	InitData();
	if( TP_FExist("LOG") ){
		/* Load Serials to Confirm */
		total_txn_to_confirm = 0;
		CT_ReadLogOwnerFile();
		CT_ReadLogFile();
	}
	
    TP_DbgSerialPrn("\r\nBuild Time:%s %s\r\n",
                    __DATE__,
                    __TIME__);

    TP_SetCurrentCharSet(CHARSET_LATIN0);

    TP_kbLight(1);
    TP_Kbmute(1);
    TP_ScrBackLight(1);

#ifdef INCLUDE_PARAM_MANAGER
    TP_ParamsManager_Init(&enTag);
#endif

#ifdef INCLUDE_PHONE_SETTING
    TP_PhoneSettingInitForPowerOn();
#else
    TP_SetDefaultParam();
#endif

    DisplayPicture();

    TP_PrnInit();

#ifdef INCLUDE_CALL_RECORD
    TP_CallRecordInit();
#endif

#ifdef INCLUDE_MESSAGE_BOX
    TP_SMSStoreInit();
#endif

    /*check SimCardStatue*/
    ShowSimCardStartUPProcess();

    //¼ì²âÔ¶³Ìeý¼¶
    result = TB_Upgrade_UpgradeCheck(&AppAPNContext);
    TP_UpgradeTest_ResultPrompt(result);

    TP_DisplaySofterVersion();

    while (TP_TimerCheck(1) != 0);

    ShowIdleScreen();
}


void TPMain(void)
{
    /******************************************************************************
    * !!! Important
    * Some settings are recommended to be set before TP_SystemInit
    *******************************************************************************/

    /*
     * Setting the debug messages outputing level, default is 4
     * 0 means to see all debug messages
     */
    TP_DbgSerialPrnLevel(0);
    /*
    * Setting the default serial port rate, default is 9600
    * 115200 means 115200,8,n,1
    */
    TP_SetDefaultPortRate("115200");
    /* Register functions to use in factory mode, both default value is NULL */
    TP_FactoryMode_Register_GetSoftwareVersion_Function(TP_GetProductInfo);
    /* Setting the codepage of your application, default is CHARSET_ASCII */
    TP_SetCurrentCharSet(CHARSET_LATIN0);
    /* Register picture to prompt when battery is low, default is NULL */
    TP_SetBatteryLowWarningPic(MyBatteryLowWarningPic);
    /* Register picture to prompt when soft power-off, default is NULL */
	//TP_SetPowerOffTipPic(MyPowerOffTipPic);
    TP_SetPowerOffTipPic(PowerOffGre5Logo);// GRE5 logo

    /******************************************************************************
    * End setting before TP_SystemInit
    *******************************************************************************/

    /* System intialise for application run-time environment
     * Above settings are important to TP_SystemInit
     */
    TP_SystemInit();

    TP_FactoryMode_Register_RestoreSettings_Function(myRestoreSettings);

    tp_base_memory_log_output();

    /* Here is where your applicatin actually starts */
    myAPPMain();
}

