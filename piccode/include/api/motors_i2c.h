#ifndef MOTORS_API_H
#define MOTORS_API_H

#include <stdint.h>

static const uint8_t motors_i2c_address = 0x02;

enum MotorsCommand {
	MotorsInit	 = 0x01
};


#endif
