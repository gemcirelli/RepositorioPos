/******************************************************************************
* Include Headers
*******************************************************************************/
#include "TP_BarCode.h"

/******************************************************************************
* Macros
*******************************************************************************/
#define WIDE_PIXEL_BIG     (6)
#define NARROW_PIXEL_BIG   (2)

#define WIDE_PIXEL_SMALL   (3)
#define NARROW_PIXEL_SMALL (1)

/******************************************************************************
* Private Variables
*******************************************************************************/
static uint8
PrintBitMap[384 * 12];


/******************************************************************************
* I2OF5
*******************************************************************************/
#define I25_START_WIDTH    (4)
#define I25_CHAR_WIDTH     (9)
#define I25_END_WIDTH      (5)

#define N (0)
#define W (1)

static const uint8
I25CodeScheme[10][5] =
{
    /* 0 */
    {N,N,W,W,N},
    /* 1 */
    {W,N,N,N,W},
    /* 2 */
    {N,W,N,N,W},
    /* 3 */
    {W,W,N,N,N},
    /* 4 */
    {N,N,W,N,W},
    /* 5 */
    {W,N,W,N,N},
    /* 6 */
    {N,W,W,N,N},
    /* 7 */
    {N,N,N,W,W},
    /* 8 */
    {W,N,N,W,N},
    /* 9 */
    {N,W,N,W,N}
};

/******************************************************************************
* Private Functions
*******************************************************************************/

/**
 * Add a bar of specific type to buffer
 *
 * @author zhangl
 *
 * @version 1.0
 *
 * @param barType
 * @param outBuf
 *
 * @return
 *
 * @date 2012-8-14
 */
static uint8 addI25Bar(uint8 barType, uint8* outBuf, uint8 isBig)
{
    if (barType == W)
    {
        memset(outBuf, 0xFF, WIDE_PIXEL_SMALL * (isBig + 1));

        return WIDE_PIXEL_SMALL * (isBig + 1);
    }
    else if (barType == N)
    {
        memset(outBuf, 0xFF, NARROW_PIXEL_SMALL * (isBig + 1));

        return NARROW_PIXEL_SMALL * (isBig + 1);
    }

    return 0;
}


/**
 * Add a space of specfic type to buffer
 *
 * @author zhangl
 *
 * @version 1.0
 *
 * @param spaceType
 * @param outBuf
 *
 * @return
 *
 * @date 2012-8-14
 */
static uint8 addI25Space(uint8 spaceType, uint8* outBuf, uint8 isBig)
{
    if (spaceType == W)
    {
        memset(outBuf, 0x00, WIDE_PIXEL_SMALL * (isBig + 1));

        return  WIDE_PIXEL_SMALL * (isBig + 1);
    }
    else if (spaceType == N)
    {
        memset(outBuf, 0x00, NARROW_PIXEL_SMALL * (isBig + 1));

        return NARROW_PIXEL_SMALL * (isBig + 1);
    }

    return 0;
}


/**
* Convert a pair of digits to interleaved 2 of 5 barcode
*/
static uint8 getI25PairBitMap(const char* pInput, uint8* outBuf, uint8 isBig)
{
    uint8 iByte;
    uint8 iCode;

    iByte = 0;
    for (iCode = 0; iCode < 5; iCode++)
    {
       if (I25CodeScheme[pInput[0] - '0'][iCode] == W)
       {
           iByte += addI25Bar(W, &outBuf[iByte], isBig);
       }
       else
       {
           iByte += addI25Bar(N, &outBuf[iByte], isBig);
       }

       if (I25CodeScheme[pInput[1] - '0'][iCode] == W)
       {
           iByte += addI25Space(W, &outBuf[iByte], isBig);
       }
       else
       {
           iByte += addI25Space(N, &outBuf[iByte], isBig);
       }
    }

    return iByte;
}


