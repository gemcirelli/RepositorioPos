#include <TPTypedef.h>
#include <TPFile.h>
#include <TPSerial.h>
#include "TP_ParamsManager.h"
#include "TP_SerialDataProc.h"

#define min(x, y) ((x) < (y) ? (x) : (y))

#define MIN_SERIAL_PROCOTOL_LENGTH 7
#define SERIAL_PARAM_SEPARATOR "\x0A\x0D\x0A\x0D"

uint8 SerialDataBuf[1024] = {0};
uint16 SerialDataBufLen = 0;

/**
 *   ≈‰±£¥Ê≤Œ ˝
 *
 * @author linhx (2012/9/19)
 *
 * @param paramsId
 * @param dataVal
 *
 * @return Boolean
 */
static Boolean SetParamItemAdapt(PARAMS_NAMES paramsId,char* dataVal)
{
    Data data = {0};
    int32 value = 0;
    if (TP_Get_ParamsItem_Val(paramsId,&data)<0)
    {
        return FALSE;
    }

    switch (data.valueType)
    {
    case VAT_Int32:
        value = atoi(dataVal);
        if (TP_Format_Data(&data,VAT_Int32,sizeof(int32),&value) < 0)
        {
            return FALSE;
        }
        break;
    case VAT_String:
    case VAT_Stream:
         if (TP_Format_Data(&data,(ValueType)data.valueType,min(sizeof(data.dBuffer.cBuffer)-1,
                            strlen((char *)dataVal)),dataVal) <0)
        {
            return FALSE;
        }
        break;
    default:
        return FALSE;
    }
    if (TP_Set_ParamsItem_Val(paramsId,&data)<0)
    {
        return FALSE;
    }
    return TRUE;
}

uint8 pG_Temp[1024] = {0};
uint16 G_Len = 0;
static void Save_IniParam( const uint8* Data, const uint8 DataLen )
{
    int   SplicharLen = strlen((char *)SERIAL_PARAM_SEPARATOR);
    int32 ret;
    uint8 LastValueIndex = 0;
    uint8 i = 0;
    uint8 Len = 0;
    char Temp[MAX_STR_PARAM_LEN+8] = {0};
    int CopyLen = 0;
    PARAMS_NAMES paramsId = (PARAMS_NAMES)((uint32)NAME_PARAMS(NULL)+1);

    CopyLen = min( 1024-G_Len, DataLen );
    memcpy( pG_Temp+G_Len, Data, CopyLen );
    G_Len += CopyLen;

    if ( SplicharLen > G_Len )
    {
        return;
    }
    else
    {
        Len = G_Len - SplicharLen + 1;
    }

    for ( i = 0; i < Len; )
    {
        if ( memcmp( pG_Temp+i, SERIAL_PARAM_SEPARATOR, SplicharLen) == 0 )
        {
            /*Modify by linhx 2012-09-19*/
            if ( paramsId >= NAME_PARAMS(COUNT))
            {
                break;
            }

            CopyLen = min( MAX_STR_PARAM_LEN-1, i-LastValueIndex);

            if ( CopyLen > 0 )
            {
                memset( Temp, 0, sizeof(Temp));
                memcpy( Temp, pG_Temp+LastValueIndex, CopyLen );

                SetParamItemAdapt(paramsId,Temp);
            }

            paramsId++;
            i += SplicharLen;
            LastValueIndex = i;

            /*End modify by linhx 2012-09-19*/
        }
        else
        {
            ++i;
        }
    }

    if ( G_Len-LastValueIndex <= sizeof(Temp) )
    {
        G_Len = G_Len-LastValueIndex;
        memcpy( Temp, pG_Temp+LastValueIndex, G_Len );
        memcpy( pG_Temp, Temp, G_Len );
    }
    else
    {
        memcpy( Temp, pG_Temp+LastValueIndex, MAX_STR_PARAM_LEN );
        memcpy( pG_Temp, Temp, MAX_STR_PARAM_LEN );

        memcpy( Temp, pG_Temp + G_Len - SplicharLen, SplicharLen );
        memcpy( pG_Temp + MAX_STR_PARAM_LEN, Temp, SplicharLen );

        G_Len = MAX_STR_PARAM_LEN + SplicharLen;
    }

}

