#include <TPTypedef.h>

#if   defined(TPS300)
/* Default LCD Drvier */
const char TP_LCD_DefaultDriver[256] = {"TPS300_YUSHUN"};
#elif defined(TPS320)
const char TP_LCD_DefaultDriver[256] = {"TPS320_JINCAI"};
#else
const char TP_LCD_DefaultDriver[256] = {"TP_PUBLIC_G_JINCAI"};
#endif

#if   defined(TPS300)
/* Default LCD Contrast Level */
const uint8 TP_LCD_DefaultContrastLevel = 25;
#elif defined(TPS320)
const uint8 TP_LCD_DefaultContrastLevel = 30;
#else
const uint8 TP_LCD_DefaultContrastLevel = 25;
#endif

#if   defined(TPS300)
/* Default Keypad Driver */
const char TP_Keypad_DefaultDriver[256] = {"TPS300"};
#elif defined(TPS320)
const char TP_Keypad_DefaultDriver[256] = {"TPS320"};
#else
const char TP_Keypad_DefaultDriver[256] = {"TP_PUBLIC_G"};
#endif

#if   defined(TPS300)
/* Default Printer Driver */
#if   defined(WITH_175X_ARM)
const char TP_Printer_DefaultDriver[256] = {"1754"};
#else
const char TP_Printer_DefaultDriver[256] = {"PRTNew"};
#endif
#elif defined(TPS320)
const char TP_Printer_DefaultDriver[256] = {"PRTNew"};
#else
const char TP_Printer_DefaultDriver[256] = {"None"};
#endif