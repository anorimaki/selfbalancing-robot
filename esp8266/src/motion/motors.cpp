#include "motion/motors.h"
#include "motors_protocol.h"
#include "util/trace.h"
#include "Wire.h"

namespace motion
{



static bool send( uint8_t reg_address, uint8_t* data, uint8_t length )
{
#if 0
	if(twi_writeTo( 0x10, NULL, 0, true ) != 0)
		TRACE_ERROR_AND_RETURN(false)

	  return true;
#endif

	Wire.beginTransmission( motors_i2c_address );

	if ( Wire.write(reg_address) != 1 )
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


static bool send( uint8_t reg_address, uint8_t data )
{
	return send( reg_address, &data, 1 );
}

enum Commands {

};


bool Motors::init()
{
	if ( !send( MOTORSREG_STATUS, 0 ) )
		return false;
	delay( 100 );
	return send( MOTORSREG_STATUS, MOTORSREGBIT_STATUS_ON );

}



}
