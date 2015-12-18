#ifndef TP_PHONE
#define TP_PHONE
#ifdef __cplusplus
extern "C" {
#endif


void TP_ShowDialDilog(uint8 *Phone);
void TP_AutoDialTest(void);
void TP_PhoneHookUp(void);
void TP_PhoneHungUp(void);
void TP_ShowIncomingDilog(void);

#ifdef __cplusplus
}
#endif

#endif


