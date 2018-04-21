#include "pitch.h"

PID pitch_data;

#define PITCH_PID_STORE_SIZE	200

static PIDStateEntry pitch_pid_store[PITCH_PID_STORE_SIZE];

void pitch_init()
{
	pid_init( &pitch_data, pitch_pid_store, PITCH_PID_STORE_SIZE );
		
			//Max bits of constants = 15-PID_DATA_BITS to avoid overflows
	pitch_data.settings.k_p = 2500;
	pitch_data.settings.k_i = 61;
	pitch_data.settings.k_d = 7000;
	
	pitch_data.target = 0;
}
