#ifndef CT_GLOBALDATA
#define CT_GLOBALDATA
#ifdef __cplusplus
extern "C" {
#endif
#include <TPTypedef.h>
#include <TPBase.h>

#define PIN_LENGHT 21
#define PIN_PRINTABLE_LENGHT 25
#define INF_LENGHT 255
#define LOG_LENGHT 7
#define AMOUNT_LENGHT 13
#define SERIAL_LENGHT 17
#define SUSC_LENGHT 21
#define NUMBER_COMPANY_RECHARGE 2
#define NUMBER_COMPANY_PAYMENT 1
#define MAX_TXN 600

#define LENGHT_CONFIRM_SERIAL (SERIAL_LENGHT-1)*600
#define LENGHT_CONFIRM_LOG (LOG_LENGHT-1)*600

typedef enum SESSION_STATE_TAG{

	ONLINE,
	OFFLINE,
	EXPIRED,
	
}SESSION_STATE;


typedef struct SALE_INFO_TAG{

	uint8 pin_status;
	uint8 type;
	uint16 serviceCompany;
	char cSerialNo[SERIAL_LENGHT];
	char subscriber[SUSC_LENGHT];
	char topupDate[15];
	char log[LOG_LENGHT];
	char isoAmount[AMOUNT_LENGHT];
	
}SALE_INFO;

typedef struct{

	uint8 pin_status; //1
	uint8 type; //1
	uint16 serviceCompany; //2
	char cSerialNo[SERIAL_LENGHT]; //17
	char cPinNo[PIN_LENGHT]; //21
	char subscriber[SUSC_LENGHT]; //21
	char topupDate[15]; //15
	char log[LOG_LENGHT]; //7
	char isoAmount[AMOUNT_LENGHT]; //13

}SALE_INFO_FULL;


typedef struct USER_DATA_TAG
{	

	SESSION_STATE sessionState;				

	char userName[15];			
	char passWord[15];			
	char securityToken[7];
	char posId[7];

	char userCompany[7];				
	char userNumber[7];
	
	double balance;
	double pendingBalance;
	
	double totalUnclosedAmount;
	int totalUnclosedTxns;
	
	SALE_INFO txnsLog[50];
	
}USER_DATA;

typedef struct COMM_DATA_TAG{
	char serverIp[16];
	char serverPort[6];
	char apn[30];
}COMM_DATA;

typedef struct Item_Def{
	uint16 id;
	char name[30];
}Item;

//Item company_list_recharge[NUMBER_COMPANY_RECHARGE];
//Item company_list_payment[NUMBER_COMPANY_PAYMENT];
//
//char serials_to_confirm[LENGHT_CONFIRM_SERIAL];
//char logs_to_confirm[LENGHT_CONFIRM_LOG];

/*double total_amount;
int32 total_txn_to_confirm;*/


void InitCommData();
void InitCompanyList();

#ifdef __cplusplus
}
#endif

#endif