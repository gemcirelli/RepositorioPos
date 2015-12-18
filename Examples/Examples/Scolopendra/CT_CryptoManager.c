#include <TPTypedef.h>
#include <TPBase.h>
#include <TPPPP.h>
#include <TPNet.h>
#include <TPNetwork.h>
#include "TP_DisplayAPI.h"
#include <TPLCD.h>

#include "CT_CryptoManager.h"
#include "CT_GlobalData.h"
#include "CT_Utils.h"
#include "CT_Input.h"
#include "CT_File.h"
#include "SSL/tb_ssl.h"

#define RETRYS 3

extern USER_DATA g_userData;
extern COMM_DATA g_commData;
extern COMPANIES_TO_PRINT_TICKET g_print_ticket_companies_DATA;
extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL];
extern double total_amount;
extern char response_message[MESSAGE_LENGHT];
extern int total_txn;
extern int16 user_type;

uint8 buffer[2048];
iso8583 isoOut;
iso8583_new isoRequest;
iso8583_new isoResponse;

nodo* return_node(iso8583_new isoIn, uint8 campo){
	nodo *nuevo;
	nuevo=isoIn.first;
	for(nuevo; nuevo!= NULL; nuevo=nuevo->next ){
		if(campo==nuevo->field){
			return nuevo;
		}
	}
	nuevo=isoIn.last;
	
}

void clearIso8583_new(iso8583_new *isoIn){
	nodo* current_field;
	current_field=isoIn->first;
			do{		
				free(current_field->value);
				current_field=current_field->next;			
			} 
			while(current_field !=NULL);			
}
int32 CT_GetSecurityToken(const char *login, const char *password){
	int32 result;
	int32 type;
	char companies_to_print_ticket[999];
	char * pointer_sub_string_from;
	char * pointer_sub_string_to;
	char company_to_print_id[6];
	uint8 i = 0;
	uint8 field_present = 0;
	nodo* current_field;
	int limit;
	uint8 j=0;

	GetSecTokenIso8583(&isoRequest, login, password);
	result = sendIso8583toCrypto(&isoRequest, 0);

   if(result == 0){
	   parseIsoResponse(buffer,&isoResponse);
			
		   if(testBit(39,isoResponse.bitmap)){
				 current_field=return_node(isoResponse,39);
				 result= atoi((char*)current_field->value);
				 if(result==0){
					 current_field=return_node(isoResponse,103);
					 type = atoi((char*)current_field->value);
					 if(type == 42 || type == 103){//verified if the user is VI or CC
						switch(type){
							 case 103: strcpy((char *)g_userData.proc_code,"031000"); user_type = CC_SELL; break;							
							 case 42: strcpy((char *)g_userData.proc_code,"040000"); user_type = INDEP_SELL; break;
						}
					 
							
							 current_field=return_node(isoResponse,5);
							 g_userData.balance = atof((char*)current_field->value)/100;
							
							 current_field=return_node(isoResponse,38);
							 strcpy(g_userData.securityToken, (char*)current_field->value);
						
							 current_field=return_node(isoResponse,34);
							 strcpy(g_userData.userCompany, (char*)current_field->value);
						
							 current_field=return_node(isoResponse,104);
							 strcpy(g_userData.userNumber, (char*)current_field->value);
							
							 current_field=return_node(isoResponse,107);
							 TP_SetDateTimeEx((char*)current_field->value);
							
							 current_field=return_node(isoResponse,109);
							 strcpy(g_userData.userCompanyName, (char*)current_field->value);
							 strcpy(g_userData.userName, login);						
										  
					 }else{
					   sprintf(response_message, "Su usuario no tiene permisos.");
					 }
					 if(testBit(127-64,isoRequest.bitmap2)){
					 
						 current_field=return_node(isoResponse,127);
						 sprintf(companies_to_print_ticket,(char*)current_field->value);
					     pointer_sub_string_from = companies_to_print_ticket;
					     pointer_sub_string_to = strchr(companies_to_print_ticket,'|');
					     while (pointer_sub_string_to!=NULL){
						   strncpy (company_to_print_id, pointer_sub_string_from, pointer_sub_string_to - pointer_sub_string_from);
						   g_print_ticket_companies_DATA.print_ticket_companies[i] = (unsigned short) strtoul(company_to_print_id, NULL, 0);
						   pointer_sub_string_from = pointer_sub_string_to + 1;
						   pointer_sub_string_to = strchr(pointer_sub_string_from,'|');
						    i++;  
					     }
					 g_print_ticket_companies_DATA.print_ticket_companies[i] = (unsigned short) strtoul(pointer_sub_string_from, NULL, 0);
					 
					 }
				 

				}
				 else{
				  current_field=return_node(isoResponse,111);
				  sprintf(response_message, (char*)current_field->value);
			     }
			 
           }
   }
   else{ 
    sprintf(response_message, "Falla de Comunicacion. Nro: %d",result);
   }
  return result;

}

int32 CT_ChangePassword(const char *login, const char *password, const char *new_password){
	int32 result;
	char userXML[200];

	sprintf(userXML,"<DsUsers><DtUser><Login>%s</Login><Pwd>%s</Pwd><nPwd>%s</nPwd></DtUser></DsUsers>",login,password,new_password);

	changePasswordIso8583(&isoResponse,g_userData.userCompany,g_userData.userNumber,g_userData.securityToken, userXML);
	result = sendIso8583toCrypto(&isoRequest, 0);
	if(result == 0){
		parseIsoResponse(buffer,&isoResponse);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				sprintf(response_message, "Operacion Exitosa\nNueva clave\nActualizada");
			}else{
				sprintf(response_message, (char*)isoOut.field_111);
			}
		}
	}
	else
		sprintf(response_message, "Falla de Comunicacion. Nro: %d",result);
	return result;

}

