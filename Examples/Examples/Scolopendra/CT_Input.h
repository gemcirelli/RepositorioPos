#ifndef CT_INPUT
#define CT_INPUT
#ifdef __cplusplus
extern "C" {
#endif

#include <TP_ListBox.h>
#include "CT_GlobalData.h"
	
#define CT_DECIMAL 0
#define CT_INTEGER 1
#define CT_STRING 2
#define CT_HIDDEN 3
#define CT_DATE 4
#define	CT_IP 5

void CT_Notify(const char* text, uint32 Ms);
uint8 CT_Menu(const Item* items_list, uint8 size, uint8* selectedItem, TP_ListBoxStyle listboxStyle, const uchar* title);
uint8 CT_Confirm(const char* text);

uint8 CT_InputDecimalType(char * result);
uint8 CT_InputDateType(char * result, char * header);
uint8 CT_InputAnything(char* result, char* header, uint8 mode);

#ifdef __cplusplus
}
#endif

#endif

