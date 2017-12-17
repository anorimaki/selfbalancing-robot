#include "mpu/mpu9250.h"
#include "mpu/inv_mpu.h"
#include "mpu/inv_mpu_dmp_motion_driver.h"
#include "trace.h"


//DMP firmaware was previously loaded and sensors has been selected.
// No need to load firmware and configure sensors here.
 bool mpu9250_start() {
	struct int_param_s int_param;
	if ( mpu_init(&int_param) )
		TRACE_ERROR_AND_RETURN(false);

	if ( dmp_enable_feature( DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_GYRO_CAL ) )
		TRACE_ERROR_AND_RETURN(false);

	if ( dmp_set_fifo_rate(200) )
		TRACE_ERROR_AND_RETURN(false);
	
//	if ( mpu_set_lpf(200) )
//		TRACE_ERROR_AND_RETURN(false);

	if ( mpu_set_dmp_state(1) )
		TRACE_ERROR_AND_RETURN(false);
	
#if 0
	int16_t gyro[3];
	int16_t accel[3];
	if ( mpu_read_6500_gyro_bias( gyro ) != 0 ) {
		TRACE_ERROR_AND_RETURN(false);
	}
	if ( mpu_read_6500_accel_bias( accel ) != 0 ) {
		TRACE_ERROR_AND_RETURN(false);
	}
		
	printf( "Old gyro calibration data: %d %d %d\n", gyro[0], gyro[1], gyro[2] );
	printf( "Old accel calibration data: %d %d %d\n", accel[0], accel[1], accel[2] );
#endif
	return true;
}
 
 
bool mpu9250_end() 
{
	if ( mpu_set_dmp_state(0) )
	   TRACE_ERROR_AND_RETURN(false);
	return true;
}


MpuStatus mpu9250_get_data( MpuData* data ) {
	long quat[4];
	short gyro[3];
	short accel[3];
	short sensors;
	unsigned long timestamp;
	unsigned char more = 0xFF;

	int err = dmp_read_fifo( gyro, accel, quat, &timestamp, &sensors, &more );
	if ( err != 0 ) {
		return (more==0) ? MPU_NO_DATA : 
			(err==-6) ? MPU_OVERUN : 
			(err==-10) ? MPU_DATA_CORRUPTION : 
			MPU_ERROR;
	}
	
	//Normalization: DMP quaternation is yet normalized but in 
	//	q30 fixed point decimal. This format isn't documented but you can infer
	//	it from FIFO_CORRUPTION_CHECK code in DMP sources.
	// -> Convert it to q16 by removing lowest 14 bits
	data->quaternation.w = quat[0] >> 14;
	data->quaternation.x = quat[1] >> 14;
	data->quaternation.y = quat[2] >> 14;
	data->quaternation.z = quat[3] >> 14;

	return MPU_OK;
}