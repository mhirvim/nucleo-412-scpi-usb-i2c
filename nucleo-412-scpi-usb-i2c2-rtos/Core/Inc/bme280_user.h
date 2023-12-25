/*
 * bme280_user.h
 *
 *  Created on: Jun 25, 2023
 *      Author: Maxim
 */

#ifndef INC_BME280_USER_H_
#define INC_BME280_USER_H_
#include "main.h"
#include "bme280.h"
void user_i2c_comms_init(I2C_HandleTypeDef* hi2c);
int8_t user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len);
void user_delay_ms(uint32_t period);
int8_t user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len);
int8_t user_set_sensor_data(uint8_t sensor_mode);
int8_t user_read_sensor_data(uint8_t sensor_comp);
char* get_temperature_string_ptr ();
char* get_humidity_string_ptr ();
char* get_pressure_string_ptr ();
#endif /* INC_BME280_USER_H_ */
