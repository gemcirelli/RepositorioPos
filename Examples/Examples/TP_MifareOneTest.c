#if defined(CARD_READER_MODULE_ON)
#if defined(WITH_NFC_CARD_READER)

#include <TPTypedef.h>
#include <TPBase.h>
#include <TPFile.h>
#include <TPLCD.h>
#include <TPKeypad.h>
#include <TPSerial.h>
#include <TP_DemoDefine.h>
#include <TP_Singleime.h>
#include "TP_DisplayAPI.h"
#include "TP_ListBox.h"

#include <TPMifareOne.h>

/**
 * Mifare One NFC card demo
 * @author  Liuwl
 * @version 1.0
 * @date 20130608
 * @note  for more information refer to Mifare one datasheet
 */


/*
 *  private variable
 */
static uint8 PassWord[6];
static uint8 UID[10];
static uint8 blockbuf[16];
static uint8 dispbuf[50];
static uint8 isModuleOpened = 0;

/*
 *  private function
 */
static void disp_module_not_open(void)
{
    uint8 keyCode;

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);
    TP_ScrGotoxyEx(1, 10);
    TP_LcdPrintf("%s", "Module is not opened!");
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%21s", "Quit");
    while (1)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            TP_Kbflush();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                return;
            }
        }
    }
}

static uint8 input_block_number(uint8 minblock, uint8 maxblock)
{
    uint8 inputString[5];
    uint8 keyCode, block;

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);

    TP_LcdFreeze();
    TP_ScrGotoxyEx(1, 10);
    TP_LcdPrintf("Input Block:(%d~%d)", minblock, maxblock);
    TP_ScrDrawRect(3, 28, 125, 42);
    TP_ScrUpdate();
    TP_SetDisplayArea(4, 29, 124, 41);
    TP_ScrGotoxyEx(4, 29);
    TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, TP_KEY_NONE);
    TP_SetInputModePosition(59,52);
    TP_ShowInputNum(0, 1, 1);
    while (TRUE)
    {
        memset(inputString, 0x00, sizeof(inputString));
        keyCode = TP_GetString((char*)inputString, 0xF4, 1, 2);
        // °´cancel ¼üÖ±½oiË³ö
        if (keyCode == 0XFF)
        {
            return 0xff;
        }

        block = atoi((char*)inputString);
        if (block > maxblock)
        {
            TP_PopupMessageTimed((uint8*)"Invalid! Please Re-Input", 2000);
            TP_ScrClsDisplayArea();
        }
        else if ((minblock>0)&&(block < minblock))
        {
            TP_PopupMessageTimed((uint8*)"Invalid! Please Re-Input", 2000);
            TP_ScrClsDisplayArea();
        }
        else
        {
            break;
        }
    }

    return block;
}


static uint8 input_data(uint8 maxbyte, uint8* title, uint8* inputString, uint8 buflen)
{
    TP_DisplayInfo displayInfo;
    TP_InputInfo inputInfo;
    uint8 keyCode;

    displayInfo.FONT = CFONT;
    displayInfo.LF_KeyValue = KEY_DEMO_CONFIRM;
    displayInfo.LKeyValue = KEY_DEMO_CONFIRM;
    displayInfo.RKeyValue = KEY_DEMO_CANCEL;
    displayInfo.RF_KeyValue = KEY_DEMO_CANCEL;
    displayInfo.strTitle = title;
    displayInfo.align = ALIGN_RIGHT;
    displayInfo.strLButtonTitle = (uint8*)"Confirm";
    displayInfo.strRButtonTitle = (uint8*)"Cancel";

    inputInfo.defaultMethod = INPUTMODE_NUMBER;
    inputInfo.maxLen = maxbyte;
    inputInfo.minLen = 1;
    inputInfo.showIMIcon = TRUE;
    inputInfo.showInputNumber = TRUE;
    inputInfo.inputMode = 0xF4;
    inputInfo.timeoutInMS = 0;

    memset(inputString, 0x00, buflen);
    keyCode = TP_NumberInput(&displayInfo,&inputInfo, inputString);

    return keyCode;
}

static uint8 input_password(void)
{
    // ±ÜÃâÃ¿´Î¶¼ÊäÈë£¬±¾demo½«ÊäÈëÃÜÂë½çÃæ·ÅÔu
    // Ò»¸ö¶Àa¢µÄn¡Ïî
    return 0x0;
}


