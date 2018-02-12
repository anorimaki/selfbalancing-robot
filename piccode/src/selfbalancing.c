#include "config.h"
#include "mpu/mpu9250.h"
#include "system.h"
#include "io/input_i2c.h"
#include "io/display.h"
#include "api/motors_i2c_reg.h"
#include "mpu/inv_pic24f_adapter.h"
#include "boost/preprocessor/arithmetic/sub.hpp"
#include "control.h"
#include <stdio.h>
#include <libpic30.h>


#define SPEED_CONTROL_PERIOD	200		//In ms

//Number of pitch PID execuions between two speed PID executions
#define PITCH_SPEED_CONTROL_RATIO	(SPEED_CONTROL_PERIOD/(1000/MPU_DATA_RATE))


void main_action() 
{
	static int8_t to_speed_control = PITCH_SPEED_CONTROL_RATIO;
	
			//Pitch in radians and Q16 format
	fix16_t current_pitch;
	MpuStatus status = mpu9250_get_pitch( &current_pitch );
	display_mpu_result( status );
	if ( status != MPU_OK ) {
		return;
	}
	
	if ( !ctrl_update_motors_power( current_pitch ) ) {
		return;
	}

	if ( --to_speed_control == 0 ) {
		ctrl_update_pitch_target_and_steering();
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
	
	system_init();
	inputi2c_init();
	mpu9250_init();
	ctrl_init();
		
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