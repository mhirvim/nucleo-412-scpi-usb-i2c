/*
 * uartRingBufDMA.c
 *
 *  Created on: Aug 12, 2021
 *      Author: controllerstech.com
 *
 *      Modified.
 */


#include "RingBuf.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "main.h"


/* Define the Size Here */
#define MainBuf_SIZE 65







/*Ringbufstruct*/

struct Ringbuffer
{
	uint8_t Head;
	uint8_t Tail;
	uint8_t VirtualTail;
	uint8_t Size;
	uint8_t VirtualSize;
	uint8_t MainBuf[MainBuf_SIZE];
};

struct Ringbuffer RingBuf;


static uint8_t Ringbuf_Advance(void)
{
	uint16_t next_tailval = RingBuf.Tail +1;
	uint8_t retval = RingBuf.MainBuf[RingBuf.Tail];
	if (next_tailval>MainBuf_SIZE)
	{
			next_tailval = next_tailval%MainBuf_SIZE;
	}
	RingBuf.Tail = (uint8_t) next_tailval;
	RingBuf.Size--;
	if (RingBuf.Size <1)
	{
		RingBuf.Size =0;
	}
	return retval;
}

uint8_t* Ringbuf_getTailPointer()
{
	return &RingBuf.MainBuf[RingBuf.Tail];
}
/*Preparation for a read operation a continuous chunk of the buffer
 * Does not change the tail pointer or the size
 * After the copying process Ringbuf_Confirmcopy must be called
 * Returns the length of the chunk to be read*/
uint8_t Ringbuf_Readchunk()
{
	RingBuf.VirtualSize=RingBuf.Size;
	RingBuf.VirtualTail =RingBuf.Tail;
	uint8_t readlen = 0;
	if (RingBuf.Size >0)
	{
		if (RingBuf.Head>=RingBuf.Tail)
		{
			readlen = RingBuf.Head - RingBuf.Tail;
			RingBuf.VirtualSize = RingBuf.Size -readlen;
			RingBuf.VirtualTail=RingBuf.Head;
		}
		else
		{
			readlen = MainBuf_SIZE-RingBuf.Tail;
			RingBuf.VirtualSize = RingBuf.Size -readlen;
			RingBuf.VirtualTail = 0;
		}
	}
	return readlen;

}
/* Confirm the chunk has been read and change tail and size */
void Ringbuf_Confirmcopy()
{
	RingBuf.Tail = RingBuf.VirtualTail;
	RingBuf.Size = RingBuf.VirtualSize;
}

/* Initialize the Ring Buffer */
void Ringbuf_Init ()
{
	memset(RingBuf.MainBuf, '\0', MainBuf_SIZE);
	RingBuf.Head = RingBuf.Tail = 0;
	RingBuf.VirtualSize=RingBuf.VirtualTail =0;
	RingBuf.Size =0;
}

/* Resets the Ring buffer */
void Ringbuf_Reset (void)
{
	memset(RingBuf.MainBuf, '\0', MainBuf_SIZE);
	RingBuf.Head = RingBuf.Tail = 0;
	RingBuf.Size = 0;
	RingBuf.VirtualSize=RingBuf.VirtualTail =0;
}

uint8_t Ringbuf_isDataAvailable(void)
{
	if (RingBuf.Size > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


uint8_t Ringbuf_isVirtualDataAvailable(void)
{
	if (RingBuf.VirtualSize > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t Ringbuf_Write (uint8_t* srcbuf, uint32_t len)
{
	uint16_t Room = 0;
	uint8_t returnval = 0;
	uint8_t oldPos = RingBuf.Head;
	uint8_t newPos = 0;
	uint32_t  writelen = 0;
	Room = MainBuf_SIZE-RingBuf.Size-1;
	if ( len > Room)
	{
		writelen = Room;
		returnval = len - writelen;
	}
	else
	{
		writelen = len;
	}
	if (writelen <1)
	{
		return MainBuf_SIZE -1 + writelen;
	}
		if (writelen + oldPos > MainBuf_SIZE)  // If the current position + new data size is greater than the main buffer
		{
			uint16_t datatocopy = MainBuf_SIZE-oldPos;  // find out how much space is left in the main buffer
			memcpy ((uint8_t *)RingBuf.MainBuf+oldPos, (uint8_t *)srcbuf, datatocopy);  // copy data in that remaining space

			oldPos = 0;  // point to the start of the buffer
			memcpy ((uint8_t *)RingBuf.MainBuf, (uint8_t *)srcbuf+datatocopy, (writelen-datatocopy));  // copy the remaining data
			newPos = (writelen-datatocopy);  // update the position
		}
		else
		{
			memcpy ((uint8_t *)RingBuf.MainBuf+oldPos, (uint8_t *)srcbuf, writelen);
			newPos = writelen+oldPos;
		}

			RingBuf.Head = newPos;
			RingBuf.Size = RingBuf.Size + writelen;
			return returnval;
}



void Ringbuf_Read(uint8_t* rcvbuf, uint8_t rcvbuflen)
{
	uint8_t data_to_read = rcvbuflen;
	uint8_t i=0;
	if (RingBuf.Size>0)
	{
		if (RingBuf.Size <rcvbuflen)
		{
			data_to_read = RingBuf.Size;
		}
		for (i=0;i< data_to_read;i++)
		{
			rcvbuf[i] = Ringbuf_Advance();
		}
	if (RingBuf.Size <1)
		{
			RingBuf.Size =0;
		}
	}
}