static void MifareOneTest_Open(void)
{
    uint8 retval, keyCode;

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%21s", "Quit");

    retval = TP_M1CardOpen(ISO14443_TYPEA);
    if (retval == 0x00)
    {
        TP_ScrGotoxyEx(1, 5);
        TP_LcdPrintf("%s", "MifareOne Open OK");

        isModuleOpened = 1;
    }
    else if (retval == 0xFE)
    {
        TP_ScrGotoxyEx(1, 5);
        TP_LcdPrintf("%s", "MifareOne Open Fail");
        TP_ScrGotoxyEx(1, 18);
        TP_LcdPrintf("Module has already opened");
    }
    else
    {
        TP_ScrGotoxyEx(1, 5);
        TP_LcdPrintf("%s", "MifareOne Open Fail");
        TP_ScrGotoxyEx(1, 18);
        TP_LcdPrintf("Error code : 0x%x", retval);
    }

    while (1)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            TP_Kbflush();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                return;
            }
        }
    }
}

static void MifareOneTest_Close(void)
{
    uint8 retval, keyCode;

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%21s", "Quit");

    retval = TP_M1CardClose();
    if (retval == 0x00)
    {
        TP_ScrGotoxyEx(1, 25);
        TP_LcdPrintf("%s", "MifareOne Close OK");

        isModuleOpened = 0;
    }
    else
    {
        TP_ScrGotoxyEx(1, 5);
        TP_LcdPrintf("%s", "MifareOne Close Fail");
        TP_ScrGotoxyEx(1, 18);
        TP_LcdPrintf("Error code : 0x%x", retval);
    }

    while (1)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            TP_Kbflush();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                return;
            }
        }
    }
}

static void MifareOneTest_readid(void)
{
    uint8 retval = 0, keyCode;

    if (isModuleOpened == 0)
    {
        disp_module_not_open();
        return;
    }

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);
    TP_ScrGotoxyEx(1, 10);
    TP_LcdPrintf("%s", "Please put mifare one card on the field in 10s ...");
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%21s", "Quit");

    //  ³¢ÊÔ 10s
    TP_TimerSet(0, 10000);
    while (TP_TimerCheck(0) != 0)
    {
        retval = TP_M1CardActive(UID, 4);
        if (retval == 0)
        {
            break;
        }
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            TP_Kbflush();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                return;
            }
        }
    }

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%21s", "Quit");

    if (retval == 0x00)
    {
        TP_ScrGotoxyEx(1, 25);
        TP_LcdPrintf("%s", "MifareOne Active OK");
        TP_ScrGotoxyEx(1, 38);

        memset(dispbuf, 0, sizeof(dispbuf));
        // ×¢ÒâÊä³ö»º³å±ØÐë´oou 2±¶¼o1
        tp_8bit_bytes_to_hex_string(UID, 4, (char*)dispbuf, 10);
        TP_LcdPrintf("UID : %s", dispbuf);
    }
    else
    {
        TP_ScrGotoxyEx(1, 25);
        TP_LcdPrintf("%s", "MifareOne Active Fail");
        TP_ScrGotoxyEx(1, 38);
        TP_LcdPrintf("Error code : 0x%x", retval);
    }

    while (1)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            TP_Kbflush();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                return;
            }
        }
    }
}


static void MifareOneTest_ReadABlock(void)
{
    uint8 retval = 0, keyCode, block, passwderr;

    if (isModuleOpened == 0)
    {
        disp_module_not_open();
        return;
    }

    // ÏÈÊäÈëÄÄ¸öblock
    block = input_block_number(0, 63);
    if (block == 0xff)
    {
        return;
    }

    // use default password in the demo
    if (input_password() == 0xff)
    {
        return;
    }

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%21s", "Quit");
    TP_ScrGotoxyEx(1, 25);
    TP_LcdPrintf("%s", "Reading Card In 10s...");

    TP_TimerSet(0, 10000);
    passwderr = 0;
    while (TP_TimerCheck(0) != 0)
    {
        retval = TP_M1CardActive(UID, 4);
        if (retval == 0)
        {
            // active success then authentication use KEYA password
            retval = TP_M1CardAuthenticate(block, PassWord, 6, M1_KEYA);
            if (retval == 0)
            {
                // password ok
                retval = TP_M1CardRead(block, blockbuf, 16);
                if (retval == 0)
                {
                    // read success
                    break;
                }
            }
            else
            {
                passwderr++;
                if (passwderr > 2)
                {
                    // PASSWORD ERROR
                    retval = 0x04;
                    break;
                }
            }
        }

        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            TP_Kbflush();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                return;
            }
        }
    }

    TP_ScrCls();
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%21s", "Quit");

    if (retval == 0x00)
    {
        TP_ScrGotoxyEx(1, 5);
        TP_LcdPrintf("%s", "MifareOne Read OK");

        TP_ScrGotoxyEx(1, 15);
        memset(dispbuf, 0, sizeof(dispbuf));
        tp_8bit_bytes_to_hex_string(blockbuf, 16, (char*)dispbuf, sizeof(dispbuf));
        TP_LcdPrintf("block %d : %s", block, dispbuf);
    }
    else if (retval == 0x04)
    {
        TP_ScrGotoxyEx(1, 5);
        TP_LcdPrintf("%s", "MifareOne Read Fail");
        TP_ScrGotoxyEx(1, 18);
        TP_LcdPrintf("Password Error!");
    }
    else
    {
        TP_ScrGotoxyEx(1, 5);
        TP_LcdPrintf("%s", "MifareOne Read Fail");
        TP_ScrGotoxyEx(1, 18);
        TP_LcdPrintf("Error code : 0x%x", retval);
    }

    while (1)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            TP_Kbflush();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                return;
            }
        }
    }
}


