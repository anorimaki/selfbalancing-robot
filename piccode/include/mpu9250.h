#ifndef MPU9250_H
#define	MPU9250_H

#define MPU_SENSOR_X_GYRO      (0x40)
#define MPU_SENSOR_Y_GYRO      (0x20)
#define MPU_SENSOR_Z_GYRO      (0x10)
#define MPU_SENSOR_XYZ_GYRO    (MPU_SENSOR_X_GYRO | MPU_SENSOR_Y_GYRO | MPU_SENSOR_Z_GYRO)
#define MPU_SENSOR_XYZ_ACCEL   (0x08)
#define MPU_SENSOR_XYZ_COMPASS (0x01)
void mpu_set_sensors(int8 sensors);

void mpu_configure_fifo(int8 sensors);

#define MPU_FSR_250DPS	(0<<3)
#define MPU_FSR_500DPS	(1<<3)
#define MPU_FSR_1000DPS	(2<<3)
#define MPU_FSR_2000DPS (3<<3)
void mpu_set_gyro_fsr(int8 fsr);

#define MPU_FSR_2G		(0<<3)
#define MPU_FSR_4G		(1<<3)
#define MPU_FSR_8G		(2<<3)
#define MPU_FSR_16G		(3<<3)
void mpu_set_accel_fsr(int8 fsr);

void mpu_set_lpf(int8 lpf);

void mpu_set_sample_rate( int16 rate );

void mpu_set_int_latched( int1 enable );

void mpu_set_bypass( int1 bypass_on );

void mpu_get_accel_reg( int16 *data, int32* timestamp );

void mpu_reset_fifo();
void mpu_configure_fifo(int8 sensors);

void mpu_init();

#endif	/* MPU9250_H */

