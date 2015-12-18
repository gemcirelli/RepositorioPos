#ifndef CT_FILE
#define CT_FILE

#include "CT_GlobalData.h"
#include "CT_Setting.h"

int32 CT_CreateLogFile();

int32 CT_CreateLastTransFile();

int32 CT_DeleteLogFile();

int CT_DeleteLastTransFile();

int32 CT_RegisterInLogFile(uint16 serviceCompany, char * cSerialNo, char * eCardSerialNo, char * subscriber, char * topupDate, char * amount, char * log, uint8 type, const char * confirmation_phone );

int32 CT_RegisterInLastTransFile(const uint16 serviceCompany, const char * cSerialNo, const char * cPinNo, const char * subscriber, const char * topupDate, const char * amount, const char * service_serialNumber, const char * service_pin, const char * service_lot, const char * log, const uint8 type, const char * confirmation_phone );

void CT_RegisterPrintSuccess();

void CT_PassTransToLogFile();

int32 CT_RegisterPrintAction();

int32 CT_RegisterConfirmAction();

int32 CT_ReadLogOwnerFile();

int32 CT_ReadLogFile();

int32 CT_FileReadRegister(uint8 fd, uint32 offset, SALE_INFO * sale_info);

int32 CT_LastTransaction(SALE_INFO_FULL * sale_info);

int32 CT_LastTransactionInLog(SALE_INFO * sale_info);

void CT_CleanLastTransFile();

void CT_RegisterConfirmations(char * lenField, char * field);

int32 CT_RegisterConfirmActionBySerial(const char * serial);

char * CT_MessageError( uint32 error );

//CFG file 

int32 CT_CreateCFGFile();

int32 CT_DeleteCFGFile();

int32 CT_InitCFGFile();

int32 CT_WriteCFGFile(SETTINGS_INFO recv_setting_info);

int32 CT_ReadCFGFile(SETTINGS_INFO * recv_setting_info);

#endif