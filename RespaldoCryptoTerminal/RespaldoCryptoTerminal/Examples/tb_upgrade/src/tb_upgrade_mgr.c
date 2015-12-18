/******************************************************************************
* Include Headers
*******************************************************************************/
#include <TPTypedef.h>
#include <TPBase.h>
#include <TPFonts.h>
#include <TPUpgrade.h>
#include <TPFile.h>
#include <TPKeypad.h>
#include <TPLCD.h>
#include <TPMobileCard.h>
#include <TPNetwork.h>
#include <TPPhone.h>

#include "tb_upgrade.h"
#include "./tb_upgrade_display.h"
#include "./tb_upgrade_comm.h"

/******************************************************************************
* Manifest Constants
*******************************************************************************/
#define TB_UPGRADE_VERSION "V2.0"

/******************************************************************************
* Typedefs
*******************************************************************************/
/**
 * ÎÄ±¾¹ýÂË´æ´¢Æ÷
 */
typedef struct __FilteredData
{
    /** Êý¾ÝÆðÊ¼µØÖ·*/
    char*  addr;
    /** Êý¾Ý³¤¶È*/
    uint16 len;
} ST_FilteredData;

/**
 * ÎÄ±¾¹ýÂËÆ÷
 */
typedef struct __TextFilter
{
    /** ¸nÊ½Æ¥Åä×Ö·û´®*/
    char*   token;
    /** ÊÇ·n±£´æÊý¾Ý*/
    Boolean needSave;
} ST_TextFilter;

/******************************************************************************
* Constatns
*******************************************************************************/
/**
* ÇëÇoeÏ±¨µ±Ç°ÖÕ¶ËÈi¼þ°æ±¾ÏûÏ¢¹ýÂËÆ÷<BR>
* [Ö¡i·]\x0D\x0A<BR>
* Ãüaî(\x30\x37)\x0D\x0A<BR>
* [ÖÕ¶ËÐòaÐºÅ]\x0D\x0A
*/
static const ST_TextFilter GetVersionFilter[] =
{
    /*Ö¡i·+Ãüaî*/
    {"\x0D\x0A\x30\x37\x0D\x0A", FALSE},
    /*ÖÕ¶ËÐòaÐºÅ*/
    {"\x0D\x0A",                 FALSE},
};

/**
* ÇëÇo½øÐÐÔ¶³Ìeý¼¶ÏûÏ¢¹ýÂËÆ÷<BR>
* Ö¡i·\x0D\x0A<BR>
* Ãüaî(\x30\x34)\x0D\x0A<BR>
* [ÖÕ¶ËÐòaÐºÅ]\x0D\x0A<BR>
* [ID],[°æ±¾],[HTTPµØÖ·]\x0D\x0A
*/
static const ST_TextFilter UpdateFileFilter[] =
{
    /*Ö¡i·+Ãüaî*/
    {"\x0D\x0A\x30\x34\x0D\x0A", FALSE},
    /*ÖÕ¶ËÐòaÐºÅ*/
    {"\x0D\x0A",                 FALSE},
    /*ID*/
    {",",                        FALSE},
    /*°æ±¾*/
    {",",                        TRUE },
    /*HTTPµØÖ·*/
    {"\x0D\x0A",                 TRUE }
};

extern const char SoftwareVersion[10];

const TB_Upgrade_APNContextT AppAPNContext =
{
    "cmnet",
    "",
    ""
};

