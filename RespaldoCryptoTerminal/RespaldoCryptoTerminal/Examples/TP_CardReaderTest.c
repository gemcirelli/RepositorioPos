#if defined(CARD_READER_MODULE_ON)/* Build Options */

/******************************************************************************
* Include Headers
*******************************************************************************/
#include <TPTypedef.h>
#include <TPBase.h>
#include <TPLCD.h>
#include <TPKeypad.h>
#include <TPSerial.h>
#include <TPCardReader.h>
#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"

/**
* Display a window, with time control
* @author He Wendong
* @version 1.0
* @param *msg Display the string
*        Ms Display time in milliseconds
* @return
* @since January 3, 2012
*/
void ShowTimePopupDialogMsg( char *msg,uint32 Ms)
{
    uint8 TmpBuffer[256];
    //uint8 Canreturn;
    TP_ScrRestore(0);
    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(10,10,117,47);
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect(11,11,116,47);
    TP_SetDisplayArea(12,12,115,46);
    TP_ScrGotoxyEx(12,12);

    memset(TmpBuffer,0,sizeof(TmpBuffer));
    //sprintf(TmpBuffer,"The remaining time %d seconds",Ms/1000);
    //sprintf(TmpBuffer,*msg,Ms/1000);
    memcpy(TmpBuffer,msg,strlen((char *)msg));
    TP_LcdPrintf("%s", TmpBuffer);
    TP_TimerSet(0, Ms);
    while(1)
    {
        if(TP_TimerCheck(0) == 0)
        {
            TP_ScrRestore(1);
            return;
        }
        if (TP_Kbhit())
        {
            ///Any key to exit
            TP_Kbflush(); ///Need to clear the key assignments in keys
            TP_ScrRestore(1);
            return;

        }
    }
}


// SLE4442
uint16 ICReaderTest_4442(uint8* pCardInfo,
                         TP_4442SectorE sector,
                         uint16 readPos, uint16 readLen)
{
    uint8 retval;
    uint8 keyCode;
    uint8 passwd[3];

#if 0
    // check pasw default password is 0xff 0xff 0xff , 3byte
    passwd[0] = 0xff;
    passwd[1] = 0xff;
    passwd[2] = 0xff;
    retval = TP_ICCardPasswordVerify(passwd, 3);
    if (retval != 0)
    {
        TP_ICReaderStop();

        return 0;
    }

    // write data to user sector
    memset(pCardInfo, 0x34, 15);
    if (TP_ICCardWrite_4442(pCardInfo, 0, 15) != 0x00)
    {
        TP_ICReaderStop();

        return 0;
    }
#endif

    memset(pCardInfo, 0x00, 256);
    if (sector == SECTOR_PROTECTED)
    {
        // read protected data
        if (TP_ICCardRead_4442(SECTOR_PROTECTED, readPos, readLen) != 0x00)
        {
            TP_ICReaderStop();

            return 0;
        }
    }
    else
    {
        if (TP_ICCardRead_4442(SECTOR_USERDATA, readPos, readLen) != 0x00)
        {
            TP_ICReaderStop();

            return 0;
        }
    }

    while (TP_ICReaderDataCheck() == 0)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                TP_Kbflush();
                TP_ICReaderStop();

                return 0;
            }
        }
    }

    TP_ICReaderDataGet(pCardInfo, readLen);

    return readLen;
}

// 4406
uint16 ICReaderTest_4406(uint8* pCardInfo, uint16 readLen)
{
    uint8 keyCode;

    memset(pCardInfo, 0x00, 256);

    if (TP_ICCardRead_4406(readLen) != 0x00)
    {
        TP_ICReaderStop();

        return 0;
    }

    while (TP_ICReaderDataCheck() == 0)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                TP_Kbflush();
                TP_ICReaderStop();

                return 0;
            }
        }
    }

    TP_ICReaderDataGet(pCardInfo, readLen);

    return readLen;
}

// 24CXX
uint16 ICReaderTest_24C0X(uint8* pCardInfo, uint16 readPos, uint16 readLen)
{
    uint8 keyCode;

#if 0
    memset(pCardInfo, 0x39 , 30);

    if (TP_ICCardWrite_24CXX(pCardInfo, 0, 30) != 0x00)
    {
        TP_ICReaderStop();

        return 0;
    }
#endif

    memset(pCardInfo, 0x00, 256);
    if (TP_ICCardRead_24CXX(readPos, readLen) != 0x00)
    {
        TP_ICReaderStop();

        return 0;
    }

    while (TP_ICReaderDataCheck() == 0)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                TP_Kbflush();
                TP_ICReaderStop();

                return 0;
            }
        }
    }

    TP_ICReaderDataGet(pCardInfo, readLen);

    return readLen;
}


