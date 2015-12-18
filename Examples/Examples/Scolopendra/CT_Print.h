#ifndef CT_PRINT
#define CT_PRINT
#ifdef __cplusplus
extern "C" {
#endif

#include "CT_Code39.h"
#include "CT_utils.h"

#define CT_DIRECTV 1
#define CT_GRE5 3
#define CT_MOVILNET 219
#define CT_BIGOTT 221



uint8 CT_PrintReceipt(uint8 company, uint8 type, const char * header, const char* title, const char * pin_panel, const char * information, const char * serial, const char * summary);
uint8 CT_PrintReceiptBarcode(const char* title, const char* summary, const char* text, uint8 company, const char* pin);
uint8 CT_PrintCode39(const char* pin);

uint8 CT_PrintReceiptClient(uint8 type, const char * companyName, uint16 companyId, const char * suscriptorNumber, const char * amount, const char * serialNumber, const char * pin, const char * systemTime, const char * program_days, const char * manager_id, const char * pos, const char * user, const char * product_code, const char * client_balance, const char * transaction_id, const char * desconection_date, const char * userCompanyName);
uint8 CT_PrintReceiptCompany(uint8 type, const char * companyName, uint16 companyId, const char * suscriptorNumber, const char * amount, const char * serialNumber, const char * systemTime);
uint8 CT_PrintReceiptClientAndCompany(uint8 type, const char * companyName, uint16 companyId, const char * suscriptorNumber, const char * amount, const char * serialNumber, const char * pin, const char * systemTime, const char * program_days, const char * service_lot, const char * client_balance, const char * transaction_id, const char * desconection_date);

/* Function used to print printer's errors
 * @param cTemp The errror code that returned the print operation
 */
void CT_NotifyPrintError(int32 cTemp);

uint8 CT_PrintCloseOutSummary(char * total_txn_amount, double total_amount, int total_txn, char * system_time);

void CT_PrintLastTransaction();

#ifdef __cplusplus
}
#endif

#endif