/**
* Build interleaved 2 of 5 barcode bitmap for the input string
* @author Yang Zhirui
* @version 1.0
* @param *numString buffer of figures
* @return 0xFE length of input string is invalid
          0xFD Input invalid char
*         0x00 Succeed
*
* @since Aug 12, 2012
*/
static int32
getI25CodeBitMap(uint8*      outBuf,
                 uint32      bufSize,
                 const char* pInput,
                 uint8       nInput)
{

    uint32 iByte;
    uint32 width;
    uint32 nInputs;
    uint8  iChar;
    uint8  isBig;

    nInputs = nInput;

    /* Choose pixels */
    isBig = 1;
    width = ((uint32)(I25_START_WIDTH * 2)          +
             (uint32)(nInputs * I25_CHAR_WIDTH * 2) +
             (uint32)(I25_END_WIDTH * 2));

    if (width > TP_PRINT_RIGHT_BOUNDARY)
    {
        isBig = 0;
        width = ((uint32)(I25_START_WIDTH)          +
                 (uint32)(nInputs * I25_CHAR_WIDTH) +
                 (uint32)(I25_END_WIDTH));
    }

    if (width > TP_PRINT_RIGHT_BOUNDARY)
    {
        return -1;
    }

    /* Center-Align the bitmap */
    iByte = (TP_PRINT_RIGHT_BOUNDARY - width) / 2;

    /* Start Zone */
    iByte += addI25Bar(N,   &outBuf[iByte], isBig);
    iByte += addI25Space(N, &outBuf[iByte], isBig);
    iByte += addI25Bar(N,   &outBuf[iByte], isBig);
    iByte += addI25Space(N, &outBuf[iByte], isBig);

    /* Code Zone */
    for (iChar = 0; iChar < nInput; iChar += 2)
    {
        iByte += getI25PairBitMap(&pInput[iChar], &outBuf[iByte], isBig);
    }

    /* Stop Zone */
    iByte += addI25Bar(W,   &outBuf[iByte], isBig);
    iByte += addI25Space(N, &outBuf[iByte], isBig);
    iByte += addI25Bar(N,   &outBuf[iByte], isBig);

    /* Adjust the height */
    for (iByte = 1; iByte < 12; iByte++)
    {
        memcpy(&outBuf[384 * iByte], outBuf, 384);
    }

    return 96;
}


static uint8
printI25Code(const char* pInput,
             uint8       nInput)
{
    int32  height;
    uint8  retVal;
    uint8  iChar;

    if (pInput == PNULL)
    {
        return 0x01;
    }

    if (nInput == 0)
    {
        return 0x02;
    }

    if (nInput > 40)
    {
        return 0x02;
    }

    if ((nInput % 2) != 0)
    {
        return 0x02;
    }

    for (iChar = 0; iChar < nInput; iChar++)
    {
        if (isdigit((char)pInput[iChar]) == 0)
        {
            return 0x03;
        }
    }

    memset(PrintBitMap, 0x00, sizeof(PrintBitMap));
    height = getI25CodeBitMap(PrintBitMap, sizeof(PrintBitMap), pInput, nInput);
    if (height < 0)
    {
        retVal = 0x04;
    }
    else
    {
        retVal = TP_PrnLogo(PrintBitMap, 384, height);
    }

    return retVal;
}

#undef N
#undef W


/******************************************************************************
* CODE 39
*******************************************************************************/
#define CODE39_CHAR_WIDTH  (9 * NARROW_PIXEL_SMALL + 3 * (WIDE_PIXEL_SMALL - NARROW_PIXEL_SMALL))

#define CODE128_CHAR_WIDTH  (11 * NARROW_PIXEL_SMALL)
#define CODE128_STOP_CHAR_WIDTH  (13 * NARROW_PIXEL_SMALL)

#define B (0)
#define b (1)
#define W (2)
#define w (3)

static char
Code39ValidChars[44] =
{
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-. $/+%"
};

