#ifndef SPEED_H
#define	SPEED_H

#include "pid.h"

extern PID speed_data;

void speed_init();

#define speed_i2c_read(address) \
    pid_i2c_read( &speed_data, address )

#define speed_i2c_write(address, data) \
    pid_i2c_write( &speed_data, address, data )

#endif	/* SPEED_H */