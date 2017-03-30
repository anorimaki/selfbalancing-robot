#include "mpu9250.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "util/trace.h"
#include "Wire.h"





namespace mpu
{



void Mpu9250::init( ErrorHandler errorHandler )
{
	m_errorHandler = errorHandler;

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


Optional<MpuData> Mpu9250::getData()
{
	long quat[4];
	short gyro[3];
	short accel[3];
	short sensors;
	unsigned long timestamp;
	unsigned char more = 0xFF;

	if ( dmp_read_fifo( gyro, accel, quat, &timestamp, &sensors, &more ) ) {
		if ( more != 0 )
			handleError();
		return Optional<MpuData>();
	}

	return MpuData( Quaternation(quat[0], quat[1], quat[2], quat[3]), timestamp );
}


void Mpu9250::handleError()
{
	if ( m_errorHandler ) {
		(*m_errorHandler)();
	}

}

}
