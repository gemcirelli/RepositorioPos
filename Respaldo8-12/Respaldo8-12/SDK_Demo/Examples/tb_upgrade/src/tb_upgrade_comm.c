/******************************************************************************
* Include Headers
*******************************************************************************/
#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeypad.h>
#include <TPUpgrade.h>
#include <TPNetwork.h>
#include <TPPPP.h>
#include <TPSMS.h>
#include <TPSerial.h>

#include "TP_DemoDefine.h"
#include "tb_upgrade.h"

#include "./tb_upgrade_display.h"
#include "./tb_upgrade_http.h"
#include "./tb_upgrade_comm.h"
#include "Scolopendra/CT_GlobalData.h"
#include "Scolopendra/CT_CryptoManager.h"

extern COMM_DATA g_commData;
/******************************************************************************
* Manifest Constants
*******************************************************************************/
#define MAX_DOWNLOAD_SIZE (5 * 1024)

/******************************************************************************
* Private Variables
*******************************************************************************/
/** eý¼¶¿ªÊ¼»Ø¸´¶ÌÐÅÄuÈÝ*/
static char SmsFotaStart[] =
{
    0x30,0x30,0x30,0x34,
    0x31,0x30,
    0x31,
    0x31,0x0D,0x0A,
    0x38,0x34,0x0D,0x0A,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x0D,0x0A,
    0x32,0x30,0x0D,0x0A
};


/** eý¼¶Ê§°Ü»Ø¸´¶ÌÐÅÄuÈÝ*/
static char SmsFotaFailed[] =
{
    0x30,0x30,0x30,0x34,
    0x31,0x30,
    0x31,
    0x31,0x0D,0x0A,
    0x30,0x32,0x0D,0x0A,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x0D,0x0A,
    0x30, 0x0D, 0x0A,
    '3','0','0','8','1',',','1','C',0x0D,0x0A
};

/** eý¼¶¼´½«½aÊø»Ø¸´¶ÌÐÅÄuÈÝ*/
static char SmsFotaBeforeFinish[] =
{
    0x30,0x30,0x30,0x34,
    0x31,0x30,
    0x31,
    0x31,0x0D,0x0A,
    0x30,0x32,0x0D,0x0A,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30, 0x30,0x0D,0x0A,
    0x30, 0x0D, 0x0A,
    '3','0','0','8','0',',','1','C',0x0D,0x0A
};

/** eý¼¶½aÊø»Ø¸´¶ÌÐÅÄuÈÝ£¬ÐèÌî³ä°æ±¾ºÅ*/
static char SmsFotaFinish[] =
{
    0x30,0x30,0x30,0x34,
    0x31,0x30,
    0x31,
    0x31,0x0D,0x0A,
    0x38,0x37,0x0D,0x0A,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x0D,0x0A,
    '2','0',0x0D,0x0A,
    '3','0','0','8',',','0','0','0','0','0','0',0x0D,0x0A
};

/** eÏ±¨°æ±¾ºÅ»Ø¸´¶ÌÐÅÄuÈÝ£¬ÐèÌî³ä°æ±¾ºÅ*/
static char SmsFotaGetVersion[] =
{
    0x30,0x30,0x30,0x34,
    0x31,0x30,
    0x31,
    0x31,0x0D,0x0A,
    0x38,0x37,0x0D,0x0A,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x0D,0x0A,
    '2','0',0x0D,0x0A,
    '3','0','0','8',',','0','0','0','0','0','0',0x0D,0x0A
};

/** Ð­Òe´iÎo»Ø¸´¶ÌÐÅÄuÈÝ*/
static char SmsFotaProtocolError[] =
{
    /*a÷Ë®ºÅ*/
    0x30,0x30,0x30,0x34,
    /*ÏûÏ¢Ð­Òe°æ±¾*/
    0x31,0x30,
    /*×ÜÖ¡Êý*/
    0x31,
    /*µ±Ç°Ö¡ºÅ*/
    0x31,0x0D,0x0A,
    /*Ãüaî*/
    0x38,0x34,0x0D,0x0A,
    /*ÖÕ¶ËÐòaÐºÅ*/
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x0D,0x0A,
    /*Ïìo¦½a¹û*/
    0x30,0x31,0x0D,0x0A
};

