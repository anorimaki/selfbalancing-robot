#ifndef MOTORS_I2C_MODEL_H
#define	MOTORS_I2C_MODEL_H

#include <stdint.h>
/*
 *  PID model
 */
typedef struct __attribute((__packed__))
{
    int8_t k_p;
    int8_t k_d;
    int8_t k_i;
} PIDSettings;

typedef struct __attribute((__packed__))
{
    int16_t previous_error; 
    int32_t integral_error;
    int16_t target;
    int16_t current;
} PIDState;

typedef struct __attribute((__packed__))
{
    uint32_t index; 
    PIDState state;
} PIDStateEntry;

typedef struct __attribute((__packed__))
{
    PIDStateEntry current; 
    uint8_t size;       //Entries left in store
} PIDFifo;


#endif

