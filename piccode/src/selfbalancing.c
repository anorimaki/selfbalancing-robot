

#include "config.h"
#include "mpu/mpu9250.h"
#include "system.h"
#include "io/input_i2c.h"
#include "io/motors_reg.h"
#include "mpu/inv_pic24f_adapter.h"
#include "pid.h"
#include "motors.h"
#include <stdio.h>
#include <libpic30.h>


void waitInitialization() {
	while( !system_is_on() ) {}
	printf( "System on\n" );
	
	mpu9250_init();
}

// pitch_pid->current: in Q14 format 
// pitch_pid->target: in Q14 format
static PID* pitch_pid = (PID*)&system_registers[MOTORSREG_PITCH_PID_KP];

static int16_t process_mpu_data()
{
	MpuData data;
	MpuStatus status = mpu9250_get_data( &data );
	if ( status == MPU_ERROR ) {
		printf( "MPU communication error\n" );
	}
	if ( status != MPU_OK )
		return 0xFFFF;
	
	fix16_t pitch = quat_to_pitch( &data.quaternation );
	
	char pitch_str[MAX_FIX16_STR_SIZE];
	fix16_to_str( pitch, pitch_str );
	
		//Adapt to PID algorithm.
		//This limit 'current' input pitch range to ~ [-57º..57º]
		//Important: 'target' pitch range must be shorter! ~ [-40º..40º]??
	pitch >>= 2;		// Q16 -> Q14
	if ( pitch > PID_MAX_INPUT )
		pitch = PID_MAX_INPUT;
	else if ( pitch < PID_MIN_INPUT )
		pitch = PID_MIN_INPUT;
	
	pitch_pid->current = pitch;
	int16_t power = pid_compute( pitch_pid ) ;

			//Adapt to PWM resolution of 12 bits
#if (PID_DATA_BIT_SIZE > 12)	
	power >>= (PID_DATA_BIT_SIZE-12);
#else
	power <<= (12-PID_DATA_BIT_SIZE);
#endif
	
	printf( "Pitch: %s. Power: %d\n", pitch_str, power );
	
	return power;
}


static void init_pitch_data()
{
	pid_init( pitch_pid );
	pitch_pid->target=0;
			//Max bits of constants = 15-PID_DATA_BITS to avoid overflows
	pitch_pid->k_p=127;
	pitch_pid->k_i=0;
	pitch_pid->k_d=0;
}


int main(void)
{
    SYSTEM_Initialize();
	
	system_init();
	inputi2c_init();
	motors_init();
	
	init_pitch_data();
	
	printf( "System initialized\n" );
	
	while( 1 )
    {
		if ( !system_is_on() ) {
			waitInitialization();
		}
		
		int16_t motors_power = process_mpu_data();
		if ( motors_power != 0xFFFF )
			motors_set_power( motors_power );
    }

    return -1;
}
