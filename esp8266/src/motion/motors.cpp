#include "motion/motors.h"
#include "motors_protocol.h"
#include "util/trace.h"
#include "i2c/i2c.h"

namespace motion
{



static bool send( uint8_t reg_address, uint8_t* data, uint8_t length )
{
	return i2c::write( motors_i2c_address, reg_address, data, length );
}


static bool send( uint8_t reg_address, uint8_t data )
{
	return send( reg_address, &data, 1 );
}


static bool receive( uint8_t reg_address, uint8_t* data, uint8_t length )
{
	return i2c::read( motors_i2c_address, reg_address, data, length );
}

template <typename T>
static bool receive( uint8_t reg_address, T& obj )
{
	return receive( reg_address, reinterpret_cast<uint8_t*>(&obj), sizeof(T) );
}




bool Motors::init()
{
	if ( !send( MOTORSREG_STATUS, 0 ) )
		return false;
	delay( 100 );
	return send( MOTORSREG_STATUS, MOTORSREGBIT_STATUS_ON );
}


bool Motors::pitchState( Pitch::State& state  )
{
	return receive( MOTORSREG_PITCH_PID_PREV_ERR, state );
}



}
