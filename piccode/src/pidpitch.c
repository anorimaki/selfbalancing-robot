#include "pidpitch.h"

PID pidpitch_data;

#define PIDPITCH_STORE_SIZE	200

static PIDStateEntry pidpitch_store[PIDPITCH_STORE_SIZE];

void pidpitch_init()
{
	pid_init( &pidpitch_data, pidpitch_store, PIDPITCH_STORE_SIZE );
		
			//Max bits of constants = 15-PID_DATA_BITS to avoid overflows
	pidpitch_data.settings.k_p = 127;
	pidpitch_data.settings.k_i = 2;
	pidpitch_data.settings.k_d = 100;
}