static uint8
Code39EncodingScheme[44][9] =
{
    /* 0 */
    {b, w, b, W, B, w, B, w, b},
    /* 1 */
    {B, w, b, W, b, w, b, w, B},
    /* 2 */
    {b, w, B, W, b, w, b, w, B},
    /* 3 */
    {B, w, B, W, b, w, b, w, b},
    /* 4 */
    {b, w, b, W, B, w, b, w, B},
    /* 5 */
    {B, w, b, W, B, w, b, w, b},
    /* 6 */
    {b, w, B, W, B, w, b, w, b},
    /* 7 */
    {b, w, b, W, b, w, B, w, B},
    /* 8 */
    {B, w, b, W, b, w, B, w, b},
    /* 9 */
    {b, w, B, W, b, w, B, w, b},
    /* A */
    {B, w, b, w, b, W, b, w, B},
    /* B */
    {b, w, B, w, b, W, b, w, B},
    /* C */
    {B, w, B, w, b, W, b, w, b},
    /* D */
    {b, w, b, w, B, W, b, w, B},
    /* E */
    {B, w, b, w, B, W, b, w, b},
    /* F */
    {b, w, B, w, B, W, b, w, b},
    /* G */
    {b, w, b, w, b, W, B, w, B},
    /* H */
    {B, w, b, w, b, W, B, w, b},
    /* I */
    {b, w, B, w, b, W, B, w, b},
    /* J */
    {b, w, b, w, B, W, B, w, b},
    /* K */
    {B, w, b, w, b, w, b, W, B},
    /* L */
    {b, w, B, w, b, w, b, W, B},
    /* M */
    {B, w, B, w, b, w, b, W, b},
    /* N */
    {b, w, b, w, B, w, b, W, B},
    /* O */
    {B, w, b, w, B, w, b, W, b},
    /* P */
    {b, w, B, w, B, w, b, W, b},
    /* Q */
    {b, w, b, w, b, w, B, W, B},
    /* R */
    {B, w, b, w, b, w, B, W, b},
    /* S */
    {b, w, B, w, b, w, B, W, b},
    /* T */
    {b, w, b, w, B, w, B, W, b},
    /* U */
    {B, W, b, w, b, w, b, w, B},
    /* V */
    {b, W, B, w, b, w, b, w, B},
    /* W */
    {B, W, B, w, b, w, b, w, b},
    /* X */
    {b, W, b, w, B, w, b, w, B},
    /* Y */
    {B, W, b, w, B, w, b, w, b},
    /* Z */
    {b, W, B, w, B, w, b, w, b},
    /* - */
    {b, W, b, w, b, w, B, w, B},
    /* . */
    {B, W, b, w, b, w, B, w, b},
    /* 0x20 */
    {b, W, B, w, b, w, B, w, b},
    /* $ */
    {b, W, b, W, b, W, b, w, b},
    /* / */
    {b, W, b, W, b, w, b, W, b},
    /* + */
    {b, W, b, w, b, W, b, W, b},
    /* % */
    {b, w, b, W, b, W, b, W, b},
    /* * */
    {b, W, b, w, B, w, B, w, b}
};

