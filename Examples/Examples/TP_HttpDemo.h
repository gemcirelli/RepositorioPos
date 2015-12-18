#ifndef DEMO_HTTP_H
#define DEMO_HTTP_H

/** Http Http Max Address Length*/
#define DEMO_HTTP_MAX_ADDR_LENGTH      512
/** Http Demo Packet Size*/
#define DEMO_HTTP_PACKET_SIZE          (1024 * 5)

/** Http Demo Receive Packet Tag*/
typedef struct DemoHttpRecvPacketTag
{
    /** Start Address of Data*/
    uint32  startAddr;
    /** End Address of Data*/
    uint32  endAddr;
    /** Data Content*/
    uint8   content[DEMO_HTTP_PACKET_SIZE];
} Demo_HttpRecvPacketT;

/** HTTP Error Code:<BR>
 *  -11 GPRS Dial Error<BR>
 *  -10 SOCKET Creation Fail<BR>
 *  -9  SOCKET Connection Fail<BR>
 *  -8  No Connection to the server<BR>
 *  -7  Already Connected to server<BR>
 *  -6  Http Address Error<BR>
 *  -5  Network Connection Error<BR>
 *  -4  Server Return404<BR>
 *  -3  Server Return416<BR>
 *  -2  Server Return others Error<BR>
 *  -1  Message Error*/
typedef enum DemoHttpErrorCodeTag
{
    /** GRPS Dial Error*/
    HTTPERR_FAIL_PPP_DIAL = -11,
    /** SOCKET Creation Fail*/
    HTTPERR_FAIL_SOCKET_CREATE,
    /** SOCKET Connection Fail*/
    HTTPERR_FAIL_SOCKET_CONNECT,
    /** No Connection to the server*/
    HTTPERR_NOT_CONNECTED,
    /** Already Connected to server*/
    HTTPERR_CONNECTED,
    /** Http Address Error*/
    HTTPERR_BAD_ADDRESS,
    /** Network Connection Error*/
    HTTPERR_NET,
    /** Server Return404*/
    HTTPERR_RET_404,
    /** Server Return416*/
    HTTPERR_RET_416,
    /** Server Return others Error*/
    HTTPERR_RET_OTHERS,
    /** Message Error*/
    HTTPERR_WRONG_MESSAGE_SIZE
} Demo_HttpErrorCodeE;

/**
 * Http Process Initialization
 *
 * @author Zhang Lv
 *
 * @version 1.0
 *
 * @date Feb 2nd, 2012
 */
extern void
Demo_HttpInit(void);

/**
 * Connect to the specified HTTP Server
 *
 * @author Zhang Lv
 *
 * @version 1.0
 *
 * @param ipAddr IP Address£¬by the order in Network Byte
 * @param ipPort Port Number£¬by the order in Network Byte
 *
 * @return TRUE£¬Connect to the server successfully<BR>
 *         FALSE£¬Connect to the server failed£¬use{@link #TP_GetLastError()}to get the Error code,
 *                meaning for Error code, see{@link #TP_HttpErrorCodeE}
 *
 * @date Feb 1st, 2012
 */
extern Boolean
Demo_HttpConnect(uint32 ipAddr, uint16 ipPort);

/**
 * HTTP Get<BR>
 * the message content of Get Return saved to packet_p£¬the message content length is£¨endAddr - startAddr + 1£©
 *
 * @author Zhang Lv
 *
 * @param packet_p Http receive message content pack
 * @param httpAddr Http Address
 * @param httpAddrLen Http Address length
 *
 * @return >0£¬size of Get Request object<BR>
 *         =0£¬Get failed£¬Use{@link #TP_GetLastError()}to receive
 *             Error code£¬meaning of Error code, see{@link #TP_HttpErrorCodeE}
 * @date Feb 1st, 2012
 */
extern uint32
Demo_HttpGet(Demo_HttpRecvPacketT* packet_p,
             const char* httpAddr, uint16 httpAddrLen);

/**
 * Cut the Connection with HTTP Server
 *
 * @author Zhang Lv
 *
 * @version 1.0
 *
 * @return TRUE£¬Cut the connection successfully<BR>
 *         FALSE£¬No Connection
 *
 * @date Feb 1st, 2012
 */
extern Boolean
Demo_HttpExit(void);

#endif/*Include Guard End*/
/*End Of File*/
