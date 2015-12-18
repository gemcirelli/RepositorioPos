#include "CT_Iso8583.h"

//bitmap de 64 bits, bitpos 1-64
void setBit(int bitPos, uint8 *bitmap){
	if(!testBit(bitPos, bitmap))
		bitmap[(bitPos-1) / 8] += 128 >> ((bitPos-1) % 8);
}

void add_element_p(iso8583_new * iso, char* cadena, uint8 field) { 
	nodo *nuevo;
	uint8 length;
	
	nuevo = malloc (sizeof(nodo));
	nuevo->next=NULL;
	nuevo->field=field;
	length = strlen(cadena);
	nuevo->value = malloc((sizeof(char)*length)+1);
	strcpy((char*)nuevo->value, cadena);
	if(iso->first==NULL){
		iso->first=nuevo;
		iso->last=nuevo;
	}
	if(iso->last !=NULL){
		iso->last->next=nuevo;
		iso->last=nuevo;
	
	}
	  
}
uint8 testBit(int bitPos, uint8 *bitmap){
	uint8 byte_position = ((bitPos-1) / 8);
	uint8 testNum = bitmap[byte_position];
	uint8 bit_position = ((bitPos-1) % 8);
	uint8 test = (128 >> bit_position);
	return testNum & test;
}


void parseIsoResponse(uint8 *input, iso8583_new *isoOut){
	int index = 26;
	int fieldLen = 0;
	int i;
	size_t leng;
	char* value;
	
	nodo *current_field;
	uint8 fielddos[300];
	uint8 field_llv_len[300];
	uint8 field_lllv_len[400];
	uint8 auxlen[400];
	uint8 aux[400];
	uint8 field_llllv_len[500];
	uint8 field_index6[700];
	uint8 field_index10[1100];
	uint8 field_index12[1300];
	uint8 field_index3[400];
	uint8 fieldllvar[300];
	uint8 fieldlllvar[400];
	uint8 fieldllllvar[9999];
	uint8 limit=64;
	uint8 field_present = 0;
	memcpy(isoOut->mti, &input[6], 4);
	isoOut->mti[4] = '\0';
	memcpy(isoOut->bitmap, &input[10], 8);
	isoOut->bitmap[8] = '\0';
	memcpy(isoOut->bitmap2, &input[18], 8);
	isoOut->bitmap2[8] = '\0';
	value=NULL;
	
	//checks if the second bitmap is present
	if(testBit(1,isoOut->bitmap)){
	 limit=128;
	}

	for(i=2;i<=limit;i++){
		if(i<65){
			field_present = testBit(i,isoOut->bitmap);
		 } else {
			field_present = testBit(i-64,isoOut->bitmap2);
		 }
		 if(field_present>0){
			switch(i){
			        case 2 : case 32 :case 33 : case 34 :  case 44 : 
					case 45:  case 107: case 108: case 109: case 118:  case 123: case 124:
						memcpy(field_llv_len, &input[index], 2);
						field_llv_len[2]= '\0';
						index += 2;
						fieldLen = atoi((char *)field_llv_len);
						memcpy(fieldllvar, &input[index], fieldLen);
						value= (char*) fieldllvar;
						value[fieldLen] = '\0';
						add_element_p(isoOut,value,i);
						index += fieldLen;
					break;

					case 100:  case 102: case 103: case 105: case 122:
						memcpy(field_llv_len, &input[index], 2);
						field_llv_len[3]= '\0';
						index += 2;
						fieldLen = atoi((char *)field_llv_len);
						memcpy(fieldllvar, &input[index], fieldLen);
						value= (char*) fieldllvar;
						value[fieldLen] = '\0';
						add_element_p(isoOut,value,i);
						index += fieldLen;
					break;
					
					case 39:
						memcpy(fielddos, &input[index], 2);
						index += 2;
						value= (char*) fielddos;
						value[2]= '\0';
						add_element_p(isoOut, value , i);
					break;	
				case 4: case 5: case 6:	 
						memcpy(field_index12, &input[index], 12);
						index += 12;
						value= (char*) field_index12;
						value[12]= '\0';
						add_element_p(isoOut, value , i);
					break;	

				case 7: 
						memcpy(field_index10, &input[index], 10);
						index += 10;
						value= (char*) field_index10;
						value[10]= '\0';
						add_element_p(isoOut, value , i);
					break;	

				case 3: case 11: case 38: 	
						memcpy(field_index6, &input[index], 6);
						index += 6;
						value= (char*) field_index6;
						value[6]= '\0';
						add_element_p(isoOut, value , i);								
						break;	

				case 40:
						memcpy(field_index3, &input[index], 3);
						index += 3;
						value= (char*) field_index3;
						value[3]= '\0';
						add_element_p(isoOut, value , i);	
					break;

			    case 48:    case 104:    case 111:
			    case 119: case 120: case 121:  
						memcpy(auxlen, &input[index], 3);
						auxlen[3]= '\0';
						index += 3;
						fieldLen = atoi((char *)auxlen);
						memcpy(aux, &input[index], fieldLen);
						value= (char*) aux;
						value[fieldLen]= '\0';
						add_element_p(isoOut, value , i);
						index += fieldLen;
					break;
				
				case 127:
						memcpy(field_llllv_len, &input[index], 4);
						field_llllv_len[3]= '\0';
						index += 4;
						fieldLen = atoi((char *)field_llllv_len);
						memcpy(fieldllllvar, &input[index], fieldLen);
						value= (char*) fieldllllvar;
						value[fieldLen]= '\0';
						add_element_p(isoOut, value , i);
						index += fieldLen;
						break;	


			}
		}  
	}
}

