#include <TPTypedef.h>

#ifndef _WIN32
#pragma arm section rodata="SDKINFO"
#endif

/* This region should not be altered by user */
const struct _SDKInfo 
TP_Data_SDKInfo =
{
    /** Work Module */
    "SIM840W",

    /** Work Flash Type */
    "TYPE1",

    /** Work Flash Size */
    "64M",

    /** Work Core Version */
    "AE201305241021",

    /** Attachment Information */
    #if defined(TPS300)
    "TPS300",
    #elif defined(TPS320)
    "TPS320",
    #elif defined(TP_PUBLIC_G)
    "TP_PUBLIC_G",
    #else
    "Unkown",
    #endif

    /** SDK Version*/
    #if   defined(TPS300)
        #ifdef CARD_READER_MODULE_ON
    "TPSDK(G50.1).0.Release130626",
        #else
    "TPSDK(G50.0).0.Release130626",
        #endif
    #elif defined(TPS320)
    "TPSDK(G60.0).0.Release130626",
    #elif defined(TP_PUBLIC_G)
    "TPSDK(G61).0.Release130626",
    #else
    "TPSDK(UNkown).0.Release130626",
    #endif

    {0}
};

#ifndef _WIN32
#pragma arm section rodata
#endif

#ifndef _WIN32
#pragma arm section rodata="APPINFO"
#endif

struct _APPInfo 
TP_Data_APPInfo =
{
    #ifdef APP_PRODUCT_NAME
    APP_PRODUCT_NAME,
    #else
    "\x0",
    #endif

    #ifdef APP_PRODUCT_VERSION
    APP_PRODUCT_VERSION,
    #else
    "\x0",
    #endif

    #ifdef APP_PRODUCT_DESC
    APP_PRODUCT_DESC,
    #else
    "\x0",
    #endif

    #ifdef APP_PRODUCT_TERMINAL
    APP_PRODUCT_TERMINAL,
    #else
    "\x0",
    #endif

    #ifdef APP_UPGRADE_FROM_VERSION
    APP_UPGRADE_FROM_VERSION,
    #else
    "\x0",
    #endif

    #ifdef APP_UPGRADE_MUST
    '1',
    #else
    '0',
    #endif

    {0}
};

#ifndef _WIN32
#pragma arm section rodata


#include <rt_misc.h>

#pragma arm section code
__value_in_regs struct __initial_stackheap __user_initial_stackheap(unsigned R0, unsigned SP, unsigned R2, unsigned SL)
{
   struct __initial_stackheap config;

   config.heap_base = R0;
   config.heap_limit =  R2;
   config.stack_base =  SP;
   config.stack_limit =   SL;
   return config;
}
#pragma import(__user_initial_stackheap)

void $Sub$$_fp_init(void)
{
}


void $Sub$$_initio(void)
{
}


__value_in_regs struct __argc_argv $Sub$$__ARM_get_argv(void *a )
{
    struct __argc_argv arg;
    arg.argc = 0;
    arg.r2 = 0;
    arg.r3 = 0;
    return arg;
}

#endif

