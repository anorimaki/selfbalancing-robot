
#ifndef PID_H
#define	PID_H

#include "math/util.h"
#include <stdint.h>

#define PID_INPUT_BIT_SIZE      15      //Limited to 15 bits signed integers to 
                                        // avoid internals overflows
#define PID_MAX_INPUT           ((1 << (PID_INPUT_BIT_SIZE-1))-1)
#define PID_MIN_INPUT           (-PID_MAX_INPUT)

#define PID_OUTPUT_BIT_SIZE     16
#define PID_MAX_OUTPUT          ((1U << (PID_OUTPUT_BIT_SIZE-1))-1)
#define PID_MIN_OUTPUT          (-(signed)PID_MAX_OUTPUT)

#define PID_CONSTANT_BIT_SIZE   8       //Limited to 8 bitssigned integers to 
                                        // avoid internals overflows
#define PID_MAX_CONSTANT        ((1 << (PID_CONSTANT_BIT_SIZE-1))-1)
#define PID_MIN_CONSTANT        (-PID_MAX_CONSTANT)

#define PID_SCALE_INPUT(in, in_bits) \
            SCALE_VALUE( in, in_bits, PID_INPUT_BIT_SIZE )
             

typedef struct 
{
    int8_t k_p;
    int8_t k_d;
    int8_t k_i;
    int16_t previous_error; 
    int32_t integral_error;
    int16_t target;
    int16_t current;
} PID;

void pid_init( PID* pid );
int16_t pid_compute( PID* pid );


#endif	

