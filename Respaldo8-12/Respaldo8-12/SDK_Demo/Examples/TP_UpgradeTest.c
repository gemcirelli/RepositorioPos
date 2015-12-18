#include <TPTypedef.h>
#include <TPBase.h>
#include <TPLCD.h>
#include <TPKeypad.h>
#include <TP_SingleIME.h>
#include <TPUpgrade.h>
#include <TPFlashParam.h>
#include <TPFile.h>
#include "./TP_DemoDefine.h"
#include "./display/tp_displayapi.h"
#include "tb_upgrade.h"
#include "Scolopendra\CT_Input.h"
#include "Scolopendra\CT_GlobalData.h"

extern COMM_DATA g_commData;

void TP_UpgradeTest_ResultPrompt(uint8 result)
{
    switch (result)
    {
    case 0x01:
        TP_PopupMessageTimed("Error de Inicio de actualizacion\nReiniciar en el proximo encendido", 3000);
        break;

    case 0x02:
        TP_PopupMessageTimed("Error de conexion al servidor\nReiniciar en el proximo encendido", 3000);
        break;

    case 0x03:
        TP_PopupMessageTimed("El archivo no existe\nChequear la URL", 3000);
        TB_Upgrade_UpgradeCancel();
        break;

    case 0x04:
        TP_PopupMessageTimed("La descarga fallo\nReiniciar en el proximo encendido", 3000);
        break;

    case 0x05:
        TP_PopupMessageTimed("Fallo escritura a memoria flash\nReiniciar en el proximo encendido", 3000);
        break;

    case 0x06:
        TP_PopupMessageTimed("La actualizacion reiniciara en el proximo encendido", 3000);
        break;

    case 0x07:
        TP_PopupMessageTimed("Datos de actualizacion invalidos\nChequear la URL", 3000);
        TB_Upgrade_UpgradeCancel();
        break;

    case 0x08:
        TP_PopupMessageTimed("La actualizacion del software no es para este product!", 3000);
        TB_Upgrade_UpgradeCancel();
        break;

    case 0x09:
        TP_PopupMessageTimed("La actualizacion del software no es para este terminal!", 3000);
        TB_Upgrade_UpgradeCancel();
        break;

    case 0x0A:
        TP_PopupMessageTimed("La ultima version se encuentra instalada!", 3000);
        TB_Upgrade_UpgradeCancel();
        break;

    case 0x0B:
        TP_PopupMessageTimed("La actualizacion del software no es para esta version!", 3000);
        TB_Upgrade_UpgradeCancel();
        break;
    }
}

void TP_UpgradeTest(void)
{
    TB_Upgrade_UpgradeInfoT upgradeInfo;
	TB_Upgrade_APNContextT AppAPNContextNovared;
    uint8  result;
    char   upgradeFile[101];
    char   serverIP[50];
    uint16 serverPort;

	if ((TP_FExist( "LOG" ) == TRUE) || (TP_FExist( "LOG_OWNER" ) == TRUE)) {
		CT_Notify("Debe realizar cierre del POS antes de actualizar",0);
	}
	else {
		memset((void*)&upgradeInfo, 0x00, sizeof(TB_Upgrade_UpgradeInfoT));
		sprintf(serverIP, "%s", "190.216.246.77");
		serverPort = 46015;
		sprintf(upgradeFile, "%s", "tp/APP.bin");
		sprintf(upgradeInfo.fileURL,
				"https://%s:%d/%s",
				serverIP,
				serverPort,
				upgradeFile);

		sprintf(AppAPNContextNovared.apn, "%s", g_commData.apn);
		sprintf(AppAPNContextNovared.password, "%s", g_commData.password);
		sprintf(AppAPNContextNovared.username, "%s", g_commData.userName);

		upgradeInfo.type = TB_UPGRADE_TYPE_UPGRADE;
		
		result = TB_Upgrade_ManualUpgrade(&upgradeInfo, &AppAPNContextNovared);

		TP_UpgradeTest_ResultPrompt(result);
	}
}
/*End Of File*/
