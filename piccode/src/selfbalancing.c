

#include "config.h"
#include "mpu/mpu9250.h"
#include "system.h"
#include "io/input_i2c.h"
#include "io/display.h"
#include "api/motors_i2c_reg.h"
#include "mpu/inv_pic24f_adapter.h"
#include "heading.h"
#include "pitch.h"
#include "speed.h"
#include "pid.h"
#include "motors.h"
#include "boost/preprocessor/arithmetic/sub.hpp"
#include <stdio.h>
#include <libpic30.h>


//In Q16 format and RADs, it is equivalent to [-14.2º .. +14.2º] range
#define MAX_PITCH_ANGLE_BITS	15		//In signed fomat
#define MAX_PITCH_ANGLE         ((1 << (MAX_PITCH_ANGLE_BITS-1))-1)	//Q16 format
#define MIN_PITCH_ANGLE			(-MAX_PITCH_ANGLE)					//Q16 format

#define PITCH_CONTROL_PERIOD	500		//In ms

//Number of pitch PID execuions between two speed PID executions
#define PITCH_SPEED_CONTROL_RATIO	(PITCH_CONTROL_PERIOD/(1000/MPU_DATA_RATE))


/*
 * target_pitch range must be adjusted to PID algorithm input.
 */
bool cal_motors_power( int16_t* power )
{
		//Pitch in radians and Q16 format
	fix16_t current_pitch;
	MpuStatus status = mpu9250_get_pitch( &current_pitch );
	display_mpu_result( status );
	if ( status != MPU_OK )
		return false;
		
		//Limit angle range to:
		//	[-14.2 .. +14.2] degrees
	if ( (current_pitch > MAX_PITCH_ANGLE) ||
		(current_pitch < MIN_PITCH_ANGLE) ) {
		*power = 0;		//Max angle reached. Robot has fallen. Turn off motors
		return true;
	}

	int16_t pid_current_pitch = current_pitch;	//Integer part is discarded
												// because it's out from
												// allowed pitch range.
	pid_current_pitch = SCALE_VALUE( pid_current_pitch, MAX_PITCH_ANGLE_BITS,
								PID_INPUT_BIT_SIZE );
	
#if 0
	char bb[MAX_FIX16_STR_SIZE];
	fix16_to_str( MAX_PITCH_ANGLE, bb );
	printf( "p: %s %d %d\n", bb, pid_current_pitch, current_pitch );
#endif	
	
	*power = pid_compute( &pitch_data, pid_current_pitch ) ;
		
	*power = SCALE_VALUE( *power, PID_OUTPUT_BIT_SIZE, 16 );
	
	return true;
}


/*
 * Returns pitch target range adjusteded to PID algorithm input.
 */
int16_t cal_pitch_target()
{
	int16_t speed = motors_speed();
	
	speed = SCALE_VALUE( speed, MOTORS_SPEED_BITS, PID_INPUT_BIT_SIZE );

	int16_t ret = pid_compute( &speed_data, speed ) ;
	
	//Adjust to PID algorithm input and restrict target to the half of max angle
	return SCALE_VALUE( ret, PID_OUTPUT_BIT_SIZE,
						BOOST_PP_SUB( PID_INPUT_BIT_SIZE, 1 ) );
}


void main_action() 
{
	static int8_t to_speed_control = PITCH_SPEED_CONTROL_RATIO;
	
	int16_t motors_power;
	if ( cal_motors_power( &motors_power ) ) {
		motors_set_power( motors_power );
	}

	if ( --to_speed_control == 0 ) {
		pitch_data.target = cal_pitch_target();
		to_speed_control = PITCH_SPEED_CONTROL_RATIO;
	}
}


inline void mpu_blocking_error()
{
	display_mpu_error();
	while(true) {
		__delay_ms(5000);
	}
}


void main_resume() 
{
	if ( !mpu9250_start() ) {
		mpu_blocking_error();
	}
	system_running();
	
	display_system_starting();
	__delay_ms(10000);		//Wait for MPU to stabilize it
	
	display_system_running();
}


void main_pause()
{
	if ( !mpu9250_end() ) {
		mpu_blocking_error();
	}
	display_system_paused();
	system_paused();
}


int main(void)
{
    SYSTEM_Initialize();
	
	display_system_initialization();
	
	motors_init();
	system_init();
	inputi2c_init();
	pitch_init();
	speed_init();
	heading_init();
	mpu9250_init();
		
	display_system_paused();
	system_paused();
	
	while( 1 )
    {
		if ( system_is_running() ) {
			if ( system_pause_request() ) {
				main_pause();
			}
			else {
				main_action();
			}
		}
		else {
			if ( system_run_request() ) {
				main_resume();
			}
		}
		
		// 3ms to do all main_action computations
		__delay_ms((1000/MPU_DATA_RATE) - 2);
	}
		
	return -1;
}