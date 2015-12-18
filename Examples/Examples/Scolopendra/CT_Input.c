#include <TPTypedef.h>
#include <TPBase.h>
#include <TPKeyPad.h>
#include <TPLCD.h>
#include <TP_SingleIME.h>

#include "TP_DemoDefine.h"
#include "TP_DisplayAPI.h"
#include "CT_Input.h"
#include "CT_Utils.h"

uint8 CT_Menu(const Item* items_list, uint8 size, uint8* selectedItem, TP_ListBoxStyle listboxStyle, const uchar* title){
	uint8 result;
	TP_DisplayInfo displayInfo = 
							{	"",
                                ALIGN_LEFT,
                                (uint8*)"Ok",
                                (uint8*)"Cancel",
                                KEY_DEMO_CONFIRM,
                                KEY_DEMO_CANCEL,
                                TP_KEY_OK,
                                TP_KEY_NONE,
                                CFONT
                            };

	char ** items = (char **) malloc( (size+1)*sizeof(char*) );
	int i;

	for(i=0;i<size;i++){
		items[i] = items_list[i].name;
	}

	result = TP_CreateListBoxInterface(listboxStyle, &displayInfo, (uint8**)items, size, 1, selectedItem, PNULL);

	free(items);

	return result;
}

uint8 CT_Confirm(const char* text){
	uint8 response;

	TP_SetDisplayArea(0, 0, 127, 63);
    TP_ScrFontSet(CFONT);
    TP_ScrCls();
	TP_ScrAttrSet(0);
    TP_ScrDrawRect(0, 0, 126, 63);//Draw border
    TP_ScrGotoxy(3, 0);
    TP_LcdPrintf((uint8*)text);
    TP_ScrGotoxy(3, 6);
    TP_LcdPrintf((uint8*)"Ok");
    TP_ScrGotoxy(90, 6);
    TP_LcdPrintf((uint8*)"Cancel");

	while(1)
	{
		if (TP_Kbhit())
		{
			uint8 KeyCode = TP_GetKey();
            
            if ((KeyCode == KEY_DEMO_CONFIRM))
            {
                return 0x00;
            }

			if ((KeyCode == KEY_DEMO_CANCEL))
            {
                return 0x01;
            }
            
			TP_Kbflush(); ///Key Value of The Key To Be Flushed

		}
	}
}

void CT_Notify(const char* text, uint32 Ms)
{
    uint8 text_fragment[57];
	uint8 progress=0;
	TP_Kbflush();
    TP_ScrAttrSet(0);
    TP_ScrFontSet(CFONT);
    TP_ScrClrRect(5,5,123,59);
    TP_SetDisplayArea(5,5,123,59);
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect(6,6,122,58);


    TP_SetDisplayArea(7,7,121,57);

	/*If the text exceeds the dysplay area print only a fragment*/
    TP_ScrGotoxyEx(7,7);
    if (strlen(text)<57)
		TP_LcdPrintf((uint8*)text);
	else {
		strncpy((char*)text_fragment,(char*)text,56);
		*(text_fragment+56)='\0';
		TP_LcdPrintf(text_fragment);
	}

	TP_ScrGotoxyEx(30, 45);
    TP_LcdPrintf((uint8*)"Presione Ok");
	if(Ms > 0){
		TP_TimerSet(0, Ms);
		while(1)
		{
        
			if(TP_TimerCheck(0) == 0)
			{
				return;
			}
			if (TP_Kbhit())
			{
				uint8 KeyCode = TP_GetKey();
				/* If the text exceeds the dysplay area print the previous fragment if the key pressed is up
				 * if the key pressed is down print the next fragment, any other key exit*/
				if (strlen(text)>57)  {
					switch (KeyCode) {
						case TP_KEY_UP:
							TP_Kbflush();
							if (progress>0)
								progress=progress-19;
							break;
                
						case TP_KEY_DOWN:
							TP_Kbflush();
							if (progress<(strlen(text)-57))
								progress=progress+19;
							break;

						default:
							TP_Kbflush(); ///Key Value of The Key To Be Flushed
							return;
							break;
					}

					strncpy((char*)text_fragment,(char*)(text+progress),56);
					*(text_fragment+56)='\0';
					TP_ScrClsDisplayArea();
					TP_ScrGotoxyEx(7,7);
					TP_LcdPrintf(text_fragment);
					
					TP_ScrGotoxyEx(30, 45);
					TP_LcdPrintf((uint8*)"Presione Ok");
				}
				else {
					TP_Kbflush(); ///Key Value of The Key To Be Flushed
					return;
				}
			}
		}
	}else{
		while(1)
		{
			if (TP_Kbhit())
			{
				uint8 KeyCode = TP_GetKey();
				/* If the text exceeds the dysplay area print the previous fragment if the key pressed is up
				 * if the key pressed is down print the next fragment, any other key exit*/
				if (strlen(text)>57) {
					switch (KeyCode) {
						case TP_KEY_UP:
							TP_Kbflush();
							if (progress>0)
								progress=progress-19;
							break;
                
						case TP_KEY_DOWN:
							TP_Kbflush();
							if (progress<(strlen(text)-57))
								progress=progress+19;
							break;

						default:
							TP_Kbflush(); ///Key Value of The Key To Be Flushed
							return;
							break;
					}

					strncpy((char*)text_fragment,(char*)(text+progress),56);
					*(text_fragment+56)='\0';
					TP_ScrClsDisplayArea();
					TP_ScrGotoxyEx(7,7);
					TP_LcdPrintf(text_fragment);
					
					TP_ScrGotoxyEx(30, 45);
					TP_LcdPrintf((uint8*)"Presione Ok");
				}
				else {
					TP_Kbflush(); ///Key Value of The Key To Be Flushed
					return;
				}
			}
		}
	}
    
}


