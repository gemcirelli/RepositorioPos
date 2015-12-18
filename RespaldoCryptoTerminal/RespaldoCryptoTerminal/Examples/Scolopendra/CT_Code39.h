#ifndef CT_CODE39
#define CT_CODE39
#ifdef __cplusplus
extern "C" {
#endif

#include <TPTypedef.h>
#include <TPBase.h>


#define B (0)
#define b (1)
#define W (2)
#define w (3)

#define WIDE	(9)
#define NARROW	(4)
#define MAX_SERIAL_CHARS (20)

#define CODE39_CHAR_WIDTH (WIDE*3 + NARROW*6 + NARROW)

#define CODE39_FRAME_BITSIZE (CODE39_CHAR_WIDTH * 22)

#define CODE39_FRAME_SIZE (CODE39_FRAME_BITSIZE / 8 + 1)

static const char Code39ValidChars[44] = {
	'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F',
	'G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V',
	'W','X','Y','Z','-','.',' ','$','/','+','%','*'
};

static const uint8 Code39EncodingScheme[44][9] =
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

uint32 CT_StringToCode39Bitmap(const char * input, uint8 * PrintBitMap);
#ifdef __cplusplus
}
#endif

#endif

