/*-
 * BSD 2-Clause License
 *
 * Copyright (c) 2012-2018, Jan Breuer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file   scpi-def.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  SCPI parser test
 *
 *
 */

#include <RingBuf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "scpi-def.h"
#include "command_parser_usb.h"
#include "scpi/parser.h"
#include "usbd_cdc_if.h"
#include "bme280_user.h"

const scpi_command_t scpi_commands[] = {
	{ .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
	{ .pattern = "*RST", .callback = SCPI_CoreRst,},
	{ .pattern = "CONFigure:OUTput", .callback = set_output,},
	{ .pattern = "MEASure:TEMPerature?", .callback = get_temperature,},
	{ .pattern = "MEASure:PRESsure?", .callback = get_pressure,},
	{ .pattern = "MEASure:HUMIdity?", .callback = get_humidity,},
	SCPI_CMD_LIST_END
};


scpi_result_t get_temperature (scpi_t * context)
{
	char* retubuffer = get_temperature_string_ptr();
	size_t len = strlen(retubuffer);
	if (len <1 )
	{
		USB_Send(" ",1);
	}
	else
	{
		USB_Send(retubuffer,len);
	}
	return SCPI_RES_OK;
}

scpi_result_t get_pressure (scpi_t * context)
{
	char* retubuffer = get_pressure_string_ptr();
	size_t len = strlen(retubuffer);
	if (len <1 )
	{
		USB_Send(" ",1);
	}
	else
	{
		USB_Send(retubuffer,len);
	}
	return SCPI_RES_OK;
}

scpi_result_t get_humidity (scpi_t * context)
{
	char* retubuffer = get_humidity_string_ptr();
	size_t len = strlen(retubuffer);
	if (len <1 )
	{
		USB_Send(" ",1);
	}
	else
	{
		USB_Send(retubuffer,len);
	}
	return SCPI_RES_OK;
}



scpi_result_t set_output (scpi_t * context)
{
	char pinarray[OUTPUT_VARIABLE_LENGTH]; //defined in command_parser.h
	uint8_t command_to_send[COMMAND_MAX_LENGTH]; //defined in command_parser.h
	scpi_bool_t outputstate;
	uint8_t i,j = 0;
	size_t outputlen = OUTPUT_VARIABLE_LENGTH;
	memset (pinarray,0,OUTPUT_VARIABLE_LENGTH);
	memset (command_to_send,0,COMMAND_MAX_LENGTH);
	SCPI_ParamCopyText(context, pinarray, outputlen, &outputlen, (scpi_bool_t)0);
	SCPI_ParamBool(context, &outputstate, 0);
	j = strlen(pinarray);
	if ((j >1) && (j <OUTPUT_VARIABLE_LENGTH))
	{
		if ((outputstate == 0))
		{
			memcpy(command_to_send,(uint8_t*)"RESET ",strlen("RESET "));
			i = strlen((const char*)command_to_send);
			memcpy (command_to_send+i,(uint8_t*)pinarray,j);
			parse_command(command_to_send);

		}
		else if ((outputstate ==1))
		{
			memcpy(command_to_send,(uint8_t*)"SET ",strlen("SET "));
			i = strlen((const char*)command_to_send);
			memcpy (command_to_send+i,(uint8_t*)pinarray,j);
			parse_command(command_to_send);
		}

	}
	return SCPI_RES_OK;
}


size_t myWrite(scpi_t * context, const char * data, size_t len) {
    (void) context;

    USB_Send(data, len);
    return len;
}





scpi_result_t myFlush(scpi_t * context) {
    (void) context;
    Ringbuf_Reset();
    return SCPI_RES_OK;
}

scpi_result_t myReset(scpi_t * context) {
    (void) context;
    NVIC_SystemReset();
    return SCPI_RES_OK;
}


scpi_interface_t scpi_interface = {
    .error = NULL,
    .write = myWrite,
    .control = NULL,
    .flush = myFlush,
    .reset = myReset,
};

char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];
scpi_t scpi_context;

scpi_t scpi_context;

void SCPI_START()
{
	SCPI_Init(&scpi_context,
	    scpi_commands,
	    &scpi_interface,
	    scpi_units_def,
	    SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
	    scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
	    scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);
}






void scpi_instrument_write(uint8_t* buf, size_t len, uint8_t data_remaining)
{
/*	uint8_t startpos = strlen(scpi_input_buffer);
	memcpy(scpi_input_buffer+startpos,buf,len);
	if (!data_remaining)
	{
		SCPI_Input(&scpi_context, scpi_input_buffer, len+startpos);
		memset(scpi_input_buffer,0,SCPI_INPUT_BUFFER_LENGTH);
	}
	*/
}



void scpi_instrument_write_ringbuf(uint8_t* buf, size_t len)
{
		SCPI_Input(&scpi_context, (char*)buf, len);
}
