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

	Wire.begin();
	twi_setClock(400000);
	twi_setClockStretchLimit(250);

	struct int_param_s int_param;
	if ( mpu_init(&int_param) )
		RETURN_ERROR(EMPTY);

	if ( mpu_set_sensors( INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS ) )
		RETURN_ERROR(EMPTY);

	if ( dmp_load_motion_driver_firmware() )
		RETURN_ERROR(EMPTY);

	if ( dmp_enable_feature( DMP_FEATURE_6X_LP_QUAT ) )
		RETURN_ERROR(EMPTY);

	if ( dmp_enable_gyro_cal(1) )
		RETURN_ERROR(EMPTY);

	if ( dmp_set_fifo_rate(5) )
		RETURN_ERROR(EMPTY);

	if ( mpu_set_dmp_state(1) )
		RETURN_ERROR(EMPTY);
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
