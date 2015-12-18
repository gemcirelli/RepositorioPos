#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TP_SingleIME.h>
#include <TPFile.h>


#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"

#include "CT_Input.h"
#include "CT_utils.h"
#include "CT_File.h"
#include "CT_Setting.h"

#define SIZE_BUFFER sizeof( SALE_INFO )
#define SIZE_LOG_OWNER sizeof( LOG_OWNER )

uint8 sale_buffer[ SIZE_BUFFER ];
uint8 sale_full_buffer[ sizeof(SALE_INFO_FULL) ];

extern int total_txn;
extern int32 total_txn_to_confirm;
extern double total_amount;
extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL];
extern char log_owner_userName[USER_NAME_LENGTH];
extern char log_owner_usernumber[USER_NUMBER_LENGTH];

char * CT_MessageError( uint32 error ){
	switch( error ){
		case FSERR_NAME_EXIST:	
			return "El archivo ya existe";
			break;
		case FSERR_NOT_EXIST:
			return "El archivo no existe";
			break;
		case FSERR_NOT_OPEN:
			return "El archivo no se pudo abrir";
			break;
		case FSERR_OPENED:
			return "El archivo ya se enuentra abierto";
			break;
		default:
			return "";
	}
}

int32 CT_CreateLogFile(){
	uint8 fd; /*File descriptor to the*/
	uint32 result;
	LOG_OWNER log_owner_info;
	uint8 log_owner_buffer[SIZE_LOG_OWNER];

	if (strlen(g_userData.userName)>0) { /*If doesn't exists a 'LOG' or 'LOG_OWNER_FILE' try to create them both*/
		if ((TP_FExist( "LOG" ) == FALSE) && (TP_FExist( "LOG_OWNER" ) == FALSE)) {
			/*Construct the structure to be writen in the 'LOG_OWNER_FILE'*/
			sprintf(log_owner_info.userName, "%s", g_userData.userName);
			sprintf(log_owner_info.userNumber, "%s", g_userData.userNumber);

			result = TP_Create("LOG_OWNER",SIZE_LOG_OWNER);
			if (result > 0) {
				fd = TP_Open( "LOG_OWNER" );
				if( fd != -1 ) {
					memcpy(log_owner_buffer, &log_owner_info, SIZE_LOG_OWNER);
					TP_Seek(fd, 0, SEEK_FOREWARD,SEEK_CUR);
					result = TP_Write(fd, (uint8*) log_owner_buffer, SIZE_LOG_OWNER);
					TP_Close(fd); /*Try to close the 'LOG_OWNER_FILE'*/
					if( result > 0 ){ /*Create the 'LOG' File*/
						result = TP_Create("LOG", sizeof(SALE_INFO)*100); 
						if( result > 0 ){
							/* Set the global log_owner user variables. If the user disconnect and reconnect these variables will 
							 * prevent that a different user could login before the log_owner user closes the POS*/
							sprintf(log_owner_userName, "%s", log_owner_info.userName);
							sprintf(log_owner_usernumber, "%s", log_owner_info.userNumber);
							return 0x00;
						} else {
							/*An error occurred trying to create the 'LOG' file, deleting the 'LOG_OWNER_FILE'*/
							TP_Delete("LOG_OWNER");
							return TP_GetLastError();
						}
					} else {
						/*An error occurred trying to create the 'LOG' file, deleting the 'LOG_OWNER_FILE'*/
						TP_Delete("LOG_OWNER");
						return TP_GetLastError();
					}
				} else {
					/*An error occurred trying to create the 'LOG' file, deleting the 'LOG_OWNER_FILE'*/
					TP_Close(fd); /*Try to close the 'LOG_OWNER_FILE'*/
					TP_Delete("LOG_OWNER");
					return TP_GetLastError();
				}
			} else {
				return TP_GetLastError();
			}
		} else {
			return 0x02; /*The log or log_owner files already exists*/
		}
	} else {
		return 0x01; /*There not exists any user logged in*/
	}
}

