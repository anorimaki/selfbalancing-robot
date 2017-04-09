

#include "mcc.h"
#include "mpu/mpu9250.h"
#include "system.h"
#include "io/input_i2c.h"
#include "mpu/inv_pic24f_adapter.h"
#include <stdio.h>


void waitInitialization() {
	while( !system_is_on() ) {}
	printf( "System on\n" );
	
	mpu9250_init();
}


int main(void)
{
    SYSTEM_Initialize();
	
	system_init();
	inputi2c_init();
	printf( "System initialized\n" );
	
	while( 1 )
    {
		if ( !system_is_on() ) {
			waitInitialization();
		}
		
		MpuData data;
		MpuStatus status = mpu9250_get_data( &data );
		if ( status == MPU_ERROR ) {
			printf( "MPU communication error\n" );
		}
		else if ( status == MPU_OK ) {
			fix16_t pitch = quat_pitch( &data.quaternation );
			pitch = rad_to_deg( pitch );
			char pitch_str[MAX_FIX16_STR_SIZE];
			fix16_to_str( pitch, pitch_str );
			printf( "Pitch: %s\n", pitch_str );

#if 0 
			fix16_t roll = quat_roll( &data.quaternation );
			roll = rad_to_deg( roll );
			char roll_str[MAX_FIX16_STR_SIZE];
			fix16_to_str( roll, roll_str );
			
#endif
			//printf( "Pitch, roll: %s %s\n", pitch_str, roll_str );
		
		}
		
	//	delay_ms(10);
    }

    return -1;
}
