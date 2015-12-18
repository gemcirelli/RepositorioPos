#ifndef TP_SIMCARDTEST
#define TP_SIMCARDTEST

#ifdef __cplusplus
extern "C" {
#endif


void TP_ShowIMSIDialDilog(void);
void TP_ShowIMEIDialDilog(void);
Boolean ShowPinOnDialog(void);
Boolean ShowPinOffDialog(void);
Boolean ShowPinChangeDialog(void);
void ShowSimCardStartUPProcess(void);


#ifdef __cplusplus
}
#endif


#endif