int32 CT_CreateLastTransFile(){
	return TP_Create("LAST", sizeof(SALE_INFO_FULL));
}

int32 CT_DeleteLogFile(){
	if( TP_Delete("LOG_OWNER") == TRUE ) {
		/*log_owner variables cleanup*/
		sprintf(log_owner_userName, "");
		sprintf(log_owner_usernumber, "");
		if( TP_Delete("LOG") == TRUE ) {
			return 0x00;
		} else {
			return TP_GetLastError();
		}
	} else{
		return TP_GetLastError();
	}		
}

int CT_DeleteLastTransFile(){
	if( TP_Delete("LAST") )
		return 0x00;
	else{
		return TP_GetLastError();
	}	
}

int32 CT_RegisterInLogFile(uint16 serviceCompany, char * cSerialNo, char * eCardSerialNo, char * subscriber, char * topupDate, char * amount, char * log, uint8 type, const char * confirmation_phone ){
	SALE_INFO sale_info;
	char isoAmount[AMOUNT_LENGHT];

	uint8 fd;
	uint32 size;

	sale_info.serviceCompany = serviceCompany;
	sprintf(sale_info.cSerialNo, cSerialNo);
	sprintf(sale_info.eCardSerialNo, eCardSerialNo);
	sprintf(sale_info.subscriber, subscriber);
	sprintf(sale_info.topupDate, topupDate);
	sprintf(sale_info.confirmation_phone, confirmation_phone);

	decimalStringToISO8583Decimal(amount, isoAmount);

	sprintf(sale_info.isoAmount, isoAmount);

	sprintf(sale_info.log, log);
	sale_info.type = type;

	sale_info.pin_status = 0;

	memcpy(sale_buffer, &sale_info, SIZE_BUFFER);

	if( TP_FExist( "LOG" ) ){
		
		fd = TP_Open( "LOG" );

		if( fd != -1 ){
			size = TP_FSize(fd);

			if( TP_Seek (fd, size , SEEK_FOREWARD,SEEK_SET) ){
				TP_Write(fd, (uint8 *) sale_buffer, SIZE_BUFFER);
				total_txn++;
				/*memcpy(&serials_to_confirm[(SERIAL_LENGHT-1)*total_txn_to_confirm], sale_info.cSerialNo, SERIAL_LENGHT-1);
				total_txn_to_confirm++;*/
			}
		}

		TP_Close( fd );

		if( TP_GetLastError() < 0 ){
			return TP_GetLastError();
		}else{
			return 0x00; //All right
		}
	}else{
		return 0x01; //log file no existe
	}
}

								
int32 CT_RegisterInLastTransFile(const uint16 serviceCompany, const char * cSerialNo, const char * cPinNo, const char * subscriber, const char * topupDate, const char * amount, const char * service_serialNumber, const char * service_pin, const char * service_lot, const char * log, const uint8 type, const char * confirmation_phone ){
	SALE_INFO_FULL sale_info;
	char isoAmount[AMOUNT_LENGHT];

	uint8 fd;
	uint32 size;

	sale_info.serviceCompany = serviceCompany;
	sprintf(sale_info.cSerialNo, cSerialNo);
	sprintf(sale_info.cPinNo, cPinNo);
	sprintf(sale_info.subscriber, subscriber);
	sprintf(sale_info.topupDate, topupDate);
	sprintf(sale_info.service_serialNumber, service_serialNumber);
	sprintf(sale_info.service_pin, service_pin);
	sprintf(sale_info.service_lot, service_lot);
	sprintf(sale_info.confirmation_phone, confirmation_phone);

	decimalStringToISO8583Decimal(amount, isoAmount);

	sprintf(sale_info.isoAmount, isoAmount);

	sprintf(sale_info.log, log);
	sale_info.type = type;
	sale_info.pin_status = 0;

	if( TP_FExist("LAST") ){
		CT_DeleteLastTransFile();
	}

	CT_CreateLastTransFile();

	fd = TP_Open( "LAST" );

	if( fd != -1 ){
		memcpy(sale_full_buffer, &sale_info, sizeof(SALE_INFO_FULL));

		if( TP_Seek (fd, 0 , SEEK_FOREWARD,SEEK_SET) )
			TP_Write(fd, (uint8 *) sale_full_buffer, sizeof(SALE_INFO_FULL));
	}

	TP_Close( fd );

	if( TP_GetLastError() < 0 ){
		return TP_GetLastError();
	}else{
		return 0x00; //All right
	}
}

