#include "CT_Utils.h"

extern Item company_list_recharge[NUMBER_COMPANY_RECHARGE];
extern Item company_list_payment[NUMBER_COMPANY_PAYMENT];
extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL];

void decimalStringToISO8583Decimal(const char * inputString, char * isoString){
	float decimal = atof(inputString)*100;
	int decimal_integer = decimal;

	sprintf( isoString, "%012d", decimal_integer );
}

double ISO8583DecimalTodecimalString(const char * isoString){
	double number;
	uint8 i, pos;
	uint8 lenght = strlen(isoString);
	number = atof(isoString)/100;

	return number;
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

void get_human_date(const char * time, char * outTime){
	 memcpy(&outTime[0], &time[8], 2);
	 outTime[2] = ':';
	 memcpy(&outTime[3], &time[10], 2);
	 outTime[5] = ':';
	 memcpy(&outTime[6], &time[12], 2);
	 outTime[8] = ' ';
	 memcpy(&outTime[9], &time[0], 4);
	 outTime[13] = '/';
	 memcpy(&outTime[14], &time[4], 2);
	 outTime[16] = '/';
	 memcpy(&outTime[17], &time[6], 2);
	 outTime[19] = '\0';
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

void point_to_comma_decimal( double pointDecimalNumber, char * output){
	uint32 i;

	sprintf(output, "%.2f", pointDecimalNumber);
	output[ strlen(output)-3 ] = ',';
}