static uint8
Code128EncodingScheme[107][13] =
{
    /*0*/ {b,b,w,b,b,w,w,b,b,w,w,w,w},
	/*1*/ {b,b,w,w,b,b,w,b,b,w,w,w,w},
	/*2*/ {b,b,w,w,b,b,w,w,b,b,w,w,w},
	/*3*/ {b,w,w,b,w,w,b,b,w,w,w,w,w},
	/*4*/ {b,w,w,b,w,w,w,b,b,w,w,w,w},
	/*5*/ {b,w,w,w,b,w,w,b,b,w,w,w,w},
	/*6*/ {b,w,w,b,b,w,w,b,w,w,w,w,w},
	/*7*/ {b,w,w,b,b,w,w,w,b,w,w,w,w},
	/*8*/ {b,w,w,w,b,b,w,w,b,w,w,w,w},
	/*9*/ {b,b,w,w,b,w,w,b,w,w,w,w,w},
	/*10*/ {b,b,w,w,b,w,w,w,b,w,w,w,w},
	/*11*/ {b,b,w,w,w,b,w,w,b,w,w,w,w},
	/*12*/ {b,w,b,b,w,w,b,b,b,w,w,w,w},
	/*13*/ {b,w,w,b,b,w,b,b,b,w,w,w,w},
	/*14*/ {b,w,w,b,b,w,w,b,b,b,w,w,w},
	/*15*/ {b,w,b,b,b,w,w,b,b,w,w,w,w},
	/*16*/ {b,w,w,b,b,b,w,b,b,w,w,w,w},
	/*17*/ {b,w,w,b,b,b,w,w,b,b,w,w,w},
	/*18*/ {b,b,w,w,b,b,b,w,w,b,w,w,w},
	/*19*/ {b,b,w,w,b,w,b,b,b,w,w,w,w},
	/*20*/ {b,b,w,w,b,w,w,b,b,b,w,w,w},
	/*21*/ {b,b,w,b,b,b,w,w,b,w,w,w,w},
	/*22*/ {b,b,w,w,b,b,b,w,b,w,w,w,w},
	/*23*/ {b,b,b,w,b,b,w,b,b,b,w,w,w},
	/*24*/ {b,b,b,w,b,w,w,b,b,w,w,w,w},
	/*25*/ {b,b,b,w,w,b,w,b,b,w,w,w,w},
	/*26*/ {b,b,b,w,w,b,w,w,b,b,w,w,w},
	/*27*/ {b,b,b,w,b,b,w,w,b,w,w,w,w},
	/*28*/ {b,b,b,w,w,b,b,w,b,w,w,w,w},
	/*29*/ {b,b,b,w,w,b,b,w,w,b,w,w,w},
	/*30*/ {b,b,w,b,b,w,b,b,w,w,w,w,w},
	/*31*/ {b,b,w,b,b,w,w,w,b,b,w,w,w},
	/*32*/ {b,b,w,w,w,b,b,w,b,b,w,w,w},
	/*33*/ {b,w,b,w,w,w,b,b,w,w,w,w,w},
	/*34*/ {b,w,w,w,b,w,b,b,w,w,w,w,w},
	/*35*/ {b,w,w,w,b,w,w,w,b,b,w,w,w},
	/*36*/ {b,w,b,b,w,w,w,b,w,w,w,w,w},
	/*37*/ {b,w,w,w,b,b,w,b,w,w,w,w,w},
	/*38*/ {b,w,w,w,b,b,w,w,w,b,w,w,w},
	/*39*/ {b,b,w,b,w,w,w,b,w,w,w,w,w},
	/*40*/ {b,b,w,w,w,b,w,b,w,w,w,w,w},
	/*41*/ {b,b,w,w,w,b,w,w,w,b,w,w,w},
	/*42*/ {b,w,b,b,w,b,b,b,w,w,w,w,w},
	/*43*/ {b,w,b,b,w,w,w,b,b,b,w,w,w},
	/*44*/ {b,w,w,w,b,b,w,b,b,b,w,w,w},
	/*45*/ {b,w,b,b,b,w,b,b,w,w,w,w,w},
	/*46*/ {b,w,b,b,b,w,w,w,b,b,w,w,w},
	/*47*/ {b,w,w,w,b,b,b,w,b,b,w,w,w},
	/*48*/ {b,b,b,w,b,b,b,w,b,b,w,w,w},
	/*49*/ {b,b,w,b,w,w,w,b,b,b,w,w,w},
	/*50*/ {b,b,w,w,w,b,w,b,b,b,w,w,w},
	/*51*/ {b,b,w,b,b,b,w,b,w,w,w,w,w},
	/*52*/ {b,b,w,b,b,b,w,w,w,b,w,w,w},
	/*53*/ {b,b,w,b,b,b,w,b,b,b,w,w,w},
	/*54*/ {b,b,b,w,b,w,b,b,w,w,w,w,w},
	/*55*/ {b,b,b,w,b,w,w,w,b,b,w,w,w},
	/*56*/ {b,b,b,w,w,w,b,w,b,b,w,w,w},
	/*57*/ {b,b,b,w,b,b,w,b,w,w,w,w,w},
	/*58*/ {b,b,b,w,b,b,w,w,w,b,w,w,w},
	/*59*/ {b,b,b,w,w,w,b,b,w,b,w,w,w},
	/*60*/ {b,b,b,w,b,b,b,b,w,b,w,w,w},
	/*61*/ {b,b,w,w,b,w,w,w,w,b,w,w,w},
	/*62*/ {b,b,b,b,w,w,w,b,w,b,w,w,w},
	/*63*/ {b,w,b,w,w,b,b,w,w,w,w,w,w},
	/*64*/ {b,w,b,w,w,w,w,b,b,w,w,w,w},
	/*65*/ {b,w,w,b,w,b,b,w,w,w,w,w,w},
	/*66*/ {b,w,w,b,w,w,w,w,b,b,w,w,w},
	/*67*/ {b,w,w,w,w,b,w,b,b,w,w,w,w},
	/*68*/ {b,w,w,w,w,b,w,w,b,b,w,w,w},
	/*69*/ {b,w,b,b,w,w,b,w,w,w,w,w,w},
	/*70*/ {b,w,b,b,w,w,w,w,b,w,w,w,w},
	/*71*/ {b,w,w,b,b,w,b,w,w,w,w,w,w},
	/*72*/ {b,w,w,b,b,w,w,w,w,b,w,w,w},
	/*73*/ {b,w,w,w,w,b,b,w,b,w,w,w,w},
	/*74*/ {b,w,w,w,w,b,b,w,w,b,w,w,w},
	/*75*/ {b,b,w,w,w,w,b,w,w,b,w,w,w},
	/*76*/ {b,b,w,w,b,w,b,w,w,w,w,w,w},
	/*77*/ {b,b,b,b,w,b,b,b,w,b,w,w,w},
	/*78*/ {b,b,w,w,w,w,b,w,b,w,w,w,w},
	/*79*/ {b,w,w,w,b,b,b,b,w,b,w,w,w},
	/*80*/ {b,w,b,w,w,b,b,b,b,w,w,w,w},
	/*81*/ {b,w,w,b,w,b,b,b,b,w,w,w,w},
	/*82*/ {b,w,w,b,w,w,b,b,b,b,w,w,w},
	/*83*/ {b,w,b,b,b,b,w,w,b,w,w,w,w},
	/*84*/ {b,w,w,b,b,b,b,w,b,w,w,w,w},
	/*85*/ {b,w,w,b,b,b,b,w,w,b,w,w,w},
	/*86*/ {b,b,b,b,w,b,w,w,b,w,w,w,w},
	/*87*/ {b,b,b,b,w,w,b,w,b,w,w,w,w},
	/*88*/ {b,b,b,b,w,w,b,w,w,b,w,w,w},
	/*89*/ {b,b,w,b,b,w,b,b,b,b,w,w,w},
	/*90*/ {b,b,w,b,b,b,b,w,b,b,w,w,w},
	/*91*/ {b,b,b,b,w,b,b,w,b,b,w,w,w},
	/*92*/ {b,w,b,w,b,b,b,b,w,w,w,w,w},
	/*93*/ {b,w,b,w,w,w,b,b,b,b,w,w,w},
	/*94*/ {b,w,w,w,b,w,b,b,b,b,w,w,w},
	/*95*/ {b,w,b,b,b,b,w,b,w,w,w,w,w},
	/*96*/ {b,w,b,b,b,b,w,w,w,b,w,w,w},
	/*97*/ {b,b,b,b,w,b,w,b,w,w,w,w,w},
	/*98*/ {b,b,b,b,w,b,w,w,w,b,w,w,w},
	/*99*/ {b,w,b,b,b,w,b,b,b,b,w,w,w},
	/*100*/ {b,w,b,b,b,b,w,b,b,b,w,w,w},
	/*101*/ {b,b,b,w,b,w,b,b,b,b,w,w,w},
	/*102*/ {b,b,b,b,w,b,w,b,b,b,w,w,w},
	/*103*/ {b,b,w,b,w,w,w,w,b,w,w,w,w},
	/*104*/ {b,b,w,b,w,w,b,w,w,w,w,w,w},
	/*105*/ {b,b,w,b,w,w,b,b,b,w,w,w,w},
	/*106*/ {b,b,w,w,w,b,b,b,w,b,w,b,b}
};


