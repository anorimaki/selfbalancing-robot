#include "main.h"
#include "mpu9250.h"
#include "mpu9250_motiondriver.h"
#include <math.h>

#pin_select U1TX=PIN_B15
#pin_select U1RX=PIN_B14
#use rs232(baud=38400, UART1, STOP=1)

void mpu_error_handler( int8 code ) {
	printf( "Error: %d\n", code );
	while(1) {}
}


void init()
{
	setup_spi( SPI_SS_DISABLED );
    setup_spi2( SPI_SS_DISABLED );
    setup_wdt( WDT_OFF );
    setup_adc_ports( ADC_OFF );
	
	mpu_init();
	mpu_set_sensors( MPU_SENSOR_XYZ_GYRO | 
					MPU_SENSOR_XYZ_ACCEL | MPU_SENSOR_XYZ_COMPASS );
	
	dmp_load_motion_driver_firmware();
	
	dmp_enable_feature( DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_RAW_GYRO |
							DMP_FEATURE_6X_LP_QUAT );
	dmp_enable_gyro_cal(1);
	dmp_set_fifo_rate(5);
	mpu_set_dmp_state(1);
}


float accel_to_g( signed int16 number )
{
	return (float)number / 16384.0f;
}


void compute_euler_angles( signed int32* quat, float* angles )
{
	int i;

			//Normalize quaternation
	float quatf[4];
	for( i =0;i<4;i++) 
		quatf[i]=quat[i];
	float sum = 0.0;
	for( i=0;i<4;i++) 
		sum += quatf[i]*quatf[i];
	float qlen=sqrt(sum);
	
	float dqw = quatf[0]/qlen;
    float dqx = quatf[1]/qlen;
    float dqy = quatf[2]/qlen;
    float dqz = quatf[3]/qlen;

	float ysqr = dqy * dqy;
    float t0 = 1.0f - 2.0f * (ysqr + dqz * dqz);
    float t1 = 2.0f * (dqx * dqy + dqw * dqz);
    float t2 = 2.0f * (dqw * dqy - dqx * dqz);
    float t3 = 2.0f * (dqy * dqz + dqw * dqx);
    float t4 = 1.0f - 2.0f * (dqx * dqx + ysqr);
  
	// Keep t2 within range of asin (-1, 1)
    t2 = t2 > 1.0f ? 1.0f : t2;
    t2 = t2 < -1.0f ? -1.0f : t2;
	
    angles[0] = asin(t2);
    angles[1] = atan2(t3, t4);
    angles[2] = atan2(t1, t0);
	
	for ( i = 0; i<3; ++i ) {
		float f = angles[i];
		f *= (180.0 / PI);
		angles[i] = f;
	} 
}



void main()
{
	delay_ms( 500 );
	
	init();
	
	printf( "Who am i: %X\n", mpu_who_am_i() );
	
	while(1)
	{
		signed int16 gyro[3];
		signed int16 accel[3];
		signed int32 quat[4];
		if ( dmp_read_fifo( gyro, accel, quat ) ) {
			float angles[3];
			compute_euler_angles( quat, angles );
			
			printf( "Accel: %f %f %f\n",
				accel_to_g(accel[0]), accel_to_g(accel[1]),
				accel_to_g(accel[2]) );
			
		//	printf( "Gyro: %Ld %Ld %Ld\n",
		//		gyro[0], gyro[1], gyro[2] );
			
			printf( "Quater: %08LX %08LX %08LX %08LX ->  %f %f %f\n",
				quat[0], quat[1], quat[2], quat[3],
				angles[0], angles[1], angles[2] );
		}

		delay_ms(1);
	}
}

#include "mpu9250.c"
#include "mpu9250_motiondriver.c"
#include "mpu_i2c.c"