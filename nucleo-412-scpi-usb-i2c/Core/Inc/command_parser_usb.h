#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H
#include "main.h"
#define COMMAND_MAX_LENGTH  10
#define OUTPUT_VARIABLE_LENGTH 4
#define NOT_FOUND_IN_LIST_VAL -1
uint8_t parse_command(uint8_t*);
void command_parser_init();
size_t UART_Send (const char * data, size_t len);
size_t USB_Send (const char * data, size_t len);
#endif