int32 CT_RegisterPrintAction(){
	uint8 fd;
	uint32 size;
	uint8 printed_buffer;

	printed_buffer = 1;

	if( TP_FExist( "LOG" ) ){		
		fd = TP_Open( "LOG" );

		if( fd != -1 ){
			size = TP_FSize(fd);

			if( TP_Seek (fd, (size - sizeof(uint8)), SEEK_FOREWARD,SEEK_SET) )
				size = TP_Write(fd, &printed_buffer, sizeof(uint8));
		}

		TP_Close( fd );

		if( TP_GetLastError() < 0 ){
			return TP_GetLastError();
		}else{
			return 0x00; //All right
		}
	}else{
		return 0x01; //log file no existe
	}
}

//int32 CT_RegisterConfirmAction(){
//	uint8 fd;
//	uint32 size;
//	uint8 pin_status_buffer = 1;
//
//	if( TP_FExist( "LAST" ) ){		
//		fd = TP_Open( "LAST" );
//
//		if( fd != -1 ){
//			if( TP_Seek (fd, 0, SEEK_FOREWARD,SEEK_SET) )
//				size = TP_Write(fd, &pin_status_buffer, sizeof(uint8));
//		}
//
//		TP_Close( fd );
//
//		if( TP_GetLastError() < 0 ){
//			return TP_GetLastError();
//		}else{
//			return 0x00; //All right
//		}
//	}else{
//		return 0x01; //log file no existe
//	}
//}

int32 CT_RegisterConfirmAction(){
	uint8 fd;
	uint32 size;
	uint8 pin_status_buffer = 1;

	if( TP_FExist( "LOG" ) ){		
		fd = TP_Open( "LOG" );

		if( fd != -1 ){
			size = TP_FSize(fd);

			if( TP_Seek (fd, size - SIZE_BUFFER, SEEK_FOREWARD,SEEK_SET) ){
				size = TP_Write(fd, &pin_status_buffer, sizeof(uint8));
				/*if( size > 0 ){
					total_txn_to_confirm--;
					if( total_txn_to_confirm < 0 )
						total_txn_to_confirm = 0;
					memset(&serials_to_confirm[(SERIAL_LENGHT-1)*(total_txn_to_confirm)], '\0', SERIAL_LENGHT-1);
				}*/
			}
		}

		TP_Close( fd );

		if( TP_GetLastError() < 0 ){
			return TP_GetLastError();
		}else{
			return 0x00; //All right
		}
	}else{
		return 0x01; //log file no existe
	}
}

int32 CT_RegisterConfirmActionBySerial(const char * serial){
	SALE_INFO sale_info;

	uint8 fd;
	uint32 size;
	uint32 i;

	uint32 result;

	char amountString[8];
	float amount;

	fd = TP_Open( "LOG" );

	if( fd != -1 ){
		size = TP_FSize(fd);

		TP_Seek(fd, 0, SEEK_FOREWARD,SEEK_SET);
		for(i = 0; i < size/SIZE_BUFFER; i++){

			result = CT_FileReadRegister(fd, 0, &sale_info);

			if( result < 0 ){
				return TP_GetLastError();
			}else{
				if( strcmp(sale_info.eCardSerialNo, serial) == 0 ){
					TP_Seek(fd, SIZE_BUFFER*i, SEEK_FOREWARD, SEEK_SET);
					result = TP_Write(fd, "1", sizeof(uint8));
					if( result > 0 ){
						total_txn_to_confirm--;
						TP_Close(fd);
						return 0x00;						
					}
				}
			}
		}
		TP_Close(fd);
	}

	return 0x01;
}

