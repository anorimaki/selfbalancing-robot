#ifndef MPU9250_MOTIONDRIVER_H
#define	MPU9250_MOTIONDRIVER_H

#define DMP_FEATURE_TAP             (0x001)
#define DMP_FEATURE_ANDROID_ORIENT  (0x002)
#define DMP_FEATURE_LP_QUAT         (0x004)
#define DMP_FEATURE_PEDOMETER       (0x008)
#define DMP_FEATURE_6X_LP_QUAT      (0x010)
#define DMP_FEATURE_GYRO_CAL        (0x020)
#define DMP_FEATURE_SEND_RAW_ACCEL  (0x040)
#define DMP_FEATURE_SEND_RAW_GYRO   (0x080)
#define DMP_FEATURE_SEND_CAL_GYRO   (0x100)

#define DMP_INT_GESTURE     (0x01)
#define DMP_INT_CONTINUOUS  (0x02)


void dmp_load_motion_driver_firmware();

void dmp_enable_feature( int16 mask );
void dmp_enable_lp_quat(int1 enable);
void dmp_enable_6x_lp_quat(int1 enable);

void dmp_enable_gyro_cal(int1 enable);

void dmp_set_fifo_rate( int8 rate );

int1 dmp_read_fifo_quat( int32 *quat, unsigned char *more );
int1 dmp_read_fifo( signed int16 *gyro, signed int16 *accel, signed int32 *quat );

void dmp_set_interrupt_mode(int8 mode);

#endif	/* MPU9250_MOTIONDRIVER_H */

