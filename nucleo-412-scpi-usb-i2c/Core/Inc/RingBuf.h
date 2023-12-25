/*
 * uartRingBufDMA.h
 *
 *  Created on: Aug 12, 2021
 *      Author: controllerstech.com
 *      Modified by MHi
 */

#ifndef INC_RINGBUF_H_
#define INC_RINGBUF_H_
#include "main.h"

/* Initialize the Ring buffer
 * It will also initialize the UART RECEIVE DMA
 * */
void Ringbuf_Init ();

/* Reset the ring buffer
 * Resets the Head and Tail, also the buffers
 * */
void Ringbuf_Reset (void);
void Ringbuf_Read(uint8_t* rcvbuf, uint8_t rcvbuflen);
uint8_t Ringbuf_Readchunk();
void Ringbuf_Confirmcopy();
uint8_t* Ringbuf_getTailPointer();
uint8_t Ringbuf_isDataAvailable(void);
uint8_t Ringbuf_isVirtualDataAvailable(void);
uint8_t Ringbuf_Write (uint8_t* srcbuf, uint32_t len);


#endif /* INC_RINGBUF_H_ */
