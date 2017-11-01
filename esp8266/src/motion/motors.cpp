#include "motion/motors.h"
#include "util/trace.h"
#include "i2c/i2c.h"
#include "motors_i2c_api.h"

namespace motion
{



static bool send( uint8_t reg_address, const uint8_t* data, uint8_t length )
{
	return i2c::write( motors_i2c_address, reg_address, data, length );
}


template <typename T>
static bool send( uint8_t reg_address, const T* obj )
{
	return send( reg_address, reinterpret_cast<const uint8_t*>(obj), sizeof(T) );
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




#define MOTORS_ERROR(display) { 			\
			display->motorsError(); 		\
			TRACE_ERROR_AND_RETURN(false);	\
		}


bool Motors::init( io::Display* display )
{
	m_display = display;

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
		MOTORS_ERROR(m_display);

	while( fifoSize > 0 ) {
		PitchState state;
		if ( !receive( MOTORSREG_PITCH_FIFO_CURRENT, &state ) )
			MOTORS_ERROR(m_display);
		states.push_back( state );
		--fifoSize;
	}

	return true;
}


bool Motors::pitchPIDSettins( PIDSettings& settings )
{
	if ( !receive( MOTORSREG_PITCH_PID, &settings ) )
		MOTORS_ERROR(m_display);
	return true;
}


bool Motors::setPitchPIDSettins( const PIDSettings& settings )
{
	if ( !send( MOTORSREG_PITCH_PID, &settings ) )
		MOTORS_ERROR(m_display);
	return true;
}


}