static void MifareOneTest_WriteABlock(void)
{
    uint8 retval = 0, keyCode, block, passwderr;
    uint8 nByte;

    if (isModuleOpened == 0)
    {
        disp_module_not_open();
        return;
    }

    block = input_block_number(1, 63);
    if (block == 0xff)
    {
        return;
    }

    // check
    if ((block%4) == 3)
    {
        TP_PopupMessageTimed("Not allow to write to Sector Trailer", 2000);
        return;
    }

    // Write only 0~9 digi in this demo
    memset(dispbuf, 0, sizeof(dispbuf));
    keyCode = input_data(32, (uint8*)"Input 16byte data:", dispbuf, sizeof(dispbuf));
    if (keyCode == 0XFF)
    {
        return;
    }
    nByte = tp_hex_string_to_8bit_bytes((char*)dispbuf, 32, blockbuf, 16);
    if (nByte != 16)
    {
        TP_PopupMessageTimed((uint8*)"Input Error!", 2000);
        return;
    }

    // use default password in the demo
    if (input_password() == 0xff)
    {
        return;
    }

    // write
    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%21s", "Quit");
    TP_ScrGotoxyEx(1, 25);
    TP_LcdPrintf("%s", "Writing Card ...");

    TP_TimerSet(0, 10000);
    passwderr = 0;
    while (TP_TimerCheck(0) != 0)
    {
        retval = TP_M1CardActive(UID, 4);
        if (retval == 0)
        {
            // active success then authentic KEYA password
            retval = TP_M1CardAuthenticate(block, PassWord, 6, M1_KEYA);
            if (retval == 0)
            {
                // password ok
                retval = TP_M1CardWrite(block, blockbuf, 16);
                if (retval == 0)
                {
                    // read success
                    break;
                }
            }
            else
            {
                passwderr++;
                if (passwderr > 2)
                {
                    // PASSWORD ERROR
                    retval = 0x04;
                    break;
                }
            }
        }

        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            TP_Kbflush();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                return;
            }
        }
    }

    TP_ScrCls();
    TP_ScrGotoxyEx(1, 50);
    TP_LcdPrintf("%21s", "Quit");

    if (retval == 0x00)
    {
        TP_ScrGotoxyEx(1, 5);
        TP_LcdPrintf("%s", "Write OK");

        TP_ScrGotoxyEx(1, 15);
        memset(dispbuf, 0, sizeof(dispbuf));
    }
    else if (retval == 0x04)
    {
        TP_ScrGotoxyEx(1, 5);
        TP_LcdPrintf("%s", "Write Fail");
        TP_ScrGotoxyEx(1, 18);
        TP_LcdPrintf("Password Error!");
    }
    else
    {
        TP_ScrGotoxyEx(1, 5);
        TP_LcdPrintf("%s", "Write Fail");
        TP_ScrGotoxyEx(1, 18);
        TP_LcdPrintf("Error code : 0x%x", retval);
    }

    while (1)
    {
        if (TP_Kbhit() == 0xFF)
        {
            keyCode = TP_GetKey();
            TP_Kbflush();
            if (keyCode == KEY_DEMO_CANCEL)
            {
                return;
            }
        }
    }
}