void ICReaderGetData_24C0X(uint16* pReadPos, uint16* pReadLen)
{
    uint8 keyCode;
    uint8 inputString[10];

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);

    TP_LcdFreeze();

    TP_ScrGotoxyEx(1, 1);
    TP_LcdPrintf("%s\x00", "Reading position:\x00");

    TP_ScrGotoxyEx(1, 13);
    TP_LcdPrintf("%s\x00", "(0~255)\x00");

    TP_ScrDrawRect(3, 28, 125, 42);

    TP_ScrUpdate();

    TP_SetDisplayArea(4, 29, 124, 41);
    TP_ScrGotoxyEx(4, 29);
    TP_SetInputModeControl(KEY_DEMO_CONFIRM, TP_KEY_NONE, TP_KEY_NONE);
    TP_SetInputModePosition(59,52);
    TP_ShowInputNum(0, 1, 1);
    while (TRUE)
    {
        memset(inputString, 0x00, sizeof(inputString));
        keyCode = TP_GetString(inputString, 0xF4, 1, 3);

        *pReadPos = atoi(inputString);
        if (*pReadPos > (IC_CARD_24C02_MAX_SIZE - 1))
        {
            ShowTimePopupDialogMsg("!Invalid\nPlease Re-Input", 2000);
            TP_ScrClsDisplayArea();
        }
        else
        {
            break;
        }
    }

    TP_ScrCls();

    TP_SetDisplayArea(0, 0, 127,63);

    TP_LcdFreeze();

    TP_ScrGotoxyEx(1, 1);
    TP_LcdPrintf("%s\x00", "Reading Length:\x00");
    TP_ScrGotoxyEx(1, 13);
    TP_LcdPrintf("(1~%d)\x00", IC_CARD_24C02_MAX_SIZE - *pReadPos);

    TP_ScrDrawRect(3, 28, 125, 42);

    TP_ScrUpdate();

    TP_SetDisplayArea(4, 29, 124, 41);
    TP_ScrGotoxyEx(4, 29);
    TP_SetInputModeControl(KEY_DEMO_CONFIRM, TP_KEY_NONE, TP_KEY_NONE);
    TP_SetInputModePosition(59,52);
    TP_ShowInputNum(0, 1, 1);
    while (TRUE)
    {
        memset(inputString, 0x00, sizeof(inputString));
        keyCode = TP_GetString(inputString, 0xF4, 1, 3);

        *pReadLen = atoi(inputString);
        if ((*pReadLen < 1) ||
            (*pReadLen > (IC_CARD_24C02_MAX_SIZE - *pReadPos))
           )
        {
            ShowTimePopupDialogMsg("!Invalid\nPlease Re-Input", 2000);
            TP_ScrClsDisplayArea();
        }

        else
        {
            break;
        }
    }
}

void ICReaderGetData_4406(uint16* pReadLen)
{
    uint8 keyCode;
    uint8 inputString[10];

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);

    TP_LcdFreeze();

    TP_ScrGotoxyEx(1, 1);
    TP_LcdPrintf("%s\x00", "Reading Length:\x00");
    TP_ScrGotoxyEx(1, 13);
    TP_LcdPrintf("%s\x00", "(1~256)\x00");
    TP_ScrDrawRect(3, 28, 125, 42);

    TP_ScrUpdate();

    TP_SetDisplayArea(4, 29, 124, 41);
    TP_ScrGotoxyEx(4, 29);
    TP_SetInputModeControl(KEY_DEMO_CONFIRM, TP_KEY_NONE, TP_KEY_NONE);
    TP_SetInputModePosition(59,52);
    TP_ShowInputNum(0, 1, 1);
    while (TRUE)
    {
        memset(inputString, 0x00, sizeof(inputString));
        keyCode = TP_GetString(inputString, 0xF4, 1, 3);

        *pReadLen = atoi(inputString);
        if (*pReadLen < 1 || *pReadLen > IC_CARD_4406_MAX_SIZE)
        {
            ShowTimePopupDialogMsg("!Invalid\nPlease Re-Input", 2000);
            TP_ScrClsDisplayArea();
        }
        else
        {
            break;
        }
    }
}

