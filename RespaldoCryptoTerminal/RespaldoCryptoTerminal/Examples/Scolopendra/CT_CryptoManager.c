#include <TPTypedef.h>
#include <TPBase.h>
#include <TPPPP.h>
#include <TPNet.h>
#include <TPNetwork.h>
#include "CT_CryptoManager.h"
#include "CT_GlobalData.h"
#include "CT_Utils.h"
#include "CT_Input.h"
#include "CT_File.h"

#define RETRYS 3

extern USER_DATA g_userData;
extern COMM_DATA g_commData;
extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL];
extern double total_amount;

extern int total_txn;

uint8 buffer[2048];
iso8583 isoOut;

int32 CT_GetSecurityToken(const char *login, const char *password){
	int32 result;

	GetSecTokenIso8583(&isoOut, login, password);
	result = sendIso8583toCrypto(&isoOut);


	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				strcpy(g_userData.securityToken, (char*)isoOut.field_38);
				strcpy(g_userData.userCompany, (char*)isoOut.field_34);
				strcpy(g_userData.userNumber, (char*)isoOut.field_104);
				g_userData.balance = atof((char*)isoOut.field_5)/100;

				TP_SetDateTimeEx((char*)isoOut.field_107);
			}
		}
	}
	return result;

}

int32 CT_GetBalance(){
	int32 result;
	
	GetBalanceIso8583(&isoOut,g_userData.userCompany,g_userData.userNumber,g_userData.securityToken, g_userData.posId, serials_to_confirm);
	result = sendIso8583toCrypto(&isoOut);
	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				g_userData.balance = atof((char*)isoOut.field_5)/100;
				g_userData.pendingBalance = atof((char*)isoOut.field_6)/100;
			}
		}
		if(testBit(127-64, isoOut.bitmap2)){
			CT_RegisterConfirmations((char*)isoOut.field_127_len, (char*)isoOut.field_127);
		}
	}
	return result;
}

int32 CT_AddDeposit(uint16 bank_id, const char * controlNumber, const char * amount, char * time){
	int32 result;

	char depositBank[4];

	char depositDate[11];

	char isoAmount[13];

	char * system_time;

	decimalStringToISO8583Decimal(amount, isoAmount);

	sprintf(depositBank, "%d", bank_id);

	system_time = (char*) get_system_time();

	memcpy(time, system_time, 15);

	memcpy(depositDate, system_time, 8);
	memcpy(&depositDate[8], "00\0", 3);

	system_time = PNULL;

	RegisterPaymentIso8583(&isoOut, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, g_userData.posId, depositBank, depositDate, controlNumber, isoAmount, serials_to_confirm);
	
	result = sendIso8583toCrypto(&isoOut);
	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				g_userData.pendingBalance = atoi((char*)isoOut.field_6)/100;
				g_userData.balance = atoi((char*)isoOut.field_5)/100;
			}
		}
		if(testBit(127-64, isoOut.bitmap2)){
			CT_RegisterConfirmations((char*)isoOut.field_127_len, (char*)isoOut.field_127);
		}
	}
	return result;

}

int32 CT_AddRecharge(uint16 companyId, const char * accountNumber, const char * amount, char * serialNumber, char * pin, char * time, char * log){
	int32 result;

	char serviceTime[11];
	char serviceCompany[4];
	char isoAmount[13];
	char * system_time;

	system_time = (char*) get_system_time();

	memcpy(time, system_time, 15);

	memcpy(serviceTime, &system_time[4], 10);
	serviceTime[10] = '\0';

	sprintf(serviceCompany, "%d", companyId);

	decimalStringToISO8583Decimal(amount, isoAmount);

	AddECardIso8583(&isoOut, serviceCompany, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, accountNumber, isoAmount, g_userData.posId, serviceTime, serials_to_confirm);

	result = sendIso8583toCrypto(&isoOut);
	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				sprintf(log, (char*)isoOut.field_38);
				sprintf(serialNumber, (char*)isoOut.field_107);
				sprintf(pin, (char*)isoOut.field_108);
				g_userData.balance = atof((char*)isoOut.field_5)/100;
			}
		}
		if(testBit(127-64, isoOut.bitmap2)){
			CT_RegisterConfirmations((char*)isoOut.field_127_len, (char*)isoOut.field_127);
		}
	}

	return result;
}

