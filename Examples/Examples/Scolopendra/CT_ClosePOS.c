#include "CT_ClosePOS.h"
#include "CT_Input.h"
#include "CT_File.h"
#include "CT_CryptoManager.h"
#include "CT_utils.h"
#include "CT_Print.h"

extern int total_txn;
extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL];
extern double total_amount;
extern int32 total_txn_to_confirm;
extern char response_message[MESSAGE_LENGHT];

void CT_ClosePOS(){
	char information[INF_LENGHT];
	uint32 cTemp;
	uint8 cStep;

	char total_txn_out[7];
	char total_amount_out[13];
	char total_amount_decimal[10];
	double amount;
	char system_time[15];
	char total_txn_amount[100];
	int total;
	double cpy_total_amount;
	int32 cpy_total_txn;

	char * time;//Quitar

	cStep = CLOSEOUT_INIT;

	while( cStep != CLOSEOUT_EXIT ){
		switch( cStep ){
			case CLOSEOUT_INIT:
				if( test_session() ){
					if (total_txn > 0){
						cStep = CLOSEOUT_CONFIRM;
					}else{
						CT_Notify("No posee cierre\npendiente por\nrealizar.",0);
						cStep = CLOSEOUT_EXIT;
					}
				}else{
					CT_Notify("Debe iniciar sesion para continuar.",0);
					cStep = CLOSEOUT_EXIT;
				}
				break;

			case CLOSEOUT_CONFIRM:
				cTemp = CT_Confirm("¿Desea cerrar el\n Punto de Venta?");
				if( cTemp == 0x00 ){
					cStep = CLOSEOUT_START_COMM;
				}else{
					cStep = CLOSEOUT_EXIT;
				}
				break;

			case CLOSEOUT_START_COMM:
				cTemp = CT_CloseOutPOS(total_txn_out, total_amount_out, system_time, total_txn_amount);
				if( cTemp == 0x00 ){
					cStep = CLOSEOUT_CONFIRM_COMM;
				}else{
					cStep = CLOSEOUT_FAIL;
				}
				break;

			case CLOSEOUT_CONFIRM_COMM:
				CT_CloseOutConfirm();

				cStep = CLOSEOUT_SUCCESS;
				break;

			case CLOSEOUT_SUCCESS:
				amount = ISO8583DecimalTodecimalString(total_amount_out);
				total = atoi(total_txn_out);
				sprintf(information, "Cierre Exitoso\nMonto: %0.2f Bs\nNo. Txn: %d", amount, total); 

				CT_Notify(information, 0);
				
				cpy_total_amount = total_amount;
				cpy_total_txn = total_txn;

				CT_DeleteLogFile();
				serials_to_confirm[0] = '\0';
				total_amount = 0;
				total_txn_to_confirm = 0;
				total_txn = 0;
				if(is_company_to_print_ticket(atoi(g_userData.userCompany))==1){
					cStep = CLOSEOUT_PRINT;
				}else{
					CT_Notify("Recibira un correo\nelectronico con\nla informacion",0);
					cStep=CLOSEOUT_EXIT;
				}
				
				break;

			case CLOSEOUT_PRINT:
				time = (char*) get_system_time();

				memcpy(system_time, time, 15);

				CT_PrintCloseOutSummary(total_txn_amount, cpy_total_amount, cpy_total_txn, system_time);
				cStep = CLOSEOUT_EXIT;
				break;

			case CLOSEOUT_FAIL:
				sprintf(information, "Error: %s", response_message);
				CT_Notify(information,0);
				cStep = CLOSEOUT_EXIT;
				break;
		}


	}
}