void ICReaderGetData_4442(TP_4442SectorE* pSector,
                          uint16* pReadPos, uint16* pReadLen)
{
    uint8 keyCode;
    uint8 inputString[10];

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);

    TP_LcdFreeze();

    TP_ScrGotoxyEx(1, 1);
    TP_LcdPrintf("%s\x00", "Select sector to read\x00");
    TP_ScrGotoxyEx(1, 13);
    TP_LcdPrintf("%s\x00", "1 Protected\x00");
    TP_ScrGotoxyEx(1, 25);
    TP_LcdPrintf("%s\x00", "2 User Data\x00");

    TP_ScrUpdate();

    TP_Kbflush();
    while (TRUE)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            TP_Kbflush();

            if (keyCode == TP_KEY_1)
            {
                *pSector = SECTOR_PROTECTED;

                break;
            }
            else if (keyCode == TP_KEY_2)
            {
                *pSector = SECTOR_USERDATA;

                break;
            }
            else
            {

            }
        }
    }

    TP_ScrCls();

    TP_LcdFreeze();

    TP_ScrGotoxyEx(1, 1);
    TP_LcdPrintf("%s\x00", "Reading position:\x00");
    if (*pSector == SECTOR_PROTECTED)
    {
        TP_ScrGotoxyEx(1, 13);
        TP_LcdPrintf("%s\x00", "(0~31)\x00");
    }
    else
    {
        TP_ScrGotoxyEx(1, 13);
        TP_LcdPrintf("%s\x00", "(0~223)\x00");
    }

    TP_ScrDrawRect(3, 28, 125, 42);

    TP_ScrUpdate();

    TP_SetDisplayArea(4, 29, 124, 41);
    TP_ScrGotoxyEx(4, 29);
    TP_SetInputModeControl(KEY_DEMO_CONFIRM, TP_KEY_NONE, TP_KEY_NONE);
    TP_SetInputModePosition(59,52);
    TP_ShowInputNum(0, 1, 1);
    while (TRUE)
    {
        memset(inputString, 0x00, sizeof(inputString));
        keyCode = TP_GetString(inputString, 0xF4, 1, 3);

        *pReadPos = atoi(inputString);
        if (*pSector == SECTOR_PROTECTED)
        {
            if (*pReadPos > 31)
            {
                ShowTimePopupDialogMsg("!Invalid\nPlease Re-Input",2000);
                TP_ScrClsDisplayArea();
            }
            else
            {
                break;
            }
        }
        else
        {
            if (*pReadPos > 223)
            {
                ShowTimePopupDialogMsg("!Invalid\nPlease Re-Input",2000);
                TP_ScrClsDisplayArea();
            }
            else
            {
                break;
            }
        }
    }

    TP_ScrCls();

    TP_SetDisplayArea(0, 0, 127,63);

    TP_LcdFreeze();

    TP_ScrGotoxyEx(1, 1);
    TP_LcdPrintf("%s\x00", "Reading Length:\x00");
    if (*pSector == SECTOR_PROTECTED)
    {
        TP_ScrGotoxyEx(1, 13);
        TP_LcdPrintf("(1~%d)\x00", IC_CARD_4442_PROTECT_MAX_SIZE - *pReadPos);
    }
    else
    {
        TP_ScrGotoxyEx(1, 13);
        TP_LcdPrintf("(1~%d)\x00", IC_CARD_4442_USERDATA_MAX_SIZE - *pReadPos);
    }

    TP_ScrDrawRect(3, 28, 125, 42);

    TP_ScrUpdate();

    TP_SetDisplayArea(4, 29, 124, 41);
    TP_ScrGotoxyEx(4, 29);
    TP_SetInputModeControl(KEY_DEMO_CONFIRM, TP_KEY_NONE, TP_KEY_NONE);
    TP_SetInputModePosition(59,52);
    TP_ShowInputNum(0, 1, 1);
    while (TRUE)
    {
        memset(inputString, 0x00, sizeof(inputString));
        keyCode = TP_GetString(inputString, 0xF4, 1, 3);

        *pReadLen = atoi(inputString);
        if (*pSector == SECTOR_PROTECTED)
        {
            if ((*pReadLen < 1) ||
                (*pReadLen > (IC_CARD_4442_PROTECT_MAX_SIZE - *pReadPos))
               )
            {
                ShowTimePopupDialogMsg("!Invalid\nPlease Re-Input",2000);
                TP_ScrClsDisplayArea();
            }

            else
            {
                break;
            }
        }
        else
        {
            if ((*pReadLen < 1) ||
                (*pReadLen > (IC_CARD_4442_USERDATA_MAX_SIZE - *pReadPos))
               )
            {
                ShowTimePopupDialogMsg("!Invalid\nPlease Re-Input",2000);
                TP_ScrClsDisplayArea();
            }
            else
            {
                break;
            }
        }
    }
}

