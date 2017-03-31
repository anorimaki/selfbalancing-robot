

#include "mcc.h"
#include "mpu/mpu9250.h"
#include <stdio.h>

int main(void)
{
    SYSTEM_Initialize();
	
	
	
	mpu9250_init();

    while( 1 )
    {
		MpuData data;
		MpuStatus status = mpu9250_get_data( &data );
		if ( status == MPU_OK ) {
			printf( "Quater: %ld %ld %ld %ld ->",
					data.quaternation.w, data.quaternation.x, 
					data.quaternation.y, data.quaternation.z );
			
			quat_normalize( &data.quaternation );

		}
    }

    return -1;
}
