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


static inline void m0_fordward()
{
	IO_RB11_SetHigh();
	IO_RB12_SetLow();
}

static inline void m0_backwards()
{
	IO_RB11_SetLow();
	IO_RB12_SetHigh();
}

static inline void m1_fordward()
{
	IO_RA3_SetHigh();
	IO_RA4_SetLow();
}

static inline void m1_backwards()
{
	IO_RA3_SetLow();
	IO_RA4_SetHigh();
}

static inline void motors_stop()
{
	IO_RB11_SetLow();
	IO_RB12_SetLow();
	IO_RA3_SetLow();
	IO_RA4_SetLow();
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
		m0_fordward();
		m1_fordward();
	}
	else {
		m0_backwards();
		m1_backwards();
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