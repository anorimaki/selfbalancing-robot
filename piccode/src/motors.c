#include "motors.h"
#include "oc1.h"
#include "oc2.h"
#include "pin_manager.h"
#include "math/util.h"
#include <stdlib.h>


#define MOTORS_PWM_BITS		12
#define MOTORS_MAX_POWER	((1<<MOTORS_PWM_BITS)-1)
#define MOTORS_LEFT_MIN_POWER	0x864		//Min power to move motors
#define MOTORS_RIGHT_MIN_POWER	0x864		//Min power to move motors

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


/*
 * Power input magnitude is a 15 bits unsigned integer. Output must be scaled
 * in [minPower .. 2^MOTORS_PWM_BITS-1] range.
 * 
 * Correct scale function is: 
 *		minPower + (abs(power)*(MOTORS_MAX_POWER-minPower) / ((2^15)-1))
 * But this function approximates div by (2^15)-1 with this formula to
 * avoid divisions:
 *			1/2^n + 1/2^2n + 1/2^3n + ... = 1/(2^n-1)
 * See https://stackoverflow.com/questions/33063691/how-does-this-approximation-of-division-using-bit-shift-operations-work* 
 * In this case, only make sense an aprox. with one term because second term is 
 * always 0 (mult of 12x15 bits gives 27 bits that would be shifted 30 bits)
 * So:
 *		minPower + (abs(power)*(MOTORS_MAX_POWER-minPower) / (2^15)) + 1
 * 
 * Note: Defined as macro instead of a function in order to do some
 * calculations at compile time.
 */
#define power_to_pwm(power, minPower) \
	(minPower + 1 + \
		(__builtin_muluu( abs(power), MOTORS_MAX_POWER-minPower) >> 15))

static inline void motors_set_left_power( int16_t power ) 
{
	if ( power > 0 ) {
		motors_left_fordward();
	}
	else {
		motors_left_backwards();
	}

	OC2_SecondaryValueSet( power_to_pwm(power, MOTORS_LEFT_MIN_POWER) );
}


static inline void motors_set_right_power( int16_t power ) 
{
	if ( power > 0 ) {
		motors_right_fordward();
	}
	else {
		motors_right_backwards();
	}
	OC1_SecondaryValueSet( power_to_pwm(power, MOTORS_RIGHT_MIN_POWER) );
}


void motors_set_power( int16_t power, int8_t steering )
{
	steering >>= 1;		//Half of desired steering to each motor.
	motors_set_left_power( power + steering );
	motors_set_right_power( power - steering );
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