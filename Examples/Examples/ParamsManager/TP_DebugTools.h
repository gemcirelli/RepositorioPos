#ifndef DEBUG_H
#define DEBUG_H


//#define _DEBUG
#undef _DEBUG

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _DEBUG

#ifdef WIN32 
#define ShowDebug _cprintf
#else
extern void showLcdDebug(uint8* fmt,...);
#define ShowDebug showLcdDebug
#endif

extern void SLEEP(int32 x);
extern void WAIT_FOR_ANYKEY();

#endif
#ifdef __cplusplus
}
#endif

#endif

