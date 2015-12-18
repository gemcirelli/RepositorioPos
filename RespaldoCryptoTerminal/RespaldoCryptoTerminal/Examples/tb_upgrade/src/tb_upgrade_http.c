#include <TPTypedef.h>
#include <TPPPP.h>
#include <TPNet.h>
#include <TPNetwork.h>
#include <TPBase.h>

#include "./tb_upgrade_http.h"

/******************************************************************************
* Manifest Constants
*******************************************************************************/
#define MAX_HEADER_LINES 20

/******************************************************************************
* Constants
*******************************************************************************/
static const char GETHeaderFmt[] =
{
    "GET %s HTTP/1.1\r\n"
    "Accept: */*\r\n"
    "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n"
    "RANGE: bytes=%d-%d\r\n"
    "Connection: Keep-Alive\r\n"
    "Host: %s:%d\r\n"
    "\r\n"
};

/******************************************************************************
* Private Functions
*******************************************************************************/
/**
 *
 *
 * @author zhangl
 *
 * @version 1.0
 *
 * @param pURLStr
 * @param pHttpContext
 *
 * @return  0x00, Parsed succeed<BR>
 *          0x01, Scheme is not HTTP<BR>
 *          0x02, Host name invalid<BR>
 *          0x03, Host port invalid<BR>
 *          0x04, Resource path invalid<BR>
 *
 *
 * @date 2012-11-29
 */
static uint8 httpParseURL(const char* pURLStr, TB_Upgrade_HttpContextT* pHttpContext)
{
    char  scheme[10];
    uint8 nDots;
    const char *p, *q;
    int   i;
    TP_SockAddrT sockAddr;

    /* Scheme Name */
    p = strstr(pURLStr, ":/");
    if (p != PNULL)
    {
        _snprintf(scheme, sizeof(scheme),
                  "%.*s", (int)(p - pURLStr), pURLStr);

        for (i = 0; i < strlen(scheme); i++)
        {
            scheme[i] = toupper(scheme[i]);
        }

        if (memcmp(scheme, "HTTP", 4) != 0)

        {
            return 0x01;
        }

        p += 2;
        /*
         * Only one slash: no host, leave slash as part of document
         * Two slashes: host follows, strip slashes
         */
        if (p[0] == '/')
        {
            p++;
        }
        else
        {
            return 0x02;
        }
    }
    else
    {
        p = pURLStr;
    }

    if (!*p || *p == '/' || *p == '.')
    {
        return 0x01;
    }

    /* Host Name */
    memset(pHttpContext->hostName, 0x00, TB_UPGRADE_MAX_HOST_NAME_LEN + 1);
    for (i = 0; (*p) && (*p != '/') && (*p != ':'); p++)
    {
        if (i < TB_UPGRADE_MAX_HOST_NAME_LEN)
        {
            pHttpContext->hostName[i++] = *p;
        }
        else
        {
            return 0x02;
        }
    }


    /* Host Port */
    pHttpContext->hostPort = 80;

    if (*p == ':')
    {
        pHttpContext->hostPort = 0;
        for (q = ++p; *q && (*q != '/'); q++)
        {
            if (isdigit(*q))
            {
                pHttpContext->hostPort = pHttpContext->hostPort * 10 + (*q - '0');
            }
            else
            {
                /* invalid port */
                return 0x03;
            }
        }
        p = q;
    }

    /* Resource Path */
    if (*p == '/')
    {
        for (i = 0; *p; p++)
        {
            if (i < TB_UPGRADE_MAX_RES_PATH_LEN)
            {
                pHttpContext->path[i++] = *p;
            }
            else
            {
                return 0x04;
            }
        }
    }
    else
    {
        return 0x04;
    }

    if (TP_SockAddrSet(&sockAddr, (uchar*)pHttpContext->hostName, pHttpContext->hostPort) ==
        NET_OK)
    {
        strcpy(pHttpContext->hostIP, pHttpContext->hostName);
    }
    else
    {
        pHttpContext->hostIP[0] = 0;
    }

    return 0x00;
}


