#include "CT_Utils.h"
#include <TPNet.h>
#include "TPPPP.h"


extern Item company_list_recharge[NUMBER_COMPANY_RECHARGE];
extern Item company_list_payment[NUMBER_COMPANY_PAYMENT];
extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL];


void decimalStringToISO8583Decimal(const char * inputString, char * isoString){	
	char iso_decimal[15];
	char iso_intiger[15];
	char iso_aux[15];
	int decimal_integer;
	
	decimal_integer = (strlen(inputString)-3);
	strncpy(iso_intiger, inputString, decimal_integer);
	iso_intiger[decimal_integer] = '\0';

	strncpy(iso_decimal, inputString+(decimal_integer+1), (decimal_integer+3));
	iso_decimal[2] = '\0';
	
	sprintf( iso_aux, "%s%s", iso_intiger, iso_decimal);
	sprintf( isoString, "%012s", iso_aux );
}

double ISO8583DecimalTodecimalString(const char * isoString){
	double number;
	uint8 i, pos;
	uint8 lenght = strlen(isoString);
	number = atof(isoString)/100;

	return number;
}

/**
* Function used to generate a pseudo random key based on the year, month, day, hour and imei of the pos; 
* so the call always generate the same key until an hour as elapsed in wich case a new key will be generated
* @return An unsigned integer as the generated key
*/
uint32 KeyGen() {
	uint32 seed;
	TP_DateTime time;
	uint32 posId = atoi(g_userData.posId);
	
	TP_GetDateTime(&time);
	seed = ((time.Date).Year * ((time.Time).Hours+1)) >> (time.Date).Month;
	seed = seed + (posId * (time.Date).Day);
	seed = seed - (posId << 5);
    seed = (seed ^ 61) ^ (seed >> 16);
    seed = seed + (seed << 3);
    seed = seed ^ (seed >> 4);
    seed = seed * 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
	seed = seed - posId;
    return seed;
}


/**
* Function used to resolve the a URL and return an IP 
* @params url The char pointer to the url to translate
* @params output A char pointer of a buffer where the IP will be saved. 
*	For all the ip to be saved  it's minimum size must be 16
* @params length The buffer's size of whose pointer is output
* @return The same error codes of the function TP_DNSResolve
*/
int32 CT_GetDNS(const char* url, const char * apn, char* output){
	int32 result;
	uint8 inputString[100];
    uint8 outputString[100];
	uint32 sendRetrys;
	uint8 iDNS;
	int k;

	k = strlen(output);
	memset(output, 0x00, strlen(output));
    sprintf((char*)inputString, url);
	TP_DNSServer(0, "8.8.8.8", 53);
    TP_DNSServer(1, "8.8.4.4", 53);
    TP_DNSTimeOut(30000);
	TP_WlPPPLogin((uint8*)apn, (uint8*)"", (uint8*)"", 0, 0, 5000);
	sendRetrys = 0;
    while (sendRetrys < 10)
    {
		result = TP_DNSResolve(inputString, outputString, 100);
        if (result == 0){
			break;
        }else{
            sendRetrys++;
        }
    }

	if(result == 0){
		sprintf(output, "%s", outputString);
	}

	return result;
}

uint8 test_session(){
	if( g_userData.sessionState == ONLINE )
		return 0x01;
	else{
		return 0x00;
	}
}

uint8 * get_system_time(){

	uint8  TmpBuffer[50];
	uint8  TimeBuffer[50];

    memset(TmpBuffer,0,sizeof(TmpBuffer));
    TP_GetTime(TmpBuffer);

    memset(TimeBuffer,0,sizeof(TimeBuffer));
    TP_HexToStr(TmpBuffer,7,TimeBuffer);
    memset(TmpBuffer,0x20,sizeof(TmpBuffer));

	memcpy(&TmpBuffer[0], "20", 2);
    memcpy(&TmpBuffer[2],&TimeBuffer[0],2);
	memcpy(&TmpBuffer[4],&TimeBuffer[2],4);
    memcpy(&TmpBuffer[8],&TimeBuffer[6],6);
    TmpBuffer[14] = '\0';

	return TmpBuffer;
}

void get_human_date(const char * time, char * outTime, uint8 type){
	 
	 switch (type){
		 case 0: //format hh:mm:ss dd/mm/yyyy
			 memcpy(&outTime[0], &time[8], 2);
			 outTime[2] = ':';
			 memcpy(&outTime[3], &time[10], 2);
			 outTime[5] = ':';
			 memcpy(&outTime[6], &time[12], 2);
			 outTime[8] = ' ';
			 memcpy(&outTime[9], &time[6], 2);
			 outTime[11] = '/';
			 memcpy(&outTime[12], &time[4], 2);
			 outTime[14] = '/';
			 memcpy(&outTime[15], &time[0], 4);
			 outTime[19] = '\0';
			 break;
		 case 1: //format hh:mm
			 memcpy(&outTime[0], &time[8], 2);
			 outTime[2] = ':';
			 memcpy(&outTime[3], &time[10], 2);
			 outTime[5] = ':';
			 memcpy(&outTime[6], &time[12], 2);
			 outTime[8] = '\0';
			 break;
		 case 2: //format dd/mm/yyyy
			 memcpy(&outTime[0], &time[6], 2);
			 outTime[2] = '/';
			 memcpy(&outTime[3], &time[4], 2);
			 outTime[5] = '/';
			 memcpy(&outTime[6], &time[0], 4);
			 outTime[10] = '\0';
			 break;
	 }
}

