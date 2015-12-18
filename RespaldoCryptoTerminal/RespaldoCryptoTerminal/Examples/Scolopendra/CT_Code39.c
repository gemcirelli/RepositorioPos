#include "CT_Code39.h"

static uint8 Code39Frame[CODE39_FRAME_BITSIZE];


void CT_CharToCode39Frame(const char input, uint8 * output){
	uint8 i, j, pos = 0;
	uint8 guide[9];
	for(i = 0; i < 44; i++){
		if(Code39ValidChars[i] == input)
			break;
	}

	for(j = 0; j < 9; j++){
		switch(Code39EncodingScheme[i][j]){
		case W:
			memset(output+pos,0,WIDE);
			pos+=WIDE;
			break;
		case B:
			memset(output+pos,1,WIDE);
			pos+=WIDE;
			break;
		case w:
			memset(output+pos,0,NARROW);
			pos+=NARROW;
			break;
		case b:
			memset(output+pos,1,NARROW);
			pos+=NARROW;
			break;
		}
		memset(output+pos,0,NARROW);
	}

}

uint32 CT_StringToCode39Frame(const char * input){
	uint32 size = strlen(input);
	uint32 i;
	uint8 code39char[CODE39_CHAR_WIDTH];
	for(i = 0; i < size; i++){
		CT_CharToCode39Frame(input[i],code39char);
		memcpy(Code39Frame+i*CODE39_CHAR_WIDTH,code39char,CODE39_CHAR_WIDTH);
	}
	return size*CODE39_CHAR_WIDTH;
}

uint32 CT_StringToCode39Bitmap(const char * input, uint8 * PrintBitMap){
	uint8 byte;
	uint32 i;
	uint32 size = CT_StringToCode39Frame(input);
	for(i = 0; i < size; i+=8){
		byte = 0;
		byte += Code39Frame[i+7] << 7;
		byte += Code39Frame[i+6] << 6;
		byte += Code39Frame[i+5] << 5;
		byte += Code39Frame[i+4] << 4;
		byte += Code39Frame[i+3] << 3;
		byte += Code39Frame[i+2] << 2;
		byte += Code39Frame[i+1] << 1;
		byte += Code39Frame[i];
		memset(PrintBitMap+i*384/8,byte,384);
	}
	return i;
}

