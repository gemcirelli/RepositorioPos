#ifndef TB_UPGRADE_DISPLAY_H
#define TB_UPGRADE_DISPLAY_H

/**
 * Upgrading Steps
 */
typedef enum __UpgradeStep
{
    /** Connecting HTTP Server*/
    TB_UPGRADE_STEP_CONNECT = 0,
    /** Downloading from HTTP server*/
    TB_UPGRADE_STEP_DOWNLOAD,
    /** Writing Data to Flash*/
    TB_UPGRADE_STEP_WRITE
} TB_Upgrade_UpgradeStepE;

extern void
tb_upgrade_display_ShowProgress(TB_Upgrade_UpgradeStepE step,
                                uint32 currValue, uint32 maxValue);

extern void
tb_upgrade_display_PromptGetVersion(void);

extern void
tb_upgrade_display_PromptStart(void);

extern void
tb_upgrade_display_PromptCheckingData(void);

extern void
tb_upgrade_display_PromptQuitDownload(void);

extern void
tb_upgrade_display_PromptBeforeUpgrade(void);

extern void
tb_upgrade_display_PromptSuccess(void);

extern void
tb_upgrade_display_PromptFail(void);

extern void
tb_upgrade_display_PromptFinish(void);

extern void
tb_upgrade_display_PromptServiceRegisting(void);

extern void
tb_upgrade_display_PromptError(uint8 result);

extern void
tb_upgrade_display_PromptErrorVersion(void);

extern void
tb_upgrade_display_PromptErrorProtocol(void);

extern void
tb_upgrade_display_PromptCheckingBP(void);

extern void
tb_upgrade_display_PromptInvalidBP(void);

extern void
tb_upgrade_display_PromptClearData(void);

extern uint8
tb_upgrade_display_QueryDoStartNewOne(void);

extern uint8
tb_upgrade_display_QueryDoUseBP(void);

extern uint8
tb_upgrade_display_QueryDoRestart(void);

extern uint8
tb_upgrade_display_QueryDoResume(void);

extern uint8
tb_upgrade_display_QueryDoClearData(void);

extern uint8
tb_upgrade_display_QueryDoCancelDownload(void);

#endif
