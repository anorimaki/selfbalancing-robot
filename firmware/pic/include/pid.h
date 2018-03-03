#ifndef PID_H
#define	PID_H

#include "math/util.h"
#include "api/motors_i2c_model.h"
#include "api/motors_i2c_reg.h"
#include <stdint.h>

#define PID_INPUT_BIT_SIZE      15      //Limited to 
                                        // avoid internals overflows
#define PID_MAX_INPUT           ((1 << (PID_INPUT_BIT_SIZE-1))-1)
#define PID_MIN_INPUT           (-PID_MAX_INPUT)

#define PID_OUTPUT_BIT_SIZE     16
#define PID_MAX_OUTPUT          ((1U << (PID_OUTPUT_BIT_SIZE-1))-1)
#define PID_MIN_OUTPUT          (-(signed)PID_MAX_OUTPUT)

#define PID_CONSTANT_BIT_SIZE   13       //Limited to 
                                        // avoid internals overflows
#define PID_MAX_CONSTANT        ((1 << (PID_CONSTANT_BIT_SIZE-1))-1)
#define PID_MIN_CONSTANT        (-PID_MAX_CONSTANT)

#define PID_SCALE_INPUT(in, in_bits) \
            SCALE_VALUE( in, in_bits, PID_INPUT_BIT_SIZE )

typedef struct {
    PIDStateEntry* begin;
    PIDStateEntry* end;
    PIDStateEntry* read_ptr;
    PIDStateEntry* write_ptr;
    uint8_t max_size;
    uint8_t size;       //Important: This is the first field of PID structure
                        //that is addressed as raw memory from I2C interface
                        //It must take an odd address to respect PIC24
                        //aligment.
                        //PIDSettings.k_p (the next field addressed by I2C)
                        //is a 16 bit integer so it starts at even address.
                        //Not very robust strategy for I2C interface but it
                        //is fast.
} PIDStore;

typedef struct __attribute((__packed__)) {
    PIDStore store;
    PIDSettings settings;
    int16_t target;
} PID;


void pid_init( PID* pid, PIDStateEntry* store, uint8_t store_size );
void pid_clear( PID* pid );
int16_t pid_compute( PID* pid, int16_t current );


static inline PIDStateEntry* pid_next_state_entry( PIDStore* pid_store,
											  PIDStateEntry* current )
{
	++current;
	return (current==pid_store->end) ? pid_store->begin : current;
}


static inline uint8_t pid_i2c_read( PID* pid, uint8_t address )
{
    if ( address < sizeof(PIDStateEntry) ) {
        uint8_t ret = *byte_ptr(pid->store.read_ptr, address);
        if ( address == (sizeof(PIDStateEntry)-1) ) {
            --pid->store.size;
             pid->store.read_ptr = pid_next_state_entry( &pid->store,
                                                        pid->store.read_ptr );
        }
        return ret;
    }
    // store.size, settings and taget are mappeable from pid
    return *byte_ptr(&pid->store.size, address-sizeof(PIDStateEntry));
}

static inline void pid_i2c_write( PID* pid, uint8_t address, uint8_t value )
{  //Only PID settings and target are writable: skip FIFO (entry and store.size)
    if ( address >= sizeof(PIDFifo) ) {
        *byte_ptr(&pid->settings, address-sizeof(PIDFifo)) = value;
    }
}

#endif	

