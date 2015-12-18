#ifndef TP_DEMO_DEFINE_H
#define TP_DEMO_DEFINE_H

#define TPDEMO_VERSION  "V0.1"

/*Include CBB  Modules */
#define INCLUDE_PHONE_SETTING
#define INCLUDE_SYSTEM_TEST
#define INCLUDE_SECRET_CODE
#define INCLUDE_PHONE_SETTING
#define INCLUDE_CALL_RECORD
#define INCLUDE_MESSAGE_BOX
#if defined(INCLUDE_PHONE_SETTING) || defined(INCLUDE_MESSAGE_BOX)
#define INCLUDE_PARAM_MANAGER
#endif

/*Other Defines*/
#define INCLUDE_MULTI_LANGUAGE

/*Demo Key Defines*/
#ifdef TPS300
    #define KEY_DEMO_MENU	    TP_KEY_MENU
	#define KEY_DEMO_CONFIRM    TP_KEY_OK
    #define KEY_DEMO_CANCEL     TP_KEY_CANCEL
    #define KEY_DEMO_POUND      TP_KEY_FUNC
#else
    #define KEY_DEMO_CONFIRM    TP_KEY_CONFIRM
    #define KEY_DEMO_CANCEL     TP_KEY_CANCEL
    #define KEY_DEMO_POUND      TP_KEY_POUND
#endif


#endif