int32 CT_FileReadRegister(uint8 fd, uint32 offset, SALE_INFO * sale_info){
	TP_Seek(fd, offset, SEEK_FOREWARD,SEEK_CUR);
	
	TP_Read(fd, (uint8*) sale_buffer, SIZE_BUFFER);

	if( TP_GetLastError() < 0 ){
		return TP_GetLastError();
	}else{
		memcpy(sale_info, sale_buffer, SIZE_BUFFER);
		return 0x00;
	}
}

int32 CT_ReadLogOwnerFile(){
	uint8 fd; /*File descriptor to the "LOG_OWNER" file*/
	LOG_OWNER log_owner_info;
	uint8 log_owner_buffer[SIZE_LOG_OWNER];
	int32 result;
	
	if (TP_FExist("LOG_OWNER")) {
		fd = TP_Open("LOG_OWNER");
	
		if( fd != -1 ){
			/*Read the file information*/
			TP_Seek(fd, 0, SEEK_FOREWARD,SEEK_CUR);
			result = TP_Read(fd, log_owner_buffer, SIZE_LOG_OWNER);
			if( result > 0 ){
				/*If the reading was sucessfull save the results in the global variables*/
				memcpy(&log_owner_info, log_owner_buffer, SIZE_LOG_OWNER);
				sprintf(log_owner_userName, "%s", log_owner_info.userName);
				sprintf(log_owner_usernumber, "%s", log_owner_info.userNumber);
				result = 0x00;
			}else {
				sprintf(log_owner_userName, "");
				sprintf(log_owner_usernumber, "");
				result = TP_GetLastError();			
			}
		} else {
			sprintf(log_owner_userName, "%s", "");
			sprintf(log_owner_usernumber, "%s", "");
			result = TP_GetLastError();
		}
		TP_Close(fd);
	} else {
		sprintf(log_owner_userName, "%s", "");
		sprintf(log_owner_usernumber, "%s", "");
		result = 0x01; /*The file doesn't exists*/
	}
	return result;
}

int32 CT_ReadLogFile(){
	char information[INF_LENGHT];
	char serial_to_confirm_with_phone[SERIAL_TO_CONFIRM_UNIT];
	SALE_INFO sale_info;

	uint8 fd;
	uint32 size;
	uint32 i;
	int count = 0;

	uint32 result;
	double amount;

	if( TP_FExist( "LOG" ) ){
		fd = TP_Open( "LOG" );

		total_txn = 0;
		total_amount = 0;

		if( fd != -1 ){
			size = TP_FSize(fd);

			TP_Seek(fd, 0, SEEK_FOREWARD,SEEK_SET);
			for(i = 0; i < size/SIZE_BUFFER; i++){

				result = CT_FileReadRegister(fd, 0, &sale_info);

				if( result < 0 ){
					return TP_GetLastError();
				}else{
					total_txn++;

					amount = ISO8583DecimalTodecimalString(sale_info.isoAmount);
					total_amount += amount;

					if( sale_info.pin_status == 0 ){
						sprintf(serial_to_confirm_with_phone, sale_info.eCardSerialNo);
						strcat(serial_to_confirm_with_phone,sale_info.confirmation_phone);
						memcpy(&serials_to_confirm[(SERIAL_TO_CONFIRM_UNIT-1)*count], serial_to_confirm_with_phone, SERIAL_TO_CONFIRM_UNIT-1);
						count++;
						total_txn_to_confirm = count;
					}
				}
			}
			serials_to_confirm[(SERIAL_TO_CONFIRM_UNIT-1)*total_txn_to_confirm] = '\0';
			TP_Close(fd);
		}



		if( TP_GetLastError() < 0 ){
			return TP_GetLastError();
		}else{
			return count;
		}
	}
}

