/*******************************************************************************
 * TELPO Software Copyright (c) 1997-2012 TELPO Ltd
 * Licensed to Simcom Ltd
 ******************************************************************************/

/*******************************************************************************
 * File Location:.\demo\TP_HttpDemo.c
 * File Description: Http Protocol Implement Demo
 * Create Date: Feb 1st, 2012
 *   Creator: Zhang Lv
 * Edit Date:
 * Edit Description:
 * Edit Date:
 ******************************************************************************/

/*******************************************************************************
 * Head File
*******************************************************************************/
#include <TPTypedef.h>
#include <TPBase.h>
#include <TPPPP.h>
#include <TPNet.h>
#include <TPSerial.h>
#include "TP_HttpDemo.h"

#define LINKTYPE_UDP 0
#define LINKTYPE_TCP 1

extern void TP_SetLastError(int32 ErrCode);

/******************************************************************************
* Type Definition
*******************************************************************************/
/** Http Process Status*/
typedef enum DemoHttpStateTag
{
    /** Connected*/
    HTTP_IDLE = 0,
    /** Connected*/
    HTTP_CONNECTED
} E_DemoHttpState;

/** Http Context*/
typedef struct DemoHttpInfoTag
{
    /** Http Process Status*/
    E_DemoHttpState state;
    /** Whether to release PPP connection when Exit*/
    Boolean         needExit;
    /** SOCKET for Http Connection*/
    uint16          socket;
} ST_DemoHttpInfo;

/*******************************************************************************
 * Static Variables
*******************************************************************************/
/** GET Method*/
static const char getStr[]  =
    "GET ";
/** Http Protocol Head*/
static const char httpStr[] =
    " HTTP/1.1\r\n";
static const char acceptStr[] =
    "Accept: text/html, application/xhtml+xml, */*\r\n";
/** String for receiving language*/
static const char languageStr[] =
    "Accept-Language: zh-cn\r\n";
/** String for receiving code*/
static const char encodeStr[]   =
    "Accept-Encoding: gzip, deflate\r\n";
/** String for user agen*/
static const char userAgentStr[] =
    "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n";
/** String for host location*/
static const char hostStr[] =
    "Host: ";
/** String for asking range*/
static const char rangeStr[] =
    "RANGE:bytes=";
/** String for connection type*/
static const char connectionStr[] =
    "Connection: Keep-Alive\r\n\r\n";
/** String for content length*/
static const char contentLengthStr[] =
    "Content-Length: ";
/** String for content range*/
static const char contentRangeStr[] =
    "Content-Range: bytes ";

/** Http Context*/
static ST_DemoHttpInfo HttpInfo;

/******************************************************************************
* Private Function
*******************************************************************************/
/** Analysis Http address，analysis out the Host location and file path*/
static uint8 anlyzeHttpAddr(const char* httpAddr, uint16 httpAddrLen,
                            char* hostBuf, char* pathBuf)
{
    uint16 iChar;
    uint16 hostStartPos = 0;
    uint16 pathStartPos = 0;

    /*Skip protocol head*/
    if (httpAddrLen <= strlen((char *)"http://"))
    {
        return 0;
    }

    if (memcmp(httpAddr, "http://", strlen((char *)"http://")) != 0)
    {
        return 0;
    }

    hostStartPos = strlen((char *)"http://");
    /*Find the location when file path start*/
    for (iChar = hostStartPos; iChar < httpAddrLen; iChar++)
    {
        if (httpAddr[iChar] == '/')
        {
            pathStartPos = iChar;
            break;
        }
    }

    if (pathStartPos == 0)
    {
        return 0;
    }

    /*Get Host Address*/
    memcpy(hostBuf, &httpAddr[hostStartPos], pathStartPos - hostStartPos);
    strcat(hostBuf, "\r\n");

    /*Get File Path*/
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
                return 0;
            }
        }
    }

    if ((strstr(pathBuf, ".hex") != PNULL) ||
        (strstr(pathBuf, ".bin") != PNULL) ||
        (strstr(pathBuf, ".mot") != PNULL) ||
        (strstr(pathBuf, ".xml") != PNULL) ||
        (strstr(pathBuf, ".txt") != PNULL) ||
        (strstr(pathBuf, ".dat") != PNULL))
    {/*Static Files*/
        return 2;
    }
    else
    {/*Dynamic Files*/
        return 1;
    }
}

/** Fill RANGE field*/
static uint16 fillRange(char* buf_p, uint32 startAddr, uint32 endAddr)
{
    sprintf(buf_p, "%d-%d\r\n", startAddr, endAddr);
    return strlen((char *)buf_p);
}

