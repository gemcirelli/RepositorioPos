#ifndef _ISO8583_H_
#define _ISO8583_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <TPTypedef.h>
#include <TPBase.h>
typedef struct iso8583_frame{
	uint8 len_header[7];
	uint8 mti[5];
	uint8 bitmap[9];
	uint8 bitmap2[9];
	//smart card / account number (LLVAR)
	uint8 field_2_len[3];
	uint8 field_2[20];
	//processing code
	uint8 field_3[7];
	//amount 1
	uint8 field_4[13];
	//amount 2 / balance
	uint8 field_5[13];
	//amount 3 / pending balance
	uint8 field_6[13];
	//date 10
	uint8 field_7[11];
	//security token
	uint8 field_11[7];
	//company id
	uint8 field_32_len[3];
	uint8 field_32[12];
	//user id
	uint8 field_33_len[3];
	uint8 field_33[12];
	//user company id
	uint8 field_34_len[3];
	uint8 field_34[29];
	//log code
	uint8 field_38[7];
	//event code
	uint8 field_39[3];
	//response code
	uint8 field_40[4];
	//id POS
	uint8 field_44_len[3];
	uint8 field_44[26];
	//distributer id
	uint8 field_45_len[3];
	uint8 field_45[9];
	//bank id
	uint8 field_46_len[4];
	uint8 field_46[9];
	//route id
	uint8 field_48_len[4];
	uint8 field_48[9];
	//e_customer id
	uint8 field_100_len[4];
	uint8 field_100[12];
	//product type
	uint8 field_102_len[4];
	uint8 field_102[29];
	//user level or product code
	uint8 field_103_len[4];
	uint8 field_103[29];
	//user data or product item 
	uint8 field_104_len[4];
	uint8 field_104[100];
	//product quantity 
	uint8 field_105_len[4];
	uint8 field_105[100];
	//aditional data
	uint8 field_107_len[4];
	uint8 field_107[25];
	//aditional data / PIN
	uint8 field_108_len[4];
	uint8 field_108[25];
	//aditional data / program days
	uint8 field_109_len[4];
	uint8 field_109[25];
	//extra data / response message
	uint8 field_111_len[4];
	uint8 field_111[100];
	//extra data 
	uint8 field_118_len[3];
	uint8 field_118[100];
	//DIRECTV TransactionID
	uint8 field_119_len[4];
	uint8 field_119[100];
	//DIRECTV Days of Programation
	uint8 field_120_len[4];
	uint8 field_120[100];
	//DIRECTV Desconection Date
	uint8 field_121_len[4];
	uint8 field_121[100];
	//extra data 
	uint8 field_122_len[4];
	uint8 field_122[100];
	//extra data
	uint8 field_123_len[4];
	uint8 field_123[100];
	//extra data
	uint8 field_124_len[4];
	uint8 field_124[100];
	//extra data
	uint8 field_125_len[4];
	uint8 field_125[100];
	//serials to confirm / serials to confirm
	uint8 field_127_len[5];
	uint8 field_127[9999];
} iso8583;

typedef struct iso8583_new{
	uint8 len_header[7];
	uint8 mti[5];
	uint8 bitmap[9];
	uint8 bitmap2[9];
    struct node *first;
	struct node *last;

}iso8583_new;

typedef struct node{
 // char *campop;
  uint8 field;
  char* value;
  struct node *next;
}nodo;


void setBit(int bitPos, uint8 *bitmap);
uint8 testBit(int bitPos, uint8 *bitmap);
//void parseIsoResponse(uint8 *input, iso8583 *isoOut);
void parseIsoResponse(uint8 *input, iso8583_new *isoOut);
uint16 isoToByteArray(iso8583_new *isoIn, uint8 *output);
void clearIso8583(iso8583 *isoIn);
//void clearIso8583L(iso8583_new *isoIn);

#ifdef __cplusplus
}
#endif

#endif