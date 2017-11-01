#include "motors.h"
#include "oc1.h"
#include "oc2.h"
#include "pin_manager.h"
#include "math/util.h"
#include <stdlib.h>


#define MOTORS_PWM_BITS		12
#define MOTORS_MAX_POWER	((1<<MOTORS_PWM_BITS)-1)
#define MOTORS_MIN_POWER	0x864		//Min power to move motors

int16_t _motors_left_speed;
int16_t _motors_right_speed;


static inline void motors_left_fordward()
{
	motors_left_dir0_SetHigh();
	motors_left_dir1_SetLow();
}

static inline void motors_left_backwards()
{
	motors_left_dir0_SetLow();
	motors_left_dir1_SetHigh();
}

static inline void motors_right_fordward()
{
	motors_right_dir0_SetHigh();
	motors_right_dir1_SetLow();
}

static inline void motors_right_backwards()
{
	motors_right_dir0_SetLow();
	motors_right_dir1_SetHigh();
}

static inline void motors_stop()
{
	motors_left_dir0_SetLow();
	motors_left_dir1_SetLow();
	motors_right_dir0_SetLow();
	motors_right_dir1_SetLow();
}


void motors_init()
{
	motors_stop();
	_motors_right_speed = 0;
	_motors_left_speed = 0;
}

void motors_set_power( int16_t power )
{
	if ( power > 0 ) {
		motors_left_fordward();
		motors_right_fordward();
	}
	else {
		motors_left_backwards();
		motors_right_backwards();
	}
	uint16_t mag = abs( power );
	mag = SCALE_VALUE( mag, 15, MOTORS_PWM_BITS );
	mag += MOTORS_MIN_POWER;		//Add minimun power to move motors
	if ( mag > MOTORS_MAX_POWER )
		mag = MOTORS_MAX_POWER;
	
	OC1_SecondaryValueSet( mag );
	OC2_SecondaryValueSet( mag );
}


int16_t motors_right_speed()
{
	int16_t ret = _motors_right_speed;
	_motors_right_speed = 0;
	return ret;
}


int16_t motors_left_speed()
{
	int16_t ret = _motors_left_speed;
	_motors_left_speed = 0;
	return ret;
}