static uint16
code39_code_font_add(uint8 type,
                     uint8* outBuf,
                     uint8 isBig)
{
    if (type == b)
    {
        memset(outBuf, 0xFF, NARROW_PIXEL_SMALL * (isBig + 1));

        return NARROW_PIXEL_SMALL * (isBig + 1);
    }
    else if (type == B)
    {
        memset(outBuf, 0xFF, WIDE_PIXEL_SMALL * (isBig + 1));

        return WIDE_PIXEL_SMALL * (isBig + 1);
    }
    else if (type == w)
    {
        memset(outBuf, 0x00, NARROW_PIXEL_SMALL * (isBig + 1));

        return NARROW_PIXEL_SMALL * (isBig + 1);
    }
    else if (type == W)
    {
        memset(outBuf, 0x00, WIDE_PIXEL_SMALL * (isBig + 1));

        return WIDE_PIXEL_SMALL * (isBig + 1);
    }
    else
    {
        return 0;
    }
}


static uint16
code39_char_add(char   c,
                uint8* outBuf,
                uint8  isBig)
{
    uint8* pEncoding;
    char*  pChar;
    uint8  iType;
    uint32 iByte;

    if (c == '*')
    {
        pEncoding = &Code39EncodingScheme[43][0];
    }
    else
    {
        pChar = strchr(Code39ValidChars, c);
        if (pChar == NULL)
        {
            return 0;
        }

        pEncoding = &Code39EncodingScheme[pChar - Code39ValidChars][0];
    }

    iByte = 0;
    for (iType = 0; iType < 9; iType++)
    {
        iByte += code39_code_font_add(pEncoding[iType], &outBuf[iByte], isBig);
    }


    return iByte;
}