/* Ya no se usa */
int32 CT_AddRechargeRetry(uint16 companyId, const char * accountNumber, const char * amount, const char * lastSerialNumber, char * serialNumber, char * pin, char * time){
	int32 result;

	char serviceTime[11];
	char serviceCompany[4];
	char isoAmount[13];
	char * system_time;
	uint8 retrys = 0;
	
	system_time = (char*) get_system_time();

	memcpy(time, system_time, 15);

	memcpy(serviceTime, &system_time[4], 10);
	serviceTime[10] = '\0';

	sprintf(serviceCompany, "%d", companyId);

	decimalStringToISO8583Decimal(amount, isoAmount);

	AddECardIso8583Retry(&isoOut, serviceCompany, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, accountNumber, isoAmount, g_userData.posId, serviceTime, 1, lastSerialNumber);

	do{
		TP_ScrCls();
		TP_ScrGotoxy(3, 6);
		TP_LcdPrintf((uint8*)"Reintentando...");
		result = sendIso8583toCrypto(&isoOut);
		retrys++;
	}while( result != 0 && retrys < RETRYS );

	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				sprintf(serialNumber, (char*)isoOut.field_107);
				sprintf(pin, (char*)isoOut.field_108);
			}
		}
	}

	return result;
}
int32 CT_AddPayment(uint16 companyId, const char * billNumber, const char * amount, char * serialNumber, char * pin, char * time, char * log){
	int32 result;

	char serviceTime[11];
	char serviceCompany[4];
	char isoAmount[13];
	char * system_time;

	system_time = (char*) get_system_time();

	memcpy(time, system_time, 15);

	memcpy(serviceTime, &system_time[4], 10);
	serviceTime[10] = '\0';

	sprintf(serviceCompany, "%d", companyId);

	decimalStringToISO8583Decimal(amount, isoAmount);

	AddECardNoTopupIso8583(&isoOut, serviceCompany, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, billNumber, isoAmount, g_userData.posId, serviceTime, serials_to_confirm);

	result = sendIso8583toCrypto(&isoOut);
	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				sprintf(log, (char*)isoOut.field_38);
				sprintf(serialNumber, (char*)isoOut.field_107);
				sprintf(pin, (char*)isoOut.field_108);
				g_userData.balance = atof((char*)isoOut.field_5)/100;
			}
		}
		if(testBit(127-64, isoOut.bitmap2)){
			CT_RegisterConfirmations((char*)isoOut.field_127_len, (char*)isoOut.field_127);
		}
	}

	return result;
}

int32 CT_ConfirmOperation(uint16 companyId, const char * accountNumber, const char * amount, const char * serialNumber, const char * log){
	int32 result;

	char serviceTime[11];
	char serviceCompany[4];
	char isoAmount[13];
	char * system_time;
	uint8 retrys = 0;
	
	system_time = (char*) get_system_time();

	memcpy(serviceTime, &system_time[4], 10);
	serviceTime[10] = '\0';

	sprintf(serviceCompany, "%d", companyId);

	decimalStringToISO8583Decimal(amount, isoAmount);

	ConfirmOperationIso8583(&isoOut, serviceCompany, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, accountNumber, isoAmount, g_userData.posId, serviceTime, 1, serialNumber, log);

	result = sendIso8583toCrypto(&isoOut);
	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				/* Result */
			}
		}
		if(testBit(127-64, isoOut.bitmap2)){
			CT_RegisterConfirmations((char*)isoOut.field_127_len, (char*)isoOut.field_127);
		}
	}

	return result;
}

