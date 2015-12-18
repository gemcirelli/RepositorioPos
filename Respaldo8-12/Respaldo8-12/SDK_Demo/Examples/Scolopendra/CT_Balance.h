#ifndef CT_BALANCE
#define CT_BALANCE
#ifdef __cplusplus
extern "C" {
#endif

typedef enum BALANCE_STEP_TAG
{
	BALANCE_INIT,
	BALANCE_START_COMM,	
	BALANCE_PRINT_PROMPT,
	BALANCE_PRINT,
	BALANCE_FAIL,
	BALANCE_EXIT,
	
}BALANCE_STEP;

void CT_Balance(void);

#ifdef __cplusplus
}
#endif

#endif