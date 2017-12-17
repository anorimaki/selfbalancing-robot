

#include "config.h"
#include "mpu/mpu9250.h"
#include "system.h"
#include "io/input_i2c.h"
#include "io/display.h"
#include "api/motors_i2c_reg.h"
#include "mpu/inv_pic24f_adapter.h"
#include "pidpitch.h"
#include "pidspeed.h"
#include "pid.h"
#include "motors.h"
#include <stdio.h>
#include <libpic30.h>

//In Q16 format and RADs, it is equivalent to [-14.2º .. +14.2º] range
#define MAX_PITCH_ANGLE_BITS	15		//In signed fomat
#define MAX_PITCH_ANGLE         ((1 << (MAX_PITCH_ANGLE_BITS-1))-1)	//Q16 format
#define MIN_PITCH_ANGLE			(-MAX_PITCH_ANGLE)					//Q16 format


#define SPEED_CONTROL_PERIOD	3	//Number of pitch PID execuions between 
									// two speed PID executions

bool cal_motors_power( int16_t target_pitch, int16_t* power )
{
	MpuData data;
	MpuStatus status = mpu9250_get_data( &data );
	display_mpu_result( status );
	if ( status != MPU_OK )
		return false;

	//Pitch in radians and Q16 format
	fix16_t current_pitch = quat_to_pitch( &data.quaternation );
		
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
	fix16_to_str( current_pitch, bb );
	printf( "p: %s %d\n", bb, pid_current_pitch );
#endif	
	
	*power = pid_compute( &pidpitch_data, target_pitch, pid_current_pitch ) ;
		
	return true;
}


int16_t cal_pitch_target( int16_t speed_target )
{
	int16_t left_speed = motors_right_speed();
	int16_t right_speed = motors_left_speed();
	
	int16_t speed = left_speed + right_speed;

	return pid_compute( &pidspeed_data, speed_target, speed ) ;
}


void main_action() 
{
	static int16_t pitch_target = 0;		//Must be adapted to PID algorithm input
	static int16_t speed_target = 0;
	static int8_t to_speed_control = SPEED_CONTROL_PERIOD;
	
	int16_t motors_power;
	if ( cal_motors_power( pitch_target, &motors_power ) ) {
		motors_set_power( motors_power, 0 );
	}

	if ( --to_speed_control == 0 ) {
//			pitch_target = cal_pitch_target( speed_target );
		to_speed_control = SPEED_CONTROL_PERIOD;
	}
}


inline void mpu_blocking_error()
{
	display_mpu_error();
	while(true) {
		__delay_ms(1000);
	}
}


void main_resume() 
{
	if ( !mpu9250_start() ) {
		mpu_blocking_error();
	}
}


void main_pause()
{
	if ( !mpu9250_end() ) {
		mpu_blocking_error();
	}
}


int main(void)
{
    SYSTEM_Initialize();
	
	display_system_initialization();
	
	motors_init();
	system_init();
	inputi2c_init();
	pidpitch_init();
	pidspeed_init();
		
	display_system_paused();
	system_paused();
	
	while( 1 )
    {
		if ( system_is_running() ) {
			if ( system_pause_request() ) {
				main_pause();
				display_system_paused();
				system_paused();
			}
			else {
				main_action();
			}
		}
		else {
			if ( system_run_request() ) {
				main_resume();
				display_system_running();
				system_running();
			}
		}
		
		__delay_ms(2);
	}
		
	return -1;
}