int CT_CloseOutPOS(char * total_txn_out, char * total_amount_out){
	int32 result;

	int32 txnToConfirm;
	char closeOutDate[11];
	char totalTxn[4];
	char totalAmountString[7];
	char isoAmount[13];
	char * system_time;

	system_time = (char*) get_system_time();

	memcpy(closeOutDate, system_time, 8);
	memcpy(&closeOutDate[8], "00\0", 3);

	CT_ReadLogFile();

	sprintf(totalTxn, "%d", total_txn);
	sprintf(totalAmountString, "%.2f", total_amount);
	decimalStringToISO8583Decimal(totalAmountString, isoAmount);

	CloseOutEposIso8583(&isoOut, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, g_userData.posId, closeOutDate, totalTxn, isoAmount, serials_to_confirm);

	result = sendIso8583toCrypto(&isoOut);
	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if( result == 0 ){
				sprintf(total_txn_out, (char*)isoOut.field_2);
				sprintf(total_amount_out, (char*)isoOut.field_4);
			}
		}
	}
	
	return result;
}

int CT_CloseOutConfirm(){
	int result;

	CloseOutConfirmEposIso8583(&isoOut, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, g_userData.posId);

	result = sendIso8583toCrypto(&isoOut);
	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
		}
	}

	return result;
}

void ConfirmOperationIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
accountNumber, const char* amount, const char* posId,  const char* serviceTime, uint8 is_serial, const char * serial, const char * log){
	size_t len;
	char lenStr[5];
	int i;
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	setBit(1,isoOut->bitmap);

	strcpy((char *)isoOut->mti,"0100");

	setBit(3,isoOut->bitmap);
	strcpy((char *)isoOut->field_3,"040000");

	setBit(4,isoOut->bitmap);
	strcpy((char *)isoOut->field_4,amount);

	setBit(7,isoOut->bitmap);
	strcpy((char *)isoOut->field_7,serviceTime);

	setBit(11,isoOut->bitmap);
	strcpy((char *)isoOut->field_11,securityToken);

	setBit(32,isoOut->bitmap);
	len = strlen(serviceCompany);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_32_len,lenStr);
	strcpy((char *)isoOut->field_32,serviceCompany);

	setBit(33,isoOut->bitmap);
	len = strlen(user);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_33_len,lenStr);
	strcpy((char *)isoOut->field_33,user);

	setBit(34,isoOut->bitmap);
	len = strlen(userCompany);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_34_len,lenStr);
	strcpy((char *)isoOut->field_34,userCompany);

	setBit(38,isoOut->bitmap);
	strcpy((char *)isoOut->field_38,log);

	setBit(40,isoOut->bitmap);
	strcpy((char *)isoOut->field_40,"022");

	setBit(44,isoOut->bitmap);
	len = strlen(posId);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_44_len,lenStr);
	strcpy((char *)isoOut->field_44,posId);

	if( is_serial ){
		setBit(45, isoOut->bitmap);
		len = strlen(serial);
		sprintf(lenStr, "%02d", len);
		strcpy((char *)isoOut->field_45_len, lenStr);
		strcpy((char *)isoOut->field_45, serial);
	}

	setBit(111-64,isoOut->bitmap2);
	len = strlen(accountNumber);
	sprintf(lenStr,"%03d",len);
	strcpy((char *)isoOut->field_111_len,lenStr);
	strcpy((char *)isoOut->field_111,accountNumber);
}

void GetSecTokenIso8583(iso8583 *isoOut, const char *login, const char *password){
	size_t len;
	int i;
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	setBit(1,isoOut->bitmap);

	strcpy((char *)isoOut->mti,"0100");

	setBit(3,isoOut->bitmap);
	strcpy((char *)isoOut->field_3,"040000");

	setBit(40,isoOut->bitmap);
	strcpy((char *)isoOut->field_40,"000");

	setBit(104-64,isoOut->bitmap2);
	len = 62 + strlen(login) + strlen(password);
	sprintf((char *)isoOut->field_104_len,"%03d",len);
	strcpy((char *)isoOut->field_104,"<DsUsers><DtUser><Login>");
	strcat((char *)isoOut->field_104,login);
	strcat((char *)isoOut->field_104,"</Login><Pwd>");
	strcat((char *)isoOut->field_104,password);
	strcat((char *)isoOut->field_104,"</Pwd></DtUser></DsUsers>");

}


