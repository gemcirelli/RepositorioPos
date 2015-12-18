/*******************************************************************************
 * TELPO Software Copyright (c) 1997-2011 TELPO Ltd
 * Licensed to Simcom Ltd
 ******************************************************************************/

/*******************************************************************************
 * File Location:.\TPCode\TPFota\TPFota_display.c
 * File Description: Fota start upgrade Manually¡¢Fota related function design sample
 * Create Date: Feb 3rd, 2012
 *   Creator: Zhang Lv
 * Edit Date:
 * Edit Description:
 * Edit Date:
 ******************************************************************************/

/******************************************************************************
* Include Headers
*******************************************************************************/
#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeypad.h>
#include <TPLCD.h>
#include <TPFota.h>
#include <TPNet.h>
#include <TPPPP.h>
#include <TPSerial.h>
#include <TPPhone.h>

#include "TP_HttpDemo.h"

/******************************************************************************
* Macros
*******************************************************************************/
#define CHAR_WIDTH  (6)
#define CHAR_HEIGHT (12)

/******************************************************************************
* Protected Variables
*******************************************************************************/
uint8 UpgradeFaultFlag = 0;

extern const char
SoftwareVersion[10];

/******************************************************************************
* Private Functions
*******************************************************************************/
Boolean checkHttpAddr(const char* httpAddr, uint16 httpAddrLen)
{
    uint16  iChar;
    uint16  hostStartPos = 0;
    uint16  pathStartPos = 0;
    char    pathBuf[256];
    char    filenameStr[256];
    Boolean isRightFile;

    TP_DbgSerialPrn("%s\x00", "HTTP Address:");
    TP_DbgSerialPrnHex((uint8*)httpAddr, httpAddrLen);

    /* Check protocol head */
    if (httpAddrLen <= strlen((char *)"http://"))
    {
        return FALSE;
    }

    if (memcmp(httpAddr, "http://", strlen((char *)"http://")) != 0)
    {
        return FALSE;
    }

    /* Find the location when file path start */
    hostStartPos = strlen((char *)"http://");

    for (iChar = hostStartPos; iChar < httpAddrLen; iChar++)
    {
        if (httpAddr[iChar] == '/')
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
    memcpy(pathBuf, &httpAddr[pathStartPos], httpAddrLen - pathStartPos);
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

    /* Check File Name */
    isRightFile = FALSE;
    for (iChar = 0; iChar < strlen((char *)pathBuf); iChar++)
    {
        if (pathBuf[iChar] == '.')
        {
            pathStartPos = iChar;

            break;
        }
    }
    if (iChar == strlen((char *)pathBuf))
    {
        return 0;
    }

    for ( ; iChar > 0; iChar--)
    {
        if (pathBuf[iChar] == '/')
        {
            iChar++;

            break;
        }
    }

    memset(filenameStr, 0x00, sizeof(filenameStr));
    memcpy(filenameStr, &pathBuf[iChar], pathStartPos - iChar);

    TP_DbgSerialPrn("%s\x00", filenameStr);

    #ifdef TPS300
    if (strlen(filenameStr) != strlen("IXXXXXXXX\x00"))
    {
        return FALSE;
    }
    #elif defined(TPS320)
    if (strlen(filenameStr) != strlen("IIXXXXXXXX\x00"))
    {
        return FALSE;
    }
    #else
    if (strlen(filenameStr) != strlen("IIIXXXXXXXX\x00"))
    {
        return FALSE;
    }
    #endif

    iChar = 0;
    #ifdef TPS300
    if (filenameStr[iChar] == 'I')
    {
        iChar++;
    #elif defined(TPS320)
    if ((filenameStr[iChar]     == 'I') &&
        (filenameStr[iChar + 1] == 'I'))
    {
        iChar += 2;
    #else
    if ((filenameStr[iChar]     == 'I') &&
        (filenameStr[iChar + 1] == 'I') &&
        (filenameStr[iChar + 2] == 'I'))
    {
        iChar += 3;
    #endif
        if (filenameStr[iChar] == '8')
        {
            iChar++;
            #if defined(TP_CORE_800)
            if (filenameStr[iChar] == '0')
            #elif defined(TP_CORE_840)
            if (filenameStr[iChar] == '4')
            #endif
            {
                isRightFile = TRUE;
            }
        }
    }

    if (isRightFile == FALSE)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


/******************************************************************************
* Callback Functions
*******************************************************************************/
void TP_OnFotaSMSNotify(TP_SMSUpgradeInfoT* upgradeInfo_p)
{
    uint8 result;

    if (upgradeInfo_p->type == UPGRADE_GETVERSION)
    {
        TP_FotaSmsReply(FOTA_UPGRADE_GETVERSION,
                        upgradeInfo_p->smeAddr);
    }
    else if (upgradeInfo_p->type == UPGRADE_SMS)
    {
        if (memcmp(upgradeInfo_p->version,
                   SoftwareVersion,
                   strlen((char *)upgradeInfo_p->version)) == 0)
        {
            TP_FotaSmsReply(FOTA_UPGRADE_ERROR_VERSION,
                            upgradeInfo_p->smeAddr);

            return;
        }

        if (checkHttpAddr(upgradeInfo_p->httpAddr, upgradeInfo_p->httpAddrLen) == FALSE)
        {
            TP_FotaSmsReply(FOTA_UPGRADE_ERROR_PROTOCOL,
                            upgradeInfo_p->smeAddr);

            return;
        }

        Demo_HttpInit();

        TP_ScrFontSet(CFONT);
        TP_ScrAttrSet(0);
        TP_ScrDrawRect(13, 1, 114, 62);
        TP_SetDisplayArea(14, 2, 113, 61);
        TP_ScrClrRect(14, 2, 113, 61);
        TP_ScrGotoxyEx(14, 2);

        TP_LcdPrintf((uint8*)"S/W Upgrading...");
        result = TP_FotaSMSUpgrade(upgradeInfo_p);
        if (result == 0x00)
        {
            TP_SetDisplayArea(0, 0, 127, 63);
            TP_ScrClsDisplayArea();
            TP_ScrGotoxyEx(0,24);
            TP_LcdPrintf((uint8*)"%s",
                         "  Upgrading File Installing, It will take about 5 minutes. Do not Power Off");

            TP_TimerSet(1, 1000);
            while (TP_TimerCheck(1) != 0);

            TP_DbgSerialPrnEx(5, "To Reboot...\r\n");
            TP_DbgSerialPrnEx(5, "TP_Reboot:%p\r\n", TP_Reboot);

            while (TRUE)
            {
                TP_Reboot();
            }
        }
        else
        {
            TP_ScrClrRect(14, 2, 113, 61);
            TP_ScrGotoxyEx(14, 2);
            switch (result)
            {
            case 0x01:
                TP_LcdPrintf((uint8*)"!Block Erasing Failed");
                break;
            case 0x02:
                TP_LcdPrintf((uint8*)"!Server Connecting Failed");
                break;
            case 0x03:
                TP_LcdPrintf((uint8*)"!Upgrade File Do Not Exist");
                break;
            case 0x04:
                TP_LcdPrintf((uint8*)"!Download Upgrade File Failed");
                break;
            case 0x05:
                TP_LcdPrintf((uint8*)"!Write File Failed");
                break;
            }

            TP_TimerSet(1, 3000);
            TP_Kbflush();
            while (TRUE)
            {
                if (TP_TimerCheck(1) == 0)
                {
                    break;
                }

                if (TP_Kbhit() == 0xFF)
                {
                    TP_GetKey();
                    break;
                }
            }

            TP_DbgSerialPrnEx(5, "To Reboot...\r\n");
            TP_DbgSerialPrnEx(5, "TP_Reboot:%p\r\n", TP_Reboot);

            while (TRUE)
            {
                TP_Reboot();
            }
        }
    }
}


uint8 TP_FotaSMSDownloadData(TP_SMSUpgradeInfoT* upgradeInfo_p)
{
    Demo_HttpRecvPacketT recvPacket;
    uint32  fileSize, result, readEndAddr, readStartAddr;
    uint16  dataLen;
    uint8   connectCounts;

    connectCounts = 0;
    while (connectCounts < 10)
    {
        TP_FotaProgressDisplay(UPGRADE_STEP_CONNECT,
                               connectCounts + 1, 10);

        if (Demo_HttpConnect(upgradeInfo_p->ipAddr, upgradeInfo_p->ipPort) ==
            TRUE)
        {
            break;
        }
        else
        {
            connectCounts++;
            Demo_HttpExit();

            TP_TimerSet(1, 5000);
            while (TP_TimerCheck(1) != 0);
        }
    }

    if (connectCounts >= 10)
    {
        Demo_HttpExit();

        return 0x02;
    }

    fileSize             = 0;
    connectCounts        = 0;
    recvPacket.startAddr = readStartAddr = 0;
    recvPacket.endAddr   = readEndAddr   = DEMO_HTTP_PACKET_SIZE - 1;
    memset(recvPacket.content, 0x00, DEMO_HTTP_PACKET_SIZE);
    while (TRUE)
    {
        if (connectCounts >= 10)
        {
            Demo_HttpExit();

            return 0x04;
        }

        TP_FotaProgressDisplay(UPGRADE_STEP_DOWNLOAD,
                               recvPacket.startAddr,
                               fileSize);

        result = Demo_HttpGet(&recvPacket,
                              upgradeInfo_p->httpAddr,
                              upgradeInfo_p->httpAddrLen);
        if (result == 0)
        {
            if (TP_GetLastError() == HTTPERR_RET_404)
            {
                Demo_HttpExit();

                return 0x03;
            }
            else if (TP_GetLastError() == HTTPERR_RET_OTHERS)
            {
                Demo_HttpExit();

                return 0x04;
            }

            while (connectCounts < 10)
            {
                Demo_HttpExit();

                TP_TimerSet(1, 1000);
                while (TP_TimerCheck(1) != 0);

                if (Demo_HttpConnect(upgradeInfo_p->ipAddr, upgradeInfo_p->ipPort) == TRUE)
                {
                    connectCounts = 0;
                    break;
                }

                connectCounts++;
            }
        }
        else
        {
            fileSize = result;

            connectCounts = 0;

            if(recvPacket.endAddr != readEndAddr)
            {
                if ((recvPacket.endAddr + 1) != fileSize)
                {//invalid packet,download again
                    recvPacket.startAddr = readStartAddr;
                    recvPacket.endAddr   = readEndAddr;

                    continue;
                }
            }

            //write data of the packet to FLASH
            TP_FotaProgressDisplay(UPGRADE_STEP_WRITE,
                                   recvPacket.startAddr,
                                   fileSize);
            TP_TimerSet(1, 2000);

            dataLen = recvPacket.endAddr - recvPacket.startAddr + 1;
            if (TP_FotaWriteData(recvPacket.startAddr,
                                 recvPacket.content,
                                 dataLen) == FALSE)
            {
                Demo_HttpExit();

                return 0x05;
            }

            while (TP_TimerCheck(1) != 0);

            //¼ÆËãÏÂÒ»°üÇëÇoµÄÊý¾Ý·¶Î§
            if (fileSize != (recvPacket.endAddr + 1))
            {
                recvPacket.startAddr = recvPacket.endAddr + 1;
                recvPacket.endAddr   = recvPacket.startAddr + DEMO_HTTP_PACKET_SIZE - 1;
                if ((recvPacket.endAddr + 1) > fileSize)
                {
                    recvPacket.endAddr = fileSize - 1;
                }
                readStartAddr = recvPacket.startAddr;
                readEndAddr   = recvPacket.endAddr;
            }
            else
            {
                Demo_HttpExit();

                TP_FotaProgressDisplay(UPGRADE_STEP_DOWNLOAD,
                                       fileSize,
                                       fileSize);

                TP_TimerSet(1, 2000);
                while (TP_TimerCheck(1) != 0);

                TP_FotaProgressDisplay(UPGRADE_STEP_WRITE,
                                       fileSize,
                                       fileSize);

                TP_TimerSet(1, 2000);
                while (TP_TimerCheck(1) != 0);

                return 0x00;
            }
        }
    }
}


uint8 TP_FotaDownloadData(TP_UpgradeInfoT* upgradeInfo_p)
{
    /*TODO: Add your implemention of download upgrading file */
}


void TP_FotaProgressDisplay(TP_UpgradeStepE step,
                            uint32 currValue, uint32 maxValue)
{
    uint8 tmp[64];
    uint8 cprompt[] = "S/W Upgrading";

    static int32 num = 1;

    TP_ScrFontSet(CFONT);
    TP_ScrClrRect(14, 2, 113, 61);
    TP_ScrGotoxyEx(14, 2);

    memset(tmp, 0, sizeof(tmp));
    sprintf((char *)tmp, "%s", cprompt);
    memset(&tmp[strlen((char *)cprompt)], '.', num++ % 4);
    TP_LcdPrintf(tmp);

    if (step == UPGRADE_STEP_ERASE)
    {
        TP_ScrGotoxyEx(14, 14);
        TP_LcdPrintf((uint8*)"Block Erasing:");
        TP_ScrGotoxyEx(14, 26);
        TP_LcdPrintf((uint8*)"%d / %d", currValue, maxValue);
    }
    else if (step == UPGRADE_STEP_CONNECT)
    {
        TP_ScrGotoxyEx(14, 14);
        TP_LcdPrintf((uint8*)"Server Connecting:");
        TP_ScrGotoxyEx(14, 38);
        TP_LcdPrintf((uint8*)"Tries: %d / %d", currValue, maxValue);
    }
    else if (step == UPGRADE_STEP_DOWNLOAD)
    {
        TP_ScrGotoxyEx(14, 14);
        TP_LcdPrintf((uint8*)"File Downloading:");
        if (maxValue > 0)
        {
            TP_ScrGotoxyEx(14, 38);
            TP_LcdPrintf((uint8*)"%d / %d", currValue, maxValue);
        }
        else
        {
            TP_ScrGotoxyEx(14, 38);
            TP_LcdPrintf((uint8*)"%d / Unknown", currValue);
        }
    }
    else if (step == UPGRADE_STEP_WRITE)
    {
        TP_ScrGotoxyEx(14, 14);
        TP_LcdPrintf((uint8*)"File Writing:");
        if (maxValue > 0)
        {
            TP_ScrGotoxyEx(14, 38);
            TP_LcdPrintf((uint8*)"%d / %d", currValue, maxValue);
        }
    }
}


void TP_FotaUpgradeResultPrompt(TP_UpgradeResultE value)
{
    /* ÊÖ¶¯µ¥´Îeý¼¶Ê§°Ü»ò¶ÌÐÅeý¼¶ÕûÌåÊ§°Ü */
    if (value == UPGRADE_FAIL)
    {
        UpgradeFaultFlag = 3;
    }
    else if (value == UPGRADE_SUCCESS)
    {
        UpgradeFaultFlag = 1;
    }
    else if (value == UPGRADE_RETRY)
    {
        UpgradeFaultFlag = 2;
    }
}


void TP_AppUpgradeCheck(void)
{
    TP_SMSUpgradeInfoT upgradeInfo;

    memset((uint8*)&upgradeInfo, 0x00, sizeof(TP_SMSUpgradeInfoT));
    TP_FotaReadUpgradeFile(&upgradeInfo);

    if (UpgradeFaultFlag == 1)
    {
        TP_ScrRestore(0);

        TP_ScrFontSet(CFONT);
        TP_ScrAttrSet(0);
        TP_ScrDrawRect(13, 1, 114, 62);
        TP_SetDisplayArea(14, 2, 113, 61);
        TP_ScrClrRect(14, 2, 113, 61);
        TP_ScrGotoxyEx(14, 2);

        TP_LcdPrintf((uint8*)"S/W Upgrade Successful");

        TP_TimerSet(1, 3000);
        TP_Kbflush();
        while (TRUE)
        {
            if (TP_TimerCheck(1) == 0)
            {
                break;
            }

            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();

                break;
            }
        }

        TP_TimerSet(1, 180000);
        while (TRUE)
        {
            if (TP_TimerCheck(1) == 0)
            {
                break;
            }

            if (TP_WlPPPCheck() == WL_RET_ERR_LINEOFF)
            {
                break;
            }
        }

        TP_DbgSerialPrnHex((uint8 *)upgradeInfo.smeAddr, 20);

        TP_FotaSmsReply(FOTA_UPGRADE_BEFOREFINISH, upgradeInfo.smeAddr);

        TP_FotaSmsReply(FOTA_UPGRADE_FINISH, upgradeInfo.smeAddr);

        TP_ScrRestore(1);
    }
    else if (UpgradeFaultFlag == 2)
    {
        TP_ScrRestore(0);

        /* ±¾´Î¶ÌÐÅeý¼¶Ê§°Ü */
        TP_ScrFontSet(CFONT);
        TP_ScrAttrSet(0);
        TP_ScrDrawRect(13, 1, 114, 62);
        TP_SetDisplayArea(14, 2, 113, 61);
        TP_ScrClrRect(14, 2, 113, 61);
        TP_ScrGotoxyEx(14, 2);

        TP_LcdPrintf((uint8*)"!S/W Upgrade Failed¡£\nPress any key to restart upgrade");

        TP_TimerSet(1, 3000);
        TP_Kbflush();
        while (TRUE)
        {
            if (TP_TimerCheck(1) == 0)
            {
                break;
            }

            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();

                break;
            }
        }

        TP_ScrAttrSet(0);
        TP_ScrCls();
        TP_ScrDrawRect(13, 1, 114, 62);
        TP_SetDisplayArea(14, 2, 113, 61);
        TP_ScrClrRect(14, 2, 113, 61);
        TP_ScrGotoxyEx(14, 2);
        TP_LcdPrintf((uint8*)"%s", "Service Registering...");
        TP_TimerSet(1, 180000);

        while (TRUE)
        {
            if (TP_TimerCheck(1) == 0)
            {
                TP_ScrClrRect(14, 2, 113, 61);
                TP_ScrGotoxyEx(14, 2);
                TP_LcdPrintf((uint8*)"%s", "!Service Register Failed¡£\nPress any key to reboot");

                TP_TimerSet(1, 3000);
                TP_Kbflush();
                while (TRUE)
                {
                    if (TP_TimerCheck(1) == 0)
                    {
                        break;
                    }

                    if (TP_Kbhit() == 0xFF)
                    {
                        TP_GetKey();

                        break;
                    }
                }

                TP_DbgSerialPrnEx(5, "To Reboot...\r\n");
                TP_DbgSerialPrnEx(5, "fpReset:%p\r\n", TP_Reboot);

                while (TRUE)
                {
                    TP_Reboot();
                }
            }

            if (TP_WlPPPCheck() == WL_RET_ERR_LINEOFF)
            {
                break;
            }
        }

        TP_DbgSerialPrnHexEx(5, (uint8 *)upgradeInfo.smeAddr, 20);

        TP_BanIncomingCall(TRUE);

        TP_FotaSmsReply(FOTA_UPGRADE_FAILED, upgradeInfo.smeAddr);

        TP_OnFotaSMSNotify(&upgradeInfo);

        TP_BanIncomingCall(FALSE);
    }
    else if (UpgradeFaultFlag == 3)
    {
        TP_ScrRestore(0);

        TP_ScrFontSet(CFONT);
        TP_ScrAttrSet(0);
        TP_ScrDrawRect(13, 1, 114, 62);
        TP_SetDisplayArea(14, 2, 113, 61);
        TP_ScrClrRect(14, 2, 113, 61);

        TP_ScrGotoxyEx(14, 2);
        TP_LcdPrintf((uint8*)"!S/W Upgrade Failed");

        TP_TimerSet(1, 3000);
        TP_Kbflush();
        while (TRUE)
        {
            if (TP_TimerCheck(1) == 0)
            {
                break;
            }

            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();

                break;
            }
        }

        TP_TimerSet(1, 180000);
        while (TRUE)
        {
            if (TP_TimerCheck(1) == 0)
            {
                break;
            }

            if (TP_WlPPPCheck() == WL_RET_ERR_LINEOFF)
            {
                break;
            }
        }

        TP_DbgSerialPrnHexEx(5, (uint8 *)upgradeInfo.smeAddr, 20);

        TP_FotaSmsReply(FOTA_UPGRADE_FAILED, upgradeInfo.smeAddr);

        TP_ScrRestore(1);
    }
}


void SMSUpgradeTest(void)
{
    TP_SMSUpgradeInfoT upgradeInfo;
    TP_SockAddrT       sockAddr;

    #ifdef TPS300
    sprintf(upgradeInfo.httpAddr, "%s",
            "http://121.9.230.130:8104/I84120906.bin\x00");
    #elif defined(TPS320)
    sprintf(upgradeInfo.httpAddr, "%s",
            "http://121.9.230.130:8104/II84120906.bin\x00");
    #else
    sprintf(upgradeInfo.httpAddr, "%s",
            "http://121.9.230.130:8104/III84120906.bin\x00");
    #endif
    upgradeInfo.httpAddrLen = strlen(upgradeInfo.httpAddr);
    TP_SockAddrSet(&sockAddr, (uchar*)"121.9.230.130", 8104);
    upgradeInfo.ipAddr = sockAddr.ipAddr;
    upgradeInfo.ipPort = sockAddr.port;
    upgradeInfo.numRetryEnable = 5;
    sprintf(upgradeInfo.smeAddr, "%s", "15915860859\x00");
    upgradeInfo.smeAddrLen = strlen(upgradeInfo.smeAddr);
    upgradeInfo.type = UPGRADE_SMS;
    sprintf(upgradeInfo.version, "%s", "V84120906");

    TP_OnFotaSMSNotify(&upgradeInfo);
}

/*End Of File*/
