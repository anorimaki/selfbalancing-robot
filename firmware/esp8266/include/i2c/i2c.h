#ifndef I2C_I2C_H
#define I2C_I2C_H

#include <stdint.h>

namespace i2c
{

static const uint8_t MPU_SCL = 5;		//D1
static const uint8_t MPU_SDA = 4;		//D2

static const uint8_t MOTORS_SCL = 12;	//D6
static const uint8_t MOTORS_SDA = 13;	//D7


void init( int sda, int scl );

bool write( unsigned char slave_addr, unsigned char reg_addr,
		const unsigned char* data, unsigned char length );

bool read( unsigned char slave_addr, unsigned char reg_addr,
		unsigned char* data, unsigned char length );

}


#endif