int32 CT_LastTransactionInLog(SALE_INFO * sale_info){
	uint8 fd;
	uint32 size;

	uint8 result;

	if( TP_FExist( "LOG" ) ){
		fd = TP_Open( "LOG" );

		if( fd != -1 ){
			size = TP_FSize(fd);

			if( size == 0 ){
				TP_Close(fd);
				return size;
			}

			TP_Seek (fd, (size - SIZE_BUFFER), SEEK_FOREWARD,SEEK_SET);
			result = CT_FileReadRegister(fd, 0, sale_info);

			TP_Close(fd);
			if( result < 0 ){
				return TP_GetLastError();
			}else{
				return size;
			}
		}

		if( TP_GetLastError() < 0 ){
			return TP_GetLastError();
		}
	}else{
		return 0x01;
	}
}

int32 CT_LastTransaction(SALE_INFO_FULL * sale_info){
	uint8 fd;
	uint32 size;

	uint8 sale_full_buffer[ sizeof(SALE_INFO_FULL) ];

	uint8 result;

	if( TP_FExist( "LAST" ) ){
		fd = TP_Open( "LAST" );

		if( fd != -1 ){
			size = TP_FSize(fd);

			TP_Seek (fd, 0, SEEK_FOREWARD,SEEK_SET);
			result = TP_Read(fd, (uint8*) sale_full_buffer, sizeof(SALE_INFO_FULL));

			TP_Close(fd);
			if( result < 0 ){
				return TP_GetLastError();
			}else{
				memcpy(sale_info, sale_full_buffer, sizeof(SALE_INFO_FULL));
				return 0x00;
			}
		}

		if( TP_GetLastError() < 0 ){
			return TP_GetLastError();
		}
	}else{
		return 0x01;
	}
}

void CT_RegisterPrintSuccess(){
	//SALE_INFO_FULL sale_info_full;

	//CT_LastTransaction(&sale_info_full);

	//CT_RegisterInLogFile(sale_info_full.serviceCompany, sale_info_full.cSerialNo, sale_info_full.cPinNo, sale_info_full.subscriber, sale_info_full.topupDate, sale_info_full.isoAmount, sale_info_full.log, sale_info_full.pin_status, sale_info_full.type);

	CT_DeleteLastTransFile();
}

void CT_PassTransToLogFile(){
	SALE_INFO_FULL sale_info_full;

	CT_LastTransaction(&sale_info_full);

	//CT_RegisterInLogFile(sale_info_full.serviceCompany, sale_info_full.cSerialNo, sale_info_full.cPinNo, sale_info_full.subscriber, sale_info_full.topupDate, sale_info_full.isoAmount, sale_info_full.log, sale_info_full.pin_status, sale_info_full.type);
}

void CT_RegisterConfirmations(char * lenField, char * field){
	uint32 len;
	int16 result_conf;
	char resp_conf[3];
	int32 i;
	int32 j = 0;
	char serial[SERIAL_LENGHT];

	len = atoi(lenField);

	for(i=0;i<len;i+=2){
		memcpy(resp_conf, &field[i], 2);
		resp_conf[2] = '\0';
		result_conf = atoi(resp_conf);
		if( result_conf == 0 ){
			memcpy(serial, &serials_to_confirm[(SERIAL_LENGHT-1)*j], SERIAL_LENGHT-1);
			serial[SERIAL_LENGHT-1] = '\0';
			CT_RegisterConfirmActionBySerial( serial );
			/*Pass to the next serials in the unconfirmed serials list*/
			j=j++;
		}
	}
	/*We clean the serials to confirm variable and fill it with the new serials without confirmation*/
	memset(serials_to_confirm, 0x00, LENGHT_CONFIRM_SERIAL);
	CT_ReadLogFile();
}

//CFG file

int32 CT_CreateCFGFile(){
	return TP_Create("CFG", sizeof(SETTINGS_INFO));
}

