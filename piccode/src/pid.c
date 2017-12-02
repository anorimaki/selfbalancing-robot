#include "pid.h"
#include "boost/preprocessor/arithmetic/add.hpp"
#include "boost/preprocessor/arithmetic/sub.hpp"

//#define PID_MAX_INTEGRAL_ERROR	(PID_MAX_OUTPUT*2L)
//#define PID_MIN_INTEGRAL_ERROR	(PID_MIN_OUTPUT*2L)


void pid_init( PID* pid, PIDStateEntry* store, uint8_t store_size )
{
	pid->store.begin = store;
	pid->store.end = store+store_size;
	pid->store.read_ptr = pid->store.begin;
	pid->store.write_ptr = pid->store.begin;
	pid->store.max_size = store_size;
	
	pid->store.write_ptr->index = 0;
	pid->store.write_ptr->state.current = 0;
	pid->store.write_ptr->state.target = 0;
	pid->store.write_ptr->state.integral_error = 0;
	pid->store.write_ptr->state.previous_error = 0;
	pid->store.size = 1;
}


int16_t pid_compute( PID* pid, int16_t target, int16_t current )
{
	PIDStateEntry* current_entry = pid->store.write_ptr;

	int16_t error = target - current;
	
	int16_t integral_error = current_entry->state.integral_error + error;
	if ( (error>0) && (current_entry->state.integral_error>0) && 
		(integral_error<0) )
		integral_error = 0x7FFF;
	else if ( (error<0) && (current_entry->state.integral_error<0) && 
		(integral_error>0) )
		integral_error = 0x8001;
				
	int16_t derivative_error = error - current_entry->state.previous_error;
	
	int32_t out = __builtin_mulss( error,  pid->settings.k_p ) + 
				__builtin_mulss( integral_error,  pid->settings.k_i ) + 
				__builtin_mulss( derivative_error, pid->settings.k_d );

	current_entry->state.current = current;
	current_entry->state.target = target;
	++pid->store.size;
	
	//Update read_ptr if needed
	//Must be done before new enrty (next writable entry) is added 
	if ( pid->store.size == pid->store.max_size ) {
		--pid->store.size;		//i2c read can interrupt while writing 
								//in next_write_ptr, so don't lie about size
								//if this happens
		pid->store.read_ptr = pid_next_state_entry( &pid->store,
											 pid->store.read_ptr );
	}

	PIDStateEntry* next_write_ptr =
						pid_next_state_entry( &pid->store, current_entry );
	next_write_ptr->index = current_entry->index+1;
	next_write_ptr->state.previous_error = error;
	next_write_ptr->state.integral_error = integral_error;
	pid->store.write_ptr = next_write_ptr;
	
			//Scale to remove constant factors (-1?)
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