void get_hour_and_date(const char * time, char * date, char * hour){
	memcpy(&hour[0], &time[8], 2);
	hour[2] = ':';
	memcpy(&hour[3], &time[10], 2);
	hour[5] = ':';
	memcpy(&hour[6], &time[12], 2);
	hour[8] = '\0';

	memcpy(&date[0], &time[6], 2);
	date[2] = '/';
	memcpy(&date[3], &time[4], 2);
	date[5] = '/';
	memcpy(&date[6], &time[0], 4);
	date[10] = '\0';
}


void format_pin(const char * pin, char * pin_formated){
	uint8 i, j;

	for( i=0, j=0 ; i<PIN_LENGHT-1 ; i+=4 ){
		memcpy(&pin_formated[j], &pin[i], 4);
		j+=4;
		pin_formated[j++] = ' ';
	}

	pin_formated[j-1] = '\0';
}

void CT_Wait(uint32 milliSeconds){
	uint8 time = TP_TimerAllocate();

	TP_TimerSet(time, 1000);
	while(1)
	{
        
		if(TP_TimerCheck(0) == 0)
		{
			break;
		}
	}

	TP_TimerFree(time);
}

void to_upper(const char * word, char * wordUpper){
	uint16 i = 0;
	
	while( word[i] != '\0' ){
		wordUpper[i] = toupper( word[i] );
		i++;
	}
	wordUpper[i] = '\0';
}

void get_name_from_company_recharge(uint32 id, char * company_name){
	uint8 i;
	for(i=0; i<NUMBER_COMPANY_RECHARGE;i++){
		if( company_list_recharge[i].id == id ){
			sprintf(company_name, company_list_recharge[i].name);
		}
	}
}

void get_name_from_company_payment(uint32 id, char * company_name){
	uint8 i;
	for(i=0; i<NUMBER_COMPANY_PAYMENT;i++){
		if( company_list_payment[i].id == id ){
			sprintf(company_name, company_list_payment[i].name);
		}
	}
}

void point_to_comma_decimal( const long double input, char * output){
	uint32 i;

	sprintf(output, "%.2lf", input);
	output[ strlen(output)-3 ] = ',';
}

void get_date_from_number(const char * input, char * output){
	int i,length = strlen(input);

	for(i=0; i < 10; i++){
		if(i == 2 || i==5){
			output[i] = '/';
			i++;length++;
		}
		
		if(i < length){
			if(i > 5)
				output[i] = input[i-2];
			else if(i > 2)
				output[i] = input[i-1];
			else
				output[i] = input[i];
		}else{
			if(i > 5)
				output[i] = 'A';
			else if(i > 2)
				output[i] = 'M';
			else
				output[i] = 'D';
			}
		}
}

uint8 validation_date(char *date){
	int d, m, y, d2, m2, y2, valid, valid_today, bisiesto;
	char * system_time;
	valid_today = 0;
	valid = 0;
	bisiesto = 0;
	system_time = (char*) get_system_time();

	y2 = (system_time[0]-48)*1000 + (system_time[1]-48)*100 + (system_time[2]-48)*10 + (system_time[3]-48);
	m2 = (system_time[4]-48)*10 + (system_time[5]-48);
	d2 = (system_time[6]-48)*10 + (system_time[7]-48);

	d = (date[0]-48)*10 + (date[1]-48);
	m = (date[2]-48)*10 + (date[3]-48);
	y = (date[4]-48)*1000 + (date[5]-48)*100 + (date[6]-48)*10 + (date[7]-48);

	//validar que no sea mayor que hoy
	if(y == y2){
		if(m == m2){
			if(d <= d2){
				valid_today = 1;
			}
		}else if(m < m2){
			valid_today = 1;
		}
	}else if(y < y2){
		valid_today = 1;
	}



	//comprobamos si el año es bisiesto  
	if(y%4==0 && y%100!=100 || y%400==0)  
		bisiesto = 1;
  
	//comprobamos que los datos ingresados esten en un rango valido  
	if(d>0 && d<32 && m>0 && m<13 && y>0 && valid_today){  
		if(m==1 || m==3 || m==5 || m==7 || m==8 || m==10 || m==12)  
		{  
			valid = 1;
		}  
		else  
		{  
			if(m==2 && d<30 && bisiesto)  
				valid = 1;  
			else if(m==2 && d<29 && !bisiesto)  
				valid = 1;  
			else if(m!=2 && d<31)  
				valid = 1;  
			else  
				valid = 0;
		}  
	}  

	return valid;
}

void date_to_iso_date(char * date, char * iso_date){
	int d, m, y;
	iso_date[6] = date[0];
	iso_date[7] = date[1];
	iso_date[4] = date[2];
	iso_date[5] = date[3];
	iso_date[0] = date[4];
	iso_date[1] = date[5];
	iso_date[2] = date[6];
	iso_date[3] = date[7];
	iso_date[8] = '\0';
	/*
	d = (date[0]-48)*10 + (date[1]-48);
	m = (date[2]-48)*10 + (date[3]-48);
	y = (date[4]-48)*1000 + (date[5]-48)*100 + (date[6]-48)*10 + (date[7]-48);
	sprintf(iso_date,"%d%d%d",y,m,d);
	*/
}

uint8 is_company_to_print_ticket(uint8 company_id){
	uint8 result;
	uint8 array_size; 
	uint8 i;
	result = 0;
	array_size = sizeof(g_print_ticket_companies_DATA.print_ticket_companies)/sizeof(unsigned short);
	for(i=0; i<array_size;i++){
		if(company_id == g_print_ticket_companies_DATA.print_ticket_companies[i]){
			result = 1;
			break;
		}
	}
	return result;
}