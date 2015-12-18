#include <TPTypedef.h>
#include <TPBase.h>
#include <TPFile.h>
#include <TPLCD.h>
#include <TPKeypad.h>
#include <TPSerial.h>

#include "TP_ListBox.h"
#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"
#include "TP_IdleScreen.h"

#include <TPSMS.h>
#ifdef INCLUDE_MESSAGE_BOX
#include "TP_SmsStore.h"
#include "TP_MsgMenu.h"
#endif
#ifdef INCLUDE_CALL_RECORD
#include "TP_CallRecord.h"
#endif

static char FileName[33];

void TP_FileTest(uint16 itemId)
{
    int32  fd;
    int32  result;
    uint32 iBlock;
    uint32 iBlockCount;
    uint8  paramData[1025];
    uint8  wdata[2049];
    uint32 fileSize;
    TP_ListBoxRect rect = {27,40,77,49};

    TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_ScrCls();
    TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
    TP_SetDisplayArea(2, 2, 125, 61);

    switch (itemId)
    {
    case 103:
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"FS free size:\n%dK", (TP_FSFreeSize() / 1024));
        TP_ScrGotoxy(3, 5);
        TP_LcdPrintf((uint8*)"Press any key to");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"return");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                break;
            }
        }
        break;

    case 104:
        TP_ScrCls();
        TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
        TP_ScrGotoxy(40, 0);
        TP_LcdPrintf((uint8*)"Warnning!!!!");
        TP_ScrGotoxy(3, 2);
        TP_LcdPrintf((uint8*)"All data on FS will be cleared, continue?");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Quit");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                result = TP_GetKey();
                if (result == KEY_DEMO_CONFIRM)
                {
                    result = TP_FSFormat();
                    if (result == TRUE)
                    {
                        TP_ScrCls();
                        TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
                        TP_ScrGotoxy(3, 0);
                        TP_LcdPrintf((uint8*)"Format successfully");
                        //After format FS,some reinitialization is needed
                        #ifdef INCLUDE_MESSAGE_BOX
                            TP_SMSStoreInit();
                        #endif

                        #ifdef INCLUDE_CALL_RECORD
                            TP_CallRecordInit();
                        #endif
                    }
                    else
                    {
                        TP_ScrCls();
                        TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
                        TP_ScrGotoxy(3, 0);
                        TP_LcdPrintf((uint8*)"Format failed");
                    }
                    TP_ScrGotoxy(3, 5);
                    TP_LcdPrintf((uint8*)"Press any key to");
                    TP_ScrGotoxy(3, 6);
                    TP_LcdPrintf((uint8*)"return");
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
                else if (result == TP_KEY_CANCEL)
                {
                    break;
                }
            }
        }
        break;

    case 106:
        TP_ScrCls();
        TP_ScrAttrSet(0);
        TP_ScrFontSet(CFONT);
        TP_SetDisplayArea(0, 0, 127,63);

        TP_LcdFreeze();

        if (TP_GetItemNum(TP_PARAM_FILE_2) == 0)
        {
            TP_ScrGotoxyEx(1, 1);
            TP_LcdPrintf((uint8*)"%s\x00", "Param File 2 is empty!\x00");

            TP_ScrUpdate();

            TP_Kbflush();
            while (TRUE)
            {
                if (TP_Kbhit() == 0xFF)
                {
                    TP_Kbflush();

                    return;
                }
            }
        }

        TP_ScrGotoxyEx(1, 1);
        TP_LcdPrintf((uint8*)"Read Item Num:\x00");

        TP_ScrGotoxyEx(1, 13);
        TP_LcdPrintf((uint8*)"(0~%d)\x00", TP_GetItemNum(TP_PARAM_FILE_2) - 1);

        TP_ScrDrawRect(3, 28, 125, 42);

        TP_ScrUpdate();

        TP_SetDisplayArea(4, 29, 124, 41);
        TP_ScrGotoxyEx(4, 29);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, TP_KEY_NONE, TP_KEY_NONE);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        while (TRUE)
        {
            memset(wdata, 0x00, sizeof(wdata));
            result = TP_GetString((char*)wdata, 0xF4, 1, 4);

            iBlock = atoi((char*)wdata);
            if (iBlock >= TP_GetItemNum(TP_PARAM_FILE_2))
            {
                TP_PopupMessageTimed((uint8*)"!Invalid\nPlease Re-Input", 2000);
                TP_ScrClsDisplayArea();
            }
            else
            {
                break;
            }
        }

        memset(paramData, 0x00, sizeof(paramData));
        result = TP_ReadParam(TP_PARAM_FILE_2, iBlock,
                              paramData, TP_GetItemSize(TP_PARAM_FILE_2, iBlock));
        if (result == TRUE)
        {
            memset(wdata, 0x00, sizeof(wdata));
            TP_HexToStr(paramData, TP_GetItemSize(TP_PARAM_FILE_2, iBlock), wdata);
            {
                TP_DisplayInfo dispInfo;
                TP_ListBoxRect dispArea;

                memset(paramData, 0x00, sizeof(paramData));
                sprintf((char*)paramData, "Item %d, Size %d",
                        iBlock, TP_GetItemSize(TP_PARAM_FILE_2, iBlock));
                dispInfo.align = ALIGN_LEFT;
                dispInfo.FONT  = CFONT;
                dispInfo.LF_KeyValue = TP_KEY_NONE;
                dispInfo.LKeyValue   = KEY_DEMO_CONFIRM;
                dispInfo.RF_KeyValue = TP_KEY_NONE;
                dispInfo.RKeyValue   = TP_KEY_NONE;
                dispInfo.strLButtonTitle = (uint8*)"Confirm";
                dispInfo.strRButtonTitle = PNULL;
                dispInfo.strTitle        = paramData;

                TP_ShowTextFullScr(&dispInfo, (uchar*)"%s", wdata);
            }
        }
        else
        {
            TP_ScrCls();
            TP_ScrGotoxy(3, 2);
            TP_LcdPrintf((uint8*)"Read failed:%d", TP_GetLastError());
            TP_ScrGotoxy(3, 6);
            TP_LcdPrintf((uint8*)"Press any key return");
            TP_Kbflush();
            while (TRUE)
            {
                if (TP_Kbhit() == 0xFF)
                {
                    TP_Kbflush();

                    break;
                }
            }
        }
        break;

    case 107:
        TP_ScrCls();
        TP_ScrAttrSet(0);
        TP_ScrFontSet(CFONT);
        TP_SetDisplayArea(0, 0, 127,63);

        TP_LcdFreeze();

        TP_ScrGotoxyEx(1, 1);
        TP_LcdPrintf((uint8*)"Write Item Num:\x00");

        TP_ScrGotoxyEx(1, 13);
        TP_LcdPrintf((uint8*)"(0~1023)\x00");

        TP_ScrDrawRect(3, 28, 125, 42);

        TP_ScrUpdate();

        TP_SetDisplayArea(4, 29, 124, 41);
        TP_ScrGotoxyEx(4, 29);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, TP_KEY_NONE, TP_KEY_NONE);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        while (TRUE)
        {
            memset(wdata, 0x00, sizeof(wdata));
            result = TP_GetString((char*)wdata, 0xF4, 1, 4);

            iBlock = atoi((char*)wdata);
            if (iBlock >= 1024)
            {
                TP_PopupMessageTimed((uchar*)"!Invalid\nPlease Re-Input", 2000);
                TP_ScrClsDisplayArea();
            }
            else
            {
                break;
            }
        }

        result = 0;
        for (iBlockCount = 0; iBlockCount < iBlock; iBlockCount++)
        {
            result += TP_GetItemSize(TP_PARAM_FILE_2, iBlockCount);
        }

        TP_ScrCls();
        TP_ScrAttrSet(0);
        TP_ScrFontSet(CFONT);
        TP_SetDisplayArea(0, 0, 127,63);

        if ((1024 - result) == 0)
        {
            TP_PopupMessageTimed((uchar*)"No space!", 3 * 1000);

            return;
        }

        TP_LcdFreeze();

        TP_ScrGotoxyEx(1, 1);
        TP_LcdPrintf((uint8*)"Write size:\x00");

        TP_ScrGotoxyEx(1, 13);
        TP_LcdPrintf((uint8*)"(1~%d)\x00", 1024 - result);

        TP_ScrDrawRect(3, 28, 125, 42);

        TP_ScrUpdate();

        TP_SetDisplayArea(4, 29, 124, 41);
        TP_ScrGotoxyEx(4, 29);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, TP_KEY_NONE, TP_KEY_NONE);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        while (TRUE)
        {
            memset(wdata, 0x00, sizeof(wdata));
            TP_GetString((char*)wdata, 0xF4, 1, 4);

            iBlockCount = atoi((char*)wdata);
            if (iBlockCount > (1024 - result))
            {
                TP_PopupMessageTimed((uchar*)"!Invalid\nPlease Re-Input", 2000);
                TP_ScrClsDisplayArea();
            }
            else
            {
                break;
            }
        }

        memset(wdata, 't', iBlockCount);
        result = TP_WriteParam(TP_PARAM_FILE_2, iBlock,
                               wdata, iBlockCount);
        TP_ScrCls();
        if (result == TRUE)
        {
            TP_ScrGotoxy(3, 2);
            TP_LcdPrintf((uint8*)"Write Success");
        }
        else
        {
            TP_ScrGotoxy(3, 2);
            TP_LcdPrintf((uint8*)"Write failed:%d", TP_GetLastError());
        }
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Press any key return");
        TP_Kbflush();
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_Kbflush();
                break;
            }
        }
        break;

    case 109:
        TP_ScrCls();
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Input Filename:");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 16, 125, 31);
        TP_SetDisplayArea(4, 17, 124, 30);
        TP_ScrGotoxyEx(4, 17);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        memset(FileName, 0x00, sizeof(FileName));
        result = TP_GetHzString((char*)FileName, 0, 16);
        TP_SetDisplayArea(0, 0, 127, 63);
        if (result == 0xFF)
        {
            return;
        }

        TP_ScrCls();
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Input File Size(K):");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(3, 16, 125, 31);
        TP_SetDisplayArea(4, 17, 124, 30);
        TP_ScrGotoxyEx(4, 17);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59,52);
        TP_ShowInputNum(0, 1, 1);
        memset(wdata, 0x00, sizeof(wdata));
        result = TP_GetString((char*)wdata, 0xD4, 1, 3);
        TP_SetDisplayArea(0, 0, 127, 63);
        if (result == 0xFF)
        {
            return;
        }

        iBlock = atoi((char*)wdata);
        if (iBlock > (TP_FSFreeSize() / 1024))
        {
            iBlock = (TP_FSFreeSize() / 1024);
        }
        TP_ScrFontSet(CFONT);
        TP_ScrCls();
        TP_ScrGotoxy(3, 0);
        TP_LcdPrintf((uint8*)"Create file %s of %dK size",
                     FileName,
                     iBlock);
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

        iBlockCount = TP_Create(FileName, iBlock * 1024);
        if (iBlockCount == 0)
        {
            TP_ScrClrLine(4, 7);
            TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
            TP_ScrGotoxy(3, 4);
            TP_LcdPrintf((uint8*)"Create failed:%d\n", TP_GetLastError());
            TP_ScrGotoxy(3, 5);
            TP_LcdPrintf((uint8*)"Press any key to");
            TP_ScrGotoxy(3, 6);
            TP_LcdPrintf((uint8*)"return");
            while (TRUE)
            {
                if (TP_Kbhit() == 0xFF)
                {
                    TP_GetKey();
                    break;
                }
            }

            return;
        }

        TP_WriteParam(TP_PARAM_FILE_4, 0, (uint8*)FileName, strlen((char *)FileName));

        TP_ScrClrLine(4, 7);
        TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
        TP_ScrGotoxy(3, 4);
        TP_LcdPrintf((uint8*)"Create successfully");
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

    case 110:
        TP_ScrCls();
        TP_ScrGotoxyEx(0, 0);
        TP_LcdPrintf((uint8*)"%15s", "Write Test");
        TP_ScrGotoxyEx(3, 15);
        TP_LcdPrintf((uint8*)"%s", "Input Filename:");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(0, 28, 127, 43);
        TP_SetDisplayArea(1, 29, 126, 42);
        TP_ScrGotoxyEx(1, 29);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59, 52);
        TP_ShowInputNum(0, 1, 1);
        memset(FileName, 0x00, sizeof(FileName));
        result = TP_GetHzString(FileName, 0, 16);
        TP_SetDisplayArea(0, 0, 127, 63);
        if (result == 0xFF)
        {
            return;
        }

        TP_ScrClrRect(0, 16, 127, 63);
        fd = TP_Open(FileName);
        if (fd < 0)
        {
            TP_ScrGotoxy(3, 2);
            TP_LcdPrintf((uint8*)"Open file failed:%d", TP_GetLastError());
            TP_ScrGotoxy(3, 5);
            TP_LcdPrintf((uint8*)"Press any key to");
            TP_ScrGotoxy(3, 6);
            TP_LcdPrintf((uint8*)"return");
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

        TP_Truncate(fd, 0);

        iBlockCount = (TP_FFreeSize(fd) / 2048);
        TP_ScrGotoxy(3, 2);
        TP_LcdPrintf((uint8*)"Write %c to %s", 0x23, FileName);
        memset(wdata, 0x23, sizeof(wdata));
        TP_Seek(fd, 0, SEEK_FOREWARD, SEEK_SET);
        for (iBlock = 0; iBlock < iBlockCount; iBlock++)
        {
            TP_ScrClrLine(4, 6);
            TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
            TP_ScrGotoxy(3, 4);
            TP_LcdPrintf((uint8*)"%d%%", (iBlock * 100) / (iBlockCount));

            TP_Seek(fd, 0, SEEK_FOREWARD, SEEK_CUR);
            TP_Write(fd, wdata, 2048);
            if (TP_GetLastError() < 0)
            {
                TP_ScrClrLine(4, 7);
                TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
                TP_ScrGotoxy(3, 4);
                TP_LcdPrintf((uint8*)"Write failed:%d\n", TP_GetLastError());
                TP_ScrGotoxy(3, 5);
                TP_LcdPrintf((uint8*)"Press any key to");
                TP_ScrGotoxy(3, 6);
                TP_LcdPrintf((uint8*)"return");
                TP_Close(fd);
                TP_Kbflush();
                while (TRUE)
                {
                    if (TP_Kbhit() == 0xFF)
                    {
                        TP_GetKey();
                        break;
                    }
                }
                return;
            }
        }

        TP_ScrClrLine(4, 7);
        TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
        TP_ScrGotoxy(3, 4);
        TP_LcdPrintf((uint8*)"Write successfully");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Press any key return");
        TP_Close(fd);
        TP_Kbflush();
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                break;
            }
        }
        break;

    case 200:
        TP_ScrCls();
        TP_ScrGotoxyEx(0, 0);
        TP_LcdPrintf((uint8*)"%15s", "Read Test");
        TP_ScrGotoxyEx(3, 15);
        TP_LcdPrintf((uint8*)"%s", "Input Filename:");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(0, 28, 127, 43);
        TP_SetDisplayArea(1, 29, 126, 42);
        TP_ScrGotoxyEx(1, 29);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59, 52);
        TP_ShowInputNum(0, 1, 1);
        memset(FileName, 0x00, sizeof(FileName));
        result = TP_GetHzString(FileName, 0, 16);
        TP_SetDisplayArea(0, 0, 127, 63);
        if (result == 0xFF)
        {
            return;
        }

        TP_ScrClrRect(0, 16, 127, 63);
        fd = TP_Open(FileName);
        if (fd < 0)
        {
            TP_ScrGotoxy(3, 2);
            TP_LcdPrintf((uint8*)"Open file failed:%d", TP_GetLastError());
            TP_ScrGotoxy(3, 5);
            TP_LcdPrintf((uint8*)"Press any key to");
            TP_ScrGotoxy(3, 6);
            TP_LcdPrintf((uint8*)"return");
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

        TP_ScrGotoxy(3, 2);
        TP_LcdPrintf((uint8*)"Verifying...");
        TP_Seek(fd, 0, SEEK_FOREWARD, SEEK_SET);
        fileSize = TP_FSize(fd);
        iBlockCount = fileSize/100;
        for (iBlock = 0; iBlock < fileSize; iBlock++)
        {
            TP_Read(fd, &wdata[0], 1);
            if (iBlock%iBlockCount==0)
            {
                TP_ShowProgressBar(0,iBlock,fileSize);
            }
            if (TP_GetLastError() < 0)
            {
                TP_ScrClrLine(4, 7);
                TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
                TP_ScrGotoxy(3, 4);
                TP_LcdPrintf((uint8*)"Read failed:%d\n", TP_GetLastError());
                TP_ScrGotoxy(3, 5);
                TP_LcdPrintf((uint8*)"Press any key to");
                TP_ScrGotoxy(3, 6);
                TP_LcdPrintf((uint8*)"return");
                TP_Close(fd);
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

            if (wdata[0] != 0x23)
            {
                TP_ScrClrLine(4, 7);
                TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
                TP_ScrGotoxy(3, 4);
                TP_LcdPrintf((uint8*)"Verify error:%d:%#X", iBlock, wdata[0]);
                TP_ScrGotoxy(3, 5);
                TP_LcdPrintf((uint8*)"Press any key to");
                TP_ScrGotoxy(3, 6);
                TP_LcdPrintf((uint8*)"continue");
                while (TRUE)
                {
                    if (TP_Kbhit() == 0xFF)
                    {
                        TP_GetKey();
                        break;
                    }
                }
            }
        }

        TP_Close(fd);
        TP_ScrClrLine(2, 7);
        TP_ScrDrawRect(0, 0, 127, 63);//Draw Rectangular of Window
        TP_ScrGotoxy(3, 2);
        TP_LcdPrintf((uint8*)"Verify end\n");
        TP_ScrGotoxy(3, 5);
        TP_LcdPrintf((uint8*)"Press any key to");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"return");
        while (TRUE)
        {
            if (TP_Kbhit() == 0xFF)
            {
                TP_GetKey();
                break;
            }
        }
        break;

    case 201:
        TP_ScrCls();
        TP_ScrGotoxyEx(0, 0);
        TP_LcdPrintf((uint8*)"%15s", "Delete Test");
        TP_ScrGotoxyEx(3, 15);
        TP_LcdPrintf((uint8*)"%s", "Input Filename:");
        TP_ScrGotoxy(3, 6);
        TP_LcdPrintf((uint8*)"Confirm");
        TP_ScrGotoxy(88, 6);
        TP_LcdPrintf((uint8*)"Cancel");
        TP_ScrDrawRect(0, 28, 127, 43);
        TP_SetDisplayArea(1, 29, 126, 42);
        TP_ScrGotoxyEx(1, 29);
        TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
        TP_SetInputModePosition(59, 52);
        TP_ShowInputNum(0, 1, 1);
        memset(FileName, 0x00, sizeof(FileName));
        result = TP_GetHzString(FileName, 0, 16);
        TP_SetDisplayArea(0, 0, 127, 63);
        if (result == 0xFF)
        {
            return;
        }

        TP_ScrClrRect(0, 16, 127, 63);
        fd = TP_Delete(FileName);
        if (fd == FALSE)
        {
            TP_ScrGotoxy(3, 2);
            TP_LcdPrintf((uint8*)"Delete file failed:%d", TP_GetLastError());
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
            TP_ScrGotoxy(3, 2);
            TP_LcdPrintf((uint8*)"%s has been deleted", FileName);
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
        break;
    }
}