void RegisterPaymentIso8583(iso8583 *isoOut, const char* userCompany, const char* user, const char* securityToken, const char * posId, const char* depositBank, const 
char* depositDate, const char* depositControlNumber, const char* depositAmount, const char * serialsToConfirm){
	size_t len;
	char lenStr[5];
	int i;
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	setBit(1,isoOut->bitmap);

	strcpy((char *)isoOut->mti,"0100");

	setBit(3,isoOut->bitmap);
	strcpy((char *)isoOut->field_3,"040000");

	setBit(4,isoOut->bitmap);
	strcpy((char *)isoOut->field_4,depositAmount);

	setBit(7,isoOut->bitmap);
	strcpy((char *)isoOut->field_7,depositDate);

	setBit(11,isoOut->bitmap);
	strcpy((char *)isoOut->field_11,securityToken);

	setBit(33,isoOut->bitmap);
	len = strlen(user);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_33_len,lenStr);
	strcpy((char *)isoOut->field_33,user);
	
	setBit(34,isoOut->bitmap);
	len = strlen(userCompany);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_34_len,lenStr);
	strcpy((char *)isoOut->field_34,userCompany);
	
	setBit(40,isoOut->bitmap);
	strcpy((char *)isoOut->field_40,"010");

	setBit(44,isoOut->bitmap);
	len = strlen(posId);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_44_len,lenStr);
	strcpy((char *)isoOut->field_44,posId);

	setBit(45,isoOut->bitmap);
	strcpy((char *)isoOut->field_45_len,"01");
	strcpy((char *)isoOut->field_45,"3");

	setBit(46,isoOut->bitmap);
	len = strlen(depositBank);
	sprintf(lenStr,"%03d",len);
	strcpy((char *)isoOut->field_46_len,lenStr);
	strcpy((char *)isoOut->field_46,depositBank);

	setBit(48,isoOut->bitmap);
	strcpy((char *)isoOut->field_48_len,"001");
	strcpy((char *)isoOut->field_48,"1");

	setBit(104-64,isoOut->bitmap2);
	len = strlen(depositControlNumber);
	sprintf(lenStr,"%03d",len);
	strcpy((char *)isoOut->field_104_len,lenStr);
	strcpy((char *)isoOut->field_104,depositControlNumber);

	/* Serials To Confirm */
	if( strlen(serialsToConfirm) > 0 ){
		setBit(127-64,isoOut->bitmap2);
		len = strlen(serialsToConfirm);
		sprintf(lenStr,"%04d",len);
		strcpy((char *)isoOut->field_127_len,lenStr);
		strcpy((char *)isoOut->field_127,serialsToConfirm);
	}
}

void AddECardIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
accountNumber, const char* amount, const char* posId,  const char* serviceTime, const char * serialsToConfirm){
	size_t len;
	char lenStr[5];
	int i;
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	setBit(1,isoOut->bitmap);

	strcpy((char *)isoOut->mti,"0100");

	setBit(3,isoOut->bitmap);
	strcpy((char *)isoOut->field_3,"040000");

	setBit(4,isoOut->bitmap);
	strcpy((char *)isoOut->field_4,amount);

	setBit(7,isoOut->bitmap);
	strcpy((char *)isoOut->field_7,serviceTime);

	setBit(11,isoOut->bitmap);
	strcpy((char *)isoOut->field_11,securityToken);

	setBit(32,isoOut->bitmap);
	len = strlen(serviceCompany);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_32_len,lenStr);
	strcpy((char *)isoOut->field_32,serviceCompany);

	setBit(33,isoOut->bitmap);
	len = strlen(user);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_33_len,lenStr);
	strcpy((char *)isoOut->field_33,user);

	setBit(34,isoOut->bitmap);
	len = strlen(userCompany);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_34_len,lenStr);
	strcpy((char *)isoOut->field_34,userCompany);

	setBit(40,isoOut->bitmap);
	strcpy((char *)isoOut->field_40,"021");

	setBit(44,isoOut->bitmap);
	len = strlen(posId);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_44_len,lenStr);
	strcpy((char *)isoOut->field_44,posId);

	setBit(111-64,isoOut->bitmap2);
	len = strlen(accountNumber);
	sprintf(lenStr,"%03d",len);
	strcpy((char *)isoOut->field_111_len,lenStr);
	strcpy((char *)isoOut->field_111,accountNumber);

	/* Serials To Confirm */
	if( strlen(serialsToConfirm) > 0 ){
		setBit(127-64,isoOut->bitmap2);
		len = strlen(serialsToConfirm);
		sprintf(lenStr,"%04d",len);
		strcpy((char *)isoOut->field_127_len,lenStr);
		strcpy((char *)isoOut->field_127,serialsToConfirm);
	}
}

