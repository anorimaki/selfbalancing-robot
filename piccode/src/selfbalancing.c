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

#define TIMER_PERIOD			4						//In us
#define TIMER_STEPS_BY_MS		(1000/TIMER_PERIOD)

#define MOTORS_CONTROL_TIMER_STEPS		(1000000/MPU_DATA_RATE)/TIMER_PERIOD
#define MAX_MOTORS_CONTROL_TIMER_STEPS	(MOTORS_CONTROL_TIMER_STEPS*2)

#define SPEED_CONTROL_PERIOD	200					//In ms
#define SPEED_CONTROL_TIMER_STEPS	((SPEED_CONTROL_PERIOD*1000LU)/TIMER_PERIOD)


void main_action() 
{
	static uint32_t last_motors_ctrl_time = 0;
	static uint32_t next_speed_crtl_time = 0;
	
			//Pitch in radians and Q16 format
	fix16_t current_pitch;
	MpuStatus status = mpu9250_get_pitch( &current_pitch );
	display_mpu_result( status );
	if ( status != MPU_OK ) {
		return;
	}

	uint32_t current_time = TMR4_Counter32BitGet();
	uint16_t diff_time = current_time - last_motors_ctrl_time;
	if ( diff_time > MAX_MOTORS_CONTROL_TIMER_STEPS ) {
		display_motor_control_overtime();
	}
	last_motors_ctrl_time = current_time;


	if ( !ctrl_update_motors_power( current_pitch ) ) {
		return;
	}

	if ( current_time > next_speed_crtl_time ) {
		ctrl_update_pitch_target_and_steering();
		next_speed_crtl_time = current_time + SPEED_CONTROL_TIMER_STEPS;
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
		__delay_ms((1000/MPU_DATA_RATE) - 4);
	}
		
	return -1;
}