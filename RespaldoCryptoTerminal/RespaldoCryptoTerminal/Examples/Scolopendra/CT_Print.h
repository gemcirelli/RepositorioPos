#ifndef CT_PRINT
#define CT_PRINT
#ifdef __cplusplus
extern "C" {
#endif

#include "CT_Code39.h"
#include "CT_utils.h"

#define CT_DIRECTV 1
#define CT_MOVILNET 219
#define CT_BIGOTT 221



uint8 CT_PrintReceipt(const char* title, const char* summary, const char* text, uint8 company, uint8 type);
uint8 CT_PrintReceiptBarcode(const char* title, const char* summary, const char* text, uint8 company, const char* pin);
uint8 CT_PrintCode39(const char* pin);

uint8 CT_PrintReceiptClient(uint8 type, const char * companyName, uint16 companyId, const char * suscriptorNumber, const char * amount, const char * serialNumber, const char * pin, const char * systemTime);
uint8 CT_PrintReceiptCompany(uint8 type, const char * companyName, uint16 companyId, const char * suscriptorNumber, const char * amount, const char * serialNumber, const char * systemTime);
uint8 CT_PrintReceiptClientAndCompany(uint8 type, const char * companyName, uint16 companyId, const char * suscriptorNumber, const char * amount, const char * serialNumber, const char * pin, const char * systemTime);

void CT_PrintLastTransaction();

#ifdef __cplusplus
}
#endif

#endif