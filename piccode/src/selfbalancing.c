

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
		
		//Read pitch is limited to 
		// [-57.2948º..57.2948º] range (16 LSB of Q16 pitch plus sign)
		//Important: target pitch range must be shorter! ~ [-40º..40º]??
	current_pitch = PID_SCALE_INPUT( current_pitch, 15 );	//Adapt pitch to PID algorithm
	if ( current_pitch > PID_MAX_INPUT )			//Keep pitch in [-57º..57º] range
		current_pitch = PID_MAX_INPUT;
	else if ( current_pitch < PID_MIN_INPUT )
		current_pitch = PID_MIN_INPUT;

	*power = pid_compute( &pidpitch_data, target_pitch, current_pitch ) ;
		
	return true;
}


int16_t cal_pitch_target( int16_t speed_target )
{
	int16_t left_speed = motors_right_speed();
	int16_t right_speed = motors_left_speed();
	
	int16_t speed = left_speed + right_speed;

	return pid_compute( &pidspeed_data, speed_target, speed ) ;
}


int main(void)
{
    SYSTEM_Initialize();
	
	display_system_initialization();
	
	__delay_ms(2);
	
	system_init();
	inputi2c_init();
	pidpitch_init();
	pidspeed_init();
	motors_init();
	
	display_system_ready();
	
	int16_t pitch_target = 0;		//Must be adapted to PID algorithm input
	int16_t speed_target = 0;
	int8_t to_speed_control = SPEED_CONTROL_PERIOD;
	
	while( 1 )
    {
		if ( !system_is_on() ) {
			display_system_wait();
			while( !system_is_on() ) {}
			display_system_ready();
			
			if ( !mpu9250_init() ) {
				display_mpu_init_error();
				return -1;
			}
			
			to_speed_control = SPEED_CONTROL_PERIOD;
		}

		int16_t motors_power;
		if ( cal_motors_power( pitch_target, &motors_power ) ) {
			motors_set_power( motors_power, 0 );
		}
		
		if ( --to_speed_control == 0 ) {
			pitch_target = cal_pitch_target( speed_target );
			to_speed_control = SPEED_CONTROL_PERIOD;
		}
		
		__delay_ms(3);
    }

    return -1;
}
