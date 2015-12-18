#ifndef TP_PUBLIC_H
#define TP_PUBLIC_H

#define BUFFER_LENGTH_CHECK(a, b) if((a)<(b)) {return (a-b);}
//assert()
#ifdef __cplusplus
extern "C" {
#endif
#include "TPTypedef.h"

uint16 usCRC16(uint8 * puchMsg, uint16  uiDataLen);
uint16 uiGet1XLHData(uint8 * pucData);

#ifdef __cplusplus
}
#endif

#endif