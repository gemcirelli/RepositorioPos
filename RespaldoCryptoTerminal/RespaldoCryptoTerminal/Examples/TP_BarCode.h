#ifndef TP_BARCODE_H
#define TP_BARCODE_H
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
* Nested Include Headers
*******************************************************************************/
#include <TPTypedef.h>
#include <TPPrint.h>

/******************************************************************************
* Typedefs
*******************************************************************************/

/**
 * Supported type of barcode
 */
typedef enum BarCodeType
{
    /**
     * Interleaved 2 of 5<BR>
     * Only numeric characters is valid<BR>
     * Max number of characters is 40<BR>
     * Number of characters must be even
     */
    BCT_I2OF5 = 0,

    /**
     * Code 39<br>
     * Only space,"$","%","+","-",".","/",0~9,A~Z is valid<br>
     * Max number of characters is 22
     */
    BCT_CODE39,

    BCT_MAX_SUPPORT_NUM,

    BCT_NULL
} TP_BarCodeTypeE;

/**
 * Print barcode for the input strings
 *
 * @param pInput Pointer to input characters buffer
 * @param nInput Number of input characters
 * @param type {@link #TP_BarCodeTypeE}
 *
 * @return 0x00, Print succeed<BR>
 *         0x01, Invalid pointer<BR>
 *         0x02, Invalid number of input characters<BR>
 *         0x03, Invalid input characters<BR>
 *         0x04, Unkown error when printing<BR>
 *         0x05, Unsupported barcode type<BR>
 *         0xEE, Printer device not respond<BR>
 *         0xFA, Printer is out of paper<BR>
 *         0xFC, Printer head over temperature<BR>
 *         0xFD, Printer device access denied<BR>
 *         0xFF, Printer is not ready
 *
 * @author  Yang Zhirui
 *
 * @version 1.0
 *
 * @date 20120812
 */
extern uint8
TP_BarCodePrint(const char* pInput, uint8 nInput, TP_BarCodeTypeE type);


#ifdef __cplusplus
}
#endif

#endif
/* End Of File */