int32 CT_GetBalance(){
	int32 result;
	nodo* current_field;

	GetBalanceIso8583(&isoRequest,g_userData.userCompany,g_userData.userNumber,g_userData.securityToken, g_userData.posId, serials_to_confirm);
	result = sendIso8583toCrypto(&isoRequest, 0);
	if(result == 0){
		parseIsoResponse(buffer,&isoResponse);
		if(testBit(39,isoResponse.bitmap)){
			current_field=return_node(isoResponse,39);
			result = atoi((char*)current_field->value);
			if(result == 0){
				current_field=return_node(isoResponse,5);
				g_userData.balance = atof((char*)current_field->value)/100;
				current_field=return_node(isoResponse,6);
				g_userData.pendingBalance = atof((char*)current_field->value)/100;
			}else{
				current_field=return_node(isoResponse,111);
				sprintf(response_message, (char*)current_field->value);
			}
		}
		if(testBit(127-64, isoOut.bitmap2)){
			current_field=return_node(isoResponse,127);
			CT_RegisterConfirmations((char*)isoOut.field_127_len, (char*)isoOut.field_127);
		}
	}
	else
		sprintf(response_message, "Falla de Comunicacion. Nro: %d",result);
	return result;
}

int32 CT_AddDeposit(uint16 bank_id, const char * controlNumber, const char * amount, char * date){
	int32 result;
	char depositBank[4];
	char depositDate[11];
	char isoAmount[13];
	char iso_date[10];
	nodo* current_field;
	size_t leng;
	char lenStr[5];

	decimalStringToISO8583Decimal(amount, isoAmount);

	sprintf(depositBank, "%d", bank_id);

	date_to_iso_date(date, iso_date);
	memcpy(depositDate, iso_date, 8);
	memcpy(&depositDate[8], "00\0", 3);

	RegisterPaymentIso8583(&isoRequest, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, g_userData.posId, depositBank, depositDate, controlNumber, isoAmount, serials_to_confirm);

	result = sendIso8583toCrypto(&isoRequest, 0);
	if(result == 0){
		parseIsoResponse(buffer,&isoResponse);
		if(testBit(39,isoResponse.bitmap)){
			current_field=return_node(isoResponse,39);
			result= atoi((char*)current_field->value);
			if(result == 0){
			    current_field= return_node(isoResponse,6);
				g_userData.pendingBalance = atoi((char*)current_field->value)/100;
				current_field=return_node(isoResponse,5);
				g_userData.balance = atof((char*)current_field->value)/100;
				
			}else{
				current_field=return_node(isoResponse,111);
				sprintf(response_message, (char*)current_field->value);
			}
		}
		if(testBit(127-64, isoOut.bitmap2)){
						current_field=return_node(isoResponse,127);
						leng= strlen(current_field->value);
						sprintf(lenStr,"%04d",leng);
						CT_RegisterConfirmations(lenStr, (char*)isoOut.field_127);		
		}
	}
	else
		sprintf(response_message, "Falla de Comunicacion. Nro: %d",result);
	return result;


}

int32 CT_AddRecharge(uint16 companyId, const char * accountNumber, const char * amount, char * customerID, char * serialNumber, char * pin, char * service_serialNumber, char * service_pin, char * service_lot, char * time, char * log, const char * confirmation_phone){
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

	AddECardIso8583(&isoOut, serviceCompany, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, accountNumber, isoAmount, g_userData.posId, serviceTime, serials_to_confirm, confirmation_phone);

	result = sendIso8583toCrypto(&isoOut, 1);
	if(result == 0){
		//parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				sprintf(log, (char*)isoOut.field_38);
				sprintf(serialNumber, (char*)isoOut.field_107);
				sprintf(pin, (char*)isoOut.field_108);
				sprintf(customerID, (char*)isoOut.field_118);
				sprintf(service_serialNumber, (char*)isoOut.field_122);
				sprintf(service_pin, (char*)isoOut.field_123);
				sprintf(service_lot, (char*)isoOut.field_124);
				g_userData.balance = atof((char*)isoOut.field_5)/100;
			}else{
				sprintf(response_message, (char*)isoOut.field_111);
			}
		}
		if(testBit(127-64, isoOut.bitmap2)){
			CT_RegisterConfirmations((char*)isoOut.field_127_len, (char*)isoOut.field_127);
		}
	}
	else
		sprintf(response_message, "Falla de Comunicacion. Nro: %d",result);
	return result;
}

/* Ya no se usa */ /*
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
		result = sendIso8583toCrypto(&isoOut, 2);
		retrys++;
	}while( result != 0 && retrys < RETRYS );

	if(result == 0){
		parseIsoResponse(buffer,&isoResponse);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				sprintf(serialNumber, (char*)isoOut.field_107);
				sprintf(pin, (char*)isoOut.field_108);
			}else{
				sprintf(response_message, (char*)isoOut.field_111);
			}
		}
	}
	else
		sprintf(response_message, "Falla de Comunicacion. Nro: %d",result);
	return result;
}
*/

