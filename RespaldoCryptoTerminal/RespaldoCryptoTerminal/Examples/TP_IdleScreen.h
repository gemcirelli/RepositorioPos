
#ifndef TP_IDLESCREEN
#define TP_IDLESCREEN
#ifdef __cplusplus
extern "C" {
#endif

uint16 TP_HexToStr(uint8* Src,uint16 Size,uint8* des);
uint16 TP_StrToHex(uint8* Src,uint16 Size,uint8* des);

/**
 * Function Description of ShowIdleScreen()£ºDisplay Main Program
 * @return None
 * @author  Zuo Jun
 * @date 20110712
 * @note Display both Icons and Time
 */
void ShowIdleScreen(void);


#ifdef __cplusplus
}
#endif

#endif