void CT_InputDisplay(char* header){
	TP_SetDisplayArea(0, 0, 127, 63);
    TP_ScrFontSet(CFONT);
    TP_ScrCls();
	TP_ScrAttrSet(0);
    TP_ScrDrawRect(0, 0, 126, 63);//Draw border
    TP_ScrGotoxy(3, 0);
    TP_LcdPrintf((uint8*)header);
    TP_ScrGotoxy(3, 6);
    TP_LcdPrintf((uint8*)"Ok");
    TP_ScrGotoxy(90, 6);
    TP_LcdPrintf((uint8*)"Cancel");
    TP_ScrDrawRect(3, 16, 125, 47);
    TP_ScrSpaceSet(0, 1);
    TP_SetDisplayArea(4, 17, 124, 46);
    TP_ScrGotoxyEx(4, 17);
  
    TP_SetInputModeControl(KEY_DEMO_CONFIRM, KEY_DEMO_CANCEL, KEY_DEMO_POUND);
    TP_SetInputModePosition(59,52);
    TP_ShowInputNum(0, 1, 1);

}

uint8 CT_InputAnything(char* result, char* header, uint8 mode){
	uint8 response;
	memset(result, 0x00, sizeof(result));

	CT_InputDisplay(header);
	switch(mode){
	case CT_DECIMAL:
		response = CT_InputDecimalType(result);
		//response = TP_GetString((char*)result, 0xF6, 1, 12);
		break;
	case CT_INTEGER:
		response = TP_GetString((char*)result, 0xF4, 1, 20);
		break;
	case CT_STRING:
		TP_SetInputMode(INPUTMODE_LOWCASE);
		response = TP_GetHzString((char*)result, 0, 30);
		break;
	case CT_HIDDEN:
		response = TP_GetString((char*)result, 0xFC, 1, 12);
		break;
	case CT_DATE:
		response = CT_InputDateType(result, header);
		break;
	case CT_IP:
		response = TP_GetString((char*)result, 0xF6, 1, 15);
		break;
	}

	return response;
}

uint8 CT_InputDecimalType(char * result){
	char input;
	char buffer[16];
	int i,j;
	i=0;j=0;
	
	memset(buffer, 0, 16);
	TP_LcdPrintf((uint8*)"%20s", "0,00");

	while(TRUE){

		if (TP_Kbhit() == 0xFF){
			input = TP_GetKey();
			if (input == TP_KEY_OK){
                return 0xFB;
			}else if (input == TP_KEY_CANCEL){
				result[0] = '\0';
                return 0xFF;
			}else if(input == TP_KEY_CLEAR){
				i--;
				if( i < 0 )
					i = 0;
				buffer[i] = '\0';
			}else{
				if( i == 0 && input == '0' )
					continue;
				if( i < 10 ){
					buffer[i] = input;
					i++;
				}else{
					continue;
				}
			}

			point_to_comma_decimal(((double)atoi(buffer)/100), result);
			TP_ScrClrRect(4, 17, 124, 46);
			TP_LcdPrintf((uint8*)"%20s", result);
		}
	}
	return 0xFB;
}

uint8 CT_InputDateType(char * result, char * header){
	char input;
	uint8 cTemp;
	char buffer[16];
	int i,j;
	i=0;j=0;
	
	memset(buffer, 0, 16);
	TP_LcdPrintf((uint8*)"%20s", "DD/MM/AAAA");

	while(TRUE){

		if (TP_Kbhit() == 0xFF){
			input = TP_GetKey();
			if (input == TP_KEY_OK){
				if(validation_date(result)){
					return 0xFB;
				}else{
					CT_Notify("    Advertencia\n  Fecha incorrecta", 0);
					CT_InputDisplay(header);
				}
			}else if (input == TP_KEY_CANCEL){
				result[0] = '\0';
                return 0xFF;
			}else if(input == TP_KEY_CLEAR){
				i--;
				if( i < 0 )
					i = 0;
				result[i] = '\0';
			}else{
				if( i < 8 ){
					result[i] = input;
					result[i+1] = '\0';
					i++;
				}else{
					continue;
				}
			}

			get_date_from_number(result, buffer);
			TP_ScrClrRect(4, 17, 124, 46);
			TP_LcdPrintf((uint8*)"%20s", buffer);
		}
	}
	
	return 0xFB;
}