/* New Function */
void AddECardIso8583Retry(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
accountNumber, const char* amount, const char* posId,  const char* serviceTime, uint8 is_lastSerial, const char * lastSerial){
	size_t len;
	char lenStr[4];
	int i;
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	setBit(1,isoOut->bitmap);

	strcpy((char *)isoOut->mti,"0100");

	setBit(3,isoOut->bitmap);
	strcpy((char *)isoOut->field_3,"040000");

	setBit(4,isoOut->bitmap);
	strcpy((char *)isoOut->field_4,amount);

	setBit(7,isoOut->bitmap);
	strcpy((char *)isoOut->field_7,serviceTime);

	setBit(11,isoOut->bitmap);
	strcpy((char *)isoOut->field_11,securityToken);

	setBit(32,isoOut->bitmap);
	len = strlen(serviceCompany);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_32_len,lenStr);
	strcpy((char *)isoOut->field_32,serviceCompany);

	setBit(33,isoOut->bitmap);
	len = strlen(user);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_33_len,lenStr);
	strcpy((char *)isoOut->field_33,user);

	setBit(34,isoOut->bitmap);
	len = strlen(userCompany);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_34_len,lenStr);
	strcpy((char *)isoOut->field_34,userCompany);

	setBit(40,isoOut->bitmap);
	strcpy((char *)isoOut->field_40,"021");

	setBit(44,isoOut->bitmap);
	len = strlen(posId);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_44_len,lenStr);
	strcpy((char *)isoOut->field_44,posId);

	if( is_lastSerial ){
		setBit(45, isoOut->bitmap);
		len = strlen(lastSerial);
		sprintf(lenStr, "%02d", len);
		strcpy((char *)isoOut->field_45_len, lenStr);
		strcpy((char *)isoOut->field_45, lastSerial);
	}

	setBit(111-64,isoOut->bitmap2);
	len = strlen(accountNumber);
	sprintf(lenStr,"%03d",len);
	strcpy((char *)isoOut->field_111_len,lenStr);
	strcpy((char *)isoOut->field_111,accountNumber);
}
/*  END  */

void AddECardNoTopupIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
accountNumber, const char* amount, const char* posId,  const char* serviceTime, const char * serialsToConfirm){
	size_t len;
	char lenStr[5];
	int i;
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	setBit(1,isoOut->bitmap);

	strcpy((char *)isoOut->mti,"0100");

	setBit(3,isoOut->bitmap);
	strcpy((char *)isoOut->field_3,"040000");

	setBit(4,isoOut->bitmap);
	strcpy((char *)isoOut->field_4,amount);

	setBit(7,isoOut->bitmap);
	strcpy((char *)isoOut->field_7,serviceTime);

	setBit(11,isoOut->bitmap);
	strcpy((char *)isoOut->field_11,securityToken);

	setBit(32,isoOut->bitmap);
	len = strlen(serviceCompany);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_32_len,lenStr);
	strcpy((char *)isoOut->field_32,serviceCompany);

	setBit(33,isoOut->bitmap);
	len = strlen(user);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_33_len,lenStr);
	strcpy((char *)isoOut->field_33,user);

	setBit(34,isoOut->bitmap);
	len = strlen(userCompany);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_34_len,lenStr);
	strcpy((char *)isoOut->field_34,userCompany);

	setBit(40,isoOut->bitmap);
	strcpy((char *)isoOut->field_40,"025");

	setBit(44,isoOut->bitmap);
	len = strlen(posId);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_44_len,lenStr);
	strcpy((char *)isoOut->field_44,posId);

	setBit(111-64,isoOut->bitmap2);
	len = strlen(accountNumber);
	sprintf(lenStr,"%03d",len);
	strcpy((char *)isoOut->field_111_len,lenStr);
	strcpy((char *)isoOut->field_111,accountNumber);

	/* Serials To Confirm */
	if( strlen(serialsToConfirm) > 0 ){
		setBit(127-64,isoOut->bitmap2);
		len = strlen(serialsToConfirm);
		sprintf(lenStr,"%04d",len);
		strcpy((char *)isoOut->field_127_len,lenStr);
		strcpy((char *)isoOut->field_127,serialsToConfirm);
	}
}