/** Get the GET Mode Sent Data packets*/
static uint16 getGETMessage(char* buf_p, uint32 startAddr, uint32 endAddr,
                            const char* httpAddr, uint16 httpAddrLen)
{
    uint16 iChar;
    uint8  addrType;
    char   pathBuf[255];
    char   hostBuf[32];

    iChar  = 0;
    memset(pathBuf, 0x00, sizeof(pathBuf));
    memset(hostBuf, 0x00, sizeof(hostBuf));
    addrType = anlyzeHttpAddr(httpAddr, httpAddrLen, hostBuf, pathBuf);
    if (addrType > 0)
    {
        memcpy(&buf_p[iChar], getStr, strlen((char *)getStr));
        iChar += strlen((char *)getStr);
        memcpy(&buf_p[iChar], pathBuf, strlen((char *)pathBuf));
        iChar += strlen((char *)pathBuf);

        memcpy(&buf_p[iChar], httpStr, strlen((char *)httpStr));
        iChar += strlen((char *)httpStr);

        memcpy(&buf_p[iChar], acceptStr, strlen((char *)acceptStr));
        iChar += strlen((char *)acceptStr);

        memcpy(&buf_p[iChar], languageStr, strlen((char *)languageStr));
        iChar += strlen((char *)languageStr);

        memcpy(&buf_p[iChar], encodeStr, strlen((char *)encodeStr));
        iChar += strlen((char *)encodeStr);

        memcpy(&buf_p[iChar], userAgentStr, strlen((char *)userAgentStr));
        iChar += strlen((char *)userAgentStr);

        memcpy(&buf_p[iChar], hostStr, strlen((char *)hostStr));
        iChar += strlen((char *)hostStr);
        memcpy(&buf_p[iChar], hostBuf, strlen((char *)hostBuf));
        iChar += strlen((char *)hostBuf);
    }

    if(addrType == 2)
    {
        memcpy(&buf_p[iChar], rangeStr, strlen((char *)rangeStr));
        iChar += strlen((char *)rangeStr);
        iChar += fillRange(&buf_p[iChar], startAddr, endAddr);
    }

    if(addrType > 0)
    {
        memcpy(&buf_p[iChar], connectionStr, strlen((char *)connectionStr));
        iChar += strlen((char *)connectionStr);
    }

    return iChar;
}


/**
 * Get the message content from Http response message<BR>
 * Message content saved to content，and give the start and end location of the Message Content*/
static uint32 getContentFromPacket(Demo_HttpRecvPacketT* packet_p,
                                   const char* recvData_p, uint16 recvDataSize)
{
    uint16 iChar;
    uint16 len;
    uint32 fileSize;
    uint16 contentLength;
    uint16 contentStartPos;

    iChar = strlen((char *)"HTTP/x.x ");
    if (memcmp(&recvData_p[iChar], "404", strlen((char *)"404")) == 0)
    {
        TP_SetLastError(HTTPERR_RET_404);

        return 0;
    }
    else if (memcmp(&recvData_p[iChar], "416", strlen((char *)"416")) == 0)
    {
        TP_SetLastError(HTTPERR_RET_416);

        return 0;
    }

    if ((memcmp(&recvData_p[iChar], "200", strlen((char *)"200")) == 0) ||
        (memcmp(&recvData_p[iChar], "206", strlen((char *)"206")) == 0))
    {
        iChar += strlen((char *)"20X");
        /*Get Start location of Message Content*/
        for (contentStartPos = iChar;
             contentStartPos < recvDataSize;
             contentStartPos++)
        {
            if (memcmp(&recvData_p[contentStartPos],
                       "\r\n\r\n", strlen((char *)"\r\n\r\n")) == 0)
            {
                contentStartPos += strlen((char *)"\r\n\r\n");
                break;
            }
        }
        if (contentStartPos == recvDataSize)
        {
            TP_SetLastError(HTTPERR_WRONG_MESSAGE_SIZE);

            return 0;
        }
        /*Get Length of Message Content*/
        len = strlen((char *)contentLengthStr);
        for ( ; iChar < contentStartPos; iChar++)
        {
            if (memcmp(&recvData_p[iChar],
                       contentLengthStr, len) == 0)
            {
                break;
            }
        }
        if (iChar == contentStartPos)
        {
            TP_SetLastError(HTTPERR_WRONG_MESSAGE_SIZE);

            return 0;
        }
        else
        {
            iChar += len;
        }
        contentLength = atoi(&recvData_p[iChar]);
        if (contentStartPos + contentLength != recvDataSize)
        {
            TP_SetLastError(HTTPERR_WRONG_MESSAGE_SIZE);

            return 0;
        }

        /* Process Content Range String */
        len = strlen((char *)contentRangeStr);
        for ( ; iChar < contentStartPos; iChar++)
        {
            if (memcmp(&recvData_p[iChar], contentRangeStr, len) == 0)
            {
                break;
            }
        }
        if (iChar == contentStartPos)
        {
            TP_SetLastError(HTTPERR_WRONG_MESSAGE_SIZE);

            return 0;
        }
        else
        {
            iChar += len;
        }

        /* Get the start address of this packet */
        packet_p->startAddr = atoi(&recvData_p[iChar]);

        /* Get the end address of this packet */
        for ( ; iChar < contentStartPos; iChar++)
        {
            if (recvData_p[iChar] == '-')
            {
                break;
            }
        }
        if (iChar == contentStartPos)
        {
            TP_SetLastError(HTTPERR_WRONG_MESSAGE_SIZE);

            return 0;
        }
        else
        {
            iChar++;
        }

        packet_p->endAddr = atoi(&recvData_p[iChar]);

        /* Get the size of requested file */
        for ( ; iChar < contentStartPos; iChar++)
        {
            if (recvData_p[iChar] == '/')
            {
                break;
            }
        }
        if (iChar == contentStartPos)
        {
            TP_SetLastError(HTTPERR_WRONG_MESSAGE_SIZE);

            return 0;
        }
        else
        {
            iChar++;
        }
        fileSize = atoi(&recvData_p[iChar]);

        if (contentLength != (packet_p->endAddr - packet_p->startAddr + 1))
        {/* range is not consistent with length */
            TP_SetLastError(HTTPERR_WRONG_MESSAGE_SIZE);

            return 0;
        }
        else
        {
            memset(packet_p->content, 0x00, DEMO_HTTP_PACKET_SIZE);
            memcpy(packet_p->content, &recvData_p[contentStartPos], contentLength);

            return fileSize;
        }
    }
    else
    {
        TP_SetLastError(HTTPERR_RET_OTHERS);

        return 0;
    }
}

