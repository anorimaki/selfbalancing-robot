#ifndef MPU_I2C_H
#define	MPU_I2C_H

void mpu_comm_init();

void mpu_read_begin( int8 reg_addr );
inline int8 mpu_read_next();
int8 mpu_read_end();


int8 mpu_read( int8 reg_addr );

void mpu_write( int8 reg_addr, int8 data );


#endif	/* MPU_I2C_H */