void GetBalanceIso8583(iso8583 *isoOut, const char* userCompany, const char* user, const char* securityToken, const char * posId, const char * serialsToConfirm){
	size_t len;
	char lenStr[5];
	int i;
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	strcpy((char *)isoOut->mti,"0100");

	setBit(1,isoOut->bitmap); //bug cuando no hay bm2 zzzzz
	
	setBit(3,isoOut->bitmap);
	strcpy((char *)isoOut->field_3,"040000");

	setBit(11,isoOut->bitmap);
	strcpy((char *)isoOut->field_11,securityToken);	

	setBit(33,isoOut->bitmap);
	len = strlen(user);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_33_len,lenStr);
	strcpy((char *)isoOut->field_33,user);
	
	setBit(34,isoOut->bitmap);
	len = strlen(userCompany);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_34_len,lenStr);
	strcpy((char *)isoOut->field_34,userCompany);

	setBit(40,isoOut->bitmap);
	strcpy((char *)isoOut->field_40,"004");

	setBit(44,isoOut->bitmap);
	len = strlen(posId);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_44_len,lenStr);
	strcpy((char *)isoOut->field_44,posId);
	
	setBit(111-64,isoOut->bitmap2);
	len = strlen(".");
	sprintf(lenStr,"%03d",len);
	strcpy((char *)isoOut->field_111_len,lenStr);
	strcpy((char *)isoOut->field_111,".");

	/* Serials To Confirm */
	if( strlen(serialsToConfirm) > 0 ){
		setBit(127-64,isoOut->bitmap2);
		len = strlen(serialsToConfirm);
		sprintf(lenStr,"%04d",len);
		strcpy((char *)isoOut->field_127_len,lenStr);
		strcpy((char *)isoOut->field_127,serialsToConfirm);
	}

}

void GetLastSuccessfulTxnIso8583(iso8583 *isoOut, const char* userCompany, const char* user, const char* securityToken, const char* posId){
	size_t len;
	char lenStr[4];
	int i;
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	strcpy((char *)isoOut->mti,"0100");

	setBit(1,isoOut->bitmap); //bug cuando no hay bm2 zzzzz
		
	setBit(3,isoOut->bitmap);
	strcpy((char *)isoOut->field_3,"040000");

	setBit(11,isoOut->bitmap);
	strcpy((char *)isoOut->field_11,securityToken);

	setBit(33,isoOut->bitmap);
	len = strlen(user);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_33_len,lenStr);
	strcpy((char *)isoOut->field_33,user);
	
	setBit(34,isoOut->bitmap);
	len = strlen(userCompany);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_34_len,lenStr);
	strcpy((char *)isoOut->field_34,userCompany);

	setBit(40,isoOut->bitmap);
	strcpy((char *)isoOut->field_40,"025");

	setBit(44,isoOut->bitmap);
	len = strlen(posId);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_44_len,lenStr);
	strcpy((char *)isoOut->field_44,posId);
	
	setBit(111-64,isoOut->bitmap2);
	len = strlen(".");
	sprintf(lenStr,"%03d",len);
	strcpy((char *)isoOut->field_111_len,lenStr);
	strcpy((char *)isoOut->field_111,".");

}

