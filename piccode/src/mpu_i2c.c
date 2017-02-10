#include "main.h"
#include "mpu_i2c.h"

#use i2c(MASTER, I2C2, FAST, NOINIT,stream=MPU_STREAM)

#define MPU_I2C_ADDR			0b1101000		

#define MPU_RECEIVE(addr)		((addr<<1) | 1)
#define MPU_SEND(addr)			(addr<<1)


#define CHECK_I2C_ACK(i) \
	if (i) { \
		i2c_stop(MPU_STREAM); \
		return; \
	}


void mpu_comm_init()
{
	i2c_init( MPU_STREAM );
	
//	output_drive( PIN_B2 );
//	output_drive( PIN_B3 );
}

void mpu_read_begin( int8 reg_addr )
{
	i2c_start( MPU_STREAM );
	
				//Send MPU address
	CHECK_I2C_ACK( i2c_write( MPU_STREAM, MPU_SEND(MPU_I2C_ADDR) ) );
	
				//Send MPU register address
	CHECK_I2C_ACK( i2c_write( MPU_STREAM, reg_addr ) );
	
	i2c_start( MPU_STREAM );
	
				//Send MPU address
	CHECK_I2C_ACK( i2c_write( MPU_STREAM, MPU_RECEIVE(MPU_I2C_ADDR) ) );
}


inline int8 mpu_read_next()
{
	return i2c_read( MPU_STREAM, 1 );
}


int8 mpu_read_end()
{
	int8 ret = i2c_read( MPU_STREAM, 0 );
	i2c_stop( MPU_STREAM );
	return ret;
}


int8 mpu_read( int8 reg_addr )
{
	mpu_read_begin( reg_addr );

	return mpu_read_end();
}


void mpu_write( int8 reg_addr, int8 data )
{
	i2c_start( MPU_STREAM );
	
				//Send MPU address
	CHECK_I2C_ACK( i2c_write( MPU_STREAM, MPU_SEND(MPU_I2C_ADDR) ) );
	
				//Send MPU register address
	CHECK_I2C_ACK( i2c_write( MPU_STREAM, reg_addr ) );
	
				//Send data
	CHECK_I2C_ACK( i2c_write( MPU_STREAM, data ) );
	
	i2c_stop( MPU_STREAM );
}


