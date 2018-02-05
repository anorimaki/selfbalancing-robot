#ifndef HEADING_H
#define	HEADING_H

#include "pid.h"

extern PID heading_data;

void heading_init();

#define heading_i2c_read(address) \
    pid_i2c_read( &heading_data, address )

#define heading_i2c_write(address, data) \
    pid_i2c_write( &heading_data, address, data )

#endif	/* HEADING_H */

