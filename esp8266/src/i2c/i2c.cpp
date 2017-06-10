#include "i2c/i2c.h"
#include "util/trace.h"
#include <Wire.h>

namespace i2c
{


void init( int sda, int scl )
{
	Wire.begin( sda, scl );
	twi_setClock(400000);
}


bool write( unsigned char slave_addr, unsigned char reg_addr,
         unsigned char* data, unsigned char length )
{
	Wire.beginTransmission(slave_addr);

	if ( Wire.write(reg_addr) != 1 )
		TRACE_ERROR_AND_RETURN(false)

	for ( unsigned char i = 0; i < length; i++)
	{
		if ( Wire.write(data[i]) != 1 )
			TRACE_ERROR_AND_RETURN(false)
	}

	if ( Wire.endTransmission(true) != 0 )
		TRACE_ERROR_AND_RETURN(false);

	return true;
}


bool read( unsigned char slave_addr, unsigned char reg_addr,
		unsigned char* data, unsigned char length )

{
#if 1
	int err = twi_writeTo(slave_addr, &reg_addr, 1, false);
	if ( err != 0 ) {
		TRACE_ERROR( "I2C write error: %d", err );
		return false;
	}

	delayMicroseconds(5);

	err = twi_readFrom(slave_addr, data, length, true);
	if ( err != 0 ) {
		TRACE_ERROR( "I2C read error: %d", err );
		return false;
	}
#else
	Wire.beginTransmission(slave_addr);
	if ( Wire.write(reg_addr) != 1 )
		TRACE_ERROR_AND_RETURN(false)

	Wire.endTransmission(false);

	delayMicroseconds(20);

	if ( Wire.requestFrom(slave_addr, length) != length )
		TRACE_ERROR_AND_RETURN(-1)

	for ( unsigned char i = 0; i < length; i++ )
		data[i] = Wire.read();
#endif

	return true;
}



}
