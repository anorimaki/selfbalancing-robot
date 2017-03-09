#include "inv_esp8266_adapter.h"

#include <Arduino.h>
#include <Wire.h>


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
		return -1;

	for ( unsigned char i = 0; i < length; i++)
	{
		if ( Wire.write(data[i]) != 1 )
			return -1;
	}

	Wire.endTransmission(true);
	
	return 0;
}


int esp8266_i2c_read(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data)
{
	Wire.beginTransmission(slave_addr);
	if ( Wire.write(reg_addr) != 1 )
		return -1;
	Wire.endTransmission(false);

	if ( Wire.requestFrom(slave_addr, length) != length )
		return -1;

	for ( unsigned char i = 0; i < length; i++ )
		data[i] = Wire.read();
	
	return 0;
}
