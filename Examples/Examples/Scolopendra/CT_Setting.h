#ifndef CT_SETTING
#define CT_SETTING
#ifdef __cplusplus
extern "C" {
#endif

#define NUMBER_SETTING_LIST 4
#define NUMBER_COMM_LIST 4 //this number is 5, it has 3 to delete username and password the APN
#define NUMBER_COMM_CHOISE_LIST 2
#define NUMBER_COMM_APN_LIST 8
#define NUMBER_COMM_PORT_LIST 10

typedef struct{

	char serverIp[20];
	char serverPort[10][6];
	char apn[30];

}SETTINGS_INFO;

typedef enum SETTINGS_STEP_TAG
{
	SETTINGS_INIT,
	SETTINGS_COMM,
	SETTINGS_READ_LOG_FILE,
	SETTINGS_CREATE_LOG_FILE,	
	SETTINGS_DELETE_LOG_FILE,
	SETTINGS_EXIT
}SETTINGS_STEP;

typedef enum COMM_STEP_TAG
{
	COMM_INIT,
	COMM_SERVER,
	COMM_PORT,
	COMM_APN,
	COMM_REST,
	COMM_USERNAME,
	COMM_PASSWORD,
	COMM_FAIL,
	COMM_SUCCEED,
	COMM_EXIT,
}COMM_STEP;

void CT_Setting();
void CT_Setting_show_key();
uint8 CT_Setting_verify();
void CT_Connectivity();
void InitSettingList();
void InitCommList();
void InitCommChoise();
void InitCommApnList();
void InitCommPort(const char serverPort[NUMBER_COMM_PORT_LIST][6]);

#ifdef __cplusplus
}
#endif

#endif