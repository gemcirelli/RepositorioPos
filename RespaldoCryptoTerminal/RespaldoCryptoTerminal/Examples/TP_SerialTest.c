#include <TPTypedef.h>
#include <TPBase.h>
#include <TPSerial.h>
#include <TPLCD.h>
#include <TPKeyPad.h>

#include "TP_DemoDefine.h"

char PortRate[7] = "9600\x00\x00\x00";
char DataBit[2]  = "8\x00";
char Parity[2]   = "n\x00";
char StopBit[2]  = "1\x00";

void TP_SerialTest(uint16 itemId)
{
    char    comAttr[13];
    char    sendData[102];
    char    recvData[1024];
    uint8   result;
    Boolean loop = TRUE;
    int32   retVal = 0;
    int32   iByte = 0;

    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_ScrSpaceSet(0, 0);
    TP_SetDisplayArea(0, 0, 127, 63);

    switch (itemId)
    {
    case 116:
        TP_ScrCls();
        TP_ScrGotoxyEx(0, 0);
        TP_LcdPrintf((uint8*)"Input string to send:");
        TP_ScrGotoxyEx(0, 52);
        TP_LcdPrintf((uint8*)"OK");
        TP_ScrGotoxyEx(91, 52);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(0, 16, 127, 48);
        TP_SetDisplayArea(1, 17, 126, 47);
        TP_ScrGotoxyEx(1, 17);
        TP_ShowInputNum(1,26,52);
        TP_SetInputModePosition(59,52);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        memset(sendData, 0x00, 51);
        result = TP_GetHzString((char*)sendData, 0, 50);
        TP_SetDisplayArea(0, 0, 127, 63);
        TP_ScrFontSet(ASCII);
        if (result == 0xFF)
        {
            return;
        }

        TP_ScrFontSet(CFONT);
        TP_ScrCls();
        TP_ScrGotoxyEx(0, 0);
        TP_LcdPrintf((uint8*)"Press any key send");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                break;
            }
        }
        sprintf(comAttr, "%s,%s,%s,%s", PortRate, DataBit, Parity, StopBit);
        result = TP_PortOpen(0, comAttr);
        if (result != 0x00)
        {
            TP_ScrGotoxyEx(0, 13);
            TP_LcdPrintf((uint8*)"Port open failed:%d", result);
            TP_ScrGotoxyEx(0, 52);
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

        }

        do
        {
            result = TP_PortSends(0, (uint8*)sendData, strlen((char *)sendData));
            TP_ScrGotoxyEx(0, 13);
            if (result == 0x00)
            {
                TP_LcdPrintf((uint8*)"Send has done");
            }
            else
            {
                TP_LcdPrintf((uint8*)"Send failed");
            }
            TP_ScrGotoxyEx(0, 52);
            TP_LcdPrintf((uint8*)"Send Again");
            TP_ScrGotoxyEx(103, 52);
            TP_LcdPrintf((uint8*)"Quit");
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
        TP_PortClose(0);
        break;

    case 117:
        TP_ScrCls();
        TP_ScrGotoxy(0, 0);
        TP_LcdPrintf((uint8*)"Press any key receive");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                break;
            }
        }

        sprintf(comAttr, "%s,%s,%s,%s", PortRate, DataBit, Parity, StopBit);
        result = TP_PortOpen(0, comAttr);
        if (result != 0x00)
        {
            TP_ScrGotoxy(0, 2);
            TP_LcdPrintf((uint8*)"Port open failed:%d", result);
            TP_ScrGotoxy(0, 6);
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

        }

        do
        {
            TP_ScrCls();
            TP_ScrGotoxy(0, 0);
            TP_LcdPrintf((uint8*)"Send at least 10 bytes data in 5s");
            memset(sendData, 0x00, sizeof(sendData));
            retVal = TP_PortRecvsEx(0, (uint8*)sendData, 10, 5000);

            TP_ScrCls();
            TP_ScrGotoxy(0, 0);
            if (retVal > 0x00)
            {

                memset(recvData, 0x00, sizeof(recvData));
                for (iByte = 0; iByte < retVal; iByte++)
                {
                    sprintf(&recvData[4 * iByte], "%#4X", sendData[iByte]);

                    if (iByte == 10)
                    {
                        break;
                    }
                }

                TP_LcdPrintf((uint8*)"Received data: %d", retVal);
                TP_ScrGotoxy(0, 2);
                TP_LcdPrintf((uint8*)"%s", recvData);
            }
            else if (retVal == 0)
            {
                TP_LcdPrintf((uint8*)"Receive timedout");
            }
            else
            {
                TP_LcdPrintf((uint8*)"Receive error: %d", retVal);
            }

            if (retVal > 0)
            {
                // clear test
                TP_TimerSet(1, 3000);
                while (TP_TimerCheck(1) != 0);

                TP_ScrCls();
                TP_ScrGotoxy(0, 0);
                TP_LcdPrintf((uint8*)"Clear data test");

                retVal = TP_PortRecvsEx(0, PNULL, 2048, 0);
                if (retVal >= 0x00)
                {
                    TP_ScrCls();
                    TP_LcdPrintf((uint8*)"Clear %d bytes", retVal);
                }
                else
                {
                    TP_LcdPrintf((uint8*)"Clear error: %d", retVal);
                }

            }

            TP_ScrGotoxy(0, 6);
            TP_LcdPrintf((uint8*)"Receive again");
            TP_ScrGotoxy(96, 6);
            TP_LcdPrintf((uint8*)"Quit");

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
        TP_PortClose(0);
        break;

    case 123:
        memset(PortRate, 0x00, sizeof(PortRate));
        sprintf(PortRate, "9600");
        break;

    case 124:
        memset(PortRate, 0x00, sizeof(PortRate));
        sprintf(PortRate, "115200");
        break;

    case 125:
        memset(DataBit, 0x00, sizeof(DataBit));
        sprintf(DataBit, "7");
        break;

    case 126:
        memset(DataBit, 0x00, sizeof(DataBit));
        sprintf(DataBit, "8");
        break;

    case 127:
        memset(Parity, 0x00, sizeof(Parity));
        sprintf(Parity, "n");
        break;

    case 128:
        memset(Parity, 0x00, sizeof(Parity));
        sprintf(Parity, "o");
        break;

    case 129:
        memset(Parity, 0x00, sizeof(Parity));
        sprintf(Parity, "e");
        break;

    case 130:
        memset(StopBit, 0x00, sizeof(StopBit));
        sprintf(StopBit, "1");
        break;

    case 131:
        memset(StopBit, 0x00, sizeof(StopBit));
        sprintf(StopBit, "2");
        break;
    }
}
