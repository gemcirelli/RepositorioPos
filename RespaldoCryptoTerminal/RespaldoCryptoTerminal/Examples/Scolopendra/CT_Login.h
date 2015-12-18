#ifndef CT_LOGIN
#define CT_LOGIN
#ifdef __cplusplus
extern "C" {
#endif

typedef enum LOGIN_STEP_TAG
{
	LOGIN_INIT,
	LOGIN_ENTER_USERNAME,
	LOGIN_ENTER_PASSWORD,
	LOGIN_START_COMM,	
	LOGIN_FAIL,
	LOGIN_SUCCEED,
	LOGIN_EXIT,
	
}LOGIN_STEP;

void CT_Login(void);

#ifdef __cplusplus
}
#endif

#endif