void CloseOutEposIso8583(iso8583 *isoOut, const char* userCompany, const char* user, const char* securityToken, const char* posId,
const char* closeOutDate, const char* totalTxn, const char* totalAmount, const char * serialsToConfirm){
	size_t len;
	char lenStr[5];
	int i;
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	strcpy((char *)isoOut->mti,"0100");

	setBit(1,isoOut->bitmap); //bug cuando no hay bm2 zzzzz
	
	setBit(2,isoOut->bitmap);
	len = strlen(totalTxn);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_2_len,lenStr);
	strcpy((char *)isoOut->field_2,totalTxn);
	
	setBit(3,isoOut->bitmap);
	strcpy((char *)isoOut->field_3,"040000");
	
	setBit(4,isoOut->bitmap);
	strcpy((char *)isoOut->field_4,totalAmount);
	
	setBit(7,isoOut->bitmap);
	strcpy((char *)isoOut->field_7,closeOutDate);
	
	setBit(11,isoOut->bitmap);
	strcpy((char *)isoOut->field_11,securityToken);

	setBit(33,isoOut->bitmap);
	len = strlen(user);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_33_len,lenStr);
	strcpy((char *)isoOut->field_33,user);

	setBit(34,isoOut->bitmap);
	len = strlen(userCompany);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_34_len,lenStr);
	strcpy((char *)isoOut->field_34,userCompany);

	setBit(40,isoOut->bitmap);
	strcpy((char *)isoOut->field_40,"043");

	setBit(44,isoOut->bitmap);
	len = strlen(posId);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_44_len,lenStr);
	strcpy((char *)isoOut->field_44,posId);
	
	setBit(111-64,isoOut->bitmap2);
	len = strlen(".");
	sprintf(lenStr,"%03d",len);
	strcpy((char *)isoOut->field_111_len,lenStr);
	strcpy((char *)isoOut->field_111,".");


	/* Serials To Confirm */
	if( strlen(serialsToConfirm) > 0 ){
		setBit(127-64,isoOut->bitmap2);
		len = strlen(serialsToConfirm);
		sprintf(lenStr,"%04d",len);
		strcpy((char *)isoOut->field_127_len,lenStr);
		strcpy((char *)isoOut->field_127,serialsToConfirm);
	}
}

void CloseOutConfirmEposIso8583(iso8583 * isoOut, char * userCompany, char * user, char * securityToken, char * posId){
	size_t len;
	char lenStr[5];
	int i;
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	strcpy((char *)isoOut->mti,"0100");

	setBit(1,isoOut->bitmap); //bug cuando no hay bm2 zzzzz

	setBit(3,isoOut->bitmap);
	strcpy((char *)isoOut->field_3,"040000");
	
	setBit(11,isoOut->bitmap);
	strcpy((char *)isoOut->field_11,securityToken);

	setBit(33,isoOut->bitmap);
	len = strlen(user);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_33_len,lenStr);
	strcpy((char *)isoOut->field_33,user);

	setBit(34,isoOut->bitmap);
	len = strlen(userCompany);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_34_len,lenStr);
	strcpy((char *)isoOut->field_34,userCompany);

	setBit(40,isoOut->bitmap);
	strcpy((char *)isoOut->field_40,"044");

	setBit(44,isoOut->bitmap);
	len = strlen(posId);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_44_len,lenStr);
	strcpy((char *)isoOut->field_44,posId);

	setBit(111-64,isoOut->bitmap2);
	len = strlen(".");
	sprintf(lenStr,"%03d",len);
	strcpy((char *)isoOut->field_111_len,lenStr);
	strcpy((char *)isoOut->field_111,".");
}