/******************************************************************************
* Private Functions
*******************************************************************************/
static Boolean isUpgradeNotify(const uint8* pShortMsg, uint16 lenShortMsg,
                               const char* pSMEAddr, uint8 lenSMEAddr,
                               TB_Upgrade_UpgradeInfoT* pInfoBuf)
{
    uint16          iChar;
    uint8           step, maxSteps;
    uint8           saveIndex;
    uint8           len;
    uint8           lastaddr;
    char            ipStr[16];
    ST_FilteredData data[5];

    memset((uint8*)pInfoBuf, 0x00, sizeof(TB_Upgrade_UpgradeInfoT));

    /*½âÎö¶ÌÐÅ£¬»nÈ¡°æ±¾ºÅºiHTTPµØÖ·*/
    lastaddr   = 0;
    saveIndex  = 0;
    step       = 0;
    maxSteps   = sizeof(UpdateFileFilter) / sizeof(ST_TextFilter);
    for (iChar = 0; iChar < lenShortMsg; iChar++)
    {
        if (saveIndex >= 2)
        {
            break;
        }

        len = strlen((char *)UpdateFileFilter[step].token);
        if (memcmp(&pShortMsg[iChar], UpdateFileFilter[step].token, len) == 0 )
        {
            if (UpdateFileFilter[step].needSave == TRUE)
            {
                data[saveIndex].addr = (char*)&(pShortMsg[lastaddr]);
                data[saveIndex].len  = iChar - lastaddr;
                saveIndex++;
            }

            iChar   += len;
            lastaddr = iChar;
            step++;
            iChar--;
        }
    }

    if ((step != maxSteps) || (saveIndex != 2))
    {
        /*½âÎöÊ§°Ü*/
        return FALSE;
    }

    /*°æ±¾ºiHTTPµØÖ·Òn»nÈ¡*/
    /*eèÖÃeý¼¶ÐÅÏ¢*/
    /*°æ±¾ºÅ*/
    memcpy(pInfoBuf->destVersion, data[0].addr, data[0].len);

    /*¶ÌÐÅ»Ø¸´ºÅÂë*/
    memcpy(pInfoBuf->serverSMEAddr, pSMEAddr, lenSMEAddr);

    /*HTTPµØÖ·*/
    memcpy(pInfoBuf->fileURL, data[1].addr, data[1].len);

    /*eèÖÃeý¼¶ÀàÐi*/
    pInfoBuf->type = TB_UPGRADE_TYPE_UPGRADE;

    return TRUE;
}