void MifareOneTest_SetPassword(void)
{
    uint8 pwdbuf[10];
    uint8 retval;
    uint32 nByte;
    TP_DisplayInfo displayInfo;
    TP_InputInfo inputInfo;

    TP_ScrCls();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_SetDisplayArea(0, 0, 127,63);

    TP_LcdFreeze();
    TP_ScrGotoxyEx(1, 1);
    TP_LcdPrintf("%s", "");
    TP_ScrDrawRect(3, 15, 125, 42);
    TP_ScrUpdate();
    TP_SetDisplayArea(4, 16, 124, 41);
    TP_ScrGotoxyEx(4, 16);
    TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, TP_KEY_NONE);
    TP_SetInputModePosition(59,52);
    TP_ShowInputNum(0, 1, 1);

    memset(dispbuf, 0x00, sizeof(dispbuf));
    // ×¢Òâ£¬ÃÜÂëÎª6¸ö×Ö½u£¬ÊäÈëÊ±o¦¸ÃÎª12¸ö×Ö·û
    displayInfo.FONT = CFONT;
    displayInfo.LF_KeyValue = KEY_DEMO_CONFIRM;
    displayInfo.LKeyValue = KEY_DEMO_CONFIRM;
    displayInfo.RKeyValue = KEY_DEMO_CANCEL;
    displayInfo.RF_KeyValue = KEY_DEMO_CANCEL;
    displayInfo.strTitle = "Input Password:";
    displayInfo.align = ALIGN_RIGHT;
    displayInfo.strLButtonTitle = (uint8*)"Confirm";
    displayInfo.strRButtonTitle = (uint8*)"Cancel";

    inputInfo.minLen = 12;
    inputInfo.maxLen = 12;
    inputInfo.showInputNumber = TRUE;
    inputInfo.defaultMethod = INPUTMODE_NUMBER;
    inputInfo.inputMode = 0xD407;
    inputInfo.timeoutInMS = 0;
    inputInfo.showIMIcon = TRUE;
    retval = TP_PassWordInput(&displayInfo, &inputInfo, '*', dispbuf);
    if (0xFB != retval)
    {
        return;
    }

    nByte = tp_hex_string_to_8bit_bytes((char*)dispbuf, 12, pwdbuf, 6);
    if (nByte == 6)
    {
        TP_PopupMessageTimed((uint8*)"Set password success!", 2000);
        memcpy(PassWord, pwdbuf, sizeof(PassWord));
    }
    else
    {
        TP_PopupMessageTimed((uint8*)"Input Error!", 2000);
    }
}

//
void TP_MifareOneTest(void)
{
    TP_ListBox*  listbox;
    TP_ListBoxRect rect;

    rect.left = 0;
    rect.top = 0;
    rect.right = 127;
    rect.bottom = 63;
    listbox = TP_NewListBox(TL_LIST, rect, "MifareOne Test", "                Exit",PNULL);
    listbox->ConfirmKey = KEY_DEMO_CONFIRM;
    listbox->CancelKey = KEY_DEMO_CANCEL;
    TP_ListBoxAddItem(listbox, "Module On",     1, 1,NULL);
    TP_ListBoxAddItem(listbox, "Module Off",    2, 0,NULL);
    TP_ListBoxAddItem(listbox, "Read Card UID", 3, 0,NULL);
    TP_ListBoxAddItem(listbox, "Read A Block",  4, 0,NULL);
    TP_ListBoxAddItem(listbox, "Write A Block", 5, 0,NULL);
    TP_ListBoxAddItem(listbox, "Set Password",  6, 0,NULL);

    // Ä¬ÈÏÃ»oÐ´ò¿ªeè±¸
    isModuleOpened = 0;
    // eèÖÃÄ¬ÈÏÃÜÂë
    memset(PassWord, 0xff, sizeof(PassWord));

    while (TRUE)
    {
        if (TP_ShowListBox(listbox))
        {
            switch (listbox->SelectItemValue[0])
            {
            case 1:
                MifareOneTest_Open();
                break;
            case 2:
                MifareOneTest_Close();
                break;
            case 3:
                MifareOneTest_readid();
                break;
            case 4:
                MifareOneTest_ReadABlock();
                break;
            case 5:
                MifareOneTest_WriteABlock();
                break;
            case 6:
                MifareOneTest_SetPassword();
                break;
            default:
                break;
            }
        }
        else
        {
            //  CANCEL °´¼ü iË³ö
            // iË³öÇ°ÏÈ¹Ø±Õeè±¸
            TP_M1CardClose();
            break;
        }
    }

    TP_ListBoxFree(listbox);
}

#endif
#endif

