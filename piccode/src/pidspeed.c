#include "pidspeed.h"

PID pidspeed_data;

#define PIDSPEED_STORE_SIZE	200

static PIDStateEntry pidspeed_store[PIDSPEED_STORE_SIZE];

void pidspeed_init()
{
	pid_init( &pidspeed_data, pidspeed_store, PIDSPEED_STORE_SIZE );
		
			//Max bits of constants = 15-PID_DATA_BITS to avoid overflows
	pidspeed_data.settings.k_p = 127;
	pidspeed_data.settings.k_i = 2;
	pidspeed_data.settings.k_d = 100;
}