static uint16
code128_char_add(uint8   c,
                uint8* outBuf,
                uint8  isBig)
{
    uint8* pEncoding;
    char*  pChar;
    uint8  iType;
    uint32 iByte;

	pEncoding = &Code128EncodingScheme[c][0];

	iByte = 0;
	if(c!=106){
		for (iType = 0; iType < 11; iType++)
		{
			iByte += code39_code_font_add(pEncoding[iType], &outBuf[iByte], isBig);
		}
	}else{
		for (iType = 0; iType < 13; iType++)
		{
			iByte += code39_code_font_add(pEncoding[iType], &outBuf[iByte], isBig);
		}
	}


    return iByte;
}

static uint32
code39_bitmap_get(uint8*      outBuf,
                  uint32      bufSize,
                  const char* pInput,
                  uint8       nInput)
{

    uint32 iByte;
    uint32 width;
    uint32 nInputs;
    uint8  iChar;
    uint8  isBig;

    nInputs = nInput;

    /* Choose pixels */
    isBig = 1;
    width = ((uint32)((CODE39_CHAR_WIDTH + NARROW_PIXEL_SMALL) * 2)           +
             (uint32)(nInputs * (CODE39_CHAR_WIDTH + NARROW_PIXEL_SMALL) * 2) +
             (uint32)(CODE39_CHAR_WIDTH * 2));

    if (width > TP_PRINT_RIGHT_BOUNDARY)
    {
        isBig = 0;
        width = ((uint32)(CODE39_CHAR_WIDTH + NARROW_PIXEL_SMALL) +
                 (uint32)(nInputs * (CODE39_CHAR_WIDTH + NARROW_PIXEL_SMALL)) +
                 (uint32)(CODE39_CHAR_WIDTH));
    }

    if (width > TP_PRINT_RIGHT_BOUNDARY)
    {
        return -1;
    }

    /* Center-Align the bitmap */
    iByte = (TP_PRINT_RIGHT_BOUNDARY - width) / 2;

    /* Start Zone */
    iByte += code39_char_add('*', &outBuf[iByte], isBig);
    iByte += code39_code_font_add(w, &outBuf[iByte], isBig);

    /* Code Zone */
    for (iChar = 0; iChar < nInput; iChar++)
    {
        iByte += code39_char_add(pInput[iChar], &outBuf[iByte], isBig);
        iByte += code39_code_font_add(w,        &outBuf[iByte], isBig);
    }

    /* Stop Zone */
    code39_char_add('*', &outBuf[iByte], isBig);

    /* Adjust the height */
    for (iByte = 1; iByte < 12; iByte++)
    {
        memcpy(&outBuf[384 * iByte], outBuf, 384);
    }

    return 96;
}

