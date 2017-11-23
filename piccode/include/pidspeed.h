#ifndef PIDSPEED_H
#define	PIDSPEED_H

#include <stdint.h>
#include "pid.h"

extern PID pidspeed_data;

void pidspeed_init();

#define pidspeed_i2c_read(address) \
    pid_i2c_read( &pidspeed_data, address )

#define pidspeed_i2c_write(address, data) \
    pid_i2c_write( &pidspeed_data, address, data )

#endif	/* PIDSPEED_H */