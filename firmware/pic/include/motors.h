#ifndef MOTORS_H
#define	MOTORS_H

#include <stdint.h>
#include "pin_manager.h"

#define MOTORS_POWER_BITS		16
#define MOTORS_MAX_POWER		((1U << (MOTORS_POWER_BITS-1))-1)
#define MOTORS_MIN_POWER		(-(signed)MOTORS_MAX_POWER)

//Motors generates ~3953 pulses in 1 second at max power (speed = 3953)
// SPEED_BITS is calculated as log2( (3953/1000) * PITCH_CONTROL_PERIOD )
#define MOTORS_SPEED_BITS		12
#define MOTORS_MAX_SPEED		((1 << (MOTORS_SPEED_BITS-1))-1)
#define MOTORS_MIN_SPEED        -MOTORS_MAX_SPEED

//Internal usage. Declared here to permit inline functions
extern int16_t motors_left_speed;
extern int16_t motors_right_speed;

void motors_init();
void motors_set_left_power( int16_t power );
void motors_set_right_power( int16_t power );


static inline void motors_left_qencoder_interrupt()
{
	if ( motors_left_qenc1_GetValue() )
		motors_left_speed--;
	else
		motors_left_speed++;
}


static inline void motors_right_qencoder_interrupt()
{
	if ( motors_right_qenc1_GetValue() )
		motors_right_speed++;
	else
		motors_right_speed--;
}


#endif	/* MOTORS_H */
