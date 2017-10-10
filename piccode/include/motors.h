#ifndef MOTORS_H
#define	MOTORS_H

#include <stdint.h>
#include "pin_manager.h"

//Internal usage. Declared here to avoid inline functions
extern int16_t _motors_left_speed;
extern int16_t _motors_right_speed;


void motors_init();
void motors_set_power( int16_t power );

int16_t motors_right_speed();
int16_t motors_left_speed();

static inline void motors_left_qencoder_interrupt()
{
	if ( IO_RB7_GetValue() )
		_motors_left_speed++;
	else
		_motors_left_speed--;
}


static inline void motors_right_qencoder_interrupt()
{
	if ( IO_RB13_GetValue() )
		_motors_right_speed++;
	else
		_motors_right_speed--;
}


#endif	/* MOTORS_H */
