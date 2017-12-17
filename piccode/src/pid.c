#include "pid.h"
#include "boost/preprocessor/arithmetic/add.hpp"
#include "boost/preprocessor/arithmetic/sub.hpp"
#include "tmr4.h"

//#define PID_MAX_INTEGRAL_ERROR	(PID_MAX_OUTPUT*2L)
//#define PID_MIN_INTEGRAL_ERROR	(PID_MIN_OUTPUT*2L)


void pid_init( PID* pid, PIDStateEntry* store, uint8_t store_size )
{
	pid->store.begin = store;
	pid->store.end = store+store_size;
	pid->store.read_ptr = pid->store.begin;
	pid->store.write_ptr = pid->store.begin;
	pid->store.max_size = store_size-1;	//One of the positions is always
										//been written. read_ptr should 
										//advance before write_ptr reach it.
	
	pid->store.write_ptr->index = TMR4_Counter32BitGet();
	pid->store.write_ptr->state.current = 0;
	pid->store.write_ptr->state.target = 0;
	pid->store.write_ptr->state.integral_error = 0;
	pid->store.write_ptr->state.previous_error = 0;
	pid->store.size = 0;
}


int16_t pid_compute( PID* pid, int16_t target, int16_t current )
{
	PIDStateEntry* current_entry = pid->store.write_ptr;

	int16_t error = target - current;
	
	int16_t integral_error = current_entry->state.integral_error + error;
	if ( (error>0) && 
				(current_entry->state.integral_error>0) && 
				(integral_error<0) )
		integral_error = 0x7FFF;
	else if ( (error<0) && 
				(current_entry->state.integral_error<0) && 
				(integral_error>0) )
		integral_error = 0x8001;
				
	int16_t derivative_error = error - current_entry->state.previous_error;
	
	int32_t proportional_part = __builtin_mulss( error,  pid->settings.k_p );
	int32_t integral_part = __builtin_mulss( integral_error,
											pid->settings.k_i );
	int32_t derivative_part = __builtin_mulss( derivative_error, 
											pid->settings.k_d );

	current_entry->state.current = current;
	current_entry->state.target = target;
	++pid->store.size;
	
	//Update read_ptr if needed
	//Must be done before new entry (next writable entry) is added 
	if ( pid->store.size == pid->store.max_size ) {
		--pid->store.size;		//i2c read can interrupt while writing 
								//in next_write_ptr, so don't lie about size
								//if this happens
		pid->store.read_ptr = pid_next_state_entry( &pid->store,
											 pid->store.read_ptr );
	}

	//Update write_ptr
	PIDStateEntry* next_write_ptr =
						pid_next_state_entry( &pid->store, current_entry );
	next_write_ptr->index = TMR4_Counter32BitGet();
	next_write_ptr->state.previous_error = error;
	next_write_ptr->state.integral_error = integral_error;
	pid->store.write_ptr = next_write_ptr;

	//Finally, sum parts and adjust output
	int32_t out = proportional_part + integral_part + derivative_part;
			//Scale to remove constant factors (-1?)
			//and adapt input bits to output bits
	out = SCALE_VALUE( out,
				BOOST_PP_ADD(PID_INPUT_BIT_SIZE, 
						BOOST_PP_SUB( PID_CONSTANT_BIT_SIZE, 2 ) ),
				PID_OUTPUT_BIT_SIZE );

#if 0
	printf("i: %lu, p: %ld, d: %ld, i: %ld, o: %ld\n", 
		 current_entry->index,
		 proportional_part,
		 derivative_part, integral_part, out );
#endif
	
	if ( out > PID_MAX_OUTPUT )
		return PID_MAX_OUTPUT;
	if ( out < PID_MIN_OUTPUT )
		return PID_MIN_OUTPUT;
	
	return out;
}