void ICReaderTest(void)
{
    uint8          cardType;
    uint16         readPos;
    uint16         readLen;
    uint16         nInfo;
    TP_4442SectorE sector;
    uint8          keyCode;
    uint8          cardInfo[257];
    uint8          displayInfo[513];

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);

    TP_LcdFreeze();

    TP_ScrGotoxyEx(1, 1);
    TP_LcdPrintf("%s\x00", "IC Card Reader Test\x00");
    TP_ScrGotoxyEx(1, 13);
    TP_LcdPrintf("%s\x00", "1 24C0X Card\x00");
    TP_ScrGotoxyEx(1, 25);
    TP_LcdPrintf("%s\x00", "2 4406 Card\x00");
    TP_ScrGotoxyEx(1, 38);
    TP_LcdPrintf("%s\x00", "3 SLE4442/5542 Card\x00");
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%21s", "Quit");

    TP_ScrUpdate();

    TP_Kbflush();
    while (TRUE)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            TP_Kbflush();

            if (keyCode == TP_KEY_1)
            {
                cardType = 1;

                break;
            }
            else if (keyCode == TP_KEY_2)
            {
                cardType = 2;

                break;
            }
            else if (keyCode == TP_KEY_3)
            {
                cardType = 3;

                break;
            }
            else if (keyCode == KEY_DEMO_CANCEL)
            {
                return;
            }
            else
            {

            }
        }
    }

    switch (cardType)
    {
    case 1:
        ICReaderGetData_24C0X(&readPos, &readLen);
        break;

    case 2:
        ICReaderGetData_4406(&readLen);
        break;

    case 3:
        ICReaderGetData_4442(&sector, &readPos, &readLen);
        break;

    default:
        return;
    }

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);

    TP_LcdFreeze();

    TP_ScrClrRect(1, 13, 127, 63);
    TP_ScrGotoxyEx(1, 13);
    TP_LcdPrintf("Please Insert Card");
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%20s", "Quit");

    TP_ScrUpdate();

    TP_ICReaderStart();

    // check card inserd
    while ( TP_ICReaderIsCardInserted() != 0)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                TP_Kbflush();
                TP_ICReaderStop();
                return;
            }
        }
    }

    TP_LcdFreeze();

    TP_ScrClrRect(1, 13, 127, 63);
    TP_ScrGotoxyEx(1, 13);
    TP_LcdPrintf("Reading...");
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%20s", "Quit");

    TP_ScrUpdate();

    switch (cardType)
    {
    case 1:
        nInfo = ICReaderTest_24C0X(cardInfo, readPos, readLen);
        break;
    case 2:
        nInfo = ICReaderTest_4406(cardInfo, readLen);
        break;
    case 3:
        nInfo = ICReaderTest_4442(cardInfo, sector, readPos, readLen);
        break;
    default:
        TP_ICReaderStop();
        return;
    }

    if (nInfo == 0)
    {
        return;
    }

    memset(displayInfo, 0x00, sizeof(displayInfo));
    TP_HexToStr(cardInfo, nInfo, displayInfo);
    TP_ICReaderStop();

    {
        TP_DisplayInfo dispInfo;

        dispInfo.align = ALIGN_LEFT;
        dispInfo.FONT  = CFONT;
        dispInfo.LF_KeyValue = TP_KEY_NONE;
        dispInfo.LKeyValue   = KEY_DEMO_CONFIRM;
        dispInfo.RF_KeyValue = TP_KEY_NONE;
        dispInfo.RKeyValue   = TP_KEY_NONE;
        dispInfo.strLButtonTitle = "Confirm";
        dispInfo.strRButtonTitle = PNULL;
        dispInfo.strTitle        = PNULL;

         TP_ShowTextFullScr(&dispInfo, "%s", displayInfo);
    }
}