int32 CT_AddPayment(uint16 companyId, const char * billNumber, const char * amount, char * serialNumber, char * pin, char * time, char * log, char * confirmation_phone){
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

	AddECardNoTopupIso8583(&isoOut, serviceCompany, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, billNumber, isoAmount, g_userData.posId, serviceTime, serials_to_confirm, confirmation_phone);

	result = sendIso8583toCrypto(&isoOut, 1);
	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				sprintf(log, (char*)isoOut.field_38);
				sprintf(serialNumber, (char*)isoOut.field_107);
				sprintf(pin, (char*)isoOut.field_108);
				g_userData.balance = atof((char*)isoOut.field_5)/100;
			}else{
				sprintf(response_message, (char*)isoOut.field_111);
			}
		}
		if(testBit(127-64, isoOut.bitmap2)){
			CT_RegisterConfirmations((char*)isoOut.field_127_len, (char*)isoOut.field_127);
		}
	}
	else
		sprintf(response_message, "Falla de Comunicacion. Nro: %d",result);
	return result;
}

int32 CT_EWalletPayment(uint16 companyId, const char * accountNumber, const char * productType, const char * productCode, const char * productItem, const char * productQuantity, char * eCustomerId, char * time, char * log){
	int32 result;

	char serviceTime[11];
	char serviceCompany[4];
	char * system_time;
	char isoAmount[13];

	system_time = (char*) get_system_time();

	memcpy(time, system_time, 15);

	memcpy(serviceTime, &system_time[4], 10);
	serviceTime[10] = '\0';

	sprintf(serviceCompany, "%d", companyId);

	decimalStringToISO8583Decimal(productQuantity, isoAmount);

	EWalletPaymentIso8583(&isoOut, serviceCompany, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, accountNumber, productType, productCode,  productItem, isoAmount, serviceTime);

	result = sendIso8583toCrypto(&isoOut, 1);
	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				//quede aqui donde debo retornar el balance del cliente, el mensaje y ya.
				sprintf(log, (char*)isoOut.field_38);
				sprintf(eCustomerId, (char*)isoOut.field_100);
			}else{
				sprintf(response_message, (char*)isoOut.field_111);
			}
		}
	} else {
		sprintf(response_message, "Falla de Comunicacion. Nro: %d",result);
	}
	return result;
}

int32 CT_EWalletPaymentConfirm(uint16 companyId, const char * log_code, const char * accountNumber, const char * productType, const char * productCode, const char * productItem, const char * productQuantity, const char * time, const char * pin, char * balance){
	int32 result;

	char serviceCompany[4];
	char isoAmount[13];

	sprintf(serviceCompany, "%d", companyId);

	decimalStringToISO8583Decimal(productQuantity, isoAmount);
	EWalletPaymentConfirmIso8583(&isoOut, serviceCompany, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, log_code, accountNumber, productType, productCode,  productItem, isoAmount, time, pin);

	result = sendIso8583toCrypto(&isoOut, 2);
	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				//Por ahora no hay nada adicional que retornar
				point_to_comma_decimal((atof((char*)isoOut.field_5)/100),balance);
				sprintf(response_message, (char*)isoOut.field_111);
			}else{
				sprintf(response_message, (char*)isoOut.field_111);
			}
		}
	} else {
		sprintf(response_message, "Falla de Comunicacion. Nro: %d",result);
	}
	return result;
}

int32 CT_ConfirmOperation(uint16 companyId, const char * accountNumber, const char * amount, const char * serialNumber, const char * log, char * program_days, const char * customerID, const char * transaction_id, const char * desconection_date){
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

	ConfirmOperationIso8583(&isoOut, serviceCompany, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, accountNumber, isoAmount, g_userData.posId, serviceTime, 1, serialNumber, log, customerID);

	result = sendIso8583toCrypto(&isoOut, 2);
	if(result == 0){
		parseIsoResponse(buffer,&isoRequest);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result == 0){
				if(testBit(119-64,isoOut.bitmap2)){
					sprintf(transaction_id, (char*)isoOut.field_119);
				}
				if(testBit(120-64,isoOut.bitmap2)){
					sprintf(program_days, (char*)isoOut.field_120);
				}
				if(testBit(121-64,isoOut.bitmap2)){
					sprintf(desconection_date, (char*)isoOut.field_121);
				}
			}else{
				sprintf(response_message, (char*)isoOut.field_111);
			}
		}
		if(testBit(127-64, isoOut.bitmap2)){
			CT_RegisterConfirmations((char*)isoOut.field_127_len, (char*)isoOut.field_127);
		}
	}
	else
		sprintf(response_message, "Falla de Comunicacion. Nro: %d",result);
	return result;
}

int CT_CloseOutPOS(char * total_txn_out, char * total_amount_out, char * time, char * total_txn_amount){
	int32 result;

	int32 txnToConfirm;
	char closeOutDate[11];
	char totalTxn[4];
	char totalAmountString[13];
	char isoAmount[13];
	char * system_time;

	system_time = (char*) get_system_time();

	memcpy(time, system_time, 15);

	memcpy(closeOutDate, system_time, 8);
	memcpy(&closeOutDate[8], "00\0", 3);

	CT_ReadLogFile();

	sprintf(totalTxn, "%d", total_txn);
	sprintf(totalAmountString, "%.2f", total_amount);
	decimalStringToISO8583Decimal(totalAmountString, isoAmount);

	CloseOutEposIso8583(&isoOut, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, g_userData.posId, closeOutDate, totalTxn, isoAmount, serials_to_confirm);

	result = sendIso8583toCrypto(&isoOut, 1);
	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if( result == 0 ){
				sprintf(total_txn_out, (char*)isoOut.field_2);
				sprintf(total_amount_out, (char*)isoOut.field_4);
				sprintf(total_txn_amount, (char*)isoOut.field_127);
			}else{
				sprintf(response_message, (char*)isoOut.field_111);
			}
		}
	}
	else
		sprintf(response_message, "Falla de Comunicacion. Nro: %d",result);
	return result;
}

