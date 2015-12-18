#include "CT_Iso8583.h"

//bitmap de 64 bits, bitpos 1-64
void setBit(int bitPos, uint8 *bitmap){
	bitmap[(bitPos-1) / 8] += 128 >> ((bitPos-1) % 8);
}

uint8 testBit(int bitPos, uint8 *bitmap){
	uint8 testNum = bitmap[(bitPos-1) / 8];
	return testNum & (128 >> ((bitPos-1) % 8));
}

void parseIsoResponse(uint8 *input, iso8583 *isoOut){
	int index = 26;
	int fieldLen = 0;
	memcpy(isoOut->bitmap, &input[10], 8);
	isoOut->bitmap[8] = '\0';
	memcpy(isoOut->bitmap2, &input[18], 8);
	isoOut->bitmap2[8] = '\0';

	if(testBit(2,isoOut->bitmap)){
		memcpy(isoOut->field_2_len, &input[index], 2);
		isoOut->field_2_len[2] = '\0';
		index += 2;
		fieldLen = atoi((char *)isoOut->field_2_len);
		memcpy(isoOut->field_2, &input[index], fieldLen);
		index += fieldLen;
		isoOut->field_2[fieldLen] = '\0';
	}

	if(testBit(3,isoOut->bitmap)){
		memcpy(isoOut->field_3, &input[index], 6);
		isoOut->field_3[6] = '\0';
		index += 6;
	}

	if(testBit(4,isoOut->bitmap)){
		memcpy(isoOut->field_4, &input[index], 12);
		isoOut->field_4[12] = '\0';
		index += 12;
	}

	if(testBit(5,isoOut->bitmap)){
		memcpy(isoOut->field_5, &input[index], 12);
		isoOut->field_5[12] = '\0';
		index += 12;
	}
	
	if(testBit(6,isoOut->bitmap)){
		memcpy(isoOut->field_6, &input[index], 12);
		isoOut->field_6[12] = '\0';
		index += 12;
	}

	if(testBit(7,isoOut->bitmap)){
		memcpy(isoOut->field_7, &input[index], 10);
		isoOut->field_7[10] = '\0';
		index += 10;
	}

	if(testBit(11,isoOut->bitmap)){
		memcpy(isoOut->field_11, &input[index], 6);
		isoOut->field_11[6] = '\0';
		index += 6;
	}

	if(testBit(32,isoOut->bitmap)){
		memcpy(isoOut->field_32_len, &input[index], 2);
		isoOut->field_32_len[2] = '\0';
		index += 2;
		fieldLen = atoi((char *)isoOut->field_32_len);
		memcpy(isoOut->field_32, &input[index], fieldLen);
		index += fieldLen;
		isoOut->field_32[fieldLen] = '\0';
	}

	if(testBit(33,isoOut->bitmap)){
		memcpy(isoOut->field_33_len, &input[index], 2);
		isoOut->field_33_len[2] = '\0';
		index += 2;
		fieldLen = atoi((char *)isoOut->field_33_len);
		memcpy(isoOut->field_33, &input[index], fieldLen);
		index += fieldLen;
		isoOut->field_33[fieldLen] = '\0';
	}

	if(testBit(34,isoOut->bitmap)){
		memcpy(isoOut->field_34_len, &input[index], 2);
		isoOut->field_34_len[2] = '\0';
		index += 2;
		fieldLen = atoi((char *)isoOut->field_34_len);
		memcpy(isoOut->field_34, &input[index], fieldLen);
		index += fieldLen;
		isoOut->field_34[fieldLen] = '\0';
	}

	if(testBit(38,isoOut->bitmap)){
		memcpy(isoOut->field_38, &input[index], 6);
		isoOut->field_38[6] = '\0';
		index += 6;
	}

	if(testBit(39,isoOut->bitmap)){
		memcpy(isoOut->field_39, &input[index], 2);
		isoOut->field_39[2] = '\0';
		index += 2;
	}

	if(testBit(40,isoOut->bitmap)){
		memcpy(isoOut->field_40, &input[index], 3);
		isoOut->field_40[3] = '\0';
		index += 3;
	}

	if(testBit(44,isoOut->bitmap)){
		memcpy(isoOut->field_44_len, &input[index], 2);
		isoOut->field_44_len[2] = '\0';
		index += 2;
		fieldLen = atoi((char *)isoOut->field_44_len);
		memcpy(isoOut->field_44, &input[index], fieldLen);
		index += fieldLen;
		isoOut->field_44[fieldLen] = '\0';
	}

	if(testBit(45,isoOut->bitmap)){
		memcpy(isoOut->field_45_len, &input[index], 2);
		isoOut->field_45_len[2] = '\0';
		index += 2;
		fieldLen = atoi((char *)isoOut->field_45_len);
		memcpy(isoOut->field_45, &input[index], fieldLen);
		index += fieldLen;
		isoOut->field_45[fieldLen] = '\0';
	}

	if(testBit(48,isoOut->bitmap)){
		memcpy(isoOut->field_48_len, &input[index], 3);
		isoOut->field_48_len[3] = '\0';
		index += 3;
		fieldLen = atoi((char *)isoOut->field_48_len);
		memcpy(isoOut->field_48, &input[index], fieldLen);
		index += fieldLen;
		isoOut->field_48[fieldLen] = '\0';
	}

	if(testBit(104-64,isoOut->bitmap2)){
		memcpy(isoOut->field_104_len, &input[index], 3);
		isoOut->field_104_len[3] = '\0';
		index += 3;
		fieldLen = atoi((char *)isoOut->field_104_len);
		memcpy(isoOut->field_104, &input[index], fieldLen);
		index += fieldLen;
		isoOut->field_104[fieldLen] = '\0';
	}

	if(testBit(107-64,isoOut->bitmap2)){
		memcpy(isoOut->field_107_len, &input[index], 2);
		isoOut->field_107_len[2] = '\0';
		index += 2;
		fieldLen = atoi((char *)isoOut->field_107_len);
		memcpy(isoOut->field_107, &input[index], fieldLen);
		index += fieldLen;
		isoOut->field_107[fieldLen] = '\0';
	}
	
	if(testBit(108-64,isoOut->bitmap2)){
		memcpy(isoOut->field_108_len, &input[index], 2);
		isoOut->field_108_len[2] = '\0';
		index += 2;
		fieldLen = atoi((char *)isoOut->field_108_len);
		memcpy(isoOut->field_108, &input[index], fieldLen);
		index += fieldLen;
		isoOut->field_108[fieldLen] = '\0';
	}
	
	if(testBit(111-64,isoOut->bitmap2)){
		memcpy(isoOut->field_111_len, &input[index], 3);
		isoOut->field_111_len[3] = '\0';
		index += 3;
		fieldLen = atoi((char *)isoOut->field_111_len);
		memcpy(isoOut->field_111, &input[index], fieldLen);
		index += fieldLen;
		isoOut->field_111[fieldLen] = '\0';
	}

	if(testBit(127-64,isoOut->bitmap2)){
		memcpy(isoOut->field_127_len, &input[index], 4);
		isoOut->field_127_len[4] = '\0';
		index += 4;
		fieldLen = atoi((char *)isoOut->field_127_len);
		memcpy(isoOut->field_127, &input[index], fieldLen);
		index += fieldLen;
		isoOut->field_127[fieldLen] = '\0';
	}
}