static Boolean isGetVersionNotify(const uint8* pShortMsg, uint16 lenShortMsg,
                                  const char* pSMEAddr, uint8 lenSMEAddr,
                                  TB_Upgrade_UpgradeInfoT* pInfoBuf)
{
    uint16  iChar;
    uint8  step, maxSteps;
    uint8  formatcout;
    uint8  len;
    uint8  lastaddr;

    memset((uint8*)pInfoBuf, 0x00, sizeof(TB_Upgrade_UpgradeInfoT));

    /*½âÎö¶ÌÐÅ*/
    lastaddr   = 0;
    step       = 0;
    maxSteps   = sizeof(GetVersionFilter) / sizeof(ST_TextFilter);
    for (iChar = 0; iChar < lenShortMsg; iChar++)
    {
        if (step >= 2)
        {
            break;
        }

        len = strlen((char *)GetVersionFilter[step].token);
        if (memcmp(&pShortMsg[iChar],
                   GetVersionFilter[step].token, len) == 0 )
        {
            iChar   += len;
            lastaddr = iChar;
            step++;
            iChar--;
        }
    }

    if (step == maxSteps)
    {/*½âÎö³e¹¦*/

        /*eèÖÃeý¼¶ÐÅÏ¢*/
        /*eý¼¶ÀàÐi*/
        pInfoBuf->type = TB_UPGRADE_TYPE_GETVERSION;

        /*¶ÌÐÅ»Ø¸´ºÅÂë*/
        memcpy(pInfoBuf->serverSMEAddr,
               pSMEAddr, lenSMEAddr);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static Boolean checkFileURL(const char* pURLStr)
{
    uint16  iChar;
    uint16  hostStartPos = 0;
    uint16  pathStartPos = 0;
    uint16  httpAddrLen  = strlen(pURLStr);
    char    pathBuf[256];
    char    filenameStr[256];
    Boolean isRightFile;

    /* Check protocol head */
    if (httpAddrLen <= strlen((char *)"http://"))
    {
        return FALSE;
    }

    if (memcmp(pURLStr, "http://", strlen((char *)"http://")) != 0)
    {
        return FALSE;
    }

    /* Find the location when file path start */
    hostStartPos = strlen((char *)"http://");

    for (iChar = hostStartPos; iChar < strlen(pURLStr); iChar++)
    {
        if (pURLStr[iChar] == '/')
        {
            pathStartPos = iChar + 1;
            break;
        }
    }
    if (pathStartPos == 0)
    {
        return FALSE;
    }

    /*Get File Path*/
    memset(pathBuf, 0x00, sizeof(pathBuf));
    memcpy(pathBuf, &pURLStr[pathStartPos], httpAddrLen - pathStartPos);
    for (iChar = 0; iChar < strlen((char *)pathBuf); iChar++)
    {
        if (memcmp(&pathBuf[iChar], "\r\n", strlen((char *)"\r\n")) == 0)
        {
            if (iChar == (strlen((char *)pathBuf) - 2))
            {
                pathBuf[iChar] = 0;
            }
            else
            {
                return FALSE;
            }
        }
    }
}


static Boolean saveUpgradeInfo(const TB_Upgrade_UpgradeInfoT* pInfo)
{
    int32 fd;

    if (pInfo == PNULL)
    {
        return FALSE;
    }

    if (TP_FExist("TB_UpgradeInfo") == FALSE)
    {
        fd = TP_Create("TB_UpgradeInfo", sizeof(TB_Upgrade_UpgradeInfoT));
        if (fd < sizeof(TB_Upgrade_UpgradeInfoT))
        {
            return FALSE;
        }
    }

    fd = TP_Open("TB_UpgradeInfo");
    if (fd >= 0)
    {
        TP_Write(fd, (uint8*)pInfo, sizeof(TB_Upgrade_UpgradeInfoT));
        TP_Close(fd);

        return TRUE;
    }

    return FALSE;
}


static void clearUpgradeInfo(void)
{
    int32 fd;

    if (TP_FExist("TB_UpgradeInfo") == TRUE)
    {
        TP_Delete("TB_UpgradeInfo");
    }
}


static Boolean readUpgradeInfo(TB_Upgrade_UpgradeInfoT* pInfoBuf)
{
    int32 fd;

    fd = TP_Open("TB_UpgradeInfo");
    if (fd >= 0)
    {
        TP_Read(fd, (uint8*)pInfoBuf, sizeof(TB_Upgrade_UpgradeInfoT));
        TP_Close(fd);

        return TRUE;
    }

    return FALSE;
}


/******************************************************************************
* Public Functions
*******************************************************************************/
uint8 TB_Upgrade_UpgradeCheck(const TB_Upgrade_APNContextT* pAPNContext)
{
    TB_Upgrade_UpgradeInfoT upgradeInfo;
    uint8          result;

    if (TP_GetMobileCardStatus() != TP_MOBILECARDOK)
    {
        return 0x00;
    }

    memset((uint8*)&upgradeInfo, 0x00, sizeof(TB_Upgrade_UpgradeInfoT));
    if (readUpgradeInfo(&upgradeInfo) == FALSE)
    {
        return 0x00;
    }

    if (upgradeInfo.type == TB_UPGRADE_TYPE_GETVERSION)
    {
        clearUpgradeInfo();

        return 0x00;
    }

    result = TP_UpgradeGetResult();

    if (result == 0x00)
    {/* Upgrade Succeed */
        TP_ScrRestore(0);

        /* Prompt User Upgrade Succeed*/
        TP_TimerSet(0, 3 * 1000);
        TP_Kbflush();
        tb_upgrade_display_PromptSuccess();
        clearUpgradeInfo();
        while (TP_TimerCheck(0) != 0)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_Kbflush();

                break;
            }
        }

        /* If need to send SMS, prompt user */
        if (strlen(upgradeInfo.serverSMEAddr) != 0)
        {
            tb_upgrade_display_PromptFinish();
            TP_TimerSet(0, 180000);
            while (TP_TimerCheck(0) != 0)
            {
                if (TP_NetworkGetStatus() == SRVCSTATUS_REGISTERED)
                {
                    tb_upgrade_comm_ReplySMS(TB_UPGRADE_FINISH,
                                             upgradeInfo.serverSMEAddr,
                                             &TP_Data_APPInfo.productVersion[5]);
                    break;
                }
            }
        }

        TP_ScrRestore(1);

        result = 0x00;
    }
    else if (result == 0x01)
    {/* Upgrade Failed */
        TP_ScrRestore(0);

        /* Prompt User Upgrade Failed*/
        TP_TimerSet(0, 3 * 1000);
        TP_Kbflush();
        tb_upgrade_display_PromptFail();
        clearUpgradeInfo();
        while (TP_TimerCheck(0) != 0)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_Kbflush();

                break;
            }
        }

        /* If need to send SMS, prompt user */
        if (strlen(upgradeInfo.serverSMEAddr) != 0)
        {
            TP_TimerSet(0, 180 * 1000);
            tb_upgrade_display_PromptFinish();
            while (TP_TimerCheck(0) != 0)
            {
                if (TP_NetworkGetStatus() == SRVCSTATUS_REGISTERED)
                {
                    tb_upgrade_comm_ReplySMS(TB_UPGRADE_FAILED, upgradeInfo.serverSMEAddr, PNULL);
                    break;
                }
            }
        }

        /* Query user wether to restart upgrade process */
        TP_Kbflush();
        result = tb_upgrade_display_QueryDoRestart();

        TP_ScrRestore(1);

        /* Restart upgrade process */
        if (result == 0x00)
        {
            TP_TimerSet(0, 180 * 1000);
            tb_upgrade_display_PromptServiceRegisting();
            while (TP_TimerCheck(0) != 0)
            {
                if (TP_NetworkGetStatus() == SRVCSTATUS_REGISTERED)
                {
                    break;
                }
            }

            result = TB_Upgrade_ManualUpgrade(&upgradeInfo, pAPNContext);
        }
        else
        {
            result = 0x00;
        }
    }
    else
    {/* A upgrade process has not been finished */
        TP_ScrRestore(0);

        /* Query user wether to restore upgrade process */
        TP_Kbflush();
        result = tb_upgrade_display_QueryDoResume();

        if (result == 0x01)
        {
            /** Query user wether to delete all upgrade data */
            TP_Kbflush();
            result = tb_upgrade_display_QueryDoClearData();
            if (result == 0x00)
            {
                tb_upgrade_display_PromptClearData();

                clearUpgradeInfo();

                TP_UpgradeStart(FALSE, PNULL);
                TP_UpgradePause();
            }

            result = 0x01;
        }

        TP_ScrRestore(1);

        if (result == 0x00)
        {
            TP_TimerSet(0, 180 * 1000);
            tb_upgrade_display_PromptServiceRegisting();
            while (TP_TimerCheck(0) != 0)
            {
                if (TP_NetworkGetStatus() == SRVCSTATUS_REGISTERED)
                {
                    break;
                }
            }

            result = TB_Upgrade_ManualUpgrade(&upgradeInfo, pAPNContext);
        }
        else
        {
            result = 0x00;
        }
    }

    return result;
}