int32 CT_DeleteCFGFile(){
	if( TP_Delete("CFG") )
		return 0x00;
	else{
		return TP_GetLastError();
	}	
}

int32 CT_InitCFGFile(){
	SETTINGS_INFO setting_info;
	uint8 imsi[60];
	char serverIP[20];
	char APN[30];
	int i;
	uint8 fd;
	uint8 settings_buffer[ sizeof(SETTINGS_INFO) ];
	char ports[10][8] = { "16000\x00", "16001\x00", "16002\x00", "16003\x00", "16004\x00",
		"37015\x00", "37016\x00", "37017\x00", "37018\x00", "37019\x00" };
	serverIP[0] = '\0';

	TP_GetMobileCardIMSI(imsi, sizeof(imsi));

	if(imsi[4] == '2'){
		sprintf(APN,"gprsweb.digitel.ve\x00");
	}else if(imsi[4] == '4'){
		sprintf(APN,"internet.movistar.ve\x00");
	}else if(imsi[4] == '6'){
		sprintf(APN,"int.movilnet.com.ve\x00");
	}else{
		sprintf(APN,"int.movilnet.com.ve\x00");
	}

	if(CT_GetDNS("srvsco1.novared.net.ve", APN, serverIP) == 0){
		sprintf(setting_info.serverIp, "%s", serverIP); //Server IP
	}else{
		//sprintf(setting_info.serverIp,"190.216.246.77"); //srvsco1 Server IP
		sprintf(setting_info.serverIp,"200.74.206.26"); //devsco1 Server IP
	}

	for(i = 0; i < 10; i++){
		sprintf(setting_info.serverPort[i], "%s", ports[i]);
	}

	sprintf(setting_info.apn, "%s", APN);

	if( TP_FExist("CFG") ){
		CT_DeleteCFGFile();
	}

	CT_CreateCFGFile();

	fd = TP_Open( "CFG" );

	if( fd != -1 ){
		memcpy(sale_full_buffer, &setting_info, sizeof(SETTINGS_INFO));

		if( TP_Seek (fd, 0 , SEEK_FOREWARD,SEEK_SET) )
			TP_Write(fd, (uint8 *) sale_full_buffer, sizeof(SETTINGS_INFO));
	}

	TP_Close( fd );

	if( TP_GetLastError() < 0 ){
		return TP_GetLastError();
	}else{
		return 0x00; //All right
	}
}

int32 CT_WriteCFGFile(SETTINGS_INFO recv_setting_info){

	uint8 fd;

	if( TP_FExist("CFG") ){
		CT_DeleteCFGFile();
	}

	CT_CreateCFGFile();

	fd = TP_Open( "CFG" );

	if( fd != -1 ){
		memcpy(sale_full_buffer, &recv_setting_info, sizeof(SETTINGS_INFO));

		if( TP_Seek (fd, 0 , SEEK_FOREWARD,SEEK_SET) )
			TP_Write(fd, (uint8 *) sale_full_buffer, sizeof(SETTINGS_INFO));
	}

	TP_Close( fd );

	if( TP_GetLastError() < 0 ){
		return TP_GetLastError();
	}else{
		return 0x00; //All right
	}
}

int32 CT_ReadCFGFile(SETTINGS_INFO * recv_setting_info){

	uint8 settings_buffer[ sizeof(SETTINGS_INFO) ];
	uint8 fd;

	if( TP_FExist( "CFG" ) ){
		fd = TP_Open( "CFG" );

		if( fd != -1 ){
			if( TP_Seek(fd, 0, SEEK_FOREWARD,SEEK_CUR) )
				TP_Read(fd, (uint8*) settings_buffer, sizeof(SETTINGS_INFO));
		}

		TP_Close( fd );

		if( TP_GetLastError() < 0 ){
			return TP_GetLastError();
		}else{
			memcpy(recv_setting_info, settings_buffer, sizeof(SETTINGS_INFO));
			return 0x00; //All right
		}
	}else{
		return 0x01; //log file no existe
	}

}