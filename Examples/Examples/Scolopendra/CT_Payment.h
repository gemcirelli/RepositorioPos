#ifndef CT_PAYMENT
#define CT_PAYMENT
#ifdef __cplusplus
extern "C" {
#endif

typedef enum PAYMENT_STEP_TAG
{
	PAYMENT_INIT,
	PAYMENT_SELECT_COMPANY,
	PAYMENT_ADD_AMOUNT,
	PAYMENT_ADD_BILL_NUMBER,
	PAYMENT_CONFIRM_BILL_NUMBER,
	PAYMENT_ADD_CONFIRMATION_PHONE,
	PAYMENT_CONFIRMATION,
	PAYMENT_START_COMM,
	PAYMENT_REGISTER,
	PAYMENT_CONFIRM_TO_CRYPTO,
	PAYMENT_PRINT,
	PAYMENT_FAIL,
	PAYMENT_SUCCESS,
	PAYMENT_CONFIRM_FAIL,	
	PAYMENT_EXIT
}PAYMENT_STEP;

void CT_Payment();

#ifdef __cplusplus
}
#endif

#endif