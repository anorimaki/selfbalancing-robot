#include "mpu/inv_pic24f_adapter.h"
#include "i2c2.h"
#include "config.h"
#include <stdarg.h> 
#include <stdio.h>
#include <string.h>
#include <libpic30.h>


void pic24f_delay_ms( unsigned long num_ms )
{
	__delay_ms( num_ms );
}


void pic24f_get_ms( unsigned long *count )
{
	*count = 0;
}

#define I2C2_MAX_BUFFER	24
static uint8_t buffer[I2C2_MAX_BUFFER];

int pic24f_i2c_write(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data)
{
	I2C2_MESSAGE_STATUS status;
	buffer[0] = reg_addr;
	memcpy( &buffer[1], data, length );
	I2C2_MasterWrite( &reg_addr, 1, slave_addr, &status );
	while( status == I2C2_MESSAGE_PENDING );
	return ! (status == I2C2_MESSAGE_COMPLETE);
}



int pic24f_i2c_read(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data)
{
	I2C2_MESSAGE_STATUS status;
	I2C2_TRANSACTION_REQUEST_BLOCK readTRB[2];
	
	// Build TRB for sending reg_addr
    I2C2_MasterWriteTRBBuild( &readTRB[0], &reg_addr, 1, slave_addr );
                
	// Build TRB for receiving data
    I2C2_MasterReadTRBBuild( &readTRB[1], data, length, slave_addr );

	// now send the transactions
    I2C2_MasterTRBInsert(2, readTRB, &status);

  	while( status == I2C2_MESSAGE_PENDING );
	return ! (status == I2C2_MESSAGE_COMPLETE);
}


void pic24f_trace( const char * format, ... )
{
	va_list arg;
	va_start(arg, format);
	char temp[64];
	vsnprintf(temp, sizeof(temp), format, arg);
	va_end(arg);
	
	printf( temp );
}
