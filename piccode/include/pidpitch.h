#ifndef PIDPITCHSTORE_H
#define	PIDPITCHSTORE_H

#include <stdint.h>
#include "pid.h"

extern PID pidpitch_data;

void pidpitch_init();

#define pidpitch_i2c_read(address) \
    pid_i2c_read( &pidpitch_data, address )

#define pidpitch_i2c_write(address, data) \
    pid_i2c_write( &pidpitch_data, address, data )

#endif	/* PIDPITCHSTORE_H */

