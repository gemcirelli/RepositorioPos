#ifndef CT_GLOBALDATA
#define CT_GLOBALDATA
#ifdef __cplusplus
extern "C" {
#endif
#include <TPTypedef.h>
#include <TPBase.h>

#define PIN_LENGHT 21
#define PIN_PRINTABLE_LENGHT 25
#define MOVILNET_LOT_LENGHT 25
#define INF_LENGHT 255
#define LOG_LENGHT 7
#define AMOUNT_LENGHT 13
#define SERIAL_LENGHT 17
#define SUSC_LENGHT 21
#define TELEPHONE_LENGTH 12
#define NUMBER_COMPANY_RECHARGE 2
#define NUMBER_COMPANY_PAYMENT 1
#define NUMBER_COMPANY_EWALLET 0
#define USER_NAME_LENGTH 15
#define USER_NUMBER_LENGTH 7
#define MAX_TXN 110
#define MESSAGE_LENGHT 50
#define CC_SELL 1
#define INDEP_SELL 2
#define SERVICE_COMPANIES_TOTAL NUMBER_COMPANY_RECHARGE + NUMBER_COMPANY_PAYMENT + NUMBER_COMPANY_EWALLET + 1 //The +1 is for the user_company

#define SERIAL_TO_CONFIRM_UNIT SERIAL_LENGHT+TELEPHONE_LENGTH-1
#define LENGHT_CONFIRM_SERIAL (SERIAL_TO_CONFIRM_UNIT)*600
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
	char eCardSerialNo[SERIAL_LENGHT];
	char subscriber[SUSC_LENGHT];
	char topupDate[15];
	char log[LOG_LENGHT];
	char isoAmount[AMOUNT_LENGHT];
	char confirmation_phone[TELEPHONE_LENGTH];
	
}SALE_INFO;

/* Structure used to save in the "LOG_OWNER" file the id of the user propietary 
 * of the transacctions of the log file*/
typedef struct{
	char userName[15];
	char userNumber[7];
}LOG_OWNER;

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
	char service_serialNumber[SERIAL_LENGHT];
	char service_pin[PIN_LENGHT];
	char service_lot[MOVILNET_LOT_LENGHT];
	char confirmation_phone[TELEPHONE_LENGTH];

}SALE_INFO_FULL;


typedef struct COMPANIES_TO_PRINT_TICKET
{
	unsigned short print_ticket_companies[SERVICE_COMPANIES_TOTAL];
}COMPANIES_TO_PRINT_TICKET;

typedef struct USER_DATA_TAG
{	
	SESSION_STATE sessionState;

	char userName[15];
	char passWord[15];
	char securityToken[7];
	char posId[8];
	char proc_code[8];

	char userCompany[7];
	char userCompanyName[25];
	char userNumber[7];
	
	double balance;
	double pendingBalance;
	
	double totalUnclosedAmount;
	int totalUnclosedTxns;
	
	SALE_INFO txnsLog[50];
	
}USER_DATA;

typedef struct COMM_DATA_TAG{
	char serverIp[20];
	char serverPort[6];
	char apn[30];
	char userName[30];
	char password[30];
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

void change_port();
void InitData();
void InitCompanyList();

#ifdef __cplusplus
}
#endif

#endif