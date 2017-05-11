#include "pid.h"
#include "boost/preprocessor/arithmetic/add.hpp"
#include "boost/preprocessor/arithmetic/sub.hpp"

#define PID_MAX_INTEGRAL_ERROR	(PID_MAX_OUTPUT*14L)
#define PID_MIN_INTEGRAL_ERROR	(PID_MIN_OUTPUT*14L)


void pid_init( PID* pid )
{
	pid->integral_error = 0;
	pid->previous_error = 0;
}


int16_t pid_compute( PID* pid )
{
	int16_t error = pid->target - pid->current;
	
	pid->integral_error += __builtin_mulss( error, pid->k_i );
	if ( pid->integral_error > PID_MAX_INTEGRAL_ERROR )
		pid->integral_error = PID_MAX_INTEGRAL_ERROR;
	if ( pid->integral_error < PID_MIN_INTEGRAL_ERROR )
		pid->integral_error = PID_MIN_INTEGRAL_ERROR;
	
	int16_t derivative_error = error - pid->previous_error;
	pid->previous_error = error;
	
	int32_t out = __builtin_mulss( error,  pid->k_p ) + 
				pid->integral_error + 
				__builtin_mulss( derivative_error, pid->k_d );
	
			//Scale to remove constant factors 
			//and adapt input bits to output bits
	out = SCALE_VALUE( out,
				BOOST_PP_ADD(PID_INPUT_BIT_SIZE, 
						BOOST_PP_SUB( PID_CONSTANT_BIT_SIZE, 1 ) ),
				PID_OUTPUT_BIT_SIZE );
	
	if ( out > PID_MAX_OUTPUT )
		return PID_MAX_OUTPUT;
	if ( out < PID_MIN_OUTPUT )
		return PID_MIN_OUTPUT;
	
	return out;
}