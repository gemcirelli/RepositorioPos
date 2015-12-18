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

#define SIZE_BUFFER sizeof( SALE_INFO )

uint8 sale_buffer[ sizeof(SALE_INFO) ];
uint8 sale_full_buffer[ sizeof(SALE_INFO_FULL) ];

extern int total_txn;
extern int32 total_txn_to_confirm;
extern double total_amount;
extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL];

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
	return TP_Create("LOG", sizeof(SALE_INFO)*100);
}

int32 CT_CreateLastTransFile(){
	return TP_Create("LAST", sizeof(SALE_INFO_FULL));
}

int32 CT_DeleteLogFile(){
	if( TP_Delete("LOG") )
		return 0x00;
	else{
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

int32 CT_RegisterInLogFile(uint16 serviceCompany, char * cSerialNo, char * subscriber, char * topupDate, char * amount, char * log, uint8 type ){
	SALE_INFO sale_info;
	char isoAmount[AMOUNT_LENGHT];

	uint8 fd;
	uint32 size;

	sale_info.serviceCompany = serviceCompany;
	sprintf(sale_info.cSerialNo, cSerialNo);
	sprintf(sale_info.subscriber, subscriber);
	sprintf(sale_info.topupDate, topupDate);

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

int32 CT_RegisterInLastTransFile(uint16 serviceCompany, char * cSerialNo, char * cPinNo, char * subscriber, char * topupDate, char * amount, char * log, uint8 type ){
	SALE_INFO_FULL sale_info;
	char isoAmount[AMOUNT_LENGHT];

	uint8 fd;
	uint32 size;

	sale_info.serviceCompany = serviceCompany;
	sprintf(sale_info.cSerialNo, cSerialNo);
	sprintf(sale_info.cPinNo, cPinNo);
	sprintf(sale_info.subscriber, subscriber);
	sprintf(sale_info.topupDate, topupDate);

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
				if( strcmp(sale_info.cSerialNo, serial) == 0 ){
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


int32 CT_ReadLogFile(){
	char information[INF_LENGHT];
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
						memcpy(&serials_to_confirm[(SERIAL_LENGHT-1)*count], sale_info.cSerialNo, SERIAL_LENGHT-1);
						count++;
						total_txn_to_confirm = count;
					}
				}
			}
			serials_to_confirm[(SERIAL_LENGHT-1)*total_txn_to_confirm] = '\0';
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
	char serial[SERIAL_LENGHT];

	len = atoi(lenField);

	for(i=0;i<len;i+=2){
		memcpy(resp_conf, &field[i], 2);
		resp_conf[2] = '\0';
		result_conf = atoi(resp_conf);
		if( result_conf == 0 ){
			memcpy(serial, &serials_to_confirm[(SERIAL_LENGHT-1)*i], SERIAL_LENGHT-1);
			serial[SERIAL_LENGHT-1] = '\0';
			CT_RegisterConfirmActionBySerial( serial );
		}
	}
	CT_ReadLogFile();
}