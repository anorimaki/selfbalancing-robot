#include "speed.h"

PID speed_data;

#define SPEED_PID_STORE_SIZE	150

static PIDStateEntry speed_pid_store[SPEED_PID_STORE_SIZE];

void speed_init()
{
	pid_init( &speed_data, speed_pid_store, SPEED_PID_STORE_SIZE );
		
			//Max bits of constants = 15-PID_DATA_BITS to avoid overflows
	speed_data.settings.k_p = 150;
	speed_data.settings.k_i = 0;
	speed_data.settings.k_d = 50;
	
	speed_data.target = 0;
}
