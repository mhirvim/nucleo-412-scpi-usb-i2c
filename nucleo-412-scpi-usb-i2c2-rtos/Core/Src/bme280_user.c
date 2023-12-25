/*
 * bme280_user.c
 *
 *  Created on: Jun 25, 2023
 *      Author: Maxim
 */

#include "main.h"
#include "bme280_user.h"
#include "bme280.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

static  I2C_HandleTypeDef* I2C;
static int32_t  temperature;
static int32_t humidity;
static int32_t pressure;

static struct bme280_dev dev;
static struct bme280_data comp_data;
static int8_t rslt;

static char hum_string[16];
static char temp_string[16];
static char press_string[16];

void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}
// Implementation of custom_citoa()
static char* int32_t_to_str(int32_t num, char* str)
{
    int i = 0;
    uint8_t isNegative = 0;

    /* Handle 0 explicitly, otherwise empty string is
     * printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    // In standard itoa(), negative numbers are handled
    // only with base 10. Otherwise numbers are
    // considered unsigned.
    if (num < 0) {
        isNegative = 1;
        num = -num;
    }
    // Process individual digits
    while (num != 0) {
        int rem = num % 10;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / 10;
    }
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
    str[i] = '\0'; // Append string terminator
    // Reverse the string
    reverse(str, i);
    return str;
}

static char* uint32_t_to_str(uint32_t num, char* str)
{
    int i = 0;
    /* Handle 0 explicitly, otherwise empty string is
     * printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    // Process individual digits
    while (num != 0) {
        int rem = num % 10;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / 10;
    }
    str[i] = '\0'; // Append string terminator
    // Reverse the string
    reverse(str, i);
    return str;
}




void user_i2c_comms_init(I2C_HandleTypeDef* hi2c)
{
	I2C = hi2c;
	/* BME280 init */
	 dev.dev_id = BME280_I2C_ADDR_PRIM;
	 dev.intf = BME280_I2C_INTF;
	 dev.read = user_i2c_read;
	 dev.write = user_i2c_write;
	 dev.delay_ms = user_delay_ms;

	 rslt = bme280_init(&dev);

	  /* BME280 settings */
	  dev.settings.osr_h = BME280_OVERSAMPLING_1X;
	  dev.settings.osr_p = BME280_OVERSAMPLING_16X;
	  dev.settings.osr_t = BME280_OVERSAMPLING_2X;
	  dev.settings.filter = BME280_FILTER_COEFF_16;
	  rslt = bme280_set_sensor_settings(BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL, &dev);
}

int8_t user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
	  if(HAL_I2C_Master_Transmit(I2C, (id << 1), &reg_addr, 1, 10) != HAL_OK) return -1;
	  if(HAL_I2C_Master_Receive(I2C, (id << 1) | 0x01, data, len, 10) != HAL_OK) return -1;

	  return 0;
}


int8_t user_set_sensor_data(uint8_t sensor_mode)
{
    rslt = bme280_set_sensor_mode(sensor_mode, &dev);
    return rslt;
}


int8_t user_read_sensor_data(uint8_t sensor_comp)
{
    	memset(hum_string, 0, sizeof(hum_string));
    	memset(temp_string, 0, sizeof(temp_string));
    	memset(press_string, 0, sizeof(press_string));
	    /*Get Data */
	    rslt = bme280_get_sensor_data(sensor_comp, &comp_data, &dev);
	    if(rslt == BME280_OK)
	    {
	      //millicelsius
	      temperature = comp_data.temperature * 10;
	      //ppm
	      humidity =(comp_data.humidity * 10000) / 1024;
	      //pascal
	      pressure = comp_data.pressure;
	      //Data to string
	      int32_t_to_str((int32_t)temperature, temp_string);
	      uint32_t_to_str((uint32_t)humidity, hum_string);
	      uint32_t_to_str((uint32_t)pressure,press_string );

	}
	    return rslt;
}

char* get_temperature_string_ptr ()
{
	rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
	if (rslt != BME280_OK)
	{
		return 0;
	}
	dev.delay_ms(40);
	rslt = user_read_sensor_data(BME280_ALL);
	if (rslt == BME280_OK)
	{
		return temp_string;
	}
	else
	{
		return 0;
	}
}

char* get_humidity_string_ptr ()
{
	rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
	if (rslt != BME280_OK)
	{
		return 0;
	}
	dev.delay_ms(40);
	rslt = user_read_sensor_data(BME280_ALL);
	if (rslt == BME280_OK)
	{
		return hum_string;
	}
	else
	{
		return 0;
	}
}

char* get_pressure_string_ptr ()
{
	rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
	if (rslt != BME280_OK)
	{
		return 0;
	}
	dev.delay_ms(40);
	rslt = user_read_sensor_data(BME280_ALL);
	if (rslt == BME280_OK)
	{
		return press_string;
	}
	else
	{
		return 0;
	}
}


void user_delay_ms(uint32_t period)

{
	HAL_Delay(period);
}
int8_t user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
	  int8_t *buf;
	  buf = malloc(len +1);
	  buf[0] = reg_addr;
	  memcpy(buf +1, data, len);

	  if(HAL_I2C_Master_Transmit(I2C, (id << 1), (uint8_t*)buf, len + 1, HAL_MAX_DELAY) != HAL_OK) return -1;

	  free(buf);
	  return 0;
}
