#ifndef CT_CRYPTOMANAGER
#define CT_CRYPTOMANAGER
#ifdef __cplusplus
extern "C" {
#endif
#include <TPTypedef.h>
#include <TPBase.h>
#include "CT_Iso8583.h"

#define SEND_RETRYS 3
#define RECV_RETRYS 3
#define CONNECT_RETRYS 2
#define TIMEOUT 20000

typedef enum CONN_ERR_DEF{
	/* Error creando el socket */
	CONN_FAIL_SOCKET_CREATE = -1, 
	/* Intentos de conexion excedidos */
	CONN_CONNECTION_ATTEMPS_ERR = -2,
	/* Intentos de envios excedidos */
	CONN_SEND_ATTEMPTS_EXCEED = -3,
	/* Intentos de recibir excedidos */
	CONN_RECV_ATTEMPTS_EXCEED = -4,
	/* Conexion finalizada por el servidor */
	CONN_FINISHED = -5
}CONN_ERR;

int32 CT_GetSecurityToken(const char *login, const char *password);
int32 CT_GetBalance();
int32 CT_AddDeposit(uint16 bank_id, const char * controlNumber, const char * amount, char * system_time);
int32 CT_AddRecharge(uint16 companyId, const char * accountNumber, const char * amount, char * serialNumber, char * pin, char * system_time, char * log);
int32 CT_AddRechargeRetry(uint16 companyId, const char * accountNumber, const char * amount, const char * lastSerialNumber, char * serialNumber, char * pin, char * time);
int32 CT_AddPayment(uint16 ompanyId, const char * billNumber, const char * amount, char * serialNumber, char * pin, char * time, char * log);
int32 CT_ConfirmOperation(uint16 companyId, const char * accountNumber, const char * amount, const char * pin, const char * log);
int CT_CloseOutPOS(char * total_txn_out, char * total_amount_out);
int CT_CloseOutConfirm();

int32 sendIso8583toCrypto(iso8583 *isoOut);

void ConfirmOperationIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
accountNumber, const char* amount, const char* posId,  const char* serviceTime, uint8 is_serial, const char * serial, const char * log);

void GetSecTokenIso8583(iso8583 *isoOut, const char *login, const char *password);
void RegisterPaymentIso8583(iso8583 *isoOut, const char* userCompany, const char* user, const char* securityToken, const char * posId, const char* depositBank, 
const char* depositDate, const char* depositControlNumber, const char* depositAmount, const char * serials_to_confirm);
void AddECardIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
accountNumber, const char* amount, const char* posId,  const char* serviceTime, const char * serialsToConfirm);
void AddECardIso8583Retry(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
accountNumber, const char* amount, const char* posId,  const char* serviceTime, uint8 is_lastSerial, const char * lastSerial);
void AddECardNoTopupIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
accountNumber, const char* amount, const char* posId,  const char* serviceTime, const char * serialsToConfirm);
void GetBalanceIso8583(iso8583 *isoOut, const char* userCompany, const char* user, const char* securityToken, const char * posId, const char * serialsToConfirm);
void GetLastSuccessfulTxnIso8583(iso8583 *isoOut, const char* userCompany, const char* user, const char* securityToken, const char* posId);
void CloseOutEposIso8583(iso8583 *isoOut, const char* userCompany, const char* user, const char* securityToken, const char* posId,
const char* closeOutDate, const char* totalTxn, const char* totalAmount, const char* serialsToConfirm);
void CloseOutConfirmEposIso8583(iso8583 * isoOut, char * userCompany, char * userNumber, char * securityToken, char * posId);


#ifdef __cplusplus
}
#endif

#endif