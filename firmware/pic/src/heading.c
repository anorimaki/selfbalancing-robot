#include "heading.h"

PID heading_data;

#define HEADING_PID_STORE_SIZE	150

static PIDStateEntry heading_pid_store[HEADING_PID_STORE_SIZE];

void heading_init()
{
	pid_init( &heading_data, heading_pid_store, HEADING_PID_STORE_SIZE );
		
			//Max bits of constants = 15-PID_DATA_BITS to avoid overflows
	heading_data.settings.k_p = 200;
	heading_data.settings.k_i = 0;
	heading_data.settings.k_d = 0;
	
	heading_data.target = 0;
}