int CT_CloseOutConfirm(){
	int result;

	CloseOutConfirmEposIso8583(&isoOut, g_userData.userCompany, g_userData.userNumber, g_userData.securityToken, g_userData.posId);

	result = sendIso8583toCrypto(&isoOut, 2);
	if(result == 0){
		parseIsoResponse(buffer,&isoOut);
		if(testBit(39,isoOut.bitmap)){
			result = atoi((char*)isoOut.field_39);
			if(result != 0){
				sprintf(response_message, (char*)isoOut.field_111);
			}
		}
	}
	else
		sprintf(response_message, "Falla de Comunicacion. Nro: %d",result);
	return result;
}

void ConfirmOperationIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
	accountNumber, const char* amount, const char* posId,  const char* serviceTime, uint8 is_serial, const char * serial, const char * log, const char * customerID){
		size_t len;
		char lenStr[5];
		int i;
		clearIso8583(isoOut);
		for(i = 0; i < 8; i++){
			isoOut->bitmap[i]=0;
			isoOut->bitmap2[i]=0;
		}
		isoOut->bitmap[8] = '\0';
		isoOut->bitmap2[8] = '\0';

		setBit(1,isoOut->bitmap);

		strcpy((char *)isoOut->mti,"0100");

		setBit(3,isoOut->bitmap);
		strcpy((char *)isoOut->field_3,g_userData.proc_code);

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

		if(strlen(customerID)>0){
			setBit(118-64,isoOut->bitmap2);
			len = strlen(customerID);
			sprintf(lenStr,"%02d",len);
			strcpy((char *)isoOut->field_118_len,lenStr);
			strcpy((char *)isoOut->field_118,customerID);
		}
}


void add_element(iso8583_new * iso, char* cadena, uint8 field) { 
	nodo *nuevo;
	uint8 length;
	
	nuevo = malloc (sizeof(nodo));
	nuevo->next=NULL;
	nuevo->field=field;
	length = strlen(cadena);
	nuevo->value = malloc((sizeof(char)*length)+1);
	strcpy((char*)nuevo->value, cadena);
	if(iso->first==NULL){
		iso->first=nuevo;
		iso->last=nuevo;
	}
	if(iso->last !=NULL){
		iso->last->next=nuevo;
		iso->last=nuevo;
	
	}
	  
}

void GetSecTokenIso8583(iso8583_new *isoOut, const char *login, const char *password){
	char aux1[200];
	int i;
	//clearIso8583(isoOut);
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	setBit(1,isoOut->bitmap);
	/*mti estandar */
	strcpy((char *)isoOut->mti,"0100");
	/*inicializacion y creacion de fields*/
	setBit(1,isoOut->bitmap);
	setBit(3,isoOut->bitmap);
	add_element(isoOut,"001000",3);
	setBit(40,isoOut->bitmap);
	add_element(isoOut,"004",40);
	setBit(104-64,isoOut->bitmap2);
	strcpy(aux1,"<DsUsers><DtUser><Login>");
	strcat(aux1, login);
	strcat(aux1, "</Login><Pwd>");
	strcat(aux1, password);
	strcat(aux1, "</Pwd></DtUser></DsUsers>");
	add_element(isoOut,aux1,104);
}
void RegisterPaymentIso8583(iso8583_new *isoOut, const char* userCompany, const char* user, const char* securityToken, const char * posId, const char* depositBank, const 
	char* depositDate, const char* depositControlNumber, const char* depositAmount, const char * serialsToConfirm){
		size_t len;
		char lenStr[5];
		int i;
		nodo* current_field;

		//clearIso8583(isoOut);
		for(i = 0; i < 8; i++){
			isoOut->bitmap[i]=0;
			isoOut->bitmap2[i]=0;
		}
		isoOut->bitmap[8] = '\0';
		isoOut->bitmap2[8] = '\0';

		setBit(1,isoOut->bitmap);

		strcpy((char *)isoOut->mti,"0100");

		setBit(3,isoOut->bitmap);
		current_field=return_node(isoResponse,3);
		strcpy((char*)current_field->value,g_userData.proc_code);

		setBit(4,isoOut->bitmap);
		current_field=return_node(isoResponse,4);
		strcpy( (char*)current_field->value, depositAmount);

		setBit(7,isoOut->bitmap);
		current_field=return_node(isoResponse,7);
		strcpy((char*)current_field->value,depositDate);

		setBit(11,isoOut->bitmap);
		current_field=return_node(isoResponse,11);
		strcpy((char*)current_field->value,securityToken);

		setBit(33,isoOut->bitmap);
		len = strlen(user);
		sprintf(lenStr,"%02d",len);
		current_field=return_node(isoRequest,33);
		strcpy(current_field->value,lenStr);
		strcat(current_field->value,user);





		setBit(34,isoOut->bitmap);
		len = strlen(userCompany);
		sprintf(lenStr,"%02d",len);
		current_field=return_node(isoRequest,34);
		strcpy(current_field->value,lenStr);
		strcat(current_field->value,userCompany);


		setBit(40,isoOut->bitmap);
		current_field=return_node(isoRequest,40);
		strcpy((char *)current_field->value,"010");

		
		setBit(44,isoOut->bitmap);
		len = strlen(posId);
		sprintf(lenStr,"%02d",len);
		current_field=return_node(isoRequest,34);
		strcpy(current_field->value,lenStr);
		strcat(current_field->value,posId);

		setBit(45,isoOut->bitmap);
		current_field=return_node(isoRequest,45);
		strcpy((char *)current_field->value,"01");
		strcat((char *)current_field->value,"3");

		setBit(46,isoOut->bitmap);
		len = strlen(depositBank);
		sprintf(lenStr,"%03d",len);
		current_field=return_node(isoRequest,46);
		strcpy((char *)current_field->value,lenStr);
		strcat((char *)current_field->value,depositBank);

		setBit(48,isoOut->bitmap);
		current_field=return_node(isoRequest,48);
		strcpy((char *)current_field->value,"001");
		strcat((char *)current_field->value,"1");

		setBit(104-64,isoOut->bitmap2);
		len = strlen(depositBank);
		sprintf(lenStr,"%03d",len);
		current_field=return_node(isoRequest,46);
		strcpy((char *)current_field->value,lenStr);
		strcat((char *)current_field->value,depositControlNumber);

		/* Serials To Confirm */
		if( strlen(serialsToConfirm) > 0 ){
			setBit(127-64,isoOut->bitmap2);
			len = strlen(depositBank);
			sprintf(lenStr,"%04d",len);
			current_field=return_node(isoRequest,46);
			strcpy((char *)current_field->value,lenStr);
			strcat((char *)current_field->value,serialsToConfirm);
		}
}


