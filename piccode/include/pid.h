
#ifndef PID_H
#define	PID_H

#include <stdint.h>

#define PID_DATA_BIT_SIZE       14      //Limited to 15 bits signed integers to 
                                        // avoid internals overflows
#define PID_MAX_INPUT           ((1 << PID_DATA_BIT_SIZE)-1)
#define PID_MIN_INPUT           (-PID_MAX_INPUT)
#define PID_MAX_OUTPUT          PID_MAX_INPUT
#define PID_MIN_OUTPUT          (-PID_MAX_OUTPUT)
#define PID_CONSTANT_BIT_SIZE   7       //Limited to 8 bitssigned integers to 
                                        // avoid internals overflows
#define PID_MAX_CONSTANT        ((1 << PID_CONSTANT_BIT_SIZE)-1)
#define PID_MIN_CONSTANT        (-PID_MAX_CONSTANT)

typedef struct 
{
    int8_t k_p;
    int8_t k_d;
    int8_t k_i;
    int16_t previous_error; 
    int16_t integral_error;
    int16_t target;
    int16_t current;
} PID;

void pid_init( PID* pid );
int16_t pid_compute( PID* pid );


#endif	

