#include "CT_ClosePOS.h"
#include "CT_Input.h"
#include "CT_File.h"
//#include "CT_GlobalData.h"
#include "CT_CryptoManager.h"
#include "CT_utils.h"

extern int total_txn;
extern char serials_to_confirm[LENGHT_CONFIRM_SERIAL];
extern double total_amount;
extern int32 total_txn_to_confirm;

void CT_ClosePOS(){
	char information[INF_LENGHT];
	uint32 cTemp;
	uint8 cStep;

	char total_txn_out[7];
	char total_amount_out[13];
	char total_amount_decimal[10];
	double amount;
	int total;

	cStep = CLOSEOUT_INIT;

	while( cStep != CLOSEOUT_EXIT ){
		switch( cStep ){
			case CLOSEOUT_INIT:
				if( test_session() )
					cStep = CLOSEOUT_CONFIRM;
				else{
					CT_Notify("Debe iniciar sesion para continuar.",0);
					cStep = CLOSEOUT_EXIT;
				}		
				break;

			case CLOSEOUT_CONFIRM:
				cTemp = CT_Confirm("¿Seguro que desea cerrar el Punto de Venta?");
				if( cTemp == 0x00 ){
					cStep = CLOSEOUT_START_COMM;
				}else{
					cStep = CLOSEOUT_EXIT;
				}
				break;

			case CLOSEOUT_START_COMM:
				cTemp = CT_CloseOutPOS(total_txn_out, total_amount_out);
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
				sprintf(information, "Cierre Exitoso!!!\nMonto: %0.2f Bs\nNo. Txn: %d", amount, total); 

				CT_Notify(information, 0);
				
				CT_DeleteLogFile();
				serials_to_confirm[0] = '\0';
				total_amount = 0;
				total_txn_to_confirm = 0;
				total_txn = 0;
				
				CT_CreateLogFile();

				cStep = CLOSEOUT_EXIT;
				break;

			case CLOSEOUT_FAIL:
				CT_Notify("No se pudo realizar el cierre", 0);
				cStep = CLOSEOUT_EXIT;
				break;
		}


	}
}