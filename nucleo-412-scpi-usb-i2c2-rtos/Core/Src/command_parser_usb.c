/*
 * command_parser.c
 *
 *  Created on: 17.6.2023
 *      Author: Maxim
 */
#include "main.h"
#include "string.h"
#include "command_parser_usb.h"
#include "usbd_cdc_if.h"


const uint8_t commands[][COMMAND_MAX_LENGTH] =
{
		"*IDN?",
		"SET",
		"RESET",
		"*RST"

};

const uint8_t outputlist[][OUTPUT_VARIABLE_LENGTH] =
{
		"PC8",
		"PC9",
		"PC10",
		"PC11",
		"PC12",
		"PD2",
		"PG2",
		"PG3",
		"PB14",
		"PB0",

};


const uint8_t aliaslist[][OUTPUT_VARIABLE_LENGTH][OUTPUT_VARIABLE_LENGTH] =
{
		{"D43","PC8"},
		{"D44","PC9"},
		{"D45","PC10"},
		{"D46","PC11"},
		{"D47","PC12"},
		{"D48","PD2"},
		{"D49","PG2"},
		{"D50","PG3"},
		{"LD3","PB14"},
		{"LD1","PB0"}
};

size_t USB_Send (const char * data, size_t len)
{

	 while (CDC_Transmit_FS((uint8_t*) data, (uint16_t) len)!= USBD_OK);
	return len;
}



static void transmit_idn()
{

}

static void failed_command (uint8_t* failtext)
{
	USB_Send ("Failed: \r\n",strlen("Failed: \r\n"));
	USB_Send ((const char*)failtext, strlen((const char*)failtext));
	USB_Send ("\r\n",2);
}






static uint8_t set_gpio (GPIO_TypeDef* port,uint16_t pin, GPIO_PinState state)
{
	HAL_GPIO_WritePin(port, pin, state);
	return 1;
}


static uint8_t select_port_pin (uint8_t* str,GPIO_PinState state, size_t len)
{
	uint8_t cmd_success = 1;
	uint8_t i =0;
	uint8_t str_len,cmplen;
	GPIO_TypeDef* port;
	uint16_t pin;
	static const uint8_t ioportlist [][1]=
	{
			  "A",
			  "B",
			  "C",
			  "D",
			  "E",
			  "F",
			  "G",
			  "H"
	};
	static const uint8_t pinlist [16][2]=
	{		  "0",
			  "1",
			  "2",
			  "3",
			  "4",
			  "5",
			  "6",
			  "7",
			  "8",
			  "9",
			  "10",
			  "11",
			  "12",
			  "13",
			  "14",
			  "15"
	};
	for (i = 0; i<sizeof(ioportlist)/sizeof(ioportlist[0]);i++)
	{
		if (ioportlist[i][0] == str[1])
		{
				cmd_success = 0;
				break;
		}
	}
	if (!(cmd_success==0))
	{
		return 0;
	}
	switch (i) {
			case 0: //A
				port = GPIOA;
				break;
			case 1: //B
				port = GPIOB;
				break;
			case 2: //C
				port = GPIOC;
				break;
			case 3: //D
				port = GPIOD;
				break;
			case 4: //E
				port = GPIOE;
				break;
			case 5: //F
				port = GPIOF;
				break;
			case 6: // G
				port = GPIOG;
				break;
			case 7: //H
				port = GPIOH;
				break;
			default:
				return 0;
				break;
		}
	cmd_success= 1;
	str_len = strlen((const char*)str+2);
	if (str_len>OUTPUT_VARIABLE_LENGTH-2)
	{
		str_len = OUTPUT_VARIABLE_LENGTH-2;
	}
	for (i = 0; i<sizeof(pinlist)/sizeof(pinlist[0]);i++)
	{
		cmplen = strlen((const char*)pinlist[i]);
		if (cmplen>OUTPUT_VARIABLE_LENGTH-2)
		{
			cmplen = OUTPUT_VARIABLE_LENGTH-2;
		}
		if (cmplen == str_len)
		{
			cmd_success = memcmp(str+2, (uint8_t*)pinlist[i], cmplen);
		}
		if (cmd_success==0)
		{
			break;
		}

	}
	if (!cmd_success==0)
	{
		return 0;
	}

	switch (i) {
		case 0:
			pin = GPIO_PIN_0;
			break;
		case 1:
			pin = GPIO_PIN_1;
			break;
		case 2:
			pin = GPIO_PIN_2;
			break;
		case 3:
			pin = GPIO_PIN_3;
			break;
		case 4:
			pin = GPIO_PIN_4;
			break;
		case 5:
			pin = GPIO_PIN_5;
			break;
		case 6:
			pin = GPIO_PIN_6;
			break;
		case 7:
			pin = GPIO_PIN_7;
			break;
		case 8:
			pin = GPIO_PIN_8;
			break;
		case 9:
			pin = GPIO_PIN_9;
			break;
		case 10:
			pin = GPIO_PIN_10;
			break;
		case 11:
			pin = GPIO_PIN_1;
			break;
		case 12:
			pin = GPIO_PIN_2;
			break;
		case 13:
			pin = GPIO_PIN_3;
			break;
		case 14:
			pin = GPIO_PIN_4;
			break;
		case 15:
			pin = GPIO_PIN_5;
			break;
		default:
			return 0;
			break;
	}
	return set_gpio(port, pin, state);
}