void AddECardIso8583(iso8583_new *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
	accountNumber, const char* amount, const char* posId,  const char* serviceTime, const char * serialsToConfirm, const char * confirmation_phone){
		size_t len;
		char lenStr[5];
		int i;
		nodo* current_field;
		
		clearIso8583(isoOut);
		
		for(i = 0; i < 8; i++){
			isoOut->bitmap[i]=0;
			isoOut->bitmap2[i]=0;
		}
		isoOut->bitmap[8] = '\0';
		isoOut->bitmap2[8] = '\0';

		setBit(1,isoOut->bitmap);

		strcpy((char *)isoOut->mti,"0100");

		setBit(3,isoOut->bitmap);
		current_field=return_node(isoOut,3);
		strcpy((char *)current_field->value,g_userData.proc_code);

		setBit(4,isoOut->bitmap);
		current_field=return_node(isoOut,4);
		strcpy((char *)current_field->value,amount);

		setBit(7,isoOut->bitmap);
		current_field=return_node(isoOut,7);
		strcpy((char *)current_field->value,serviceTime);

		setBit(11,isoOut->bitmap);
		current_field=return_node(isoOut,11);
		strcpy((char *)current_field->value,securityToken);

		setBit(32,isoOut->bitmap);
		len = strlen(serviceCompany);
		sprintf(lenStr,"%02d",len);
		current_field=return_node(isoRequest,33);
		strcpy(current_field->value,lenStr);
		strcat(current_field->value,serviceCompany);

		setBit(33,isoOut->bitmap);
		len = strlen(user);
		sprintf(lenStr,"%02d",len);
		current_field=return_node(isoRequest,33);
		strcpy(current_field->value,lenStr);
		strcat(current_field->value,user);

		setBit(34,isoOut->bitmap);
		len = strlen(userCompany);
		sprintf(lenStr,"%02d",len);
		current_field=return_node(isoRequest,34);
		strcpy(current_field->value,lenStr);
		strcat(current_field->value,userCompany);

		setBit(40,isoOut->bitmap);
		current_field=return_node(isoRequest,40);
		strcpy((char *)isoOut,"021");

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
		
		/*SMS confirmation phone number */
		if( strlen(confirmation_phone) > 0 ){
			setBit(125-64,isoOut->bitmap2);
			len = strlen(confirmation_phone);
			sprintf(lenStr,"%02d",len);
			strcpy((char *)isoOut->field_125_len,lenStr);
			strcpy((char *)isoOut->field_125,confirmation_phone);
		}

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
/*  END  */

void AddECardNoTopupIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* 
	accountNumber, const char* amount, const char* posId,  const char* serviceTime, const char * serialsToConfirm, const char * confirmation_phone){
		size_t len;
		char lenStr[5];
		int i;
		clearIso8583(isoOut);
		for(i = 0; i < 8; i++){
			isoOut->bitmap[i]=0;
			isoOut->bitmap2[i]=0;
		}
		isoOut->bitmap[8] = '\0';
		isoOut->bitmap2[8] = '\0';

		setBit(1,isoOut->bitmap);

		strcpy((char *)isoOut->mti,"0100");

		setBit(3,isoOut->bitmap);
		strcpy((char *)isoOut->field_3,g_userData.proc_code);

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

		/*SMS confirmation phone number */
		if( strlen(confirmation_phone) > 0 ){
			setBit(125-64,isoOut->bitmap2);
			len = strlen(confirmation_phone);
			sprintf(lenStr,"%02d",len);
			strcpy((char *)isoOut->field_125_len,lenStr);
			strcpy((char *)isoOut->field_125,confirmation_phone);
		}

		/* Serials To Confirm */
		if( strlen(serialsToConfirm) > 0 ){
			setBit(127-64,isoOut->bitmap2);
			len = strlen(serialsToConfirm);
			sprintf(lenStr,"%04d",len);
			strcpy((char *)isoOut->field_127_len,lenStr);
			strcpy((char *)isoOut->field_127,serialsToConfirm);
		}
}

