#include <Arduino.h>		//define min
#include "mpu/inv_esp8266_adapter.h"
#include "util/trace.h"
#include "i2c/i2c.h"
#include <stdio.h>


void esp8266_delay_ms( unsigned long num_ms )
{
	delay(num_ms);
}


void esp8266_get_ms( unsigned long *count )
{
	*count = millis();
}


int esp8266_i2c_write(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data)
{
	return i2c::write( slave_addr, reg_addr, data, length ) ? 0 : -1;
}



int esp8266_i2c_read(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data)
{
	return i2c::read( slave_addr, reg_addr, data, length ) ? 0 : -1;
}


void esp8266_trace( const char * format, ... )
{
	va_list arg;
	va_start(arg, format);
	char temp[128];
	vsnprintf(temp, sizeof(temp), format, arg);
	va_end(arg);

	Serial.printf(temp);
}


char* to_s( int buf_n, float num )
{
	static char buf[3][30];
	strcpy( buf[buf_n], String(num).c_str() );
	return buf[buf_n];
}
