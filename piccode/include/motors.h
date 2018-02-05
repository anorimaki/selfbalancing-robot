#ifndef MOTORS_H
#define	MOTORS_H

#include <stdint.h>
#include "pin_manager.h"

//Internal usage. Declared here to permit inline functions
extern int16_t motors_left_speed;
extern int16_t motors_right_speed;


//Motors generates ~3953 pulses in 1 second at max power (speed = 3953)
// SPEED_BITS is calculated as log2( (3953/1000) * PITCH_CONTROL_PERIOD )
#define MOTORS_SPEED_BITS				11	


void motors_init();
void motors_set_power( int16_t power );

int16_t motors_speed();     //Returns linear velocity of vehicle (left+right)

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
