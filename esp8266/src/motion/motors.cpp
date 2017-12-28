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


/************************************************************************/
// PidEngine
/************************************************************************/

bool PidEngine::state( std::vector<Motors::PitchState>& states  )
{
	uint8_t fifoSize;
	if ( !receive( m_fifoSizeReg, &fifoSize ) )
		MOTORS_ERROR(m_display);

	while( fifoSize > 0 ) {
		PitchState state;
		if ( !receive( m_fifoCurrentReg, &state ) )
			MOTORS_ERROR(m_display);

		states.push_back( state );
		--fifoSize;
	}

	return true;
}


bool PidEngine::settins( PIDSettings& settings )
{
	if ( !receive( m_settingsReg, &settings ) )
		MOTORS_ERROR(m_display);
	return true;
}


bool PidEngine::setSettins( const PIDSettings& settings )
{
	if ( !send( m_settingsReg, &settings ) )
		MOTORS_ERROR(m_display);
	return true;
}


/************************************************************************/
// Motors
/************************************************************************/
void Motors::init( io::Display* display )
{
	m_display = display;
	m_speed = new PidEngine( display, MOTORSREG_SPEED_PID, MOTORSREG_SPEED_FIFO_SIZE, MOTORSREG_SPEED_FIFO_CURRENT );
	m_pitch = new PidEngine( display, MOTORSREG_PITCH_PID, MOTORSREG_PITCH_FIFO_SIZE, MOTORSREG_PITCH_FIFO_CURRENT );

//twi_setClockStretchLimit( 0xFFF );
}


bool Motors::isRunning()
{
	uint8_t status;
	if ( !receive( MOTORSREG_STATUS, &status ) ) {
		MOTORS_ERROR(m_display);
		return false;
	}
	return status & MOTORSREGBIT_STATUS_RUNNING;
}


bool Motors::pause()
{
	if ( !send( MOTORSREG_STATUS, MOTORSREGBIT_STATUS_PAUSE_REQUEST ) ) {
		MOTORS_ERROR(m_display);
	}

	do {
		delay( 100 );
	} while( isRunning() );

	return true;
}


bool Motors::resume()
{
	if ( !send( MOTORSREG_STATUS, MOTORSREGBIT_STATUS_RUN_REQUEST ) ) {
		MOTORS_ERROR(m_display);
	}

	do {
		delay( 100 );
	} while( !isRunning() );

	return true;
}

bool Motors::setMpuOffset( int32_t pitchOffset )
{
	if ( !send( MOTORSREG_MPU_OFFSET, &pitchOffset ) ) {
		MOTORS_ERROR(m_display);
	}
	return true;
}

bool Motors::getMpuOffset( int32_t* pitchOffset )
{
	if ( !receive( MOTORSREG_MPU_OFFSET, pitchOffset ) ) {
		MOTORS_ERROR(m_display);
	}
	return true;
}

}
