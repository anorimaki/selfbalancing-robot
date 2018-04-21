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

#define PITCH_CONTROL_TIMER_STEPS		(1000000/MPU_DATA_RATE)/TIMER_PERIOD
#define MAX_PITCH_CONTROL_TIMER_STEPS	(PITCH_CONTROL_TIMER_STEPS*2)

#define SPEED_CONTROL_PERIOD	200					//In ms
#define SPEED_CONTROL_TIMER_STEPS	((SPEED_CONTROL_PERIOD*1000LU)/TIMER_PERIOD)


bool crtl_pitch() 
{
	fix16_t current_pitch;
	MpuStatus status = mpu9250_get_pitch( &current_pitch );
	display_mpu_result( status );
	if ( status != MPU_OK ) {
		return false;
	}
	ctrl_update_motors_power( current_pitch );
	return true;
}



void main_action() 
{
	static uint32_t last_pitch_crtl_time = 0;
	static uint32_t next_speed_crtl_time = 0;
	
	uint32_t current_time = TMR4_Counter32BitGet();
	
	uint16_t diff_time = current_time - last_pitch_crtl_time;
	if ( diff_time > (PITCH_CONTROL_TIMER_STEPS-200) ) {
		if ( diff_time > MAX_PITCH_CONTROL_TIMER_STEPS ) {
			display_motor_control_overtime();
		}
		if ( crtl_pitch() ) {
			last_pitch_crtl_time = current_time;
		}
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
	//	__delay_us(100);
	}
		
	return -1;
}