static uint8
calculate_checksum( const char* pInput, uint8 nInput ){
	uint8  I;
	uint8  CurrentChar;
	uint32  CurrentValue;
	uint8  CheckDigitValue;
	uint32 weightedTotal;
	uint8 weight;
	char C128CheckDigit;
	char pair [3];

	/* <<<< Calculate Modulo 103 Check Digit >>>> */
	/* Set WeightedTotal to the value of the start character */
	/* This includes the FNC 1, Application Identifier Value 21 (Serial) */
	weightedTotal=105 + 102 + (21 * 2);

	for(I=0;I <= nInput;I+=2)
	{
		/* Get the ASCII value of each character */
		memcpy( pair, &pInput[I], 2 );
		pair[2] = '\0';
		CurrentChar = atoi((const char *)pair);

		/* Get the Code 128 value of CurrentChar according to chart */
  		/*if(CurrentChar<135) {CurrentValue=CurrentChar-32;}
  		if(CurrentChar>134) {CurrentValue=CurrentChar-100;}*/

		/* Multiply by the weighting character */
		weight = (I/2)+3;
  		CurrentValue=CurrentChar*weight; //The +3 is because the weight is affected by the FNC1 and the Application Identifier 21

		/* Add the values together to get the weighted total*/
  		weightedTotal=weightedTotal+CurrentValue;
	}

	/* divide the WeightedTotal by 103 and get the remainder, this is the CheckDigitValue*/
	CheckDigitValue = (int) (weightedTotal % 103);

	return CheckDigitValue;
}

