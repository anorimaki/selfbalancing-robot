#include "motors.h"
#include "oc1.h"
#include "oc2.h"
#include "pin_manager.h"
#include "math/util.h"
#include <stdlib.h>


#define MOTORS_PWM_BITS		12
#define MOTORS_MAX_POWER	((1<<MOTORS_PWM_BITS)-1)
#define MOTORS_MIN_POWER	0x864		//Min power to move motors


static inline void m0_fordward()
{
	IO_RA0_SetHigh();
	IO_RA4_SetLow();
}

static inline void m0_backwards()
{
	IO_RA0_SetLow();
	IO_RA4_SetHigh();
}

static inline void m1_fordward()
{
	IO_RA2_SetHigh();
	IO_RA3_SetLow();
}

static inline void m1_backwards()
{
	IO_RA2_SetLow();
	IO_RA3_SetHigh();
}

static inline void motors_stop()
{
	IO_RA0_SetLow();
	IO_RA4_SetLow();
	IO_RA2_SetLow();
	IO_RA3_SetLow();
}


void motors_init()
{
	motors_stop();
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
	mag = SCALE_VALUE( mag, 16, MOTORS_PWM_BITS );
	mag += MOTORS_MIN_POWER;		//Add minimun power to move motors
	if ( mag > MOTORS_MAX_POWER )
		mag = MOTORS_MAX_POWER;
	
	OC1_SecondaryValueSet( mag );
	OC2_SecondaryValueSet( mag );
}