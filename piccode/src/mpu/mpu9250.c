#include "mpu/mpu9250.h"
#include "mpu/inv_mpu.h"
#include "mpu/inv_mpu_dmp_motion_driver.h"
#include "trace.h"



void mpu9250_init() {
	struct int_param_s int_param;
	if ( mpu_init(&int_param) )
		TRACE_ERROR_AND_RETURN(EMPTY());

	if ( mpu_set_sensors( INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS ) )
		TRACE_ERROR_AND_RETURN(EMPTY());

	if ( dmp_load_motion_driver_firmware() )
		TRACE_ERROR_AND_RETURN(EMPTY());

	if ( dmp_enable_feature( DMP_FEATURE_6X_LP_QUAT ) )
		TRACE_ERROR_AND_RETURN(EMPTY());

	if ( dmp_enable_gyro_cal(1) )
		TRACE_ERROR_AND_RETURN(EMPTY());

	if ( dmp_set_fifo_rate(5) )
		TRACE_ERROR_AND_RETURN(EMPTY());

	if ( mpu_set_dmp_state(1) )
		TRACE_ERROR_AND_RETURN(EMPTY());
}


MpuStatus mpu9250_get_data( MpuData* data ) {
	long quat[4];
	short gyro[3];
	short accel[3];
	short sensors;
	unsigned long timestamp;
	unsigned char more = 0xFF;

	if ( dmp_read_fifo( gyro, accel, quat, &timestamp, &sensors, &more ) ) {
		return (more != 0) ? MPU_ERROR : MPU_NO_DATA;
	}
	
	data->quaternation.w = quat[0];
	data->quaternation.x = quat[1];
	data->quaternation.y = quat[2];
	data->quaternation.z = quat[3];

	return MPU_OK;
}