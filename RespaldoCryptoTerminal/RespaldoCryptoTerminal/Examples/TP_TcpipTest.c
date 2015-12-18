#include <TPTypedef.h>
#include <TPBase.h>
#include <TPLCD.h>
#include <TPKeyPad.h>
#include <TPPPP.h>
#include <TPNet.h>
#include <TPNetwork.h>
#include <TP_Singleime.h>
#include <TPSerial.h>
#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"

static char    AccessPointName[45] = "cmnet\x00";
static char    AccessPassword[19]   = "\x00";
static char    AccessUserName[19]   = "\x00";

static char    TcpServerIp[16]     = "121.9.230.130\x00";
static char    TcpServerPort[6]    = "7012\x00";
static int32   TcpUseSocket        = -1;
static Boolean TcpSocketValid      = FALSE;

static char    UdpHostIp[16];
static char    UdpHostPort[6];

static int32   DemoUseSocket;
static uint32  DemoRunningCounts;


Boolean TP_CheckIPAddr(uint8 *ipAddr)
{
    uint8 tempi = 0;
    uint8 RadioCount = 0,NumCount = 0;
    uint8 IsErr = FALSE;
    uint8 Addrlen = 0;

    if(ipAddr == PNULL)
    {
        return TRUE;
    }
    Addrlen = strlen((char *)ipAddr);
    if(Addrlen<7 || Addrlen >15)
    {
        return TRUE;
    }
    for(tempi = 0;tempi<Addrlen;tempi++)
    {
        if(ipAddr[tempi]=='.' || (ipAddr[tempi]>='0' && ipAddr[tempi]<='9'))
        {
            continue;
        }
        else
        {
            return TRUE;
        }
    }
    //check if the data is available
    for(tempi = 0;tempi<Addrlen;tempi++)
    {
        if(ipAddr[tempi]=='.')
        {
            RadioCount++;
            //the first number should not be 0
            if(RadioCount ==1
                && (NumCount == 0 || (NumCount==1 && ipAddr[tempi-1]=='0') ))
            {
                NumCount = 0;
                IsErr = TRUE;
                break;
            }
            if(RadioCount == 3 && tempi == Addrlen-1)
            {
                NumCount = 0;
                IsErr = TRUE;
                break;
            }
            NumCount = 0;
        }
        else
        {
            NumCount++;
            if(NumCount == 3)//check if exceed 255
            {
                if(ipAddr[tempi-2]>'2'
                    || (ipAddr[tempi-2]=='2' && ipAddr[tempi-1]>'5')
                    || (ipAddr[tempi-2]=='2' && ipAddr[tempi-1]=='5' && ipAddr[tempi]>'5')  )
                {
                    IsErr = TRUE;
                    break;
                }
            }
        }
        if(NumCount>3 || RadioCount>3)
        {
            IsErr = TRUE;
            break;
        }
    }
    if(RadioCount != 3 )
    {
        IsErr = TRUE;
    }
    return IsErr;
}