/******************************************************************************
* Overall Function
*******************************************************************************/
/**
 * Connect to the specified HTTP Server
 *
 * @author Zhang Lv
 *
 * @version 1.0
 *
 * @param ipAddr IP Address，by the order in Network Byte
 * @param ipPort Port Number，by the order in Network Byte
 *
 * @return TRUE，Connect to the server successfully<BR>
 *         FALSE，Connect to the server failed，use{@link #TP_GetLastError()}to get the error code,
 *                meaning for error code, see{@link #TP_HttpErrorCodeE}
 *
 * @date Feb 1st, 2012
 */
static uint32 curRemoteAddr = 0;
static uint32 curRemotePort = 0;
void debug(uint8* fmt,...);
void showerror(uint8* fmt,...);
Boolean Demo_HttpConnect(uint32 ipAddr, uint16 ipPort)
{
    TP_SockAddrT remoteHost;
    int32 result;

    curRemoteAddr = ipAddr;
    curRemotePort = ipPort;

    TP_SetLastError(0);
    if (HttpInfo.state == HTTP_CONNECTED)
    {
        TP_SetLastError(HTTPERR_CONNECTED);
        return FALSE;
    }

    result = TP_WlPPPLogin((uint8*)"cmnet", (uint8*)"\x00", (uint8*)"\x00", 0, 0, -1);
    if (result == 0)
    {/*PPPset link successfu*/
        HttpInfo.needExit = TRUE;
    }
    else if (result == -207)
    {/*PPPlink already exist*/
        HttpInfo.needExit = FALSE;
    }
    else
    {
        TP_SetLastError(HTTPERR_FAIL_PPP_DIAL);
        return FALSE;
    }

    /*Create SOCKET*/
    result = TP_NetSocket(AF_INET, NET_SOCK_STREAM, 0);
    if (result < 0)
    {
        if (HttpInfo.needExit == TRUE)
        {
            TP_WlPPPLogout();
        }

        TP_SetLastError(HTTPERR_FAIL_SOCKET_CREATE);
        return FALSE;
    }
    HttpInfo.socket = result;

    /*Start SOCKET Connection*/
    remoteHost.ipAddr = ipAddr;
    remoteHost.port   = ipPort;
    TP_NetIOCtl(HttpInfo.socket, CMD_TO_SET, 60000);
    result = TP_NetConnect(HttpInfo.socket, &remoteHost, 0);
    if (result < 0)
    {
        TP_NetCloseSocket(HttpInfo.socket);
        if (HttpInfo.needExit == TRUE)
        {
            TP_WlPPPLogout();
        }

        TP_SetLastError(HTTPERR_FAIL_SOCKET_CONNECT);
        return FALSE;
    }

    HttpInfo.state = HTTP_CONNECTED;

    return TRUE;
}


