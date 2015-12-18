#include <TPTypedef.h>
#include <TPBase.h>
#include "CT_GlobalData.h"

extern USER_DATA g_userData;

void decimalStringToISO8583Decimal(const char * inputString, char * isoString);

double ISO8583DecimalTodecimalString(const char * isoString);

uint8 test_session();

uint8 * get_system_time();

void get_human_date(const char * time, char * outTime);

void format_pin(const char * pin, char * pin_formated);

void CT_Wait(uint32 milliSeconds);

void to_upper(const char *, char *);

void get_name_from_company_recharge(uint32 id, char * company_name);

void get_name_from_company_payment(uint32 id, char * company_name);

void point_to_comma_decimal( double pointDecimalNumber, char * output);