uint16 isoToByteArray(iso8583 *isoIn, uint8 *output){
	uint16 len = 0;

	//set fields

	output[0] = '\0';
	strcat((char *)output,"000000");
	memcpy(&output[6],isoIn->mti, 4);

	memcpy(&output[10],isoIn->bitmap, 8);
	memcpy(&output[18],isoIn->bitmap2, 8);
	output[26] = '\0';

	if(testBit(2,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_2_len);
		strcat((char *)&output[26],(char *)isoIn->field_2);
	}

	if(testBit(3,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_3);
	}

	if(testBit(4,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_4);
	}

	if(testBit(5,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_5);
	}
	
	if(testBit(6,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_6);
	}

	if(testBit(7,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_7);
	}

	if(testBit(11,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_11);
	}

	if(testBit(32,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_32_len);
		strcat((char *)&output[26],(char *)isoIn->field_32);
	}

	if(testBit(33,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_33_len);
		strcat((char *)&output[26],(char *)isoIn->field_33);
	}

	if(testBit(34,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_34_len);
		strcat((char *)&output[26],(char *)isoIn->field_34);
	}

	if(testBit(38,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_38);
	}

	if(testBit(39,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_39);
	}

	if(testBit(40,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_40);
	}

	if(testBit(44,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_44_len);
		strcat((char *)&output[26],(char *)isoIn->field_44);
	}
	if(testBit(45,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_45_len);
		strcat((char *)&output[26],(char *)isoIn->field_45);
	}
	if(testBit(46,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_46_len);
		strcat((char *)&output[26],(char *)isoIn->field_46);
	}
	if(testBit(48,isoIn->bitmap)){
		strcat((char *)&output[26],(char *)isoIn->field_48_len);
		strcat((char *)&output[26],(char *)isoIn->field_48);
	}

	if(testBit(104-64,isoIn->bitmap2)){
		strcat((char *)&output[26],(char *)isoIn->field_104_len);
		strcat((char *)&output[26],(char *)isoIn->field_104);
	}

	if(testBit(111-64,isoIn->bitmap2)){
		strcat((char *)&output[26],(char *)isoIn->field_111_len);
		strcat((char *)&output[26],(char *)isoIn->field_111);
	}

	if(testBit(127-64, isoIn->bitmap2)){
		strcat((char *)&output[26], (char *)isoIn->field_127_len);
		strcat((char *)&output[26], (char *)isoIn->field_127);
	}

	len = strlen((char *)&output[26]) + 20;
	output[5] = '0' + len % 10;
	output[4] = '0' + (len / 10) % 10;
	output[3] = '0' + (len / 100) % 10;
	output[2] = '0' + (len / 1000) % 10;
	output[1] = '0' + (len / 10000) % 10;
	output[0] = '0' + (len / 100000) % 10;

	return len+6;
}