/** °æ±¾´iÎo»Ø¸´¶ÌÐÅÄuÈÝ*/
static char SmsFotaVersionError[] =
{
    /*a÷Ë®ºÅ*/
    0x30,0x30,0x30,0x34,
    /*ÏûÏ¢Ð­Òe°æ±¾*/
    0x31,0x30,
    /*×ÜÖ¡Êý*/
    0x31,
    /*µ±Ç°Ö¡ºÅ*/
    0x31,0x0D,0x0A,
    /*Ãüaî*/
    0x38,0x34,0x0D,0x0A,
    /*ÖÕ¶ËÐòaÐºÅ*/
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x0D,0x0A,
    /*Ïìo¦½a¹û*/
    0x32,0x32,0x0D,0x0A
};

static TB_Upgrade_HttpContextT* PHttpContext = PNULL;
static TB_Upgrade_HttpResponseT HttpResponse;

/******************************************************************************
* Private Functions
*******************************************************************************/
static void destroyPPP(void)
{
    TP_WlPPPLogout();
}


static uint8 createPPP(TB_Upgrade_APNContextT* pAPNContext)
{
    int32 retVal;

    retVal = TP_WlPPPCheck();
    if (retVal == WL_RET_OK)
    {
        if (TP_NetworkGetStatus() != SRVCSTATUS_REGISTERED)
        {
            return 0x02;
        }
    }

    if (retVal == WL_RET_OK)
    {
        return 0x00;
    }
    else if ((retVal == WL_RET_ERR_LINEOFF) ||
             (retVal == WL_RET_ERR_PPP_BRK))
    {
		InitData();
		change_port();
        retVal = TP_WlPPPLogin((uint8*)g_commData.apn, (uint8*)"\x00", (uint8*)"\x00", 0, 0, TIMEOUT);

        if (retVal == WL_RET_OK)
        {
            return 0x00;
        }
    }
    else if (retVal == WL_RET_ERR_DIALING)
    {
        TP_TimerSet(0, 10 * 1000);
        do
        {
            if (TP_WlPPPCheck() == WL_RET_OK)
            {
                return 0x00;
            }
        } while (TP_TimerCheck(0) != 0);
    }
    else
    {
        return 0x02;
    }

    destroyPPP();

    return 0x03;
}


