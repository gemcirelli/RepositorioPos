#ifndef TB_UPGRADE_H/* Include Guard Start */
#define TB_UPGRADE_H

/******************************************************************************
* Manifest Constants
*******************************************************************************/
#define TB_UPGRADE_API_VERSION "2"

#define TB_UPGRADE_MAX_APN_LEN 50

#if defined(TPS300)
#define UPGRADE_KEY_CONFIRM TP_KEY_MENU
#define UPGRADE_KEY_CANCEL  TP_KEY_CANCEL
#define UPGRADE_KEY_OK      TP_KEY_OK
#define UPGRADE_KEY_POUND   TP_KEY_FUNC
#elif defined(TP_PUBLIC_G)
#define UPGRADE_KEY_CONFIRM TP_KEY_CONFIRM
#define UPGRADE_KEY_CANCEL  TP_KEY_CANCEL
#define UPGRADE_KEY_OK      TP_KEY_F1
#define UPGRADE_KEY_POUND   TP_KEY_POUND
#else
#define UPGRADE_KEY_CONFIRM TP_KEY_CONFIRM
#define UPGRADE_KEY_CANCEL  TP_KEY_CANCEL
#define UPGRADE_KEY_OK      TP_KEY_OK
#define UPGRADE_KEY_POUND   TP_KEY_POUND
#endif

/******************************************************************************
* Typedefs
*******************************************************************************/
/** APN Settings to create PPP connect */
typedef struct __APNContext
{
    char apn[TB_UPGRADE_MAX_APN_LEN + 1];
    char username[TB_UPGRADE_MAX_APN_LEN + 1];
    char password[TB_UPGRADE_MAX_APN_LEN + 1];
} TB_Upgrade_APNContextT;

/**
 * Type of Upgrade
 */
typedef enum __UpgradeType
{
    /** Ask terminal to upgrade software*/
    TB_UPGRADE_TYPE_UPGRADE = 0,
    /** Ask terminal to report software version of terminal*/
    TB_UPGRADE_TYPE_GETVERSION
} TB_Upgrade_UpgradeTypeE;

/**
 * Upgrading Settings
 */
typedef struct __UpgradeInfo
{
    /** URL of upgrade file */
    char          fileURL[513];
    /** SME address of upgrade center */
    char          serverSMEAddr[33];
    /** Software version to upgrade */
    char          destVersion[21];
    /** Upgrade Type */
    TB_Upgrade_UpgradeTypeE type;
} TB_Upgrade_UpgradeInfoT;

extern const TB_Upgrade_APNContextT AppAPNContext;
/******************************************************************************
* Public Functions
*******************************************************************************/
extern uint8
TB_Upgrade_UpgradeCheck(const TB_Upgrade_APNContextT* pAPNContext);

extern void
TB_Upgrade_UpgradeCancel(void);

extern Boolean
TB_Upgrade_IsUpgradeSMS(const uint8* pShortMsg, uint16 lenShortMsg,
                        const char* pSMEAddr, uint8 lenSMEAddr,
                        TB_Upgrade_UpgradeInfoT* pInfoBuf);

extern uint8
TB_Upgrade_SMSUpgrade(const TB_Upgrade_UpgradeInfoT* pInfo,
                      const TB_Upgrade_APNContextT* pAPNContext);

extern uint8
TB_Upgrade_ManualUpgrade(const TB_Upgrade_UpgradeInfoT* pInfo,
                         const TB_Upgrade_APNContextT* pAPNContext);


#endif