void GetBalanceIso8583(iso8583_new *isoOut, const char* userCompany, const char* user, const char* securityToken, const char * posId, const char * serialsToConfirm){
	size_t len;
	char lenStr[6];
	int i;
	char aux1[100];
	
	//clearIso8583(isoOut);
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	strcpy((char *)isoOut->mti,"0100");

	setBit(1,isoOut->bitmap); //bug cuando no hay bm2 zzzzz

	setBit(3,isoOut->bitmap);
	add_element(isoOut,g_userData.proc_code,3);
	

	setBit(11,isoOut->bitmap);
	add_element(isoOut,securityToken,11);

	setBit(33,isoOut->bitmap);
	len = strlen(user);
	sprintf(lenStr,"%02d",len);
	strcat(aux1,lenStr);
	strcat(aux1,user);
	add_element(isoOut,aux1,33);

	setBit(34,isoOut->bitmap);
	len = strlen(userCompany);
	sprintf(lenStr,"%02d",len);
	strcat(aux1,lenStr);
	strcat(aux1,userCompany);
	add_element(isoOut,aux1,34);

	setBit(40,isoOut->bitmap);
	add_element(isoOut,"004",40);


	setBit(44,isoOut->bitmap);
	len = strlen(posId);
	sprintf(lenStr,"%02d",len);
	strcat(aux1,lenStr);
	strcat(aux1,posId);
	add_element(isoOut,aux1,44);


	setBit(111-64,isoOut->bitmap2);
	len = strlen(".");
	sprintf(lenStr,"%03d",len);
	strcat(aux1,lenStr);
	strcat(aux1,".");
	add_element(isoOut,aux1,111);
	

	/* Serials To Confirm */
	if( strlen(serialsToConfirm) > 0 ){
		setBit(127-64,isoOut->bitmap2);
		len = strlen(serialsToConfirm);
		sprintf(lenStr,"%04d",len);
		strcat(aux1,lenStr);
		strcat(aux1,serialsToConfirm);
		add_element(isoOut,aux1,127);
		
	}
	lenStr[0] = '\0';
	lenStr[1] = '\0';
	lenStr[2] = '\0';
	lenStr[3] = '\0';
	lenStr[4] = '\0';
	lenStr[5] = '\0';
	lenStr[6] = '\0';
}

void changePasswordIso8583(iso8583_new *isoOut, const char* userCompany, const char* user, const char* securityToken, const char * userDataXML){
	size_t len;
	char lenStr[6];
	int i;
	nodo* current_field;
	char aux1[100];

	clearIso8583_new(isoOut);

	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	strcpy((char *)isoOut->mti,"0100");

	setBit(1,isoOut->bitmap); //bug cuando no hay bm2 zzzzz

	setBit(3,isoOut->bitmap);
	add_element(isoOut,"001000",3);
	//

	setBit(11,isoOut->bitmap);
	add_element(isoOut,securityToken,11);
	

	setBit(33,isoOut->bitmap);
	len = strlen(user);
	sprintf(lenStr,"%02d",len);
	strcat(aux1,lenStr);
	strcat(aux1,user);
	add_element(isoOut,aux1,33);

	setBit(34,isoOut->bitmap);
	len = strlen(userCompany);
	sprintf(lenStr,"%02d",len);
	strcat(aux1,lenStr);
	strcat(aux1,userCompany);
	add_element(isoOut,aux1,34);

	setBit(40,isoOut->bitmap);
	add_element(isoOut,"001",40);

	setBit(104-64,isoOut->bitmap2);
	len = strlen(userDataXML);
	sprintf(lenStr,"%03d",len);
	strcat(aux1,lenStr);
	strcat(aux1,userDataXML);
	add_element(isoOut,aux1,104);
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
	strcpy((char *)isoOut->field_3,g_userData.proc_code);

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
		clearIso8583(isoOut);
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
		strcpy((char *)isoOut->field_3,g_userData.proc_code);

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
	clearIso8583(isoOut);
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	strcpy((char *)isoOut->mti,"0100");

	setBit(1,isoOut->bitmap); //bug cuando no hay bm2 zzzzz

	setBit(3,isoOut->bitmap);
	strcpy((char *)isoOut->field_3,g_userData.proc_code);

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


void EWalletPaymentIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char* accountNumber, const char* productType, const char* productCode,  const char* productItem, const char * productQuantity, const char * serviceTime){
	size_t len;
	char lenStr[5];
	int i;
	clearIso8583(isoOut);
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	strcpy((char *)isoOut->mti,"0100");

	setBit(1,isoOut->bitmap);

	setBit(2,isoOut->bitmap);
	len = strlen(accountNumber);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_2_len,lenStr);
	strcpy((char *)isoOut->field_2,accountNumber);

	setBit(3,isoOut->bitmap);
	//TODO harcoded el processing code
	strcpy((char *)isoOut->field_3,"030000");

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
	//TODO harcoded el event code
	strcpy((char *)isoOut->field_40,"033");

	setBit(44,isoOut->bitmap);
	//TODO hardcoded en 99 como id del carrier para indicar que debe enviar un correo electronico
	len = strlen("99");
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_44_len,lenStr);
	strcpy((char *)isoOut->field_44,"99");

	setBit(102-64,isoOut->bitmap2);
	len = strlen(productType);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_102_len,lenStr);
	strcpy((char *)isoOut->field_102,productType);

	setBit(103-64,isoOut->bitmap2);
	len = strlen(productCode);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_103_len,lenStr);
	strcpy((char *)isoOut->field_103,productCode);

	setBit(104-64,isoOut->bitmap2);
	len = strlen(productItem);
	sprintf(lenStr,"%03d",len);
	strcpy((char *)isoOut->field_104_len,lenStr);
	strcpy((char *)isoOut->field_104,productItem);

	setBit(105-64,isoOut->bitmap2);
	len = strlen(productQuantity);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_105_len,lenStr);
	strcpy((char *)isoOut->field_105,productQuantity);
}