static Boolean checkUserCancel(void)
{
    uint8 result;

    if (TP_Kbhit() == 0xFF)
    {
        result = TP_GetKey();
        TP_Kbflush();
        if (result == UPGRADE_KEY_CANCEL)
        {
            TP_Kbflush();
            result = tb_upgrade_display_QueryDoCancelDownload();
            if (result == 0x00)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}


static uint8 validateAPPInfo(const TP_APPInfoT* pAPPInfo)
{
    /* Ð£neProductName */
    if (memchr(pAPPInfo->productName, 0, sizeof(pAPPInfo->productName)) == NULL)
    {
        return 0x07;
    }
    if (strlen(TP_Data_APPInfo.productName) != 0)
    {
        if (strcmp(pAPPInfo->productName, TP_Data_APPInfo.productName) != 0)
        {
            return 0x08;
        }
    }

    /* Ð£neWorkTerminal */
    if (memchr(pAPPInfo->productTerminal, 0, sizeof(pAPPInfo->productTerminal)) == NULL)
    {
        return 0x07;
    }
    if (strlen(TP_Data_APPInfo.productName) != 0)
    {
        if (strcmp(pAPPInfo->productTerminal, TP_Data_APPInfo.productTerminal) != 0)
        {
            return 0x09;
        }
    }

    /* Ð£neProductVersion */
    if (memchr(pAPPInfo->productVersion, 0, sizeof(pAPPInfo->productVersion)) == NULL)
    {
        return 0x07;
    }
    if (strlen(TP_Data_APPInfo.productVersion) != 0)
    {
        if (strcmp(pAPPInfo->productVersion, TP_Data_APPInfo.productVersion) <= 0)
        {
            return 0x0A;
        }
    }

    /* Ð£neProductLastVersion */
    /*if (memchr(pAPPInfo->upgradeFromVersion, 0, sizeof(pAPPInfo->upgradeFromVersion)) == NULL)
    {
        return 0x07;
    }
    if (strlen(pAPPInfo->upgradeFromVersion) != 0)
    {
        if (strcmp(pAPPInfo->upgradeFromVersion,
                   TP_Data_APPInfo.productVersion) != 0)
        {
            return 0x0B;
        }
    }

    /* Ð£neProductDesc */
    /*if (memchr(pAPPInfo->productdesc, 0 ,sizeof(pAPPInfo->productdesc)) == NULL)
    {
        return 0x07;
    }*/

    return 0x00;
}


/******************************************************************************
* Protected Functions
*******************************************************************************/
void tb_upgrade_comm_ReplySMS(TB_Upgrade_ReplySMSTypeE type,
                              const char* smeAddr, const char* version)
{
    char temp[100];
    uint8 retrys;
    TP_SMSSendResult result;

    if (strlen((char *)smeAddr) == 0)
    {
        return;
    }

    #define _30_SECONDS_ (30 * 1000)
    #define _10_SECONDS_ (10 * 1000)
    #define _1_SECONDS_  (1  * 1000)

    retrys = 0;
    while (retrys < 10)
    {
        switch(type)
        {
        case TB_UPGRADE_START:
            result = TP_NewSms((uint8*)smeAddr,
                               (uint8*)SmsFotaStart, sizeof(SmsFotaStart),
                               _30_SECONDS_);
            break;

        case TB_UPGRADE_FAILED:
            result = TP_NewSms((uint8*)smeAddr,
                               (uint8*)SmsFotaFailed, sizeof(SmsFotaFailed),
                               _30_SECONDS_);
            break;

        case TB_UPGRADE_BEFOREFINISH:
            result = TP_NewSms((uint8*)smeAddr,
                               (uint8*)SmsFotaBeforeFinish, sizeof(SmsFotaBeforeFinish),
                               _30_SECONDS_);
            break;

        case TB_UPGRADE_FINISH:
            if (version == PNULL)
            {
                return;
            }

            memset(temp, 0x00, sizeof(temp));
            memcpy(temp, SmsFotaFinish, sizeof(SmsFotaFinish));
            memcpy(&temp[41], version, 6);

            result = TP_NewSms((uint8*)smeAddr,
                               (uint8*)temp, strlen((char *)temp),
                               _30_SECONDS_);
            break;

        case TB_UPGRADE_ERROR_VERSION:
            result = TP_NewSms((uint8*)smeAddr,
                               (uint8*)SmsFotaVersionError, sizeof(SmsFotaVersionError),
                              _30_SECONDS_);
            break;

        case TB_UPGRADE_ERROR_PROTOCOL:
            result = TP_NewSms((uint8*)smeAddr,
                               (uint8*)SmsFotaProtocolError, sizeof(SmsFotaProtocolError),
                               _30_SECONDS_);
            break;

        case TB_UPGRADE_GETVERSION:
            if (version == PNULL)
            {
                return;
            }

            memset(temp, 0x00, sizeof(temp));
            memcpy(temp, SmsFotaGetVersion, sizeof(SmsFotaGetVersion));
            memcpy(&temp[41], version, 6);

            result = TP_NewSms((uint8*)smeAddr,
                               (uint8*)temp, strlen((char *)temp),
                               _30_SECONDS_);
            break;

        default:
            result = SMSSEND_SUCCESS;
            break;
        }

        TP_DbgSerialPrnEx(3, "SMS Send Result:%d\r\n", result);

        if (result == SMSSEND_SUCCESS)
        {
            break;
        }
        else
        {
            _TP_TimerSet(12, _1_SECONDS_);
             while (TP_TimerCheck(12) != 0)
             {
                 /*Just waitting*/
             }

             retrys++;
        }
    }

    #undef _30_SECONDS_
    #undef _10_SECONDS_
    #undef _1_SECONDS_
}


uint8 tb_upgrade_comm_DownloadData(const TB_Upgrade_UpgradeInfoT* pInfo,
                                   const TP_UpgradeBreakPointT*   pBreakPoint,
                                   const TB_Upgrade_APNContextT*  pAPNContext)
{
    uint32  result;
    uint32  readEndAddr, readStartAddr;
    uint32  contentLen;
    uint32  dataLen;
    uint8   connectCounts;

    if (pBreakPoint->nWrittenBytes != 0)
    {
        if (pBreakPoint->nWrittenBytes == pBreakPoint->nFileBytes)
        {
            return 0x00;
        }
    }

    TP_Kbflush();
    connectCounts = 0;
    while (connectCounts < 10)
    {
        tb_upgrade_display_ShowProgress(TB_UPGRADE_STEP_CONNECT,
                                        connectCounts + 1, 10);

        if (createPPP(pAPNContext) == 0x00)
        {
            if (PHttpContext == PNULL)
            {
				result = tb_upgrade_http_Create(pInfo->fileURL, &PHttpContext);
                TP_DbgSerialPrn("\r\nCreate Http:[%d]\r\n", result);
            }
            else
            {
                result = 0x00;
            }
            TP_DbgSerialPrn("\r\nPHttpContext:[%p]\r\n", PHttpContext);

            if (result == 0x00)
            {
				//result = tb_upgrade_http_Create(pInfo->fileURL, &PHttpContext);
				result = tb_upgrade_http_Connect(PHttpContext);
                TP_DbgSerialPrn("\r\nConnect Http:[%d]\r\n", result);
            }
        }
        else
        {
            result = 0x01;
        }

        if (result != 0x00)
        {
            connectCounts++;

            TP_TimerSet(0, 5 * 1000);
            while (TP_TimerCheck(0) != 0)
            {
                if (checkUserCancel() == TRUE)
                {
                    tb_upgrade_display_PromptQuitDownload();

                    tb_upgrade_http_Destroy(&PHttpContext);

                    destroyPPP();

                    return 0x06;
                }
            }
        }
        else
        {
            break;
        }
    }

    if (connectCounts >= 10)
    {
        tb_upgrade_display_PromptQuitDownload();

        tb_upgrade_http_Destroy(&PHttpContext);

        destroyPPP();

        return 0x02;
    }

    if (checkUserCancel() == TRUE)
    {
        tb_upgrade_display_PromptQuitDownload();

        tb_upgrade_http_Destroy(&PHttpContext);

        destroyPPP();

        return 0x06;
    }

    connectCounts = 0;
    if (pBreakPoint->nWrittenBytes != 0)
    {
        readStartAddr = pBreakPoint->nWrittenBytes;
        readEndAddr   = readStartAddr + MAX_DOWNLOAD_SIZE - 1;
        if (readEndAddr > (pBreakPoint->nFileBytes - 1))
        {
            readEndAddr = pBreakPoint->nFileBytes - 1;
        }
        contentLen = pBreakPoint->nFileBytes;
    }
    else
    {
        readStartAddr = 0;
        readEndAddr   = MAX_DOWNLOAD_SIZE - 1;
        contentLen    = 0;
    }
    while (TRUE)
    {
        if (connectCounts >= 10)
        {
            tb_upgrade_display_PromptQuitDownload();

            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x04;
        }

        tb_upgrade_display_ShowProgress(TB_UPGRADE_STEP_DOWNLOAD,
			readStartAddr, readEndAddr);

        result = tb_upgrade_http_WriteGET(PHttpContext, readStartAddr, readEndAddr);
        TP_DbgSerialPrn("\r\nWrite Http:[%d]\r\n", result);
        if (result != 0)
        {
            while (connectCounts < 10)
            {
                tb_upgrade_http_DisConnect(PHttpContext);

                destroyPPP();

                TP_TimerSet(1, 1000);
                while (TP_TimerCheck(1) != 0)
                {
                    if (checkUserCancel() == TRUE)
                    {
                        tb_upgrade_display_PromptQuitDownload();

                        tb_upgrade_http_Destroy(&PHttpContext);

                        destroyPPP();

                        return 0x06;
                    }
                }

                if (createPPP(pAPNContext) == 0x00)
                {
                    result = tb_upgrade_http_Connect(PHttpContext);
                    TP_DbgSerialPrn("\r\nConnect Http:[%d]\r\n", result);

                    if (result == 0x00)
                    {
                        connectCounts = 0;

                        break;
                    }
                }

                connectCounts++;
            }

            continue;
        }
        if (checkUserCancel() == TRUE)
        {
            tb_upgrade_display_PromptQuitDownload();

            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x06;
        }

        result = tb_upgrade_http_ReadResponse(PHttpContext, &HttpResponse);
        TP_DbgSerialPrn("\r\nRead Http:[%d]\r\n", result);
        if (result == 0x04)
        {
            if (HttpResponse.statusCode == 404)
            {
                tb_upgrade_display_PromptQuitDownload();

                tb_upgrade_http_Destroy(&PHttpContext);

                destroyPPP();

                return 0x03;
            }
            else if (HttpResponse.statusCode == 416)
            {
                tb_upgrade_display_PromptQuitDownload();

                tb_upgrade_http_Destroy(&PHttpContext);

                destroyPPP();

                return 0x07;
            }
        }

        if (result != 0)
        {
            while (connectCounts < 10)
            {
                tb_upgrade_http_DisConnect(PHttpContext);

                destroyPPP();

                TP_TimerSet(1, 1000);
                while (TP_TimerCheck(1) != 0)
                {
                    if (checkUserCancel() == TRUE)
                    {
                        tb_upgrade_display_PromptQuitDownload();

                        tb_upgrade_http_Destroy(&PHttpContext);

                        destroyPPP();

                        return 0x06;
                    }
                }

                if (createPPP(pAPNContext) == 0x00)
                {
                    result = tb_upgrade_http_Connect(PHttpContext);
                    TP_DbgSerialPrn("\r\nConnect Http:[%d]\r\n", result);

                    if (result == 0x00)
                    {
                        connectCounts = 0;

                        break;
                    }
                }

                connectCounts++;
            }

            continue;
        }
        if (checkUserCancel() == TRUE)
        {
            tb_upgrade_display_PromptQuitDownload();

            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x06;
        }

        if (contentLen == 0)
        {
            contentLen = HttpResponse.fileSize;
        }
        else
        {
            if (contentLen != HttpResponse.fileSize)
            {
                tb_upgrade_display_PromptQuitDownload();

                tb_upgrade_http_Destroy(&PHttpContext);

                destroyPPP();

                return 0x07;
            }
        }

        connectCounts = 0;
        if(HttpResponse.contentRangeEnd != readEndAddr)
        {
            if ((HttpResponse.contentRangeEnd + 1) != contentLen)
            {//invalid packet,download again
                    continue;
            }
        }

        //write data of the packet to FLASH
        tb_upgrade_display_ShowProgress(TB_UPGRADE_STEP_WRITE,
                                        readEndAddr,
                                        contentLen);

        dataLen = HttpResponse.contentRangeEnd - HttpResponse.contentRangeStart + 1;
        result = TP_UpgradeWrite(HttpResponse.pBody,
                                 dataLen,
                                 contentLen);
        TP_DbgSerialPrn("\r\nWrite Flash:[%d]\r\n", result);
        if (result != 0x00)
        {
                tb_upgrade_display_PromptQuitDownload();

                tb_upgrade_http_Destroy(&PHttpContext);

                destroyPPP();

                return 0x05;
        }
        if (checkUserCancel() == TRUE)
        {
            tb_upgrade_display_PromptQuitDownload();

            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x06;
        }

        //¼ÆËãÏÂÒ»°üÇëÇoµÄÊý¾Ý·¶Î§
        if (contentLen != (HttpResponse.contentRangeEnd + 1))
        {
            readStartAddr = HttpResponse.contentRangeEnd + 1;
            readEndAddr   = readStartAddr + MAX_DOWNLOAD_SIZE - 1;
            if ((readEndAddr + 1) > contentLen)
            {
                readEndAddr = contentLen - 1;
            }
        }
        else
        {
            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x00;
        }
    }
}


uint8 tb_upgrade_comm_CheckData(const TB_Upgrade_UpgradeInfoT* pInfo,
                                const TB_Upgrade_APNContextT*  pAPNContext)
{
    const TP_APPInfoT* pAPPInfo;
    uint32  result;
    uint32  readEndAddr, readStartAddr;
    uint32  contentLen;
    uint32  dataLen;
    uint8   connectCounts;

    connectCounts = 0;
    while (connectCounts < 10)
    {
        if (createPPP(pAPNContext) == 0x00)
        {
            if (PHttpContext == PNULL)
            {
				result = tb_upgrade_http_Create(pInfo->fileURL, &PHttpContext);
				TP_DbgSerialPrn("\r\nCreate Http:[%d]\r\n", result);
            }
            else
            {
                result = 0x00;
            }
            TP_DbgSerialPrn("\r\nPHttpContext:[%p]\r\n", PHttpContext);

            if (result == 0x00)
            {
				result = tb_upgrade_http_Connect(PHttpContext);
                TP_DbgSerialPrn("\r\nConnect Http:[%d]\r\n", result);
            }
        }
        else
        {
            result = 0x01;
        }

        if (result != 0x00)
        {
            connectCounts++;

            TP_TimerSet(0, 5 * 1000);
            while (TP_TimerCheck(0) != 0)
            {
                if (checkUserCancel() == TRUE)
                {
                    tb_upgrade_display_PromptQuitDownload();

                    tb_upgrade_http_Destroy(&PHttpContext);

                    destroyPPP();

                    return 0x06;
                }
            }
        }
        else
        {
            break;
        }
    }
    if (connectCounts >= 10)
    {
        tb_upgrade_http_Destroy(&PHttpContext);

        destroyPPP();

        return 0x02;
    }
    if (checkUserCancel() == TRUE)
    {
        tb_upgrade_display_PromptQuitDownload();

        tb_upgrade_http_Destroy(&PHttpContext);

        destroyPPP();

        return 0x06;
    }

    connectCounts = 0;
    //readStartAddr = 0;
	//readEndAddr = 0x71C48;
	readStartAddr = TP_APPINFO_FILE_POS;
    readEndAddr   = TP_APPINFO_FILE_POS + TP_APPINFO_SIZE - 1;
    while (TRUE)
    {
        if (connectCounts >= 10)
        {
            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x04;
        }

        result = tb_upgrade_http_WriteGET(PHttpContext, readStartAddr, readEndAddr);
        TP_DbgSerialPrn("\r\nWrite Http:[%d]\r\n", result);
        if (result != 0)
        {
            while (connectCounts < 10)
            {
                tb_upgrade_http_DisConnect(PHttpContext);

                destroyPPP();

                TP_TimerSet(1, 1000);
                while (TP_TimerCheck(1) != 0)
                {
                    if (checkUserCancel() == TRUE)
                    {
                        tb_upgrade_display_PromptQuitDownload();

                        tb_upgrade_http_Destroy(&PHttpContext);

                        destroyPPP();

                        return 0x06;
                    }
                }

                if (createPPP(pAPNContext) == 0x00)
                {
                    result = tb_upgrade_http_Connect(PHttpContext);

                    if (result == 0x00)
                    {
                        connectCounts = 0;

                        break;
                    }
                }

                connectCounts++;
            }

            continue;
        }
        if (checkUserCancel() == TRUE)
        {
            tb_upgrade_display_PromptQuitDownload();

            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x06;
        }

        result = tb_upgrade_http_ReadResponse(PHttpContext, &HttpResponse);
        TP_DbgSerialPrn("\r\nRead Http:[%d]\r\n", result);
        if (result == 0x04)
        {
            if (HttpResponse.statusCode == 404)
            {
                tb_upgrade_display_PromptQuitDownload();

                tb_upgrade_http_Destroy(&PHttpContext);

                destroyPPP();

                return 0x03;
            }
            else if (HttpResponse.statusCode == 416)
            {
                tb_upgrade_display_PromptQuitDownload();

                tb_upgrade_http_Destroy(&PHttpContext);

                destroyPPP();

                return 0x07;
            }
        }

        if (result != 0)
        {
            while (connectCounts < 10)
            {
                tb_upgrade_http_DisConnect(PHttpContext);

                destroyPPP();

                TP_TimerSet(1, 1000);
                while (TP_TimerCheck(1) != 0)
                {
                    if (checkUserCancel() == TRUE)
                    {
                        tb_upgrade_display_PromptQuitDownload();

                        tb_upgrade_http_Destroy(&PHttpContext);

                        destroyPPP();

                        return 0x06;
                    }
                }

                if (createPPP(pAPNContext) == 0x00)
                {
                    result = tb_upgrade_http_Connect(PHttpContext);

                    if (result == 0x00)
                    {
                        connectCounts = 0;

                        break;
                    }
                }

                connectCounts++;
            }

            continue;
        }
        if (checkUserCancel() == TRUE)
        {
            tb_upgrade_display_PromptQuitDownload();

            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x06;
        }

        connectCounts = 0;

        if (HttpResponse.contentRangeEnd != readEndAddr)
        {
            if ((HttpResponse.contentRangeEnd + 1) != HttpResponse.fileSize)
            {//invalid packet,download again
                continue;
            }
        }

        dataLen = HttpResponse.contentRangeEnd - HttpResponse.contentRangeStart + 1;
        if (dataLen != TP_APPINFO_SIZE)
        {
            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x07;
        }
        else
        {
            break;
        }
    }

    pAPPInfo = (struct _APPInfo *)HttpResponse.pBody;
    TP_DbgSerialPrn("\r\nUpgrade APPInfo:[");
    TP_DbgSerialPrnHex((uint8*)pAPPInfo, 512);
    TP_DbgSerialPrn("]\r\n");
    TP_DbgSerialPrn("\r\nTerminal APPInfo:[");
    TP_DbgSerialPrnHex((uint8*)&TP_Data_APPInfo, 512);
    TP_DbgSerialPrn("]\r\n");

    result = validateAPPInfo(pAPPInfo);

    TP_DbgSerialPrn("\r\nAPPInfo Validate Result:[%d]\r\n", result);

    tb_upgrade_http_Destroy(&PHttpContext);

    destroyPPP();


    return result;
}


uint8 tb_upgrade_comm_CheckBP(const TB_Upgrade_UpgradeInfoT* pInfo,
                              const TP_UpgradeBreakPointT*   pBreakPoint,
                              const TB_Upgrade_APNContextT*  pAPNContext)
{
    uint32  result;
    uint32  readEndAddr, readStartAddr;
    uint32  contentLen;
    uint32  dataLen;
    uint8   connectCounts;

    if (pBreakPoint->nWrittenBytes == 0)
    {
        return 0x01;
    }

    connectCounts = 0;
    while (connectCounts < 10)
    {
        if (createPPP(pAPNContext) == 0x00)
        {
            if (PHttpContext == PNULL)
            {
                result = tb_upgrade_http_Create(pInfo->fileURL, &PHttpContext);
                TP_DbgSerialPrn("\r\nCreate Http:[%d]\r\n", result);
            }
            else
            {
                result = 0x00;
            }
            TP_DbgSerialPrn("\r\nPHttpContext:[%p]\r\n", PHttpContext);

            if (result == 0x00)
            {
                result = tb_upgrade_http_Connect(PHttpContext);
                TP_DbgSerialPrn("\r\nConnect Http:[%d]\r\n", result);
            }
        }
        else
        {
            result = 0x01;
        }

        if (result != 0x00)
        {
            connectCounts++;

            TP_TimerSet(0, 5 * 1000);
            while (TP_TimerCheck(0) != 0)
            {
                if (checkUserCancel() == TRUE)
                {
                    tb_upgrade_display_PromptQuitDownload();

                    tb_upgrade_http_Destroy(&PHttpContext);

                    destroyPPP();

                    return 0x06;
                }
            }
        }
        else
        {
            break;
        }
    }
    if (connectCounts >= 10)
    {
        tb_upgrade_http_Destroy(&PHttpContext);

        destroyPPP();

        return 0x02;
    }
    if (checkUserCancel() == TRUE)
    {
        tb_upgrade_display_PromptQuitDownload();

        tb_upgrade_http_Destroy(&PHttpContext);

        destroyPPP();

        return 0x06;
    }

    connectCounts = 0;
    readStartAddr = 0;
    readEndAddr   = 31;
    if (readEndAddr > (pBreakPoint->nWrittenBytes - 1))
    {
        readEndAddr = pBreakPoint->nWrittenBytes - 1;
    }
    contentLen = pBreakPoint->nFileBytes;
    while (TRUE)
    {
        if (connectCounts >= 10)
        {
            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x02;
        }

        result = tb_upgrade_http_WriteGET(PHttpContext, readStartAddr, readEndAddr);
        TP_DbgSerialPrn("\r\nWrite Http:[%d]\r\n", result);
        if (result != 0)
        {
            while (connectCounts < 10)
            {
                tb_upgrade_http_DisConnect(PHttpContext);

                destroyPPP();

                TP_TimerSet(1, 1000);
                while (TP_TimerCheck(1) != 0)
                {
                    if (checkUserCancel() == TRUE)
                    {
                        tb_upgrade_display_PromptQuitDownload();

                        tb_upgrade_http_Destroy(&PHttpContext);

                        destroyPPP();

                        return 0x06;
                    }
                }

                if (createPPP(pAPNContext) == 0x00)
                {
                    result = tb_upgrade_http_Connect(PHttpContext);

                    if (result == 0x00)
                    {
                        connectCounts = 0;

                        break;
                    }
                }

                connectCounts++;
            }

            continue;
        }
        if (checkUserCancel() == TRUE)
        {
            tb_upgrade_display_PromptQuitDownload();

            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x06;
        }

        result = tb_upgrade_http_ReadResponse(PHttpContext, &HttpResponse);
        TP_DbgSerialPrn("\r\nRead Http:[%d]\r\n", result);
        if (result == 0x04)
        {
            if (HttpResponse.statusCode == 404)
            {
                tb_upgrade_display_PromptQuitDownload();

                tb_upgrade_http_Destroy(&PHttpContext);

                destroyPPP();

                return 0x03;
            }
            else if (HttpResponse.statusCode == 416)
            {
                tb_upgrade_display_PromptQuitDownload();

                tb_upgrade_http_Destroy(&PHttpContext);

                destroyPPP();

                return 0x01;
            }
        }

        if (result != 0)
        {
            while (connectCounts < 10)
            {
                tb_upgrade_http_DisConnect(PHttpContext);

                destroyPPP();

                TP_TimerSet(1, 1000);
                while (TP_TimerCheck(1) != 0);
                {
                    if (checkUserCancel() == TRUE)
                    {
                        tb_upgrade_display_PromptQuitDownload();

                        tb_upgrade_http_Destroy(&PHttpContext);

                        destroyPPP();

                        return 0x06;
                    }
                }

                if (createPPP(pAPNContext) == 0x00)
                {
                    result = tb_upgrade_http_Connect(PHttpContext);

                    if (result == 0x00)
                    {
                        connectCounts = 0;

                        break;
                    }
                }

                connectCounts++;
            }

            continue;
        }
        if (checkUserCancel() == TRUE)
        {
            tb_upgrade_display_PromptQuitDownload();

            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x06;
        }

        if (contentLen != HttpResponse.fileSize)
        {
            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x01;
        }

        connectCounts = 0;

        if (HttpResponse.contentRangeEnd != readEndAddr)
        {
            if ((HttpResponse.contentRangeEnd + 1) != contentLen)
            {//invalid packet,download again
                    continue;
            }
        }

        dataLen = HttpResponse.contentRangeEnd - HttpResponse.contentRangeStart + 1;
        TP_DbgSerialPrnHex(HttpResponse.pBody, 32);
        TP_DbgSerialPrnHex(pBreakPoint->headBytes, 32);
        if (memcmp(HttpResponse.pBody, pBreakPoint->headBytes, dataLen) != 0)
        {
            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x01;
        }
        else
        {
            break;
        }
    }


    connectCounts = 0;
    readEndAddr   = pBreakPoint->nWrittenBytes - 1;
    if (readEndAddr > 31)
    {
        readStartAddr = readEndAddr - 31;
    }
    else
    {
        readStartAddr = 0;
    }
    contentLen = pBreakPoint->nFileBytes;
    while (TRUE)
    {
        if (connectCounts >= 10)
        {
            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x02;
        }

        result = tb_upgrade_http_WriteGET(PHttpContext, readStartAddr, readEndAddr);
        TP_DbgSerialPrn("\r\nWrite Http:[%d]\r\n", result);
        if (result != 0)
        {
            while (connectCounts < 10)
            {
                tb_upgrade_http_DisConnect(PHttpContext);

                destroyPPP();

                TP_TimerSet(1, 1000);
                while (TP_TimerCheck(1) != 0)
                {
                    if (checkUserCancel() == TRUE)
                    {
                        tb_upgrade_display_PromptQuitDownload();

                        tb_upgrade_http_Destroy(&PHttpContext);

                        destroyPPP();

                        return 0x06;
                    }
                }

                if (createPPP(pAPNContext) == 0x00)
                {
                    result = tb_upgrade_http_Connect(PHttpContext);

                    if (result == 0x00)
                    {
                        connectCounts = 0;

                        break;
                    }
                }

                connectCounts++;
            }

            continue;
        }
        if (checkUserCancel() == TRUE)
        {
            tb_upgrade_display_PromptQuitDownload();

            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x06;
        }

        result = tb_upgrade_http_ReadResponse(PHttpContext, &HttpResponse);
        TP_DbgSerialPrn("\r\nRead Http:[%d]\r\n", result);
        if (result == 0x04)
        {
            if (HttpResponse.statusCode == 404)
            {
                tb_upgrade_http_Destroy(&PHttpContext);

                destroyPPP();

                return 0x01;
            }
            else if (HttpResponse.statusCode == 416)
            {
                tb_upgrade_http_Destroy(&PHttpContext);

                destroyPPP();

                return 0x01;
            }
        }

        if (result != 0)
        {
            while (connectCounts < 10)
            {
                tb_upgrade_http_DisConnect(PHttpContext);

                destroyPPP();

                TP_TimerSet(1, 1000);
                while (TP_TimerCheck(1) != 0)
                {
                    if (checkUserCancel() == TRUE)
                    {
                        tb_upgrade_display_PromptQuitDownload();

                        tb_upgrade_http_Destroy(&PHttpContext);

                        destroyPPP();

                        return 0x06;
                    }
                }

                if (createPPP(pAPNContext) == 0x00)
                {
                    result = tb_upgrade_http_Connect(PHttpContext);

                    if (result == 0x00)
                    {
                        connectCounts = 0;

                        break;
                    }
                }

                connectCounts++;
            }

            continue;
        }
        if (checkUserCancel() == TRUE)
        {
            tb_upgrade_display_PromptQuitDownload();

            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x06;
        }

        if (contentLen != HttpResponse.fileSize)
        {
            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x01;
        }

        connectCounts = 0;

        if (HttpResponse.contentRangeEnd != readEndAddr)
        {
            if ((HttpResponse.contentRangeEnd + 1) != contentLen)
            {//invalid packet,download again
                    continue;
            }
        }

        dataLen = HttpResponse.contentRangeEnd - HttpResponse.contentRangeStart + 1;
        TP_DbgSerialPrnHex(HttpResponse.pBody, 32);
        TP_DbgSerialPrnHex(pBreakPoint->tailBytes, 32);
        if (memcmp(HttpResponse.pBody, pBreakPoint->tailBytes, dataLen) != 0)
        {
            tb_upgrade_http_Destroy(&PHttpContext);

            destroyPPP();

            return 0x01;
        }
        else
        {
            break;
        }
    }

    tb_upgrade_http_Destroy(&PHttpContext);

    destroyPPP();

    return 0x00;
}
