#ifndef TP_SERIALDATAPROC_H
#define TP_SERIALDATAPROC_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Parse the received data from default serial port.
 *
 * @author linhx (2012/10/8)
 *
 * @param data_p Received data.
 * @param dataLen Length of received data.
 */
extern
void TP_DefaultSerialDataParse(uint8 *data_p,uint16 dataLen);

#ifdef __cplusplus
}
#endif


#endif