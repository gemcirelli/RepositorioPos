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
#define TIMEOUT 25000

typedef enum SENDISO_STEP_TAG
{
	SENDISO_INIT,
	SENDISO_LOGIN_APN,
	SENDISO_START_COMM,
	SENDISO_SEND_DATA,
	SENDISO_RECV_DATA,
	SENDISO_SUCCESS,
	SENDISO_FAIL,
	SENDISO_EXIT
	
}SENDISO_STEP;

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
	CONN_FINISHED = -5,
	/* Error al establecer el canal SSL con el servidor*/
	CONN_FAIL_SSL_AGREEMENT = -6, 
}CONN_ERR;

int32 CT_GetSecurityToken(const char *login, const char *password);
int32 CT_ChangePassword(const char *login, const char *password, const char *new_password);
int32 CT_GetBalance();
int32 CT_AddDeposit(uint16 bank_id, const char * controlNumber, const char * amount, char * system_time);
int32 CT_AddRecharge(uint16 companyId, const char * accountNumber, const char * amount, char * customerID, char * serialNumber, char * pin, char * service_serialNumber, char * service_pin, char * service_lot, char * time, char * log, const char * confirmation_phone);
int32 CT_AddRechargeRetry(uint16 companyId, const char * accountNumber, const char * amount, const char * lastSerialNumber, char * serialNumber, char * pin, char * time);
int32 CT_AddPayment(uint16 ompanyId, const char * billNumber, const char * amount, char * serialNumber, char * pin, char * time, char * log, char * confirmation_phone);
int32 CT_ConfirmOperation(uint16 companyId, const char * accountNumber, const char * amount, const char * pin, const char * log, char * program_days, const char * customerID, const char * transaction_id, const char * desconection_date);
int CT_CloseOutPOS(char * total_txn_out, char * total_amount_out, char * time, char * total_txn_amount);
int CT_CloseOutConfirm();

int32 CT_EWalletPayment(uint16 companyId, const char * accountNumber, const char * productType, const char * productCode, const char * productItem, const char * productQuantity, char * eCustomerId, char * time, char * log);
int32 CT_EWalletPaymentConfirm(uint16 companyId, const char * log_code, const char * accountNumber, const char * productType, const char * productCode, const char * productItem, const char * productQuantity, const char * time, const char * pin, char * balance);

int32 sendIso8583toCrypto(iso8583 *isoOut, uint8 type_send);

void ConfirmOperationIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
accountNumber, const char* amount, const char* posId,  const char* serviceTime, uint8 is_serial, const char * serial, const char * log, const char * customerID);

void GetSecTokenIso8583(iso8583_new *isoOut, const char *login, const char *password);
void changePasswordIso8583(iso8583 *isoOut, const char* userCompany, const char* user, const char* securityToken, const char * userDataXML);
void RegisterPaymentIso8583(iso8583 *isoOut, const char* userCompany, const char* user, const char* securityToken, const char * posId, const char* depositBank, 
const char* depositDate, const char* depositControlNumber, const char* depositAmount, const char * serials_to_confirm);
void AddECardIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
accountNumber, const char* amount, const char* posId,  const char* serviceTime, const char * serialsToConfirm, const char * confirmation_phone);
void AddECardIso8583Retry(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
accountNumber, const char* amount, const char* posId,  const char* serviceTime, uint8 is_lastSerial, const char * lastSerial);
void AddECardNoTopupIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
accountNumber, const char* amount, const char* posId,  const char* serviceTime, const char * serialsToConfirm, const char * confirmation_phone);
void GetBalanceIso8583(iso8583 *isoOut, const char* userCompany, const char* user, const char* securityToken, const char * posId, const char * serialsToConfirm);
void GetLastSuccessfulTxnIso8583(iso8583 *isoOut, const char* userCompany, const char* user, const char* securityToken, const char* posId);
void CloseOutEposIso8583(iso8583 *isoOut, const char* userCompany, const char* user, const char* securityToken, const char* posId,
const char* closeOutDate, const char* totalTxn, const char* totalAmount, const char* serialsToConfirm);
void CloseOutConfirmEposIso8583(iso8583 * isoOut, char * userCompany, char * userNumber, char * securityToken, char * posId);


void EWalletPaymentIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* accountNumber, const char* productType, const char* productCode,  const char* productItem, const char * productQuantity, const char * serviceTime);
void EWalletPaymentConfirmIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char * log_code, const char* accountNumber, const char* productType, const char* productCode,  const char* productItem, const char * productQuantity, const char * serviceTime, const char * pin);


#ifdef __cplusplus
}
#endif

#endif