/**
 * Cut the Connection with HTTP Server
 *
 * @author Zhang Lv
 *
 * @version 1.0
 *
 * @return TRUE，Cut the connection successfully<BR>
 *         FALSE，No Connection
 *
 * @date Feb 1st, 2012
 */
Boolean Demo_HttpExit(void)
{
    TP_SetLastError(0);

    if (HttpInfo.state == HTTP_CONNECTED)
    {
        TP_NetCloseSocket(HttpInfo.socket);
        if (HttpInfo.needExit == TRUE)
        {
            TP_WlPPPLogout();
        }

        HttpInfo.state = HTTP_IDLE;
        return TRUE;
    }

    TP_SetLastError(HTTPERR_NOT_CONNECTED);

    return FALSE;
}

/**
 * Send message to the server (Used by business agreement implementer)
 *
 * @author Zhang Lv
 *
 * @param msg Message to send
 * @param msgLen Message length
 * @param timeout Timeout value
 * @param retyrs  Retry times
 *
 * @return >=0，number of bytes actually sent<BR>
 *         -1， msg is none<BR>
 *         -2， msgLen is 0<BR>
 *         -3， timeout is 0<BR>
 *         -4， network has been disconnected
 *
 * @date 20120215
 */
int32 TCP_SendMsg(uint16 socket, const char* msg, uint16 msgLen,
                       uint32 timeout, uint32 retrys)
{
    int32  result;
    uint32 readLen;
    uint32 sendLen;
    uint32  sendRetrys;
    static int32 time = 0;

    if (timeout == 0)
    {
        return -3;
    }
        if(socket < 0)
            return -1;

    if (msg == PNULL)
    {
        return -1;
    }

    if (msgLen == 0)
    {
        return -2;
    }


    readLen    = 0;
    sendLen    = 0;
    sendRetrys = 0;
    TP_NetIOCtl(socket, CMD_IO_SET, 1);
    TP_NetIOCtl(socket, CMD_TO_SET, timeout);
    while (readLen < msgLen)
    {
        if (sendRetrys > retrys)
        {/*exceed retry times*/
            break;
        }

        /*TCP一次最多能发薸5KB*/
        if (msgLen - readLen > 5120)
        {
            sendLen = 5120;
        }
        else
        {
            sendLen = msgLen - readLen;
        }

        result = TP_NetSend(socket, (uint8*)&msg[readLen], sendLen, 0);

        if (result <= 0)
        {/*fail to send*/
            if ((result == -50) ||
                (result == -53) ||
                (result == -54) ||
                (result == -61))
        {/* network has been disconnected*/
                return -4;
            }

            sendRetrys++;
        }
        else
        {
            if (result < sendLen)
            {/*the result is less than requested value ,timeout*/
                sendRetrys++;
            }

            readLen += result;
        }
    }

    if (readLen == 0)
    {/*timout to overtime */
        return -3;
    }
    else
    {
        return readLen;
    }
}

/**
 * Receive data from Server (Used by business agreement implementer)
 *
 * @author Zhang Lv
 *
 * @param buf_p  Buffer of received data
 * @param bufLen Length of expected to receive
 * @param timeout Timeout value
 * @param retrys  Retry times
 *
 * @return >=0，number of bytes actually received<BR>
 *         -1，buf_p is none<BR>
 *         -2，reqSize is 0<BR>
 *         -3，timeout is 0<BR>
 *         -4, network has been disconnected
 *
 * @date 20120215
 */

int32 TCP_RecvMsg(uint16 socket, char* buf_p, uint16 reqSize,
                  uint32 timeout, uint32 retrys)
{
    int32        result;
    uint32       recvedSize;
    uint32       recvSize;
    uint32       recvRetrys;

    if (timeout == 0)
    {
        return -3;
    }

    if (socket < 0)
        return -1;

    if (buf_p == PNULL)
    {
        return -1;
    }

    if (reqSize == 0)
    {
        return -2;
    }


    recvSize   = 0;
    recvedSize = 0;
    recvRetrys = 0;
    /*e柚梦亲枞Ｊ街欢a一次协襡栈給收缓存*/
    TP_NetIOCtl(socket, CMD_IO_SET, 0);
    TP_NetIOCtl(socket, CMD_TO_SET, timeout);
    while (recvedSize < reqSize)
    {
        if (recvRetrys > retrys)
        {/* exceed retry times */
            break;
        }

        recvSize = reqSize - recvedSize;

        if (TP_NetIOCtl(socket, CMD_EVENT_GET, SOCK_EVENT_READ) ==
            SOCK_EVENT_READ)
        {/* received data before timeout*/
            result = TP_NetRecv(socket, (uint8*)&buf_p[recvedSize], recvSize, 0);
        }
        else
        {
            result = -60;
        }

        if (result < 0)
        {
            if ((result == -50) ||
                (result == -53) ||
                (result == -54) ||
                (result == -61))
            {/* network has been disconnected */
                return -4;
            }

            recvRetrys++;
        }
        else
        {
            if (result == 0)
            {
                recvRetrys++;
            }

            recvedSize += result;

            if (recvedSize > 0)
            {
                break;
            }
        }
    }

    return recvedSize;
}

