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
	
//	dmp_enable_feature( DMP_FEATURE_SEND_RAW_ACCEL );
	dmp_enable_6x_lp_quat(1);
	dmp_enable_gyro_cal(1);
	dmp_set_fifo_rate(5);
	mpu_set_dmp_state(1);
}

#define QUATERNATION_TO_FLOAT_MASK	0x3FFFFFFF
#define QUATERNATION_TO_FLOAT_DIV	0x40000000

float quaternation_to_float( signed int32 number, int8 q )
{
	signed int32 i = number >> 16;
	if ( number & 0x80000000 )
		i |= 0xFFFF0000;
	return i;
#if 0
	float ret = (int32)(number & QUATERNATION_TO_FLOAT_MASK);
	ret /= QUATERNATION_TO_FLOAT_DIV;
	signed int32 i = number >> 30;
	ret += i;
	return (i & 0x2) ? -ret : ret;
#if 0
	unsigned long mask;
	for (int i=0; i<q; i++)
		mask |= (1<<i);
	return (number >> q) + ((number & mask) / (float) (2<<(q-1)));
#endif
#endif
}



void compute_euler_angles( signed int32* quat, float* angles )
{
    float dqw = quaternation_to_float(quat[0], 30);
    float dqx = quaternation_to_float(quat[1], 30);
    float dqy = quaternation_to_float(quat[2], 30);
    float dqz = quaternation_to_float(quat[3], 30);

#if 1	
//	tb[1] = asin(2.0*(q[0]*q[2] - q[1]*q[3]));
//	tb[0] = atan2(2.0*(q[2]*q[3] + q[0]*q[1]), 1.0 - 2.0*(q[1]*q[1] + q[2]*q[2]));
//	tb[2] = atan2(2.0*(q[1]*q[2] + q[0]*q[3]), 1.0 - 2.0*(q[2]*q[2] + q[3]*q[3]));
	float ysqr = dqy * dqy;
    float t0 = 1.0f - 2.0f * (ysqr + dqz * dqz);
    float t1 = 2.0f * (dqx * dqy + dqw * dqz);
    float t2 = 2.0f * (dqw * dqy - dqx * dqz);
    float t3 = 2.0f * (dqy * dqz + dqw * dqx);
    float t4 = 1.0f - 2.0f * (dqx * dqx + ysqr);
#else
    float ysqr = dqy * dqy;
    float t0 = -2.0f * (ysqr + dqz * dqz) + 1.0f;
    float t1 = +2.0f * (dqx * dqy - dqw * dqz);
    float t2 = -2.0f * (dqx * dqz + dqw * dqy);
    float t3 = +2.0f * (dqy * dqz - dqw * dqx);
    float t4 = -2.0f * (dqx * dqx + ysqr) + 1.0f;
#endif
  
	// Keep t2 within range of asin (-1, 1)
//    t2 = t2 > 1.0f ? 1.0f : t2;
//    t2 = t2 < -1.0f ? -1.0f : t2;
  
    angles[0] = asin(t2);
    angles[1] = atan2(t3, t4);
    angles[2] = atan2(t1, t0);
	
	for ( int i = 0; i<3; ++i ) {
		float f = angles[i];
		f *= (180.0 / PI);
	//	if ( f < 0 ) 
	//		f = 360.0 + f;
		angles[i] = f;
	} 
}



void main()
{
	delay_ms( 500 );
	
	init();
	
	while(1)
	{
#if 0
		signed int16 accel[3];
		if ( dmp_read_fifo( 0, accel, 0 ) ) {
			printf( "%05Ld %05Ld %05Ld\n", accel[0], accel[1], accel[2]  );
		}
#else
		signed int32 quat[4];
		int8 more;
		if ( dmp_read_fifo_quat( quat, &more ) )
		{
			float angles[3];
			compute_euler_angles( quat, angles );
					
			printf( "%LX %LX %LX %LX  ->  %f %f %f\n",
				quat[0], quat[1], quat[2], quat[3], 
				angles[0], angles[1], angles[2]  );
		}
#endif
		delay_ms(1);
	}
}

#include "mpu9250.c"
#include "mpu9250_motiondriver.c"
#include "mpu_i2c.c"