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

#define B (0)
#define b (1)
#define W (2)
#define w (3)

static const char
Code39ValidChars[43] =
{
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-. $/+%"
};

static const uint8
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
