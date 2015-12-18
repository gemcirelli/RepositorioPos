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

	//amount 2
	uint8 field_5[13];

	//amount 3
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

	//bank number
	uint8 field_44_len[3];
	uint8 field_44[26];

	//distributer id
	uint8 field_45_len[3];
	uint8 field_45[9];

	uint8 field_46_len[4];
	uint8 field_46[9];

	//route id
	uint8 field_48_len[4];
	uint8 field_48[9];

	//user data
	uint8 field_104_len[4];
	uint8 field_104[250];
	
	//aditional data
	uint8 field_107_len[4];
	uint8 field_107[25];
	
	//aditional data
	uint8 field_108_len[4];
	uint8 field_108[25];

	//extra data
	uint8 field_111_len[4];
	uint8 field_111[11];

	//serials to confirm
	uint8 field_127_len[5];
	uint8 field_127[9999];

} iso8583;

void setBit(int bitPos, uint8 *bitmap);
uint8 testBit(int bitPos, uint8 *bitmap);
void parseIsoResponse(uint8 *input, iso8583 *isoOut);
uint16 isoToByteArray(iso8583 *isoIn, uint8 *output);


#ifdef __cplusplus
}
#endif

#endif