static void httpSetVariable(TB_Upgrade_HttpResponseT* pResponse, char* pName, char* pValue)
{
    int32 contentRangeStart,contentRangeEnd,fileSize;
    if (strcmp(pName, "Content-Range") == 0)
    {
        sscanf(pValue, "bytes %d-%d/%d",
               &contentRangeStart,
               &contentRangeEnd,
               &fileSize);
        pResponse->contentRangeStart = contentRangeStart;
        pResponse->contentRangeEnd   = contentRangeEnd;
        pResponse->fileSize          = fileSize;
    }
}


static int32 httpReadLine(int32 sock, char* pLineBuf, int32 sizeLineBuf)
{
    char c;
    int  i = 0;
    int32 rc;

    TP_NetIOCtl(sock, CMD_IO_SET, 1);
    TP_NetIOCtl(sock, CMD_TO_SET, 30 * 1000);
    while (i < sizeLineBuf)
    {
        rc = TP_NetRecv(sock, (uint8*)&c, 1, 0);
        if (rc <= 0)
        {
            return rc;
        }

        pLineBuf[i++]=c;

        if (c == '\r')  // GOT CR
        {
            rc = TP_NetRecv(sock, (uint8*)&c, 1, 0);
            if (rc <= 0)
            {
                return rc;
            }

            pLineBuf[i++]=c;

            break ;
        }
    }

    if (i >= sizeLineBuf)
    {
        return 0;
    }

    pLineBuf[i] = 0;

    return i;
}


static int32 httpReadHeader(int32 sock, char* pHeaderBuf, int32 sizeHeaderBuf)
{
    char buffer[1024];
    int rc, bytes;

    bytes = 0;
    for (;;)
    {
        rc = httpReadLine(sock, buffer, 1023);
        if (rc <= 0)
        {
            return rc;
        }
        buffer[rc] = 0;

        strcat(pHeaderBuf, buffer);
        bytes += rc;

        if (buffer[0] == '\r' && buffer[1] == '\n')
        {
            break;
        }
    }

    return bytes;
}


static int32 httpReadBody(int32 sock, char* pBodyBuf, int32 sizeBody)
{
    TP_NetIOCtl(sock, CMD_IO_SET, 1);
    TP_NetIOCtl(sock, CMD_TO_SET, 30 * 1000);

    return TP_NetRecv(sock, (uint8*)pBodyBuf, sizeBody, 0);
}


static int32 httpSplitHeader(char* pHeader, int32 lenHeader, char** pLinesBuf)
{
    int lines = 0;
    unsigned long i;

    pLinesBuf[lines] = pHeader;
    for (i = 0; i < lenHeader && lines < MAX_HEADER_LINES; i++)
    {
        if (pHeader[i] == '\r')
        {
            pHeader[i] = '\0';
        }

        if (pHeader[i] == '\n')
        {
            lines++;
            pHeader[i] = '\0';

            if (lines >= MAX_HEADER_LINES)
            {
                return MAX_HEADER_LINES;
            }

            if ((i + 1) < lenHeader)
            {
                if (pHeader[i + 1] == '\n' || pHeader[i + 1] == '\r')
                {
                    break;
                }

                pLinesBuf[lines] = &pHeader[i + 1];
            }
        }
    }

    return lines;
}


static int32 httpParseHeaders(TB_Upgrade_HttpResponseT* pResponse, char** line, int lines)
{
    int i,l;
    int whitespace, where, slen;
    char* name  = PNULL;
    char* value = PNULL;

    /* parse the name: value lines. */
    for (l = 1; l < lines; l++)
    {
        where      = 0;
        whitespace = 0;
        name       = line[l];
        value      = PNULL;
        slen = strlen(line[l]);
        for (i = 0; i < slen; i++)
        {
            if (line[l][i] == ':')
            {
                whitespace = 1;
                line[l][i] = '\0';
            }
            else
            {
                if (whitespace != 0)
                {
                    whitespace = 0;
                    while (i < slen && line[l][i] == ' ')
                    {
                        i++;
                    }

                    if (i < slen)
                    {
                        value = &line[l][i];
                    }

                    break;
                }
            }
        }

        if (name != PNULL && value != PNULL)
        {
            httpSetVariable(pResponse, name, value);
            name  = PNULL;
            value = PNULL;
        }
    }
}