/**
 * HTTP Get<BR>
 * the message content of Get Return saved to packet_p，the message content length is（endAddr - startAddr + 1）
 *
 * @author Zhang Lv
 *
 * @param packet_p Http receive message content pack
 * @param httpAddr Http Address
 * @param httpAddrLen Http Address length
 *
 * @return >0，size of Get Request object<BR>
 *         =0，Get failed，Use{@link #TP_GetLastError()}to receive
 *             error code，meaning of error code, see{@link #TP_HttpErrorCodeE}
 * @date Feb 1st, 2012
 */
static char sendPacket[DEMO_HTTP_PACKET_SIZE + 512];
static char getMsg[512];
uint32 Demo_HttpGet(Demo_HttpRecvPacketT* packet_p,
                    const char* httpAddr, uint16 httpAddrLen)
{
    int32  result;
    int32  sendSize;
    int32  recvSize;
    int32  tryTime;
    uint32 fileSize;

    TP_SetLastError(0);

    if (HttpInfo.state != HTTP_CONNECTED)
    {
        TP_SetLastError(HTTPERR_NOT_CONNECTED);

        return 0;
    }


    /*set GET request*/
    memset(getMsg, 0x00, sizeof(getMsg));
    sendSize = getGETMessage(getMsg,
                             packet_p->startAddr, packet_p->endAddr,
                             httpAddr, httpAddrLen);

    if (sendSize == 0)
    {
        TP_SetLastError(HTTPERR_BAD_ADDRESS);

        return 0;
    }

    tryTime = 0;
    while (tryTime < 3)
    {
        /*Clear receive buffer*/
        while (TCP_RecvMsg(HttpInfo.socket,
                           (char*)sendPacket, DEMO_HTTP_PACKET_SIZE + 512,
                           100, 0) > 0);

        /*send GET request*/
        result = TCP_SendMsg(HttpInfo.socket, (char*)getMsg, sendSize, 9000, 0);

        if (result < sendSize)
        {
            TP_SetLastError(HTTPERR_NET);

            if (result == -4)
            {
                break;
            }

            TP_TimerSet(1, 1000);
            while (TP_TimerCheck(1) != 0);

            tryTime++;
        }
        else
        {
            break;
        }
    }
    if (tryTime == 3)
    {
        return 0;
    }

    tryTime  = 0;
    fileSize = 0;
    recvSize = 0;
    memset(sendPacket, 0x00, sizeof(sendPacket));
    while (TRUE)
    {
        sendSize = TCP_RecvMsg(HttpInfo.socket,
                               &sendPacket[recvSize],
                               DEMO_HTTP_PACKET_SIZE + 512, 1000, 30);
        TP_DbgSerialPrn("sendSize[%d]\r\n", sendSize);

        if (sendSize <= 0)
        {
            TP_SetLastError(HTTPERR_NET);

            break;
        }
        else
        {
            recvSize += sendSize;

            TP_DbgSerialPrn("recvSize[%d]\r\n", recvSize);

            /*get the message content and property from the server response*/
            fileSize = getContentFromPacket(packet_p, sendPacket, recvSize);

            TP_DbgSerialPrn("fileSize[%d]\r\n", fileSize);

            if (fileSize > 0)
            {
                break;
            }
            else
            {
                if (TP_GetLastError() != HTTPERR_WRONG_MESSAGE_SIZE)
                {
                    break;
                }
            }
        }
    }

    return fileSize;
}


/**
 * Http Process Initialization
 *
 * @author Zhang Lv
 *
 * @version 1.0
 *
 * @date Feb 2nd, 2012
 */
void Demo_HttpInit(void)
{
    memset((uint8*)&HttpInfo, 0x00, sizeof(ST_DemoHttpInfo));
    HttpInfo.state    = HTTP_IDLE;
    HttpInfo.needExit = FALSE;
}

/*End Of File*/
