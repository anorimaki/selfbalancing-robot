

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

int16_t process_mpu_data()
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
	
	
		//Read pitch is limited to [-57.2948º..57.2948º] range (17 lowerts bits)
		//Important: 'target' pitch range must be shorter! ~ [-40º..40º]??
	pitch = PID_SCALE_INPUT( pitch, 17 );	//Adapt pitch to PID algorithm
	if ( pitch > PID_MAX_INPUT )			//Keep pitch in [-57º..57º] range
		pitch = PID_MAX_INPUT;
	else if ( pitch < PID_MIN_INPUT )
		pitch = PID_MIN_INPUT;
	
	pitch_pid->current = pitch;
	int16_t power = pid_compute( pitch_pid ) ;
	
//	printf( "Pitch: %s. Power: %d\n", pitch_str, power );
	
	return power;
}


static void init_pitch_data()
{
	pid_init( pitch_pid );
	pitch_pid->target=0;
			//Max bits of constants = 15-PID_DATA_BITS to avoid overflows
	pitch_pid->k_p=127;
	pitch_pid->k_i=50;
	pitch_pid->k_d=10;
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