/******************************************************************************
* Public Functions
*******************************************************************************/
/**
 * Create HTTP context to use according to specific URL
 *
 * @author zhangl
 *
 * @version 2.0
 *
 * @param pURLStr URL in ASCII string
 * @param pHttpContextPointer Pointer to allocated HTTP context
 *
 * @return 0x00, Create succeed<BR>
 *         0x01, Scheme of URL not HTTP<BR>
 *         0x02, Host name of URL invalid<BR>
 *         0x03, Host port of URL invalid<BR>
 *         0x04, Resource path of URL invalid<BR>
 *         0xF0, <code>pURLStr</code> invalid<BR>
 *         0xF1, <code>pHttpContextPointer</code> not null <BR>
 *         0xFB, System memory overflow
 *
 * @date 2012-11-29
 */
uint8 tb_upgrade_http_Create(const char* pURLStr, TB_Upgrade_HttpContextT** pHttpContextPointer)
{
    TB_Upgrade_HttpContextT* pHttpContext;
    uint8 result;

    if ((pURLStr         == PNULL) ||
        (strlen(pURLStr) <  8))
    {
        return 0xF0;
    }

    if (*pHttpContextPointer != PNULL)
    {
        return 0xF1;
    }

    TP_AllocMemory(sizeof(TB_Upgrade_HttpContextT), (void**)pHttpContextPointer);
    if (*pHttpContextPointer == PNULL)
    {
        return 0xFB;
    }

    pHttpContext = *pHttpContextPointer;
    memset((void*)pHttpContext, 0x00, sizeof(TB_Upgrade_HttpContextT));

    pHttpContext->sockdes = -1;

    pHttpContext->pAllocBuf = PNULL;
    TP_AllocMemory(5 * 1024 + 512, (void**)&(pHttpContext->pAllocBuf));
    if (pHttpContext->pAllocBuf == PNULL)
    {
        tb_upgrade_http_Destroy(pHttpContextPointer);

        return 0xFB;
    }
    pHttpContext->sizeAllocBuf = 5 * 1024 + 512;

    result = httpParseURL(pURLStr, pHttpContext);
    if (result != 0x00)
    {
        tb_upgrade_http_Destroy(pHttpContextPointer);

        return result;
    }

    return 0x00;
}


/**
 * Destroy allocated HTTP context
 *
 * @author zhangl
 *
 * @version 2.0
 *
 * @param pHttpContextPointer Pointer to allocated HTTP context
 *
 * @date 2012-11-29
 */
void tb_upgrade_http_Destroy(TB_Upgrade_HttpContextT** pHttpContextPointer)
{
    TB_Upgrade_HttpContextT* pHttpContext = *pHttpContextPointer;

    if (pHttpContext == PNULL)
    {
        return;
    }

    if (pHttpContext->sockdes != -1)
    {
        tb_upgrade_http_DisConnect(pHttpContext);
    }

    if (pHttpContext->pAllocBuf != PNULL)
    {
        TP_FreeMemory((void**)&(pHttpContext->pAllocBuf));
    }

    TP_FreeMemory((void**)pHttpContextPointer);
}


/**
 * Connect to HTTP server
 *
 * @author zhangl
 *
 * @version 2.0
 *
 * @param pHttpContext Allocated HTTP context
 *
 * @return 0x00, Connected<BR>
 *         0x01, Connected already<BR>
 *         0x02, DNS resolve host name failed<BR>
 *         0x03, Socket creat faild<BR>
 *         0x04, Connect failed<BR>
 *         0xF0, HTTP context invalid<BR>
 *         0xFF, PPP link not available
 *
 * @date 2012-11-29
 */
