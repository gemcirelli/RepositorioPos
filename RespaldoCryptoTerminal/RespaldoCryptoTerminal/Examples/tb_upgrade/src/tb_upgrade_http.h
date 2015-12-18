#ifndef TB_UPGRADE_HTTP_H
#define TB_UPGRADE_HTTP_H

#define TB_UPGRADE_MAX_HOST_IP_LEN   15
#define TB_UPGRADE_MAX_HOST_NAME_LEN 63
#define TB_UPGRADE_MAX_RES_PATH_LEN  255

typedef struct __HttpContext
{
    uint8*  pAllocBuf;
    uint32  sizeAllocBuf;
    int32   sockdes;
    uint16  hostPort;
    char    hostIP[TB_UPGRADE_MAX_HOST_IP_LEN + 1];
    char    hostName[TB_UPGRADE_MAX_HOST_NAME_LEN + 1];
    char    path[TB_UPGRADE_MAX_RES_PATH_LEN + 1];
} TB_Upgrade_HttpContextT;

typedef struct __HttpResponse
{
    uint8* pHeader;
    uint8* pBody;
    uint32 fileSize;
    uint32 contentRangeStart;
    uint32 contentRangeEnd;
    uint16 statusCode;
} TB_Upgrade_HttpResponseT;

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
extern uint8
tb_upgrade_http_Create(const char* pURLStr, TB_Upgrade_HttpContextT** pHttpContextPointer);

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
extern uint8
tb_upgrade_http_Connect(TB_Upgrade_HttpContextT* pHttpContext);

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
extern uint8
tb_upgrade_http_WriteGET(TB_Upgrade_HttpContextT* pHttpContext,
                         uint32 contentRangeStart, uint32 contentRangeEnd);

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
extern uint8
tb_upgrade_http_ReadResponse(TB_Upgrade_HttpContextT* pHttpContext,
                             TB_Upgrade_HttpResponseT* pResponseBuf);

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
extern uint8
tb_upgrade_http_DisConnect(TB_Upgrade_HttpContextT* pHttpContext);

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
extern void
tb_upgrade_http_Destroy(TB_Upgrade_HttpContextT** pHttpContextPointer);

#endif