int32 sendIso8583toCrypto(iso8583 *isoOut){
	TP_SockAddrT addr;

	uint8 sizeBuff[7];
	uint16 size;

	uint8 sendRetrys = 0;
	uint8 recvRetrys = 0;
	uint8 connectRetrys = 0;

	int32 socket, result = 0;
	InitCommData();
	result = TP_WlPPPLogin((uint8*)g_commData.apn, (uint8*)"\x00", (uint8*)"\x00", 0, 0, TIMEOUT);
	if(result < 0 && result != -207){
		return result;
	}
	/* Create Socket */
	socket = TP_NetSocket(AF_INET,NET_SOCK_STREAM,0);
	if(socket < 0){
		/* Error Creando el Socket */
		return CONN_FAIL_SOCKET_CREATE;
	}
	size = isoToByteArray(isoOut,buffer);

	/* Assable Ip and Port */
	result = TP_SockAddrSet(&addr,g_commData.serverIp,atoi(g_commData.serverPort));
	if(result < 0){
		TP_NetCloseSocket(socket);
		return result;
	}

	/* Start Connection */
	TP_NetIOCtl(socket, CMD_TO_SET, TIMEOUT);

	TP_ScrCls();
	TP_ScrGotoxy(3, 6);
    TP_LcdPrintf((uint8*)"Conectando...");
	while(1){
		result = TP_NetConnect(socket,&addr,sizeof(TP_SockAddrT)); 
		if(result < 0){
			/* Error Conectando */
			connectRetrys++;
			if( connectRetrys >= CONNECT_RETRYS ){
				TP_NetCloseSocket( socket );
				return -1;//TP_GetLastError();
			}
		}else{
			break;
		}
	}

	/* Send Data to Crypto */
	TP_NetIOCtl(socket, CMD_IO_SET, 1);
	TP_ScrCls();
	TP_ScrGotoxy(3, 6);
    TP_LcdPrintf((uint8*)"Enviando...");
	while( 1 ){
		if( sendRetrys >= SEND_RETRYS ){
			TP_NetCloseSocket(socket);
			return CONN_SEND_ATTEMPTS_EXCEED;
		}
		result = TP_NetSend(socket,buffer,size,0);
		if(result <= 0){
			/* fail to send */
			if((result == -50) || (result == -53) || (result == -54) || (result == -61)){
				/* Network has been disconnected */
				TP_NetCloseSocket(socket);
				return CONN_FINISHED;
			}
			sendRetrys++;
		}else{
			if( result < size ){
				/*the result is less than requested value ,timeout*/
				sendRetrys++;
			}else{
				break;
			}
		}
		if( sendRetrys >= SEND_RETRYS ){
			TP_NetCloseSocket( socket );
			return CONN_CONNECTION_ATTEMPS_ERR;
		}
	}
	
	TP_NetIOCtl(socket, CMD_IO_SET, 0);
	TP_ScrCls();
	TP_ScrGotoxy(3, 6);
    TP_LcdPrintf((uint8*)"Recibiendo...");
	while(1){
		if( recvRetrys >= RECV_RETRYS ){
			TP_NetCloseSocket(socket);
			return CONN_RECV_ATTEMPTS_EXCEED;
		}
		if (TP_NetIOCtl(socket, CMD_EVENT_GET, SOCK_EVENT_READ) == SOCK_EVENT_READ){
			result = TP_NetRecv(socket,buffer,6,0);
		}
		if(result < 0){
			if ((result == -50) || (result == -53) || (result == -54) || (result == -61)){
				/* network has been disconnected */
				TP_NetCloseSocket(socket);
                return CONN_FINISHED;
            }
			recvRetrys++;
		}else{
            if (result == 0){
				/* No se recibio nada */
                recvRetrys++;
            }else{				
				/* All right */
				break;
            }
        }
		if( recvRetrys >= RECV_RETRYS ){
			TP_NetCloseSocket( socket );
			return CONN_CONNECTION_ATTEMPS_ERR;
		}
	}
	buffer[6]='\0';

	size = atoi((char*)buffer);
	recvRetrys = 0;

	while(1){
		if( recvRetrys >= RECV_RETRYS ){
			TP_NetCloseSocket(socket);
			return CONN_RECV_ATTEMPTS_EXCEED;
		}
		if (TP_NetIOCtl(socket, CMD_EVENT_GET, SOCK_EVENT_READ) == SOCK_EVENT_READ){
			result = TP_NetRecv(socket,&buffer[6],size,0);
		}else{
			TP_NetCloseSocket(socket);
			return -60;
		}
		if(result < 0){
			if ((result == -50) || (result == -53) || (result == -54) || (result == -61)){
				/* network has been disconnected */
				TP_NetCloseSocket(socket);
                return CONN_FINISHED;
            }
			recvRetrys++;

		}else{
            if (result == 0){
                recvRetrys++;
            }else{
				/* All right */
                break;
            }
        }
		if( recvRetrys >= RECV_RETRYS ){
			TP_NetCloseSocket( socket );
			return CONN_CONNECTION_ATTEMPS_ERR;
		}
	}
	TP_ScrCls();

	result = TP_NetCloseSocket(socket);
	return result;
}