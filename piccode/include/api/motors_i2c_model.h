#ifndef MOTORS_I2C_MODEL_H
#define	MOTORS_I2C_MODEL_H

#include <stdint.h>

typedef int16_t PIDConstant;

/*
 *  PID model
 */
typedef struct __attribute((__packed__))
{
	PIDConstant k_p;
	PIDConstant k_d;
	PIDConstant k_i;
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


typedef struct __attribute((__packed__))
{
    int32_t pitch_offset;   //Should be a fix16_t but it's a int32_t
} MpuConfiguration;         // to avoid include q16.h.


#define byte_ptr(base, address) \
    (((uint8_t*)base)+address)


#endif

