#include <Arduino.h>		//define min
#include "inv_esp8266_adapter.h"
#include "util/trace.h"
#include <Wire.h>
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
	Wire.beginTransmission(slave_addr);

	if ( Wire.write(reg_addr) != 1 )
		RETURN_ERROR(-1)

	for ( unsigned char i = 0; i < length; i++)
	{
		if ( Wire.write(data[i]) != 1 )
			RETURN_ERROR(-1)
	}

	Wire.endTransmission(true);
	
	return 0;
}



int esp8266_i2c_read(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data)
{
	Wire.beginTransmission(slave_addr);
	if ( Wire.write(reg_addr) != 1 )
		RETURN_ERROR(-1)

	Wire.endTransmission(false);

	delayMicroseconds(5);

#if 1
	int read = twi_readFrom(slave_addr, data, length, true);
	if ( read != 0 ) {
		return -1;
	}
#else
	if ( Wire.requestFrom(slave_addr, length) != length )
		RETURN_ERROR(-1)

	for ( unsigned char i = 0; i < length; i++ )
		data[i] = Wire.read();
#endif
	
	return 0;
}


void esp8266_trace( const char * format, ... )
{
	va_list arg;
	va_start(arg, format);
	char temp[64];
	vsnprintf(temp, sizeof(temp), format, arg);
	va_end(arg);

	Serial.printf(temp);
}