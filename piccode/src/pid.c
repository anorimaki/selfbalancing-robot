#include "pid.h"


#define PID_MAX_INTEGRAL_ERROR	(PID_MAX_OUTPUT/PID_MAX_CONSTANT)
#define PID_MIN_INTEGRAL_ERROR	(PID_MIN_OUTPUT/PID_MAX_CONSTANT)


void pid_init( PID* pid )
{
	pid->integral_error = 0;
	pid->previous_error = 0;
}


int16_t pid_compute( PID* pid )
{
	int16_t error = pid->target - pid->current;
	
	int16_t integral_error = pid->integral_error + error;
	if ( integral_error > PID_MAX_INTEGRAL_ERROR )
		integral_error = PID_MAX_INTEGRAL_ERROR;
	if ( integral_error < PID_MIN_INTEGRAL_ERROR )
		integral_error = PID_MIN_INTEGRAL_ERROR;
	pid->integral_error = integral_error;
	
	int16_t derivative_error = error - pid->previous_error;
	pid->previous_error = error;
	
	int32_t out = __builtin_mulss( error,  pid->k_p ) + 
				__builtin_mulss( error, pid->k_i ) + 
				__builtin_mulss( derivative_error, pid->k_d );
	
			//Scale to remove constant factors
	out >>= (PID_CONSTANT_BIT_SIZE-1); 
	
	if ( out > PID_MAX_OUTPUT )
		return PID_MAX_OUTPUT;
	if ( out < PID_MIN_OUTPUT )
		return PID_MIN_OUTPUT;
	
	return out;
}