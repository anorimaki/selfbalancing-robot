

#include "config.h"
#include "mpu/mpu9250.h"
#include "system.h"
#include "io/input_i2c.h"
#include "api/motors_i2c_reg.h"
#include "mpu/inv_pic24f_adapter.h"
#include "pidpitch.h"
#include "pid.h"
#include "motors.h"
#include <stdio.h>
#include <libpic30.h>


void wait_initialization() {
	while( !system_is_on() ) {}
	printf( "System on\n" );
	
	mpu9250_init();
}


bool cal_motors_power( int16_t target_pitch, int16_t* power )
{
	MpuData data;
	MpuStatus status = mpu9250_get_data( &data );
	if ( status == MPU_OVERUN )
		printf( "MPU FIFO overflow\n" );
	else if ( status == MPU_DATA_CORRUPTION )
		printf( "MPU data corruption\n" );
	else if ( status == MPU_ERROR )
		printf( "MPU communication error\n" );
	
	if ( status != MPU_OK )
		return false;

	fix16_t current_pitch = quat_to_pitch( &data.quaternation );
		
		//Read pitch is limited to 
		// [-57.2948º..57.2948º] range (17 LSB of Q16 pitch)
		//Important: 'target' pitch range must be shorter! ~ [-40º..40º]??
	current_pitch = PID_SCALE_INPUT( current_pitch, 17 );	//Adapt pitch to PID algorithm
	if ( current_pitch > PID_MAX_INPUT )			//Keep pitch in [-57º..57º] range
		current_pitch = PID_MAX_INPUT;
	else if ( current_pitch < PID_MIN_INPUT )
		current_pitch = PID_MIN_INPUT;
	
#if 0	
char bb[MAX_FIX16_STR_SIZE];
fix16_to_str( current_pitch, bb );
printf( "p: %lX %lX %lX %lX -- %lu %s\n",
		data.quaternation.w, data.quaternation.x,
	   data.quaternation.y, data.quaternation.z,
		pidpitch_data.store.write_ptr->index, bb );
#endif

	*power = pid_compute( &pidpitch_data, target_pitch, current_pitch ) ;
		
	return true;
}


int main(void)
{
    SYSTEM_Initialize();
	
	delay_ms(2);
	
	system_init();
	inputi2c_init();
	pidpitch_init();
	motors_init();
	
	printf( "System initialized\n" );
	
	int16_t pitch_target = 0;		//Must be adapted to PID algorithm input
	int i=0;
	while( 1 )
    {
		if ( !system_is_on() ) {
			wait_initialization();
		}

		int16_t motors_power;
		if ( cal_motors_power( pitch_target, &motors_power ) ) {
			motors_set_power( motors_power );
		}
		else {
			i++;
		}

	//	if ( i % 10 == 0 ) {
	//		printf( "i: %d\n", i );
	//	}
		
		__delay_ms(5);
    }

    return -1;
}
