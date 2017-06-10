#ifndef PID_H
#define	PID_H

#include <stdint.h>

namespace control
{

struct PID
{
	struct Settings
	{
		int8_t k_p;
		int8_t k_d;
		int8_t k_i;
	};


	struct State
	{
		int32_t index;
		int16_t previous_error;
		int32_t integral_error;
		int16_t target;
		int16_t current;
	};

	Settings settings;
	State state;
};

}

#endif