void EWalletPaymentConfirmIso8583(iso8583 *isoOut, const char* serviceCompany, const char* userCompany, const char* user, const char* securityToken, const char * log_code, const char* accountNumber, const char* productType, const char* productCode,  const char* productItem, const char * productQuantity, const char * serviceTime, const char * pin){
	size_t len;
	char lenStr[5];
	int i;
	clearIso8583(isoOut);
	for(i = 0; i < 8; i++){
		isoOut->bitmap[i]=0;
		isoOut->bitmap2[i]=0;
	}
	isoOut->bitmap[8] = '\0';
	isoOut->bitmap2[8] = '\0';

	strcpy((char *)isoOut->mti,"0100");

	setBit(1,isoOut->bitmap);

	setBit(2,isoOut->bitmap);
	len = strlen(accountNumber);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_2_len,lenStr);
	strcpy((char *)isoOut->field_2,accountNumber);

	setBit(3,isoOut->bitmap);
	//TODO harcoded el processing code
	strcpy((char *)isoOut->field_3,"030000");

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
	strcpy((char *)isoOut->field_38,log_code);

	setBit(40,isoOut->bitmap);
	//TODO harcoded el event code
	strcpy((char *)isoOut->field_40,"041");

	setBit(44,isoOut->bitmap);
	//TODO hardcoded en 99 como id del carrier para indicar que debe enviar un correo electronico
	len = strlen("99");
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_44_len,lenStr);
	strcpy((char *)isoOut->field_44,"99");

	setBit(102-64,isoOut->bitmap2);
	len = strlen(productType);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_102_len,lenStr);
	strcpy((char *)isoOut->field_102,productType);

	setBit(103-64,isoOut->bitmap2);
	len = strlen(productCode);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_103_len,lenStr);
	strcpy((char *)isoOut->field_103,productCode);

	setBit(104-64,isoOut->bitmap2);
	len = strlen(productItem);
	sprintf(lenStr,"%03d",len);
	strcpy((char *)isoOut->field_104_len,lenStr);
	strcpy((char *)isoOut->field_104,productItem);

	setBit(105-64,isoOut->bitmap2);
	len = strlen(productQuantity);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_105_len,lenStr);
	strcpy((char *)isoOut->field_105,productQuantity);

	setBit(108-64,isoOut->bitmap2);
	len = strlen(pin);
	sprintf(lenStr,"%02d",len);
	strcpy((char *)isoOut->field_108_len,lenStr);
	strcpy((char *)isoOut->field_108,pin);
}

int32 sendIso8583toCrypto(iso8583_new *isoOut, uint8 type_send){
	SENDISO_STEP cStep;
	uint8 sizeBuff[7];
	char information[100];
	uint16 size;
	uint8 retrys = 0;
	int32 cTemp;
	uint8 progress = 0;
	int32 result, socket = 0, ssl_socket = 0;
	Boolean login=FALSE, start_comm=FALSE, send_d=FALSE;

	size = isoToByteArray(isoOut,buffer);
	cStep = SENDISO_INIT;

	switch(type_send){
		case 0:progress = 10; type_send=1; break;
		case 1:progress = 10; type_send=2; break;
		case 2:progress = 50; type_send=2; break;
	}
	while(cStep!=SENDISO_EXIT)
	{	
		switch(cStep)
		{
		case SENDISO_INIT:
			TP_ScrCls();
			TP_ShowProgressBar((uchar*)"Conectando, espere", progress, 100);
			change_port();
			cStep = SENDISO_LOGIN_APN;			
			progress += (10/type_send);
			break;
		case SENDISO_LOGIN_APN:
			TP_ScrCls();
			//CT_Notify("SENDISO_LOGIN_APN",0);
			TP_ShowProgressBar((uchar*)"Conectando, espere.", (progress+retrys), 100);
			cTemp = loging_apn();
			if(cTemp==0){
				cStep = SENDISO_START_COMM;
				if(!login){
					progress += (10/type_send);
					login=TRUE;
				}
			}else{
				cStep = SENDISO_LOGIN_APN;
				retrys++;			
			}
			break;
		case SENDISO_START_COMM:
			/*initiating the socket*/
			//CT_Notify("SENDISO_START_COMM",0);
			TP_ScrCls();
			TP_ShowProgressBar((uchar*)"Conectando, espere..", (progress+retrys), 100);
			cTemp = connect_to_server(&socket, &ssl_socket);
			if(cTemp==0){
				cStep = SENDISO_SEND_DATA;
				if(!start_comm){progress += (20/type_send);start_comm=TRUE;}
			}else{
				cStep = SENDISO_LOGIN_APN;
				//sprintf(information, "socket: %d\nSSL: %d", socket, ssl_socket);
				//CT_Notify(information, 0);
				tb_ssl_close(ssl_socket);
				TP_NetCloseSocket(socket);
				TP_WlPPPLogout();
				retrys++;
			}
			break;
		case SENDISO_SEND_DATA:
			/* Send Data to Crypto */
			//CT_Notify("SENDISO_SEND_DATA",0);
			TP_NetIOCtl(socket, CMD_IO_SET, 1);
			TP_ScrCls();
			TP_ShowProgressBar((uchar*)"Enviando, espere...", (progress+retrys), 100);
			cTemp = send_data(socket, ssl_socket, size);
			if(cTemp==0){
				cStep = SENDISO_RECV_DATA;
				if(!send_d){progress += (20/type_send);send_d=TRUE;}
			}else{
				cStep = SENDISO_LOGIN_APN;
				tb_ssl_close(ssl_socket);
				TP_NetCloseSocket(socket);
				retrys++;
			}
			break;
		case SENDISO_RECV_DATA:
			//CT_Notify("SENDISO_RECV_DATA",0);
			/*initiating the socket*/
			TP_NetIOCtl(socket, CMD_IO_SET, 0);
			TP_ScrCls();
			TP_ShowProgressBar((uchar*)"Recibiendo, espere...", (progress+retrys), 100);
			cTemp = recv_data(socket, ssl_socket);
			if(cTemp==0){
				cStep = SENDISO_SUCCESS;
				progress += (20/type_send);
			}else{
				cStep = SENDISO_FAIL;				
				retrys++;
			}
			break;
		case SENDISO_SUCCESS:
			//CT_Notify("SENDISO_SUCCESS",0);
			TP_ScrCls();
			TP_ShowProgressBar((uchar*)"Finalizando..", progress, 100);
		case SENDISO_FAIL:
			tb_ssl_close(ssl_socket);
			TP_NetCloseSocket(socket);			
			cStep = SENDISO_EXIT;
		}
		if(retrys > CONNECT_RETRYS){
			break;
		}
	}
	result = cTemp;						

	return result;
}

