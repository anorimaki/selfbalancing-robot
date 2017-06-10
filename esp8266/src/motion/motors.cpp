#include "motion/motors.h"
#include "util/trace.h"
#include "i2c/i2c.h"
#include "motors_i2c_api.h"

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
static bool receive( uint8_t reg_address, T* obj )
{
	return receive( reg_address, reinterpret_cast<uint8_t*>(obj), sizeof(T) );
}




bool Motors::init()
{
twi_setClockStretchLimit( 0xFFF );
	if ( !send( MOTORSREG_STATUS, 0 ) )
		return false;
	delay( 100 );
	return send( MOTORSREG_STATUS, MOTORSREGBIT_STATUS_ON );
}



bool Motors::pitchState( std::vector<Motors::PitchState>& states  )
{
	uint8_t fifoSize;
	if ( !receive( MOTORSREG_PITCH_FIFO_SIZE, &fifoSize ) )
		TRACE_ERROR_AND_RETURN(false)

	while( fifoSize > 0 ) {
		PitchState state;
		if ( !receive( MOTORSREG_PITCH_FIFO_CURRENT, &state ) )
			TRACE_ERROR_AND_RETURN(false)
		states.push_back( state );
		--fifoSize;
	}

	return true;
}



}
