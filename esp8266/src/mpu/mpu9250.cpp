#include "mpu9250.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "util/trace.h"
#include "Wire.h"


namespace mpu
{


bool Mpu9250::init()
{
	struct int_param_s int_param;
	int err;
	if ( (err=mpu_init(&int_param)) != 0 ) {
		TRACE_ERROR( "MPU init error: %d", err );
		return false;
	}

	if ( mpu_set_sensors( INV_XYZ_GYRO | INV_XYZ_ACCEL ) )
		TRACE_ERROR_AND_RETURN(false);

	if ( dmp_load_motion_driver_firmware() )
		TRACE_ERROR_AND_RETURN(false);

	if ( dmp_enable_feature( DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_GYRO_CAL |
						DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO ) )
		TRACE_ERROR_AND_RETURN(false);

	if ( dmp_set_fifo_rate(5) )
		TRACE_ERROR_AND_RETURN(false);

	if ( mpu_set_dmp_state(1) )
		TRACE_ERROR_AND_RETURN(false);

	return true;
}


bool Mpu9250::end()
{
	if ( mpu_set_dmp_state(0) )
		TRACE_ERROR_AND_RETURN(false);

	return true;
}


void Mpu9250::test()
{
	long gyro[4];
	long accel[4];
	int result = mpu_run_6500_self_test( gyro, accel, true );
	Serial.printf( "result: %X\n", result );
}


bool Mpu9250::getData( Optional<MpuData>& data )
{
	long quat[4];
	short gyro[3];
	short accel[3];
	short sensors;
	unsigned long timestamp;
	unsigned char more = 0xFF;

	int err = dmp_read_fifo( gyro, accel, quat, &timestamp, &sensors, &more );
	if ( err == -1 ) {
		if ( more != 0 )
			return false;
		data = Optional<MpuData>();
		return true;
	}
	if ( err == -2 ) {
		TRACE_ERROR_MSG_AND_RETURN( "Mpu FIFO overflow", false );
	}

	data = MpuData( timestamp );

	if ( sensors & INV_WXYZ_QUAT ) {
		data->setQuaternation( Quaternation(quat) );
	}

	if ( sensors & INV_XYZ_ACCEL ) {
		data->setAccel( Accel(accel) );
	}

	if ( sensors & INV_XYZ_GYRO ) {
		data->setGyro( Gyro(gyro) );
	}

	return true;
}


}
