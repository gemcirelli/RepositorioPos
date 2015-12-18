#ifndef CT_RECHARGE
#define CT_RECHARGE
#ifdef __cplusplus
extern "C" {
#endif

typedef enum RECHARGE_STEP_TAG
{
	RECHARGE_INIT,
	RECHARGE_SELECT_COMPANY,
	RECHARGE_SELECT_AMOUNT,
	RECHARGE_ADD_SUSC_NUMBER,
	RECHARGE_CONFIRM_SUSC_NUMBER,
	RECHARGE_ADD_CONFIRMATION_PHONE,
	RECHARGE_CONFIRMATION,
	RECHARGE_START_COMM,
	RECHARGE_REGISTER,
	RECHARGE_CONFIRM_TO_CRYPTO,
	RECHARGE_PRINT, 
	RECHARGE_FAIL,
	RECHARGE_SUCCESS,
	RECHARGE_CONFIRM_FAIL,
	RECHARGE_EXIT,
	RECHARGE_RETRY_COMM
}RECHARGE_STEP;

void CT_Recharge();

#ifdef __cplusplus
}
#endif

#endif