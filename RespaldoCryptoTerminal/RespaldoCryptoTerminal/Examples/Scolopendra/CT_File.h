#ifndef CT_FILE
#define CT_FILE

#include "CT_GlobalData.h"

int32 CT_CreateLogFile();

int32 CT_CreateLastTransFile();

int32 CT_DeleteLogFile();

int CT_DeleteLastTransFile();

int32 CT_RegisterInLogFile(uint16 serviceCompany, char * cSerialNo, char * subscriber, char * topupDate, char * amount, char * log, uint8 type);

int CT_RegisterInLastTransFile(uint16 serviceCompany, char * cSerialNo, char * cPinNo, char * subscriber, char * topupDate, char * amount, char * log, uint8 type);

void CT_RegisterPrintSuccess();

void CT_PassTransToLogFile();

int32 CT_RegisterPrintAction();

int32 CT_RegisterConfirmAction();

int32 CT_ReadLogFile();

int32 CT_FileReadRegister(uint8 fd, uint32 offset, SALE_INFO * sale_info);

int32 CT_LastTransaction(SALE_INFO_FULL * sale_info);

int32 CT_LastTransactionInLog(SALE_INFO * sale_info);

void CT_CleanLastTransFile();

void CT_RegisterConfirmations(char * lenField, char * field);

int32 CT_RegisterConfirmActionBySerial(const char * serial);

char * CT_MessageError( uint32 error );

#endif