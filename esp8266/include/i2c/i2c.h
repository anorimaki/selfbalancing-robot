#ifndef I2C_I2C_H
#define I2C_I2C_H

namespace i2c
{

void init( int sda, int scl );

bool write( unsigned char slave_addr, unsigned char reg_addr,
		unsigned char* data, unsigned char length );

bool read( unsigned char slave_addr, unsigned char reg_addr,
		unsigned char* data, unsigned char length );

}


#endif
