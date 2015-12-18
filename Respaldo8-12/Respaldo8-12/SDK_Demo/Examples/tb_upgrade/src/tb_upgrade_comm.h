#ifndef TB_UPGRADE_COMM_H
#define TB_UPGRADE_COMM_H

/**
 * Type of SMS upgrade replies
 */
typedef enum __UpgradeReplySMSType
{
    /** 升级开始*/
    TB_UPGRADE_START = 0,
    /** 升级准备结束*/
    TB_UPGRADE_BEFOREFINISH,
    /** 升级结束*/
    TB_UPGRADE_FINISH,
    /** 升级失败*/
    TB_UPGRADE_FAILED,
    /** 未升级，协议错误*/
    TB_UPGRADE_ERROR_PROTOCOL,
    /** 未升级，已是最新版本*/
    TB_UPGRADE_ERROR_VERSION,
    /** 上报当前软件版本号*/
    TB_UPGRADE_GETVERSION
} TB_Upgrade_ReplySMSTypeE;

extern void
tb_upgrade_comm_ReplySMS(TB_Upgrade_ReplySMSTypeE type,
                         const char* smeAddr, const char* version);

extern uint8
tb_upgrade_comm_CheckData(const TB_Upgrade_UpgradeInfoT* pInfo,
                          const TB_Upgrade_APNContextT*  pAPNContext);

extern uint8
tb_upgrade_comm_DownloadData(const TB_Upgrade_UpgradeInfoT* pInfo,
                             const TP_UpgradeBreakPointT* pBreakPoint,
                             const TB_Upgrade_APNContextT* pAPNContext);

extern uint8
tb_upgrade_comm_CheckBP(const TB_Upgrade_UpgradeInfoT* pInfo,
                        const TP_UpgradeBreakPointT* pBreakPoint,
                        const TB_Upgrade_APNContextT* pAPNContext);
#endif