static uint8_t* convert_pin (uint8_t* str)
{
	uint8_t cmd_success =1;
	uint8_t* pinname;
	uint8_t pinarray[OUTPUT_VARIABLE_LENGTH];
	uint8_t i,j;
	size_t len;
	pinname=(uint8_t*)&pinarray;
	//Prepare an a proper length array
	memset((uint8_t*) pinname, 0, OUTPUT_VARIABLE_LENGTH);
	i = 0;
	j = 0;
	while (((str[j]==32)||(str[j]==95)) && (str[j]!=0)) // space or null or underscore
	{
		j++;
	}
	len = strlen((const char*)str+j);
	if (len>OUTPUT_VARIABLE_LENGTH)
	{
		len = OUTPUT_VARIABLE_LENGTH;
	}
	else if (len<1)
	{
		return 0;
	}
	memcpy(pinname,(uint8_t*) str+j , OUTPUT_VARIABLE_LENGTH);

	for (i = 0; i<sizeof(aliaslist)/sizeof(aliaslist[0]);i++)
		{
			cmd_success= memcmp(pinname,(uint8_t*)aliaslist[i][0],len);
			if (cmd_success == 0)
			{
				break;
			}
		}
	if (cmd_success==0)
	{
		return (uint8_t*)aliaslist[i][1];

	}
	else
	{
		return pinname;
	}
}





static uint8_t pin_command(uint8_t* cmdstr, uint8_t cmd_end, size_t len)
{
	GPIO_PinState  outdir;
	uint8_t str_len, iolen,cmplen,i,cmd_success;
	uint8_t* str;
	uint8_t pintable[OUTPUT_VARIABLE_LENGTH];
	if (cmd_end >=len)
	{
		failed_command(cmdstr);
		return 0;
	}
	if (cmd_end>3) //Select output based on the command length.
	{
		outdir = GPIO_PIN_RESET;
	}
	else
	{
		outdir = GPIO_PIN_SET;
	}
	str= convert_pin(cmdstr+cmd_end);
	str_len = strlen((const char*)str);

	if (str_len < 1)
	{
		failed_command(cmdstr);
		return 0;
	}
	memset(pintable, 0, OUTPUT_VARIABLE_LENGTH);
	for ( i =0;i<OUTPUT_VARIABLE_LENGTH;i++)
		pintable[i] = str[i];

	for (i = 0; i<sizeof(outputlist)/sizeof(outputlist[0]);i++)
		{
			iolen = strlen((const char*) outputlist[i]);
			if (iolen<=str_len)
			{
				cmplen = iolen;
			}
			else
			{
				cmplen = str_len;
			}
			str = pintable;
			cmd_success= memcmp(str,(uint8_t*)outputlist[i],cmplen);
			if (cmd_success==0)
			{
				break;
			}
		}
		if (cmd_success==0)
		{
			if (((uint8_t)outputlist[i][0]) == (uint8_t)'P')
			{
				if(!select_port_pin((uint8_t*)outputlist[i],outdir,cmplen))
				{
					return 0;
				}
			}
		else
			{
				failed_command(cmdstr);
				return 0;
			}
		}
		else
		{
			failed_command(cmdstr);
			return 0;
		}
		return 1;
	}




uint8_t parse_command(uint8_t* commandstring)
{
	uint8_t commandstrlen = strlen((const char*)commandstring);
	uint8_t available_commandlen,cmplen,cmd_success;
	cmd_success= 1;
	uint8_t i = 0;
	if (commandstrlen>COMMAND_MAX_LENGTH)
	{
		failed_command(commandstring);
		return 0;
	}

	for (i = 0; i<sizeof(commands)/sizeof(commands[0]);i++)
	{
		available_commandlen = strlen((const char*)commands[i]);
		if (available_commandlen<=commandstrlen)
		{
			cmplen = available_commandlen;
		}
		else
		{
			cmplen = commandstrlen;
		}
		cmd_success= memcmp(commandstring,(uint8_t*)commands[i],cmplen); // returns 0 if same
		if (cmd_success ==0)
		{
			break;
		}
	}
	if (cmd_success ==0)
	{
		switch (i) {
			case 0: //Idn
				transmit_idn();
				break;
			case 1: //SET
				if (pin_command(commandstring,strlen((const char*)commands[i]), commandstrlen))
				{
					USB_Send("1\r\n",3);
					return 1;
				}

				break;
			case 2: //RESET
				if (pin_command(commandstring,strlen((const char*)commands[i]), commandstrlen))
				{
					USB_Send("0\r\n",3);
					return 1;
				}
				break;
			case 3: //RST*
				{
					USB_Send("\r\nRESET\r\n",strlen("\r\nRESET\r\n"));
					NVIC_SystemReset();
				}
				// In case command failed. Should not go here
				failed_command(commandstring);
				break;
			default:
				failed_command(commandstring);
				return 0;
				break;
		}
	}
	else
	{
		failed_command(commandstring);
		return 0;
	}
	return 0;
}
void command_parser_init()
{
	return;
}
