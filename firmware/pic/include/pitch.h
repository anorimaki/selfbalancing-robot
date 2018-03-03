#ifndef PITCH_H
#define	PITCH_H

#include "pid.h"

extern PID pitch_data;

void pitch_init();

#define pitch_i2c_read(address) \
    pid_i2c_read( &pitch_data, address )

#define pitch_i2c_write(address, data) \
    pid_i2c_write( &pitch_data, address, data )

#endif	/* PIDPITCH_H */