uint8 tb_upgrade_http_Connect(TB_Upgrade_HttpContextT* pHttpContext)
{
    TP_SockAddrT hostAddr;
    int32 retVal;
    uint8 iDNS;

    if (pHttpContext == PNULL)
    {
        return 0xF0;
    }

    if (pHttpContext->sockdes != -1)
    {
        return 0x01;
    }

    if (TP_WlPPPCheck() != WL_RET_OK)
    {
        return 0xFF;
    }
    else if (TP_NetworkGetStatus() != SRVCSTATUS_REGISTERED)
    {
        return 0xFF;
    }

    if (strlen(pHttpContext->hostIP) == 0)
    {
        TP_DNSTimeOut(10 * 1000);
        for (iDNS = 0; iDNS < 3; iDNS++)
        {
            if (TP_DNSResolve((uchar*)pHttpContext->hostName,
                              (uchar*)pHttpContext->hostIP,
                              TB_UPGRADE_MAX_HOST_IP_LEN + 1) == NET_OK)
            {
                break;
            }
        }

        if (iDNS == 3)
        {
            return 0x02;
        }
    }

    pHttpContext->sockdes = TP_NetSocket(AF_INET, NET_SOCK_STREAM, 0);
    if (pHttpContext->sockdes <  0)
    {
        pHttpContext->sockdes = -1;

        return 0x03;
    }

    TP_SockAddrSet(&hostAddr, (uchar*)pHttpContext->hostIP, pHttpContext->hostPort);
    TP_NetIOCtl(pHttpContext->sockdes, CMD_IO_SET, 1);
    TP_NetIOCtl(pHttpContext->sockdes, CMD_TO_SET, 60 * 1000);
    retVal = TP_NetConnect(pHttpContext->sockdes,
                           &hostAddr, sizeof(TP_SockAddrT));
    if (retVal != NET_OK)
    {
        TP_NetCloseSocket(pHttpContext->sockdes);

        pHttpContext->sockdes = -1;

        return 0x04;
    }

    return 0x00;
}


/**
 * Send GET request to HTTP server
 *
 * @author zhangl
 *
 * @version 1.0
 *
 * @param pHttpContext Allocated HTTP context
 * @param contentRangeStart Start address of require content
 * @param contentRangeEnd End address of require content
 *
 * @return 0x00, Send succeed<BR>
 *         0x01, Server not connected<BR>
 *         0x02, Socket error occured<BR>
 *         0xF0, HTTP context invalid<BR>
 *         0xF1, <code>contentRangeEnd</code> invalid<BR>
 *         0xFB, System memory overflowed<BR>
 *
 *
 * @date 2012-11-29
 */

uint8 tb_upgrade_http_WriteGET(TB_Upgrade_HttpContextT* pHttpContext,
                           uint32 contentRangeStart, uint32 contentRangeEnd)
{
    int32  retVal;
    uint16 sendSize;

    if (pHttpContext == PNULL)
    {
        return 0xF0;
    }

    if (contentRangeEnd <= contentRangeStart)
    {
        return 0xF1;
    }

    if (pHttpContext->sockdes == -1)
    {
        return 0x01;
    }

    if ((contentRangeEnd - contentRangeStart) > (pHttpContext->sizeAllocBuf - 512))
    {
        TP_FreeMemory((void**)&(pHttpContext->pAllocBuf));
        TP_AllocMemory(contentRangeEnd - contentRangeStart + 512,
                       (void**)&(pHttpContext->pAllocBuf));
        if (pHttpContext->pAllocBuf == PNULL)
        {
            return 0xFB;
        }
        pHttpContext->sizeAllocBuf = contentRangeEnd - contentRangeStart + 512;
    }
    memset(pHttpContext->pAllocBuf, 0x00, pHttpContext->sizeAllocBuf);

    /** Clear the received data first */
    TP_NetIOCtl(pHttpContext->sockdes, CMD_IO_SET, 1);
    TP_NetIOCtl(pHttpContext->sockdes, CMD_TO_SET, 100);
    while (TP_NetRecv(pHttpContext->sockdes, (uint8*)&sendSize, 1, 0) == 1)
    {
        /** receive and receive again */
    }

    sprintf((char*)pHttpContext->pAllocBuf,
            GETHeaderFmt,
            pHttpContext->path,
            contentRangeStart, contentRangeEnd,
            pHttpContext->hostName,
            pHttpContext->hostPort);
    sendSize = strlen((char*)pHttpContext->pAllocBuf);

    TP_NetIOCtl(pHttpContext->sockdes, CMD_IO_SET, 1);
    TP_NetIOCtl(pHttpContext->sockdes, CMD_TO_SET, 30 * 1000);
    retVal = TP_NetSend(pHttpContext->sockdes,
                        pHttpContext->pAllocBuf, sendSize,
                        0);

    if (retVal != sendSize)
    {
        if ((retVal == NET_ERR_CONNABORTED) ||
            (retVal == NET_ERR_CONNRESET))
        {
            tb_upgrade_http_DisConnect(pHttpContext);
        }

        return 0x02;
    }

    return 0x00;
}