void TB_Upgrade_UpgradeCancel(void)
{
    clearUpgradeInfo();
}


Boolean TB_Upgrade_IsUpgradeSMS(const uint8* pShortMsg, uint16 lenShortMsg,
                                const char* pSMEAddr, uint8 lenSMEAddr,
                                TB_Upgrade_UpgradeInfoT* pInfoBuf)
{
    if (isUpgradeNotify(pShortMsg, lenShortMsg,
                        pSMEAddr, lenSMEAddr,
                        pInfoBuf) == TRUE)
    {
        return TRUE;
    }

    if (isGetVersionNotify(pShortMsg, lenShortMsg,
                           pSMEAddr, lenSMEAddr,
                           pInfoBuf) == TRUE)
    {
        return TRUE;
    }

    return FALSE;
}


uint8 TB_Upgrade_SMSUpgrade(const TB_Upgrade_UpgradeInfoT* pInfo,
                            const TB_Upgrade_APNContextT* pAPNContext)
{
    TP_UpgradeBreakPointT savedBreakPoint;
    uint8 result;

    if (pInfo->type == TB_UPGRADE_TYPE_GETVERSION)
    {
        TP_ScrRestore(0);

        TP_TimerSet(0, 3 * 1000);
        TP_Kbflush();
        tb_upgrade_display_PromptGetVersion();
        TP_BanIncomingCall(TRUE);

        tb_upgrade_comm_ReplySMS(TB_UPGRADE_GETVERSION,
                                 pInfo->serverSMEAddr,
                                 &TP_Data_APPInfo.productVersion[5]);

        while (TP_TimerCheck(0) != 0)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_Kbflush();

                break;
            }
        }

        TP_BanIncomingCall(FALSE);
        TP_ScrRestore(1);

        result = 0x00;
    }
    else if (pInfo->type == TB_UPGRADE_TYPE_UPGRADE)
    {
        if (checkFileURL(pInfo->fileURL) == FALSE)
        {
            tb_upgrade_comm_ReplySMS(TB_UPGRADE_FAILED,
                                     pInfo->serverSMEAddr,
                                     PNULL);
            return 0x03;
        }

        TP_ScrRestore(0);

        tb_upgrade_display_PromptStart();
        TP_BanIncomingCall(TRUE);

        result = TP_UpgradeStart(FALSE, PNULL);
        if (result == 0x00)
        {
            saveUpgradeInfo(pInfo);
            tb_upgrade_comm_ReplySMS(TB_UPGRADE_START,
                                     pInfo->serverSMEAddr,
                                     PNULL);

            tb_upgrade_display_PromptCheckingData();
            result = tb_upgrade_comm_CheckData(pInfo, pAPNContext);
            if (result == 0x00)
            {
                memset((void*)&savedBreakPoint, 0x00, sizeof(TP_UpgradeBreakPointT));
                result = tb_upgrade_comm_DownloadData(pInfo, &savedBreakPoint, pAPNContext);
            }

            if (result == 0x00)
            {
                tb_upgrade_display_PromptBeforeUpgrade();

                tb_upgrade_comm_ReplySMS(TB_UPGRADE_BEFOREFINISH,
                                         pInfo->serverSMEAddr,
                                         PNULL);

                result = TP_UpgradeExcute();
                if (result != 0x00)
                {
                    result = 0x07;
                }
            }
        }
        else
        {
            result = 0x01;
        }

        TP_UpgradePause();

        if ((result == 0x08) ||
            (result == 0x0A))
        {
            tb_upgrade_comm_ReplySMS(TB_UPGRADE_ERROR_VERSION,
                                     pInfo->serverSMEAddr,
                                     PNULL);
        }
        else if (result == 0x09)
        {
            tb_upgrade_comm_ReplySMS(TB_UPGRADE_ERROR_PROTOCOL,
                                     pInfo->serverSMEAddr,
                                     PNULL);
        }
        else
        {
            tb_upgrade_comm_ReplySMS(TB_UPGRADE_FAILED,
                                     pInfo->serverSMEAddr,
                                     PNULL);
        }

        TP_BanIncomingCall(FALSE);
        TP_ScrRestore(1);
    }

    return result;
}