Boolean TP_TcpipTest(uint16 itemId)
{
    TP_SockAddrT remoteHost;
    uint16      portNumber;
    uint8       inputString[100];
    uint8       outputString[100];
    int32       result;
    int32       udpSocket;
    Boolean     loop;
    uint8       sendData[5121];
    uint8       wlerrCounts;
    uint32      pppRetrys;
    uint32      connectRetrys;
    uint32      sendRetrys;
    uint32      closeRetrys;
    uint8       keycode;
    uint8       tempIP[16];
    TP_DisplayInfo displayInfo =
    {
        (uint8*)"Received Data:",
        ALIGN_MIDDLE,
        (uint8*)"Confirm",
        (uint8*)"Cancel",
        KEY_DEMO_CONFIRM,
        KEY_DEMO_CANCEL,
        TP_KEY_NONE,
        TP_KEY_NONE,
        CFONT
    };
    TP_ListBoxRect rect={0,0,127,63};

    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127, 63);

    switch (itemId)
    {
    case 163://DNS Resolve
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Input Hostname");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 16, 125, 35);
        TP_SetDisplayArea(4, 17, 124, 34);
        TP_ScrGotoxyEx(4, 17);
        memset(inputString, 0x00, sizeof(inputString));
        sprintf((char*)inputString, "www.sina.com.cn\x00");
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        result = TP_GetHzString((char *)inputString, 0, 40);
        TP_SetDisplayArea(0, 0, 127, 63);
        TP_ScrFontSet(CFONT);
        if (result == 0xFF)
        {
            break;
        }

        memset(outputString, 0x00, 100);
        TP_DNSServer(0, "8.8.8.8", 53);
        TP_DNSServer(1, "8.8.4.4", 53);
        TP_DNSTimeOut(3000);
        sendRetrys = 0;
        while (sendRetrys < 10)
        {
            result = TP_DNSResolve(inputString, outputString, 100);
            if (result == 0)
            {
                break;
            }
            else
            {
                sendRetrys++;
            }
        }

        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        if (result == 0)
        {
            TP_ScrGotoxy(3, 0);
            TP_LcdPrintf((uint8*)"Return IP:");

            TP_ScrGotoxy(3, 2);
            TP_LcdPrintf((uint8*)"%s", outputString);
        }
        else
        {
            TP_ScrGotoxy(3, 0);
            TP_LcdPrintf((uint8*)"DNS resolve error:%d", result);
        }

        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Press any key return");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                break;
            }
        }
        break;

    case 164://Ping
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Input Hostname or IP");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 16, 125, 35);
        TP_SetDisplayArea(4, 17, 124, 34);
        TP_ScrGotoxyEx(4, 17);
        memset(inputString, 0x00, sizeof(inputString));
        sprintf((char*)inputString, "www.sina.com.cn\x00");
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        TP_SetInputMode(INPUTMODE_LOWCASE);
        result = TP_GetHzString((char*)inputString, 0, 40);
        TP_SetDisplayArea(0, 0, 127, 63);
        TP_ScrFontSet(CFONT);
        if (result == 0xFF)
        {
            break;
        }

        do
        {
            TP_ScrCls();
            TP_ScrDrawRect(0, 0, 127, 63);//draw border
            TP_ScrGotoxy(3, 0);
            TP_LcdPrintf((uint8*)"Ping with 32bytes of data");
            result = TP_NetPing(inputString, 1000, 32);
            TP_ScrGotoxy(3, 4);
            if (result >= 0)
            {
                TP_LcdPrintf((uint8*)"Reply time = %dms", result);
            }
            else
            {
                TP_LcdPrintf((uint8*)"error = %d", result);
            }

            TP_ScrGotoxy(3, 6);
            TP_LcdPrintf((uint8*)"Again");
            TP_ScrGotoxy(88, 6);
            TP_LcdPrintf((uint8*)"Cancel");

            while (TRUE)
            {
                if (TP_Kbhit() == 0xFF)
                {
                    result = TP_GetKey();
                    if (result == KEY_DEMO_CONFIRM)
                    {
                        loop = TRUE;
                        break;
                    }
                    else if (result == KEY_DEMO_CANCEL)
                    {
                        loop = FALSE;
                        break;
                    }
                }
            }
        } while (loop == TRUE);
        break;

    case 166://GPRS APN
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Access Point Name:");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(90, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 16, 125, 31);
        TP_SetDisplayArea(4, 17, 124, 30);
        TP_ScrGotoxyEx(4, 17);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        result = TP_GetHzString(AccessPointName, 0, 20);
        TP_DbgSerialPrn("\r\n result:%d",result);
        if ( result == 0xFF)
        {
            return FALSE;
        }
        TP_SetDisplayArea(0, 0, 127, 63);
        break;

    case 167://GPRS Username
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Input User Name:");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(90, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 16, 125, 31);
        TP_SetDisplayArea(4, 17, 124, 30);
        TP_ScrGotoxyEx(4, 17);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        if (TP_GetHzString(AccessUserName, 0, 8) == 0xFF)
        {
            return FALSE;
        }
        TP_SetDisplayArea(0, 0, 127, 63);
        break;

    case 168://GPRS Password
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Input Password:");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Conifrm");
        TP_ScrGotoxy(90, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 16, 125, 31);
        TP_SetDisplayArea(4, 17, 124, 30);
        TP_ScrGotoxyEx(4, 17);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        if( TP_GetHzString(AccessPassword, 0, 8) == 0xFF)
        {
            return FALSE;
        }
        TP_SetDisplayArea(0, 0, 127, 63);
        break;

    case 169://GPRS Network Status
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Network Status:");
        memset(outputString, 0x00, sizeof(outputString));
        result = TP_WlPPPCheck();
        switch (result)
        {
        case -202:
            sprintf((char*)outputString, "No SIM card");
            break;
        case -203:
            sprintf((char*)outputString, "Not registered to network");
            break;
        case -204:
            sprintf((char*)outputString, "Registering to network");
            break;
        case -208:
            sprintf((char*)outputString, "Idle");
            break;
        case -209:
            sprintf((char*)outputString, "PPP connection has been resetted");
            break;
        case -206:
            sprintf((char*)outputString, "PPP dialing");
            break;

        case 0:
            result = TP_NetworkGetStatus();
            switch (result)
            {
            case SRVCSTATUS_REGISTERED:
                sprintf((char*)outputString, "PPP connected");
                break;

            case SRVCSTATUS_NETWORK_REJECTED:
            case SRVCSTATUS_NOT_REGISTER:
            case SRVCSTATUS_REGISTERING:
            case SRVCSTATUS_UNKNOWN:
                sprintf((char*)outputString, "PPP connected, but service unavailable");
                break;

            default:
                sprintf((char*)outputString, "Unknow Status");
                break;
            }
            break;

        default:
            sprintf((char*)outputString, "Unknow Status");
            break;
        }
        TP_ScrGotoxy(3, 2);
        TP_LcdPrintf((uint8*)"%s", outputString);
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Press any key return");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();

                break;
            }
        }
        break;

    case 170://GPRS Signal Level

        result = TP_WlGetSignal(&loop);
        if (result != 0)
        {
            loop = 0;
        }
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Signal Level = %d", loop);
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Press any key return");
        TP_TimerSet(5,1000);
        while (TRUE)
        {
            if(TP_TimerCheck(5)==0)
            {
                TP_TimerSet(5,1000);
                result = TP_WlGetSignal(&loop);
                if (result != 0)
                {
                    loop = 0;
                }
                TP_ScrClrLine(0, 3);
                TP_ScrDrawRect(0, 0, 127, 63);//draw border
                TP_ScrGotoxy(3, 0);
                TP_LcdPrintf((uint8*)"Signal Level = %d", loop);
            }

            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                break;
            }
        }
        break;

    case 171://GPRS Dial
        do
        {
            TP_ScrCls();
            TP_ScrDrawRect(0, 0, 127, 63);//draw border
            TP_ScrGotoxy(3, 0);
            TP_LcdPrintf((uint8*)"Connecting...");

            result = TP_WlPPPLogin((uint8*)AccessPointName,
                                   (uint8*)AccessUserName,
                                   (uint8*)AccessPassword,
                                   0, 0, -1);

            if (result == 0)
            {
                TP_ScrGotoxy(3, 2);
                TP_LcdPrintf((uint8*)"Successful");
                TP_ScrGotoxy(3, 6);
                TP_LcdPrintf((uint8*)"Press any key return");
                loop = FALSE;
            }
            else
            {
                TP_ScrGotoxy(3, 2);
                TP_LcdPrintf((uint8*)"Error:%d", result);
                TP_ScrGotoxy(3, 6);
                TP_LcdPrintf((uint8*)"Try Again");
                TP_ScrGotoxy(88, 6);
                TP_LcdPrintf((uint8*)"Cancel");
                loop = TRUE;
            }

            if (loop == FALSE)
            {
                while (TRUE)
                {
                    if (TP_Kbhit() == 0xFF)
                    {
                        TP_GetKey();
                        break;
                    }
                }
            }

            if (loop == TRUE)
            {
                while (TRUE)
                {
                    if (TP_Kbhit() == 0xFF)
                    {
                        result = TP_GetKey();
                        if (result == KEY_DEMO_CONFIRM)
                        {
                            loop = TRUE;
                            break;
                        }
                        else if (result == KEY_DEMO_CANCEL)
                        {
                            loop = FALSE;
                            break;
                        }
                    }
                }
            }
        } while (loop == TRUE);
        break;

    case 172://GPRS Disconnect
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Disconnecting...");
        TP_TimerSet(0, 1000);
        while (TP_TimerCheck(0) != 0);
        result = TP_WlPPPLogout();
        TP_ScrGotoxy(3, 2);
        if (result < 0)
        {
            TP_LcdPrintf((uint8*)"Failed:%d", result);
        }
        else
        {
            TP_LcdPrintf((uint8*)"Successful");
        }
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Press any key return");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                break;
            }
        }
        break;

    case 173: //TCP Server IP
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Input Server IP:");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 16, 125, 40);
        TP_SetDisplayArea(4, 17, 124, 39);
        TP_ScrGotoxyEx(4, 17);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        memset(tempIP,0x00,16);
        memcpy(tempIP,TcpServerIp,16);
        while(1)
        {
            keycode = TP_GetString(TcpServerIp, 0xF6, 1, 15);
            if(keycode == 0xfb)
            {
                if(TP_CheckIPAddr((uint8*)TcpServerIp))
                {
                    //invalid fomat
                    TP_PopupMessageTimed((uint8*)"  Wrong format!\n xxx.xxx.xxx.xxx", 1000);
                    memcpy(TcpServerIp,tempIP,16);
                }
                else
                {
                    break;
                }
            }
            else
            {
                return FALSE;
            }
        }
        TP_SetDisplayArea(0, 0, 127, 63);
        break;

    case 174: //TCP Server Port
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Input Server Port:");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 16, 125, 40);
        TP_SetDisplayArea(4, 17, 124, 39);
        TP_ScrGotoxyEx(4, 17);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        if (TP_GetString(TcpServerPort, 0xF4, 1, 5) == 0xFF)
        {
            return FALSE;
        }
        TP_SetDisplayArea(0, 0, 127, 63);
        break;

    case 175: //TCP Connect
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Server connecting...");

        if (TcpUseSocket == -1)
        {
            TcpUseSocket = TP_NetSocket(AF_INET, NET_SOCK_STREAM, 0);
            if (TcpUseSocket < 0)
            {
                TP_ScrGotoxy(3, 2);
                TP_LcdPrintf((uint8*)"Socket create failed:%d", TcpUseSocket);
                TcpUseSocket = -1;
            }
        }

        if (TcpUseSocket >= 0)
        {
            TP_NetIOCtl(TcpUseSocket, CMD_TO_SET, 60000);

            portNumber = atoi(TcpServerPort);
            if (portNumber > 65535)
            {
                portNumber = 65535;
            }
            TP_SockAddrSet(&remoteHost, (uint8*)TcpServerIp, portNumber);
            result = TP_NetConnect(TcpUseSocket, &remoteHost, 0);
            if (result < 0)
            {
                TP_ScrGotoxy(3, 2);
                TP_LcdPrintf((uint8*)"Socket connect failed:%d", result);
                if (result == -9)
                {
                    TcpUseSocket = -1;
                }
            }
            else
            {
                TP_ScrGotoxy(3, 2);
                TP_LcdPrintf((uint8*)"Successful");
            }
        }

        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Press any key return");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                break;
            }
        }
        break;

    case 176: //TCP Close
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        result = TP_NetCloseSocket(TcpUseSocket);
        if (result < 0)
        {
            TP_ScrGotoxy(3, 0);
            TP_LcdPrintf((uint8*)"Socket close failed:%d", result);
            if (result == -9)
            {
                TcpUseSocket = -1;
            }
        }
        else
        {
            TP_ScrGotoxy(3, 0);
            TP_LcdPrintf((uint8*)"Successful");
            TcpUseSocket   = -1;
        }

        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Press any key return");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                break;
            }
        }
        break;

    case 177: //TCP Send
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_SetDisplayArea(1, 1, 126, 62);
        TP_ScrGotoxy(1, 0);
        TP_LcdPrintf((uint8*)"Input Data Length(1~5120)");
        TP_ScrGotoxy(1, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(90, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 28, 125, 42);
        TP_SetDisplayArea(4, 29, 124, 41);
        TP_ScrGotoxyEx(4, 29);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        while(1)
        {
            memset(inputString, 0x00, 100);
            result = TP_GetString((char *)inputString, 0xF4, 1, 5);
            if (result == 0xFF)
            {
                return;
            }
            portNumber = atoi((char*)inputString);
            if (portNumber < 1 || portNumber > 5120)
            {
                TP_PopupMessageTimed((uint8*)"Out of range,Please input again",2000);
                TP_ScrClsDisplayArea();
            }
            else
            {
                break;
            }

        }

        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_SetDisplayArea(1, 1, 126, 62);
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Sending...");

        memset(sendData, 0x23, sizeof(sendData));
        sendData[sizeof(sendData)-1] = 0;
        result = TP_NetSend(TcpUseSocket, sendData, portNumber, 0);
        if (result < 0)
        {
            TP_ScrGotoxy(3, 2);
            TP_LcdPrintf((uint8*)"Send failed:%d", result);
            if (result == -9)
            {
                TcpUseSocket = -1;
            }
        }
        else
        {
            TP_ScrGotoxy(3, 2);
            TP_LcdPrintf((uint8*)"Send successful", result);
        }

        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Press any key return");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                break;
            }
        }
        break;

    case 178: //TCP Receive

        TP_SetDisplayArea(0, 0, 127, 63);
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Receiving data in 5s...");
        TP_NetIOCtl(TcpUseSocket, CMD_TO_SET, 5000);

        memset(sendData, 0x00, sizeof(sendData));
        result = TP_NetRecv(TcpUseSocket, sendData, 5120, 0);
        if (result < 0)
        {
            TP_ScrClrLine(0,2);
            TP_ScrDrawRect(0, 0, 127, 63);//draw border
            TP_ScrGotoxy(3, 0);
            TP_LcdPrintf((uint8*)"Result:");
            TP_ScrGotoxy(3, 2);
            if (result == -60 )
            {
                TP_LcdPrintf((uint8*)"No data received");
            }
            else
            {
                TP_LcdPrintf((uint8*)"Receive failed:%d", result);
                if (result == -9)
                {
                    TcpUseSocket = -1;
                }
            }
            TP_ScrGotoxy(3, 6);
            TP_LcdPrintf((uint8*)"Press any key return");
            while (TRUE)
            {
                if (TP_Kbhit() == 0xFF)
                {
                    TP_GetKey();
                    break;
                }
            }
        }
        else
        {
            TP_ScrClrLine(0,2);
            TP_ScrDrawRect(0, 0, 127, 63);//draw border
            TP_ScrGotoxy(3, 0);
            TP_LcdPrintf((uint8*)"Result:");
            TP_ScrGotoxy(3, 2);
            TP_LcdPrintf((uint8*)"Receive %d bytes Data", result);

            TP_ScrGotoxyEx(3, 50);
            TP_LcdPrintf((uint8*)"Details       Return");
            while (TRUE)
            {
                if(TP_Kbhit()!=0)
                {
                    keycode = TP_GetKey();
                    if(keycode == KEY_DEMO_CONFIRM)
                    {
                        break;
                    }
                    else if(keycode == KEY_DEMO_CANCEL)
                    {
                        return;
                    }
                }
            }

            TP_ScrCls();
            TP_ShowText(&displayInfo,rect,sendData);
        }

        break;

    case 179: //UDP Send
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Input Host IP:");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(90, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 16, 125, 30);
        TP_SetDisplayArea(4, 17, 124, 29);
        TP_ScrGotoxyEx(4, 17);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        memset(UdpHostIp, 0x00, 16);
        sprintf(UdpHostIp, "121.9.230.130\x00");
        result = TP_GetString(UdpHostIp, 0xF6, 1, 15);
        TP_SetDisplayArea(0, 0, 127, 63);
        if (result == 0xFF)
        {
            return;
        }

        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxyEx(1, 1);
        TP_LcdPrintf((uint8*)"Input Host Port(1~65535):");
        TP_ScrGotoxy(1, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 26, 125, 40);
        TP_SetDisplayArea(4, 27, 124, 39);
        TP_ScrGotoxyEx(4, 27);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        memset(UdpHostPort, 0x00, 6);
        sprintf(UdpHostPort, "7006\x00");
        while(1)
        {
            result = TP_GetString(UdpHostPort, 0xF4, 1, 5);
            if (result == 0xFF)
            {
                return;
            }
            portNumber = atoi(UdpHostPort);
            if (portNumber > 65535 || portNumber < 1)
            {
                TP_PopupMessageTimed((uint8*)"Out of range,Please input again",2000);
                TP_ScrClsDisplayArea();
                memset(UdpHostPort, 0, sizeof(UdpHostPort));
            }
            else
            {
                break;
            }
        }
        TP_SetDisplayArea(0, 0, 127, 63);
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxyEx(1, 1);
        TP_LcdPrintf((uint8*)"Input Data Length(1~1024)");
        TP_ScrGotoxy(1, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 26, 125, 40);
        TP_SetDisplayArea(4, 27, 124, 39);
        TP_ScrGotoxyEx(4, 27);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        while(1)
        {
            memset(inputString, 0x00, 100);
            result = TP_GetString((char*)inputString, 0xF4, 1, 4);
            if (result == 0xFF)
            {
                return;
            }
            portNumber = atoi((char*)inputString);
            if (portNumber < 1 || portNumber > 1024 )
            {
                TP_PopupMessageTimed((uint8*)"Out of range,Please input again",2000);
                TP_ScrClsDisplayArea();
            }
            else
            {
                break;
            }
        }

        TP_SetDisplayArea(0, 0, 127, 63);
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxyEx(1, 1);
        TP_LcdPrintf((uint8*)"Sending...");
        udpSocket = TP_NetSocket(AF_INET, NET_SOCK_DGRAM, 0);
        if (udpSocket >= 0)
        {
            portNumber = atoi(UdpHostPort);
            if (portNumber > 65535)
            {
                portNumber = 65535;
            }
            TP_SockAddrSet(&remoteHost, (uint8*)UdpHostIp, portNumber);
            portNumber = atoi((char *)inputString);
            memset(sendData, 0x24, 1024);
            if(portNumber < 1)
            {
                portNumber = 1;
            }
            else if(portNumber>1024)
            {
                portNumber = 1024;
            }
            result = TP_NetSendTo(udpSocket, sendData, portNumber, 0,
                                  &remoteHost, 0);
            if (result >= 0)
            {
                TP_ScrGotoxy(1, 2);
                TP_LcdPrintf((uint8*)"Send %dbytes successful", result);
            }
            else
            {
                TP_ScrGotoxy(1, 2);
                TP_LcdPrintf((uint8*)"Send failed:%d", result);
            }

            TP_NetCloseSocket(udpSocket);
        }
        else
        {
            TP_ScrGotoxy(1, 2);
            TP_LcdPrintf((uint8*)"Socket create failed:%d", udpSocket);
        }

        TP_ScrGotoxy(1, 6);
        TP_LcdPrintf((uint8*)"Press any key return");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                break;
            }
        }
        break;

    case 180: //UDP Receive
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxyEx(1, 1);
        TP_LcdPrintf((uint8*)"Input Data Length(1~50)");
        TP_ScrGotoxy(1, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 26, 125, 40);
        TP_SetDisplayArea(4, 27, 124, 39);
        TP_ScrGotoxyEx(4, 27);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        while(1)
        {
            memset(inputString, 0x00, 100);
            result = TP_GetString((char*)inputString, 0xF4, 1, 2);

            if (result == 0xFF)
            {
                return;
            }
            portNumber = atoi((char*)inputString);
            if (portNumber < 1 || portNumber > 50)
            {
                TP_PopupMessageTimed((uint8*)"Out of range,Please input again",2000);
                TP_ScrClsDisplayArea();
            }
            else
            {
                break;
            }
        }

        TP_SetDisplayArea(0, 0, 127, 63);
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_LcdPrintf((uint8*)"Receiving...");
        udpSocket = TP_NetSocket(AF_INET, NET_SOCK_DGRAM, 0);
        if (udpSocket >= 0)
        {
            memset(&remoteHost, 0x00, sizeof(TP_SockAddrT));
            memset(sendData, 0x00, sizeof(sendData));
            TP_DbgSerialPrn("\n\r begin rece");
            result = TP_NetRecvFrom(udpSocket, sendData, portNumber, 0,
                                    &remoteHost, 0);
            TP_DbgSerialPrn("\n\r rece result:%d ",result);
            if (result > 0)
            {
                TP_SockAddrGet(&remoteHost, (uint8*)UdpHostIp, 15, &portNumber);
                TP_ScrGotoxy(1, 2);
                TP_LcdPrintf((uint8*)"Received %dbytes data from %s:%d:",
                             result, UdpHostIp, portNumber);
                TP_ScrGotoxy(1, 4);
                TP_LcdPrintf((uint8*)"%s", sendData);
            }
            else
            {
                TP_ScrGotoxy(1, 2);
                TP_LcdPrintf((uint8*)"Receive failed:%d", result);
            }

            TP_NetCloseSocket(udpSocket);
        }
        else
        {
            TP_ScrGotoxy(1, 2);
            TP_LcdPrintf((uint8*)"Socket create failed:%d", udpSocket);
        }

        TP_ScrGotoxy(1, 6);
        TP_LcdPrintf((uint8*)"Press any key return");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                break;
            }
        }
        break;



    case 165: //Pressure Test
        DemoRunningCounts = 0;
        pppRetrys         = 0;
        connectRetrys     = 0;
        sendRetrys        = 0;
        closeRetrys       = 0;
        portNumber        = atoi(TcpServerPort);
        TP_SockAddrSet(&remoteHost, (uint8*)TcpServerIp, portNumber);
        memset(sendData, 0x25, sizeof(sendData));

        TP_ScrCls();
        TP_ScrGotoxy(0, 0);
        TP_LcdPrintf((uint8*)"Pressure Test Demo");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Start");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Quit");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                result = TP_GetKey();
                if (result == KEY_DEMO_CONFIRM)
                {
                    break;
                }
                else if (result == KEY_DEMO_CANCEL)
                {
                    return;
                }
            }
        }

        loop = TRUE;
        while (loop == TRUE)
        {
            TP_ScrCls();
            TP_ScrGotoxy(0, 0);
            TP_LcdPrintf((uint8*)"Running State:%d:%d:%d:%d:%d",
                         DemoRunningCounts,
                         pppRetrys,
                         connectRetrys,
                         sendRetrys,
                         closeRetrys);

            result = 0;
            TP_ScrClrLine(4, 7);
            TP_ScrGotoxy(0, 4);
            TP_LcdPrintf((uint8*)"Waiting for 10s");
            TP_ScrGotoxy(0, 6);
            TP_LcdPrintf((uint8*)"Skip");
            TP_ScrGotoxy(88, 6);
            TP_LcdPrintf((uint8*)"Quit");
            TP_TimerSet(5, 10000);
            while (TP_TimerCheck(5) != 0)
            {
                if (TP_Kbhit() == 0xFF)
                {
                    result = TP_GetKey();
                    if (result == KEY_DEMO_CONFIRM)
                    {
                        break;
                    }
                    else if (result == KEY_DEMO_CANCEL)
                    {
                        loop = FALSE;
                        break;
                    }
                }
            }

            if (TP_WlPPPCheck() != 0)
            {
                wlerrCounts = 1;
                while (wlerrCounts < 6)
                {
                    TP_ScrClrLine(4, 7);
                    TP_ScrGotoxy(0, 4);
                    TP_LcdPrintf((uint8*)"PPP Dialing... %d", wlerrCounts);
                    result = TP_WlPPPLogin((uint8*)AccessPointName,
                                           (uint8*)AccessUserName,
                                           (uint8*)AccessPassword,
                                           0, 0, -1);
                    if (result < 0)
                    {
                        if (result == -300)
                        {
                            if (DemoUseSocket >= 0)
                            {
                                TP_NetCloseSocket(DemoUseSocket);
                                DemoUseSocket = -1;
                            }

                            if (TcpUseSocket >= 0)
                            {
                                TP_NetCloseSocket(TcpUseSocket);
                                TcpUseSocket   = -1;
                            }
                        }

                        TP_WlPPPLogout();
                        wlerrCounts++;

                        TP_TimerSet(0, 1000);
                        while (TP_TimerCheck(0) != 0)
                        {

                        }
                    }
                    else
                    {
                        break;
                    }
                }
                pppRetrys += wlerrCounts;
                if (wlerrCounts == 6)
                {
                    TP_ScrClrLine(4, 7);
                    TP_ScrGotoxy(0, 4);
                    TP_LcdPrintf((uint8*)"PPP Login failed");
                    TP_TimerSet(0, 3000);
                    while (TP_TimerCheck(0) != 0)
                    {

                    }

                    TP_WlPPPLogout();

                    continue;
                }
            }

            TP_ScrClrLine(4, 7);
            TP_ScrGotoxy(0, 4);
            TP_LcdPrintf((uint8*)"Server connecting....");
            DemoUseSocket = TP_NetSocket(AF_INET, NET_SOCK_STREAM, 0);
            if (DemoUseSocket < 0)
            {
                TP_ScrClrLine(4, 7);
                TP_ScrGotoxy(0, 4);
                TP_LcdPrintf((uint8*)"Socket creat failed:%d", DemoUseSocket);

                TP_TimerSet(0, 3000);
                while (TP_TimerCheck(0) != 0)
                {

                }

                TP_WlPPPLogout();

                continue;
            }

            TP_NetIOCtl(DemoUseSocket, CMD_TO_SET, 60000);

            wlerrCounts = 0;
            do
            {
                wlerrCounts++;
                result = TP_NetConnect(DemoUseSocket, &remoteHost, 0);
                if (result < 0)
                {
                    TP_TimerSet(0, 1000);
                    while (TP_TimerCheck(0) != 0)
                    {

                    }
                }
                else
                {
                    break;
                }
            } while (wlerrCounts < 6);
            connectRetrys += wlerrCounts;
            if (result < 0)
            {
                TP_ScrClrLine(4, 7);
                TP_ScrGotoxy(0, 4);
                TP_LcdPrintf((uint8*)"Server connect failed:%d", result);

                TP_TimerSet(0, 3000);
                while (TP_TimerCheck(0) != 0)
                {

                }

                TP_NetCloseSocket(DemoUseSocket);
                DemoUseSocket = -1;
                TP_WlPPPLogout();

                continue;
            }


            TP_ScrClrLine(4, 7);
            TP_ScrGotoxy(0, 4);
            TP_LcdPrintf((uint8*)"Sending data...");
            wlerrCounts = 0;
            do
            {
                wlerrCounts++;
                result = TP_NetSend(DemoUseSocket, sendData, 10, 0);
                if (result < 0)
                {
                    TP_TimerSet(0, 1000);
                    while (TP_TimerCheck(0) != 0)
                    {

                    }
                }
                else
                {
                    break;
                }
            } while (wlerrCounts < 6);
            sendRetrys += wlerrCounts;
            if (result < 0)
            {
                TP_ScrClrLine(4, 7);
                TP_ScrGotoxy(0, 4);
                TP_LcdPrintf((uint8*)"Send data failed:%d", result);
                TP_TimerSet(0, 3000);
                while (TP_TimerCheck(0) != 0)
                {

                }

                TP_NetCloseSocket(DemoUseSocket);
                DemoUseSocket = -1;
                TP_WlPPPLogout();

                continue;
            }

            TP_ScrClrLine(4, 7);
            TP_ScrGotoxy(0, 4);
            TP_LcdPrintf((uint8*)"Server disconnecting...");
            wlerrCounts = 0;
            do
            {
                wlerrCounts++;
                result = TP_NetCloseSocket(DemoUseSocket);
                if (result < 0)
                {
                    TP_TimerSet(0, 1000);
                    while (TP_TimerCheck(0) != 0)
                    {

                    }
                }
                else
                {
                    break;
                }
            } while (wlerrCounts < 6);
            closeRetrys += wlerrCounts;
            if (result < 0)
            {
                TP_ScrClrLine(4, 7);
                TP_ScrGotoxy(0, 4);
                TP_LcdPrintf((uint8*)"Disconnect failed:%d", result);
                TP_TimerSet(0, 3000);
                while (TP_TimerCheck(0) != 0)
                {

                }

                TP_WlPPPLogout();

                continue;
            }
            DemoUseSocket = -1;

            TP_ScrClrLine(4, 7);
            TP_ScrGotoxy(0, 4);
            TP_LcdPrintf((uint8*)"PPP disconnecting...");
            TP_WlPPPLogout();



            DemoRunningCounts++;
        }

        TP_NetCloseSocket(DemoUseSocket);
        DemoUseSocket = -1;
        TP_WlPPPLogout();
        break;

    case 210:// local IP Address
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//draw border
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Local IP Address:");
        memset(outputString, 0x00, sizeof(outputString));
        result = TP_WlGetIPAddress(outputString);
        TP_ScrGotoxy(3, 2);
        if (result != WL_RET_OK)
        {
            memset(outputString, 0x00, sizeof(outputString));
            sprintf((char*)outputString, "PPP Not Connected");
        }
        TP_LcdPrintf((uint8*)"%s", outputString);
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Press any key return");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                return FALSE;
            }
        }
        break;
    }
    return TRUE;
}

