#include "mpu9250.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "util/trace.h"
#include "Wire.h"


namespace mpu
{


/* These next two functions converts the orientation matrix (see
* gyro_orientation) to a scalar representation for use by the DMP.
* NOTE: These functions are borrowed from InvenSense's MPL.
* http://docs.ros.org/hydro/api/ric_mc/html/MPU9150Lib_8cpp_source.html
*/

static inline unsigned short inv_row_2_scale(const signed char *row)
{
	unsigned short b;

	if (row[0] > 0)
		b = 0;
	else if (row[0] < 0)
		b = 4;
	else if (row[1] > 0)
		b = 1;
	else if (row[1] < 0)
		b = 5;
	else if (row[2] > 0)
		b = 2;
	else if (row[2] < 0)
		b = 6;
	else
		b = 7;      // error
	return b;
}

/* The sensors can be mounted onto the board in any orientation. The mounting
* matrix seen below tells the MPL how to rotate the raw data from thei
* driver(s).
* TODO: The following matrices refer to the configuration on an internal test
* board at Invensense. If needed, please modify the matrices to match the
* chip-to-body matrix for your particular set up.
*/

static inline unsigned short inv_orientation_matrix_to_scalar(const signed char *mtx)
{
	unsigned short scalar;
	/*
		XYZ  010_001_000 Identity Matrix
		XZY  001_010_000
		YXZ  010_000_001
		YZX  000_010_001
		ZXY  001_000_010
		ZYX  000_001_010
	*/
	scalar = inv_row_2_scale(mtx);
	scalar |= inv_row_2_scale(mtx + 3) << 3;
	scalar |= inv_row_2_scale(mtx + 6) << 6;
	return scalar;
}


bool Mpu9250::init()
{
	struct int_param_s int_param;
	int err;
	if ( (err=mpu_init(&int_param)) != 0 ) {
		TRACE_ERROR( "MPU init error: %d", err );
		return false;
	}

	if ( mpu_set_sensors( INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS ) )
		TRACE_ERROR_AND_RETURN(false);

	return true;
}


bool Mpu9250::configure()
{
	static signed char orientation_matrix[9] = { 0, -1, 0,
													1, 0, 0,
													0, 0, 1};

	if ( mpu_set_sensors( INV_XYZ_GYRO | INV_XYZ_ACCEL ) )
		TRACE_ERROR_AND_RETURN(false);

	if ( dmp_load_motion_driver_firmware() )
		TRACE_ERROR_AND_RETURN(false);

	dmp_set_orientation( inv_orientation_matrix_to_scalar(orientation_matrix) );

	if ( dmp_enable_feature( DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_GYRO_CAL |
						DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO ) )
		TRACE_ERROR_AND_RETURN(false);

	if ( dmp_set_fifo_rate(100) )
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


bool Mpu9250::enableDmp( bool enable ) {
	if ( mpu_set_dmp_state(enable) )
		TRACE_ERROR_AND_RETURN(false);
	return true;
}


bool Mpu9250::storedCalibration() {
	static const long gyro[3] = {-27, 6, 19};
	static const long accel[3] = {21, 75, 79};

	return setCalibration( accel, gyro );
}


bool Mpu9250::readCalibration()
{
	long gyro[3];
	long accel[3];
	if ( mpu_read_6500_gyro_bias( gyro ) != 0 ) {
		TRACE_ERROR_AND_RETURN(false);
	}
	if ( mpu_read_6500_accel_bias( accel ) != 0 ) {
		TRACE_ERROR_AND_RETURN(false);
	}

	m_accelBias[0] = accel[0];
	m_accelBias[1] = accel[1];
	m_accelBias[2] = accel[2];
	m_gyroBias[0] = gyro[0];
	m_gyroBias[1] = gyro[1];
	m_gyroBias[2] = gyro[2];

	TRACE( "Gyro bias (abs): %d %d %d", m_gyroBias[0], m_gyroBias[1], m_gyroBias[2] );
	TRACE( "Accel bias (abs): %d %d %d", m_accelBias[0], m_accelBias[1], m_accelBias[2] );

	return true;
}

/*
 * Accel bias as differential offset.
 * Gyro bias as absolute offset
 */
bool Mpu9250::setCalibration( const long *accelBias, const long *gyroBias )
{
	if ( mpu_set_gyro_bias_reg( const_cast<long*>(gyroBias) ) != 0 ) {
		TRACE_ERROR_AND_RETURN(false);
	}
	if ( mpu_set_accel_bias_6500_reg( accelBias ) != 0 ) {
		TRACE_ERROR_AND_RETURN(false);
	}

	TRACE( "New gyro bias (abs): %ld %ld %ld", gyroBias[0], gyroBias[1], gyroBias[2] );
	TRACE( "New accel bias (diff): %ld %ld %ld", accelBias[0], accelBias[1], accelBias[2] );

	return readCalibration();
}


bool Mpu9250::calibrate()
{
	long gyro[3];
	long accel[3];

	//Reset gyro offset to 0 before new offset calculation
	//accel reset isn't needed because mpu_set_accel_bias_6500_reg
	// takes its arguments as a differential offset.
	gyro[0] = gyro[1] = gyro[2] = 0;
	if ( mpu_set_gyro_bias_reg(gyro) != 0 ) {
		TRACE_ERROR_AND_RETURN(false);
	}

	// calculate accel and gyro offsets
	int result = mpu_run_6500_self_test(gyro, accel, false);
	if ( (result & 0x3) != 0x3 ) {
		TRACE_ERROR_AND_RETURN(false);
	}

	/* Test passed. We can trust the gyro data here, so let's push it down
	 * to the DMP.
	 */
	unsigned char i;
	for (i = 0; i < 3; i++) {
		accel[i] *= 2048.f; 				//convert to +-16G
		accel[i] = accel[i] >> 16;
		gyro[i] = (long) (gyro[i] * 32.8f); //convert to +-1000dps
		gyro[i] = (long) (gyro[i] >> 16);
	}

	return setCalibration(accel, gyro);
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