/**
 * Get the response from HTTP server
 *
 * @author zhangl
 *
 * @version 1.0
 *
 * @param pHttpContext Allocated HTTP context
 * @param pResponseBuf Output buffer of response
 *
 * @return 0x00, Get succeed<BR>
 *         0x01, Server not connected<BR>
 *         0x02, Socket error occured<BR>
 *         0x03, Content of response is not HTTP message<BR>
 *         0x04, Server return error<BR>
 *
 * @date 2012-11-29
 */
uint8
tb_upgrade_http_ReadResponse(TB_Upgrade_HttpContextT* pHttpContext,
                             TB_Upgrade_HttpResponseT* pResponseBuf)
{
    int32  retVal;
    int32  lenHeader;
    char*  line[MAX_HEADER_LINES];
    int lines, slen, i, whitespace = 0, where = 0, code;
    int cont_len;
    char* version = PNULL, *resp_code = PNULL, *message = PNULL;

    if (pHttpContext == PNULL)
    {
        return 0xF0;
    }

    if (pResponseBuf == PNULL)
    {
        return 0xF1;
    }

    if (pHttpContext->sockdes == -1)
    {
        return 0x01;
    }

    memset((void*)pHttpContext->pAllocBuf, 0x00, pHttpContext->sizeAllocBuf);

    lenHeader = httpReadHeader(pHttpContext->sockdes,
                               (char*)pHttpContext->pAllocBuf, pHttpContext->sizeAllocBuf);
    if (lenHeader <= 0)
    {
        if (lenHeader == NET_ERR_CONNABORTED ||
            lenHeader == NET_ERR_CONNRESET)
        {
            tb_upgrade_http_DisConnect(pHttpContext);
        }

        return 0x02;
    }

    lines = httpSplitHeader((char*)pHttpContext->pAllocBuf, lenHeader, line);

    slen = strlen(line[0]);
    version = line[0];
    for (i = 0; i < slen; i++)
    {
        if (line[0][i] == ' ')
        {
            line[0][i] = 0;
            whitespace = 1;
        }
        else if (whitespace != 0)
        {
            whitespace = 0;
            where++;
            if (where == 1)
            {
                resp_code = &line[0][i];
            }
            else
            {
                message = &line[0][i];
                break;
            }
        }
    }


    if (version == PNULL || resp_code == PNULL || message == PNULL)
    {
        return 0x03;
    }

    code = atoi(resp_code);
    pResponseBuf->statusCode = code;
    if (code < 200 || code >= 300)
    {
        return 0x04;
    }

    httpParseHeaders(pResponseBuf, line, lines);
    cont_len = pResponseBuf->contentRangeEnd - pResponseBuf->contentRangeStart + 1;

    retVal = httpReadBody(pHttpContext->sockdes,
                          (char*)&pHttpContext->pAllocBuf[lenHeader], cont_len);

    if (retVal != cont_len)
    {
        return 0x02;
    }

    pResponseBuf->pHeader = pHttpContext->pAllocBuf;
    pResponseBuf->pBody   = &pHttpContext->pAllocBuf[lenHeader];

    return 0x00;
}


/**
 * Disconnect from HTTP server
 *
 * @author zhangl
 *
 * @version 1.0
 *
 * @param pHttpContext
 *
 * @return 0x00, Disconnect succeed<BR>
 *         0x01, Disconnected already<BR>
 *
 * @date 2012-11-29
 */
uint8 tb_upgrade_http_DisConnect(TB_Upgrade_HttpContextT* pHttpContext)
{
    if (pHttpContext == PNULL)
    {
        return 0xF0;
    }

    if (pHttpContext->sockdes == -1)
    {
        return 0x01;
    }

    TP_NetCloseSocket(pHttpContext->sockdes);

    pHttpContext->sockdes = -1;

    return 0x00;
}
