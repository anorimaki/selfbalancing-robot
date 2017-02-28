#include "main.h"
#include "mpu_i2c.h"

#use i2c(MASTER, I2C2, Fast, NOINIT,stream=MPU_STREAM)

#byte I2C2TRN = getenv( "SFR:I2C2TRN" )

#word I2C2STAT = getenv( "SFR:I2C2STAT" )
#bit I2C2STAT_TRSTAT = I2C2STAT.14
#bit I2C2STAT_ACKSTAT = I2C2STAT.15

#word I2C2CON = getenv( "SFR:I2C2CON" )
#bit I2C2CON_STREN = I2C2CON.6

#define MPU_RECEIVE(addr)		((addr<<1) | 1)
#define MPU_SEND(addr)			(addr<<1)


void mpu_comm_init()
{
	i2c_init( MPU_STREAM, 1 );
	
//	output_drive( PIN_B2 );
//	output_drive( PIN_B3 );
}

void mpu_read_begin( int8 addr, int8 reg_addr )
{
	i2c_start( MPU_STREAM );
	
				//Send MPU address
	if ( i2c_write( MPU_STREAM, MPU_SEND(addr) ) ) {
		mpu_error_handler( MPU_ERROR_DEVICE_NOT_PRESENT );
		i2c_stop(MPU_STREAM);
		return;
	}
	
				//Send MPU register address
	if( i2c_write( MPU_STREAM, reg_addr ) ) {
		mpu_error_handler( MPU_ERROR_MESSAGE_NOT_CONFIRMED );
		i2c_stop(MPU_STREAM);
		return;
	}
	
	i2c_start( MPU_STREAM );
	
				//Send MPU address
	if( i2c_write( MPU_STREAM, MPU_RECEIVE(addr) ) ) {
		mpu_error_handler( MPU_ERROR_MESSAGE_NOT_CONFIRMED );
		i2c_stop(MPU_STREAM);
		return;
	}
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


int8 mpu_read( int8 addr, int8 reg_addr )
{
	mpu_read_begin( addr, reg_addr );

	return mpu_read_end();
}


void mpu_write_begin( int8 addr, int8 reg_addr )
{
    i2c_start( MPU_STREAM );
	
				//Send MPU address
	if( i2c_write( MPU_STREAM, MPU_SEND(addr) ) ) {
		mpu_error_handler( MPU_ERROR_DEVICE_NOT_PRESENT );
		i2c_stop(MPU_STREAM);
		return;
	}
	
				//Send MPU register address
	if( i2c_write( MPU_STREAM, reg_addr ) ) {
		mpu_error_handler( MPU_ERROR_MESSAGE_NOT_CONFIRMED );
		i2c_stop(MPU_STREAM);
		return;
	}
}


inline void mpu_write_next( int8 data )
{			  //Send data
	if( i2c_write( MPU_STREAM, data ) ) {
		mpu_error_handler( MPU_ERROR_MESSAGE_NOT_CONFIRMED );
		i2c_stop(MPU_STREAM);
		return;
	}
}


inline void mpu_write_end()
{
    i2c_stop( MPU_STREAM );
}


void mpu_write( int8 addr, int8 reg_addr, int8 data )
{
    mpu_write_begin( addr, reg_addr );
    mpu_write_next( data );
    mpu_write_end();
}