static uint32
code128_bitmap_get(uint8*      outBuf,
                  uint32      bufSize,
                  const char* pInput,
                  uint8       nInput)
{

    uint32 iByte;
    uint32 width;
    uint32 nInputs;
	uint8 checksum;
    uint8  iChar;
    uint8  isBig;
	uint8 pair_int;
	char pair[3];

    nInputs = nInput;

    /* Choose pixels */
    isBig = 1;
    width = ((uint32)(CODE128_CHAR_WIDTH * 2)           + //START A
			 (uint32)(CODE128_CHAR_WIDTH * 2)           + //FNC 1
			 (uint32)(CODE128_CHAR_WIDTH * 2)           + // AI 21
             (uint32)((nInputs / 2) * (CODE128_CHAR_WIDTH * 2)) + //SERIAL GROUPING 2 DIGITS INTO ONE CODE
             (uint32)(CODE128_CHAR_WIDTH * 2) + //CHECKSUM
			 (uint32)(CODE128_STOP_CHAR_WIDTH * 2)); //STOP CHARACTER

    if (width > TP_PRINT_RIGHT_BOUNDARY)
    {
        isBig = 0;
        width = ((uint32)(CODE128_CHAR_WIDTH)           + //START A
				(uint32)(CODE128_CHAR_WIDTH)           + //FNC 1
				(uint32)(CODE128_CHAR_WIDTH)           + // AI 21
				(uint32)((nInputs / 2) * (CODE128_CHAR_WIDTH)) + //SERIAL
				(uint32)(CODE128_CHAR_WIDTH) + //CHECKSUM
				(uint32)(CODE128_STOP_CHAR_WIDTH)); //STOP CHARACTER
    }

    if (width > TP_PRINT_RIGHT_BOUNDARY)
    {
        return -1;
    }

    /* Center-Align the bitmap */
    iByte = (TP_PRINT_RIGHT_BOUNDARY - width) / 2;

    /* Start A*/
    iByte += code128_char_add(105, &outBuf[iByte], isBig);
    
	/* FNC 1*/
	iByte += code128_char_add(102, &outBuf[iByte], isBig);

	/* APPLICATION IDENTIFIER 21 (SERIAL) */

	iByte += code128_char_add(21, &outBuf[iByte], isBig);

    /* Code Zone */
    for (iChar = 0; iChar < nInput; iChar+=2)
    {
		memcpy( pair, &pInput[iChar], 2 );
		pair[2] = '\0';
		pair_int = atoi((const char *)pair);
        iByte += code128_char_add(pair_int, &outBuf[iByte], isBig);
    }

    /* Checksum char */
	checksum = calculate_checksum(pInput, nInput);
    iByte += code128_char_add(checksum, &outBuf[iByte], isBig);

	/* Stop Zone */
    iByte += code128_char_add(106, &outBuf[iByte], isBig);

    /* Adjust the height */
    for (iByte = 1; iByte < 12; iByte++)
    {
        memcpy(&outBuf[384 * iByte], outBuf, 384);
    }

    return 96;
}


static uint8
code39_print(const char* pInput,
             uint8       nInput)
{
    uint32 height;
    uint8  retVal;
    uint8  iChar;

    if (pInput == PNULL)
    {
        return 0x01;
    }

    if (nInput == 0)
    {
        return 0x02;
    }

    if (nInput > 22)
    {
        return 0x02;
    }

    for (iChar = 0; iChar < nInput; iChar++)
    {
        if (strchr(Code39ValidChars, pInput[iChar]) == NULL)
        {
            return 0x03;
        }
    }

    memset(PrintBitMap, 0x00, sizeof(PrintBitMap));
    height = code39_bitmap_get(PrintBitMap, sizeof(PrintBitMap), pInput, nInput);
    if (height < 0)
    {
        retVal = 0x04;
    }
    else
    {

        retVal = TP_PrnLogo(PrintBitMap, 384, height);
    }

    return retVal;
}

static uint8
code128_print(const char* pInput,
             uint8       nInput)
{
    uint32 height;
    uint8  retVal;
    uint8  iChar;

    if (pInput == PNULL)
    {
        return 0x01;
    }

    if (nInput == 0)
    {
        return 0x02;
    }

    if (nInput > 22)
    {
        return 0x02;
    }

    for (iChar = 0; iChar < nInput; iChar++)
    {
        if (strchr(Code39ValidChars, pInput[iChar]) == NULL)
        {
            return 0x03;
        }
    }

    memset(PrintBitMap, 0x00, sizeof(PrintBitMap));
    height = code128_bitmap_get(PrintBitMap, sizeof(PrintBitMap), pInput, nInput);
    if (height < 0)
    {
        retVal = 0x04;
    }
    else
    {

        retVal = TP_PrnLogo(PrintBitMap, 384, height);
    }

    return retVal;
}


/******************************************************************************
* Public Functions
*******************************************************************************/
uint8 TP_BarCodePrint(const char* pInput, uint8 nInput, TP_BarCodeTypeE type)
{
    uint8 retVal;

    switch (type)
    {
    case BCT_I2OF5:
        retVal = printI25Code(pInput, nInput);
        break;

    case BCT_CODE39:
        retVal = code39_print(pInput, nInput);
        break;

	case BCT_GS1_128:
		retVal = code128_print(pInput, nInput);

    default:
        retVal = 0x05;
        break;
    }

    return retVal;
}

#undef W
#undef w
#undef B
#undef b

/* End Of File */
