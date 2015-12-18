#include <TPTypedef.h>
#include <TPBase.h>
#include "CT_GlobalData.h"

extern USER_DATA g_userData;
extern COMPANIES_TO_PRINT_TICKET g_print_ticket_companies_DATA;

void decimalStringToISO8583Decimal(const char * inputString, char * isoString);

double ISO8583DecimalTodecimalString(const char * isoString);

/**
* Function used to generate a pseudo random key based on the year, month, day, hour and imei of the pos; 
* so the call always generate the same key until an hour as elapsed in wich case a new key will be generated
* @return An unsigned integer as the generated key
*/
uint32 KeyGen();

/**
* Function used to resolve the a URL and return an IP 
* @params url The char pointer to the url to translate
* @params output A char pointer of a buffer where the IP will be saved. 
*	For all the ip to be saved  it's minimum size must be 16
* @params length The buffer's size of whose pointer is output
* @return The same error codes of the function TP_DNSResolve
*/
int32 CT_GetDNS(const char* url, const char * apn, char* output);

uint8 test_session();

uint8 * get_system_time();

void get_human_date(const char * time, char * outTime, uint8 type);

void get_hour_and_date(const char * time, char * date, char * hour);

void format_pin(const char * pin, char * pin_formated);

void CT_Wait(uint32 milliSeconds);

void to_upper(const char *, char *);

void get_name_from_company_recharge(uint32 id, char * company_name);

void get_name_from_company_payment(uint32 id, char * company_name);

void point_to_comma_decimal( const long double input, char * output);

void get_date_from_number(const char * input, char * output);

uint8 validation_date(char *date);

void date_to_iso_date(char * date, char * iso_date);

uint8 is_company_to_print_ticket(uint8 company_id);