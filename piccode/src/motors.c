#include "motors.h"
#include "oc1.h"
#include "oc2.h"
#include "pin_manager.h"
#include <stdlib.h>

static inline void m0_fordward()
{
	IO_RA0_SetHigh();
	IO_RA1_SetLow();
}

static inline void m0_backwards()
{
	IO_RA0_SetLow();
	IO_RA1_SetHigh();
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
	IO_RA1_SetLow();
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
	power = abs( power );
	OC1_SecondaryValueSet( power );
	OC2_SecondaryValueSet( power );
}