uint8 TB_Upgrade_ManualUpgrade(const TB_Upgrade_UpgradeInfoT* pInfo,
                               const TB_Upgrade_APNContextT* pAPNContext)
{
    TB_Upgrade_UpgradeInfoT savedInfo;
    TP_UpgradeBreakPointT   savedBreakPoint;
    uint8                   result;

    if (pInfo->type == TB_UPGRADE_TYPE_GETVERSION)
    {
        return;
    }

    TP_ScrRestore(0);

    tb_upgrade_display_PromptStart();
    TP_BanIncomingCall(TRUE);

    result = 0x00;
    if (readUpgradeInfo(&savedInfo) == TRUE)
    {/* A upgrade process has not been finished */
        if (memcmp((void*)&savedInfo, (void*)pInfo, sizeof(TB_Upgrade_UpgradeInfoT)) == 0)
        {/* User manual resume unfinished one */
            result = 0x01;
        }
        else
        {/* User want to start a new one */
            /** Query user wether to start a new upgrade process or not */
            TP_Kbflush();
            result = tb_upgrade_display_QueryDoStartNewOne();
            if (0x00 == result)
            {
                memcpy((void*)&savedInfo, (void*)pInfo, sizeof(TB_Upgrade_UpgradeInfoT));
            }
        }
    }
    else
    {
        memcpy((void*)&savedInfo, (void*)pInfo, sizeof(TB_Upgrade_UpgradeInfoT));
    }

    if (result == 0x00)
    {/* A new upgrade process */
        if (checkFileURL(savedInfo.fileURL) == FALSE)
        {
            tb_upgrade_comm_ReplySMS(TB_UPGRADE_FAILED, savedInfo.serverSMEAddr, PNULL);

            TP_BanIncomingCall(FALSE);
            TP_ScrRestore(1);

            return 0x03;
        }

        saveUpgradeInfo(&savedInfo);

        result = TP_UpgradeStart(FALSE, PNULL);
    }
    else
    {/* A resumed upgrade process */
        if (TP_UpgradeExistBreakPoint() == TRUE)
        {/* There is a break point */
            /* Query user wether to use break point*/
            TP_Kbflush();
            result = tb_upgrade_display_QueryDoUseBP();

            if (result == 0x00)
            {
                result = TP_UpgradeStart(TRUE, &savedBreakPoint);
                if (result == 0x02)
                {
                    TP_UpgradePause();
                    result = TP_UpgradeStart(FALSE, PNULL);
                }
            }
            else
            {
                result = TP_UpgradeStart(FALSE, PNULL);
            }
        }
        else
        {
            result = TP_UpgradeStart(FALSE, PNULL);
        }
    }

    if (result == 0x01)
    {/* A resumed upgrade process with break point */
        tb_upgrade_display_PromptCheckingBP();
        result = tb_upgrade_comm_CheckBP(&savedInfo, &savedBreakPoint, pAPNContext);
        if (result == 0x01)
        {
            /* Prompt user break point is invalid */
            TP_TimerSet(0, 3 * 1000);
            TP_Kbflush();
            tb_upgrade_display_PromptInvalidBP();
            TP_UpgradePause();
            result = TP_UpgradeStart(FALSE, PNULL);
            while (TP_TimerCheck(0) != 0)
            {
                if (TP_Kbhit() == 0xFF)
                {
                    TP_Kbflush();

                    break;
                }
            }
        }
        else if (result != 0)
        {

        }
        else
        {
            result = 0x01;
        }
    }

    if (result == 0x00)
    {/* Download data from start */
        tb_upgrade_display_PromptCheckingData();
        result = tb_upgrade_comm_CheckData(pInfo, pAPNContext);
        if (result == 0x00)
        {
            memset((void*)&savedBreakPoint, 0x00, sizeof(TP_UpgradeBreakPointT));
            result = tb_upgrade_comm_DownloadData(&savedInfo, &savedBreakPoint, pAPNContext);
        }
    }
    else if (result == 0x01)
    {/* Download data from break point */
        result = tb_upgrade_comm_DownloadData(&savedInfo, &savedBreakPoint, pAPNContext);
    }
    else if ((result == 0xFE) ||
             (result == 0xFF))
    {/* FOTA start return error */
        result = 0x01;
    }

    if (result == 0x00)
    {
        tb_upgrade_display_PromptBeforeUpgrade();

        tb_upgrade_comm_ReplySMS(TB_UPGRADE_BEFOREFINISH,
                                 savedInfo.serverSMEAddr,
                                 PNULL);

        result = TP_UpgradeExcute();
        if (result != 0x00)
        {
            result = 0x07;
        }
    }

    TP_UpgradePause();

    if ((result == 0x08) ||
        (result == 0x0A))
    {
        tb_upgrade_comm_ReplySMS(TB_UPGRADE_ERROR_VERSION,
                                 savedInfo.serverSMEAddr,
                                 PNULL);
    }
    else if (result == 0x09)
    {
        tb_upgrade_comm_ReplySMS(TB_UPGRADE_ERROR_PROTOCOL,
                                 savedInfo.serverSMEAddr,
                                 PNULL);
    }
    else
    {
        tb_upgrade_comm_ReplySMS(TB_UPGRADE_FAILED,
                                 savedInfo.serverSMEAddr,
                                 PNULL);
    }

    TP_BanIncomingCall(FALSE);
    TP_ScrRestore(1);

    return result;
}
