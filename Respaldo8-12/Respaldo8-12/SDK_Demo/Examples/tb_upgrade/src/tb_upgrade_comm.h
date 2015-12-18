#ifndef TB_UPGRADE_COMM_H
#define TB_UPGRADE_COMM_H

/**
 * Type of SMS upgrade replies
 */
typedef enum __UpgradeReplySMSType
{
    /** ������ʼ*/
    TB_UPGRADE_START = 0,
    /** ����׼������*/
    TB_UPGRADE_BEFOREFINISH,
    /** ��������*/
    TB_UPGRADE_FINISH,
    /** ����ʧ��*/
    TB_UPGRADE_FAILED,
    /** δ������Э�����*/
    TB_UPGRADE_ERROR_PROTOCOL,
    /** δ�������������°汾*/
    TB_UPGRADE_ERROR_VERSION,
    /** �ϱ���ǰ����汾��*/
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