/**************************************************************************************************************
∫Ø ˝£∫uint8    ucComProc( uint8  uc_order, uint8  uc_suborder, uint8  uc_len,  uint8  *uc_data )
π¶ƒ‹£∫
∑µªÿ£∫
0£≠≤ªøe ∂±£¨1£≠¥¶¿i≥eπ¶£¨3£≠–ßne¥iŒo£¨
2£≠ ˝æ›‘ΩΩÁ£¨5£≠≥§∂»≤ªπª£¨ff£≠“næ≠o¶¥aÀ
◊¢“‚£∫
1°¢wuweicong 20080418 Œﬁ¬€ ≤√¥≥ˆ¥i‘≠“Ú£¨∂ºo¶¥0x03
**************************************************************************************************************/
static uint8 ucComProc( const uint8  uc_order, const uint8  uc_suborder, const uint8  uc_len,  uint8  *uc_data )
{
    //showLcdDebug( "ucComProc:%x %x %x %x\n", uc_order, uc_suborder, uc_len, *uc_data );
    switch(uc_order)
    {
    case 0x00: //±£¥Ê ˝æ›,¥Æøuœ¬‘ÿ0x00œ¬‘ÿΩa ¯œ˚œ¢
        return  0x01;

    case 0x06://–£’˝º∆∑n∆˜ ±÷o  0X06+YYMMDDHHMMSS+–«∆u[1]  PBCD
        {
            char dateTime[15] = {0};
            uint8 result = 0;

            sprintf( dateTime, "20%02x%02x%02x%02x%02x%02x",
                    uc_data[0], uc_data[1], uc_data[2], uc_data[3], uc_data[4], uc_data[5] );

            result = TP_SetDateTimeEx(dateTime);
            if ( result == 0 )
            {
                return  0x01;
            }
            else
            {
                return  0x00;
            }
            //showLcdDebug( "ucComProc 0x06 result = %d\n", result );
        }
        break;

    case 0x40: //πÃ∂®∞Ê±æ
        TP_DefaultPortSends((uint8 *)"\xfe\xfe\x60\x02\x0c\x0c\x01\x1b\xff\xff",10);
        return  0xFF;

    case 0x41://’Ê µ»iº˛∞Ê±æ
        TP_DefaultPortSends((uint8 *)"\xFE\xFE\x60\x0D\x16\x05\x0B\x33\xFF\xFF",10);
        return  0xFF;
    case 0x5C:
        if ( uc_len > 0 )
        {
            if ( uc_data[0] == 0 )
            {
                G_Len = 0;
            }
            Save_IniParam( uc_data+1, uc_len-1 );
        }
        return  0x01;
    }
}

static void UpdateParam( uint8 * Data, uint16 DataLen )
{
    int16  j = 0 ;
    int16 Check_end_Index = 0 ,check_begin_Index = 0, checknum = 0;
    uint8 ReplyBuf[] = { 0xFE, 0xFE, 0, 0, 0, 0xFF, 0xFF };
    Check_end_Index = Data[1]+4;       //«o–£neΩa ¯Œª
    if( Check_end_Index < (DataLen-2) )      //±£÷§ ˝◊e≤ª“Á≥ˆ
    {
        checknum = 0;
        check_begin_Index = 2;                 //«o–£neø™ ºŒª
        for( j = check_begin_Index; j < Check_end_Index; j++ )
        {
            checknum += Data[j];
        }
        checknum = checknum%0xfe;

        if(checknum == Data[Check_end_Index])
        {
            ReplyBuf[4] = ucComProc(Data[3],Data[2],Data[1],&Data[4]);

            if ( ReplyBuf[4] != 0xFF )
            {
                ReplyBuf[2] = checknum;
                ReplyBuf[3] = Data[2];
                TP_DefaultPortSends( ReplyBuf, sizeof(ReplyBuf));
            }
        }
    }
}

/**
 * Parse the received data from default serial port.
 *
 * @author linhx (2012/10/8)
 *
 * @param data_p Received data.
 * @param dataLen Length of received data.
 */
void TP_DefaultSerialDataParse(uint8 *data_p,uint16 dataLen)
{
    memcpy( SerialDataBuf+SerialDataBufLen , data_p, dataLen );
    SerialDataBufLen += dataLen;

    //≤Œ ˝œ¬‘ÿ
    if ( SerialDataBuf[0] == 0xFE )
    {
        if ( SerialDataBufLen < MIN_SERIAL_PROCOTOL_LENGTH + SerialDataBuf[1] )
        {
            return;
        }
        if (SerialDataBuf[MIN_SERIAL_PROCOTOL_LENGTH + SerialDataBuf[1]-2] == 0xFF &&
            SerialDataBuf[MIN_SERIAL_PROCOTOL_LENGTH + SerialDataBuf[1]-1] == 0xFF)
        {
            UpdateParam( SerialDataBuf, SerialDataBufLen );
        }
    }
    memset(SerialDataBuf,0x00,sizeof(SerialDataBuf));
    SerialDataBufLen = 0;
}