int32 loging_apn(){

	int32 result = 0;

	if(TP_WlPPPCheck() < 0){
		TP_WlPPPLogout(); //Logout of the phone company PPP
		result = TP_WlPPPLogin((uint8*)g_commData.apn, (uint8*)g_commData.userName, (uint8*)g_commData.password, 0, 0, 5000);
		if(result < 0 && result != -207){
			TP_WlPPPLogout(); //Logout of the phone company PPP
		}else{
			result = 0;
		}
	}
	return result;
}


int32 connect_to_server(int32 *socket_to_crypto, int32 *ssl_socket_to_crypto){
	TP_SockAddrT addr;
	int32 socket, result, ssl_socket;
	uint8 connectRetrys = 0;
	ssl_socket = NULL;
	socket = NULL;

	/* Create Socket */
	socket = TP_NetSocket(AF_INET,NET_SOCK_STREAM,0);
	if(socket < 0){
		/* Error Creando el Socket */
		return CONN_FAIL_SOCKET_CREATE;
	}

	/* Assable Ip and Port */
	result = TP_SockAddrSet(&addr,(uchar*)g_commData.serverIp,atoi(g_commData.serverPort));
	if(result < 0){
		TP_NetCloseSocket(socket);
		return result;
	}

	/* Start Connection */
	TP_NetIOCtl(socket, CMD_TO_SET, TIMEOUT);
	result = TP_NetConnect(socket,&addr,sizeof(TP_SockAddrT)); 
	if(result < 0){
		/* Error Conectando */
		TP_NetCloseSocket( socket );
		return -1;
	}

	ssl_socket = tb_ssl_open(socket, TIMEOUT);
	if (ssl_socket < 0 )
	{
		TP_NetCloseSocket(socket);
		return CONN_FAIL_SSL_AGREEMENT;
	}
	*socket_to_crypto = socket;
	*ssl_socket_to_crypto = ssl_socket;

	return result;
}

int32 send_data(int32 socket, int32 ssl_socket, uint16 size){
	int32 result;
	int32 n_send;

	n_send = tb_ssl_write(ssl_socket,buffer,size,TIMEOUT);
	
	if( n_send == size ){
		result = 0;
	}else{
		/* fail to send */
		if((result == -50) || (result == -53) || (result == -54) || (result == -61)){
			/* Network has been disconnected */
			result = CONN_FINISHED;
		}else{
			result = SSL_ERR_WRITE;
		}
		tb_ssl_close(ssl_socket);
		TP_NetCloseSocket(socket);		
	}
	return result;
}


int32 recv_data(int32 socket_to_crypto, int32 ssl_socket_to_crypto){
	int32 socket, result, ssl_socket;
	uint16 size;
	uint8 recvRetrys = 0;

	socket = socket_to_crypto;
	ssl_socket = ssl_socket_to_crypto;

	if (TP_NetIOCtl(socket, CMD_EVENT_GET, SOCK_EVENT_READ) == SOCK_EVENT_READ){
		result = tb_ssl_read(ssl_socket,buffer,6,TIMEOUT);
	}else{
		return -60;
	}

	if(result < 0){
		if ((result == -50) || (result == -53) || (result == -54) || (result == -61)){
			/* network has been disconnected */
			return CONN_FINISHED;
		}			
	}

	buffer[6]='\0';

	size = atoi((char*)buffer);
	recvRetrys = 0;

	if (TP_NetIOCtl(socket, CMD_EVENT_GET, SOCK_EVENT_READ) == SOCK_EVENT_READ){
		result = tb_ssl_read(ssl_socket,&buffer[6],size,TIMEOUT);
	}else{
		return -60;
	}

	if(result < 0){
		if ((result == -50) || (result == -53) || (result == -54) || (result == -61)){
			/* network has been disconnected */
			return CONN_FINISHED;
		}
	}
		
	if (result == size){
		return 0;
	}else{
		return SSL_ERR_READ;
	}
}