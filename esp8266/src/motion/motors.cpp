#include <motion/motors.h>
#include "motors_i2c.h"
#include "util/trace.h"
#include "Wire.h"

namespace motion
{

static bool send( MotorsCommand cmd, uint8_t* data, uint8_t length )
{
	Wire.beginTransmission( motors_i2c_address );

	if ( Wire.write(cmd) != 1 )
		TRACE_ERROR_AND_RETURN(false)

	for ( unsigned char i = 0; i < length; i++)
	{
		if ( Wire.write(data[i]) != 1 )
			TRACE_ERROR_AND_RETURN(false)
	}

	Wire.endTransmission(true);

	return true;
}


static bool send( MotorsCommand cmd )
{
	return send( cmd, NULL, 0 );
}

enum Commands {

};


bool Motors::init()
{
	return send( MotorsInit );

}



}