uint16 isoToByteArray(iso8583_new *isoIn, uint8 *output){
	uint16 len = 0;

	//variables nuevas
	int i;
	size_t leng;
	char lenStr[5];
	nodo* current_field;

	//set fields
	output[0] = '\0';
	strcat((char *)output,"000000");
	memcpy(&output[6],isoIn->mti, 4);
	memcpy(&output[10],isoIn->bitmap, 8);
	memcpy(&output[18],isoIn->bitmap2, 8);
	output[26] = '\0';
	
	current_field=isoIn->first;
	
	//i=2;
	while( current_field !=NULL){
		i=(current_field->field);
		switch(i){	
			case 2 : case 32 :case 33 : case 34 : case 44 : case 45: case 118:
						leng= strlen(current_field->value);
						sprintf(lenStr,"%02d",leng);
						strcat((char *)&output[26],lenStr);
						strcat((char *)&output[26],(char *)current_field->value);
						break;					
			case 46 : case 48: case 100: case 102: case 103: case 104: case 105: case 107: case 108: case 109: case 111:
			case 119: case 120: case 121: case 122: case 123: case 124: case 125:
				   		leng= strlen(current_field->value);
						sprintf(lenStr,"%03d",leng);
						strcat((char *)&output[26],lenStr);
						strcat((char *)&output[26],(char *)current_field->value);
						break;
			 case 127:
				  		leng= strlen(current_field->value);
						sprintf(lenStr,"%04d",leng);
						strcat((char *)&output[26],lenStr);
						strcat((char *)&output[26],(char *)current_field->value);
						break;
			 case 3 : case 4: case 5: case 6: case 7: case 11: case 38: case 39: case 40:
						strcat((char *)&output[26],(char *)current_field->value);
						break;

			}
			current_field=current_field->next;
		
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

 
void clearIso8583(iso8583 *isoIn){
	int i;

	memset(isoIn->mti,0,sizeof(isoIn->mti));
	memset(isoIn->bitmap,0,sizeof(isoIn->bitmap));
	memset(isoIn->bitmap2,0,sizeof(isoIn->bitmap2));
	memset(isoIn->field_2_len,0,sizeof(isoIn->field_2_len));
	memset(isoIn->field_2,0,sizeof(isoIn->field_2));
	//processing code
	//uint8 field_3[7];
	memset(isoIn->field_3,0,sizeof(isoIn->field_3));
	//amount 1
	//uint8 field_4[13];
	memset(isoIn->field_4,0,sizeof(isoIn->field_4));
	//amount 2 / balance
	//uint8 field_5[13];
	memset(isoIn->field_5,0,sizeof(isoIn->field_5));
	//amount 3 / pending balance
	//uint8 field_6[13];
	memset(isoIn->field_6,0,sizeof(isoIn->field_6));
	//date 10
	//uint8 field_7[11];
	memset(isoIn->field_7,0,sizeof(isoIn->field_7));
	//security token
	//uint8 field_11[7];
	memset(isoIn->field_11,0,sizeof(isoIn->field_11));
	//company id
	//uint8 field_32_len[3];
	//uint8 field_32[12];
	memset(isoIn->field_32_len,0,sizeof(isoIn->field_32_len));
	memset(isoIn->field_32,0,sizeof(isoIn->field_32));
	//user id
	//uint8 field_33_len[3];
	//uint8 field_33[12];
	memset(isoIn->field_33_len,0,sizeof(isoIn->field_33_len));
	memset(isoIn->field_33,0,sizeof(isoIn->field_33));
	//user company id
	//uint8 field_34_len[3];
	//uint8 field_34[29];
	memset(isoIn->field_34_len,0,sizeof(isoIn->field_34_len));
	memset(isoIn->field_34,0,sizeof(isoIn->field_34));
	//log code
	//uint8 field_38[7];
	memset(isoIn->field_38,0,sizeof(isoIn->field_38));
	//event code
	//uint8 field_39[3];
	memset(isoIn->field_39,0,sizeof(isoIn->field_39));
	//response code
	//uint8 field_40[4];
	memset(isoIn->field_40,0,sizeof(isoIn->field_40));
	//id POS
	//uint8 field_44_len[3];
	//uint8 field_44[26];
	memset(isoIn->field_44_len,0,sizeof(isoIn->field_44_len));
	memset(isoIn->field_44,0,sizeof(isoIn->field_44));
	//distributer id
	//uint8 field_45_len[3];
	//uint8 field_45[9];
	memset(isoIn->field_45_len,0,sizeof(isoIn->field_45_len));
	memset(isoIn->field_45,0,sizeof(isoIn->field_45));
	//bank id
	//uint8 field_46_len[4];
	//uint8 field_46[9];
	memset(isoIn->field_46_len,0,sizeof(isoIn->field_46_len));
	memset(isoIn->field_46,0,sizeof(isoIn->field_46));
	//route id
	//uint8 field_48_len[4];
	//uint8 field_48[9];
	memset(isoIn->field_48_len,0,sizeof(isoIn->field_48_len));
	memset(isoIn->field_48,0,sizeof(isoIn->field_48));
	//e_customer id
	//uint8 field_100_len[4];
	//uint8 field_100[12];
	memset(isoIn->field_100_len,0,sizeof(isoIn->field_100_len));
	memset(isoIn->field_100,0,sizeof(isoIn->field_100));
	//product type
	//uint8 field_102_len[4];
	//uint8 field_102[29];
	memset(isoIn->field_102_len,0,sizeof(isoIn->field_102_len));
	memset(isoIn->field_102,0,sizeof(isoIn->field_102));
	//user level or product code
	//uint8 field_103_len[4];
	//uint8 field_103[29];
	memset(isoIn->field_103_len,0,sizeof(isoIn->field_103_len));
	memset(isoIn->field_103,0,sizeof(isoIn->field_103));
	//user data or product item 
	//uint8 field_104_len[4];
	//uint8 field_104[100];
	memset(isoIn->field_104_len,0,sizeof(isoIn->field_104_len));
	memset(isoIn->field_104,0,sizeof(isoIn->field_104));
	//product quantity 
	//uint8 field_105_len[4];
	//uint8 field_105[100];
	memset(isoIn->field_105_len,0,sizeof(isoIn->field_105_len));
	memset(isoIn->field_105,0,sizeof(isoIn->field_105));
	//aditional data
	//uint8 field_107_len[4];
	//uint8 field_107[25];
	memset(isoIn->field_107_len,0,sizeof(isoIn->field_107_len));
	memset(isoIn->field_107,0,sizeof(isoIn->field_107));
	//aditional data / PIN
	//uint8 field_108_len[4];
	//uint8 field_108[25];
	memset(isoIn->field_108_len,0,sizeof(isoIn->field_108_len));
	memset(isoIn->field_108,0,sizeof(isoIn->field_108));
	//aditional data / program days
	//uint8 field_109_len[4];
	//uint8 field_109[25];
	memset(isoIn->field_109_len,0,sizeof(isoIn->field_109_len));
	memset(isoIn->field_109,0,sizeof(isoIn->field_109));
	//extra data / response message
	//uint8 field_111_len[4];
	//uint8 field_111[100];
	memset(isoIn->field_111_len,0,sizeof(isoIn->field_111_len));
	memset(isoIn->field_111,0,sizeof(isoIn->field_111));
	//extra data 
	//uint8 field_122_len[4];
	//uint8 field_122[100];
	memset(isoIn->field_118_len,0,sizeof(isoIn->field_118_len));
	memset(isoIn->field_118,0,sizeof(isoIn->field_118));
	//extra data 
	//uint8 field_122_len[4];
	//uint8 field_122[100];
	memset(isoIn->field_122_len,0,sizeof(isoIn->field_122_len));
	memset(isoIn->field_122,0,sizeof(isoIn->field_122));
	//extra data
	//uint8 field_123_len[4];
	//uint8 field_123[100];
	memset(isoIn->field_123_len,0,sizeof(isoIn->field_123_len));
	memset(isoIn->field_123,0,sizeof(isoIn->field_123));
	//extra data
	//uint8 field_124_len[4];
	//uint8 field_124[100];
	memset(isoIn->field_124_len,0,sizeof(isoIn->field_124_len));
	memset(isoIn->field_124,0,sizeof(isoIn->field_124));
	//serials to confirm / serials to confirm
	//uint8 field_127_len[5];
	//uint8 field_127[9999];
	memset(isoIn->field_127_len,0,sizeof(isoIn->field_127_len));
	memset(isoIn->field_127,0,sizeof(isoIn->field_127));
}