void MStripReaderTest(void)
{
    uint8  nInfo;
    uint8  iChar;
    uint8  keyCode;
    char   strip1[MSTRIP_READER_STRIP1_MAX_DATA_SIZE + 1];
    char   strip2[MSTRIP_READER_STRIP2_MAX_DATA_SIZE + 1];
    char   strip3[MSTRIP_READER_STRIP3_MAX_DATA_SIZE + 1];

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);

    TP_LcdFreeze();

    TP_ScrGotoxyEx(1, 1);
    TP_LcdPrintf("%s", "   Magnetic Strip");
    TP_ScrGotoxyEx(1, 13);
    TP_LcdPrintf("%s", "  Card Reader Test");

    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%s", "Start           Quit");

    TP_ScrUpdate();

    TP_Kbflush();
    while (TRUE)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();

            if (keyCode == KEY_DEMO_CONFIRM)
            {
                break;
            }
            else if (keyCode == KEY_DEMO_CANCEL)
            {
                TP_Kbflush();

                return;
            }
        }
    }

    TP_MStripReaderStart();

    TP_LcdFreeze();

    TP_ScrClrRect(1, 50, 127, 63);
    TP_ScrGotoxyEx(1, 25);
    TP_LcdPrintf("Please Swiping Card");
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%20s", "Quit");

    TP_ScrUpdate();

    while (TP_MStripReaderDataCheck() == 0xEE)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                TP_Kbflush();
                TP_MStripReaderStop();

                return;
            }
        }
    }

    /* Get data of each strip */
    /* 1st Strip */
    memset(strip1, 0x00, sizeof(strip1));
    TP_MStripReaderReadStrip(1, strip1, sizeof(strip1) - 1);

    /* 2nd Strip */
    memset(strip2, 0x00, sizeof(strip2));
    TP_MStripReaderReadStrip(2, strip2, sizeof(strip2) - 1);

    /* 3rd Strip */
    memset(strip3, 0x00, sizeof(strip3));
    TP_MStripReaderReadStrip(3, strip3, sizeof(strip3) - 1);

    TP_MStripReaderStop();

    /* Display data of each strip*/
    TP_ScrFontSet(ASCII);
    TP_ScrSpaceSet(1, 1);
    
    /* 1st Strip */
    TP_LcdFreeze();
    
    TP_ScrCls();
    TP_ScrGotoxyEx(0, 0);
    TP_LcdPrintf("%s", "1st Strip:");
    TP_ScrGotoxyEx(0, 8);
    TP_LcdPrintf("%s", strip1);
    
    TP_ScrGotoxyEx(0, 57);
    TP_LcdPrintf("Next Strip");

    TP_ScrUpdate();

    while (TRUE)
    {
        if (TP_Kbhit() == 0xFF)
        {
            TP_DbgSerialPrn("1st Strip:%s\r\n", strip1);

            keyCode = TP_GetKey();
            if (keyCode == KEY_DEMO_CONFIRM)
            {
                break;
            }
        }
    }

    /* 2nd Strip */
    TP_LcdFreeze();

    TP_ScrCls();
    TP_ScrGotoxyEx(0, 0);
    TP_LcdPrintf("%s", "2nd Strip:");
    TP_ScrGotoxyEx(0, 8);
    TP_LcdPrintf("%s", strip2);

    TP_ScrGotoxyEx(0, 57);
    TP_LcdPrintf("Next Strip");

    TP_ScrUpdate();

    while (TRUE)
    {
        if (TP_Kbhit() == 0xFF)
        {
            TP_DbgSerialPrn("2nd Strip:%s\r\n", strip2);

            keyCode = TP_GetKey();
            if (keyCode == KEY_DEMO_CONFIRM)
            {
                break;
            }
        }
    }

    /* 3rd Strip */
    TP_LcdFreeze();

    TP_ScrCls();
    TP_ScrGotoxyEx(0, 0);
    TP_LcdPrintf("%s", "Strip 3:");
    TP_ScrGotoxyEx(0, 8);
    TP_LcdPrintf("%s", strip3);

    TP_ScrGotoxyEx(0, 57);
    TP_LcdPrintf("Confirm");

    TP_ScrUpdate();

    while (TRUE)
    {
        if (TP_Kbhit() == 0xFF)
        {
            TP_DbgSerialPrn("3rd Strip:%s\r\n", strip3);

            keyCode = TP_GetKey();
            if (keyCode == KEY_DEMO_CONFIRM)
            {
                TP_Kbflush();

                break;
            }
        }
    }

    // restore
    TP_ScrFontSet(CFONT);
    TP_ScrSpaceSet(0, 0);
}


#endif
/* End Of File*/

