#include <TPTypedef.h>
#include <TPBase.h>
#include <TPLCD.h>
#include <TPKeypad.h>
#include <TP_SingleIME.h>
#include <TPUpgrade.h>
#include <TPFlashParam.h>

#include "./TP_DemoDefine.h"

#include "./display/tp_displayapi.h"
#include "tb_upgrade.h"


void TP_UpgradeTest_ResultPrompt(uint8 result)
{
    switch (result)
    {
    case 0x01:
        TP_PopupMessageTimed("Upgrade start error\nRestart next power-on", 3000);
        break;

    case 0x02:
        TP_PopupMessageTimed("Server connect error\nRestart next power-on", 3000);
        break;

    case 0x03:
        TP_PopupMessageTimed("File not exist\nCheck the URL", 3000);
        TB_Upgrade_UpgradeCancel();
        break;

    case 0x04:
        TP_PopupMessageTimed("Download failed\nRestart next power-on", 3000);
        break;

    case 0x05:
        TP_PopupMessageTimed("Write flash failed\nRestart next power-on", 3000);
        break;

    case 0x06:
        TP_PopupMessageTimed("Upgrade will restart next power-on", 3000);
        break;

    case 0x07:
        TP_PopupMessageTimed("Upgrade data invalid\nCheck the URL", 3000);
        TB_Upgrade_UpgradeCancel();
        break;

    case 0x08:
        TP_PopupMessageTimed("Software to upgrade is not for this product!", 3000);
        TB_Upgrade_UpgradeCancel();
        break;

    case 0x09:
        TP_PopupMessageTimed("Software to upgrade is not for this terminal!", 3000);
        TB_Upgrade_UpgradeCancel();
        break;

    case 0x0A:
        TP_PopupMessageTimed("The latest version already!", 3000);
        TB_Upgrade_UpgradeCancel();
        break;

    case 0x0B:
        TP_PopupMessageTimed("Software to upgrade is not for this version!", 3000);
        TB_Upgrade_UpgradeCancel();
        break;
    }
}

void TP_UpgradeTest(void)
{
    TB_Upgrade_UpgradeInfoT upgradeInfo;
    uint8  result;
    char   upgradeFile[101];
    char   serverIP[50];
    uint16 serverPort;

    TP_ScrAttrSet(0);
    TP_ScrClsPrint(1);
    TP_ScrFontSet(ASCII);
    TP_ScrSpaceSet(1, 1);
    TP_SetDisplayArea(0, 0, 127, 63);

    TP_LcdFreeze();

    TP_ScrCls();

    TP_ShowInputNum(0, 128, 64);

    TP_ScrGotoxy(0, 0);
    TP_LcdPrintf("%18s", "Manual Upgrade");

    TP_ScrGotoxy(0, 2);
    TP_LcdPrintf("Input filename:");

    TP_ScrGotoxy(0, 7);
    TP_LcdPrintf("%s%14s", "Upgrade", "Exit");

    TP_SetInputModePosition(54, 56);
    TP_SetInputMode(INPUTMODE_UPERCASE);
    TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);

    TP_ScrDrawRect(0, 24, 127, 54);
    TP_SetDisplayArea(2, 26, 125, 52);
    TP_ScrGotoxyEx(2, 26);

    TP_ScrUpdate();

    memset(upgradeFile, 0x00, sizeof(upgradeFile));
    result = TP_GetHzString(upgradeFile, 0, 50);
    if (result != 0xFB)
    {
        return;
    }

    memset((void*)&upgradeInfo, 0x00, sizeof(TB_Upgrade_UpgradeInfoT));

    if (TP_FlashParam_Read("UpgradeServerIP",
                           VAL_STRING,
                           serverIP,
                           sizeof(serverIP)) <= 0)
    {
        sprintf(serverIP, "%s", "121.9.230.130");
    }

    if (TP_FlashParam_Read("UpgradeServerPort",
                           VAL_INTERGER,
                           &serverPort,
                           sizeof(serverPort)) <= 0)
    {
        serverPort = 8104;
    }

    if (TP_FlashParam_Read("UpgradeServerSMSAddress",
                           VAL_STRING,
                           upgradeInfo.serverSMEAddr,
                           sizeof(upgradeInfo.serverSMEAddr)) <= 0)
    {
        upgradeInfo.serverSMEAddr[0] = 0;
    }

    sprintf(upgradeInfo.fileURL,
            "http://%s:%d/%s",
            serverIP,
            serverPort,
            upgradeFile);

    upgradeInfo.type = TB_UPGRADE_TYPE_UPGRADE;

    result = TB_Upgrade_ManualUpgrade(&upgradeInfo, &AppAPNContext);

    TP_UpgradeTest_ResultPrompt(result);
}

/*End Of File*/
