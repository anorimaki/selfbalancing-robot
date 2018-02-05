#include "motors.h"
#include "heading.h"
#include "pid.h"
#include "oc1.h"
#include "oc2.h"
#include "pin_manager.h"
#include "math/util.h"
#include <stdlib.h>


#define MOTORS_PWM_BITS			12
#define MOTORS_PWM_MAX_POWER	((1<<MOTORS_PWM_BITS)-1)

#define MOTORS_POWER_BITS		16
#define MOTORS_MAX_POWER		((1U << (MOTORS_POWER_BITS-1))-1)
#define MOTORS_MIN_POWER		(-(signed)MOTORS_MAX_POWER)

			//Min power to move motors
#define MOTORS_LEFT_MIN_POWER	0x734		//Rueda al lado de los conectores
#define MOTORS_RIGHT_MIN_POWER	0x734		//Min power to move motors

int16_t motors_left_speed;
int16_t motors_right_speed;
static int16_t motors_heading;


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
	motors_right_speed = 0;
	motors_left_speed = 0;
	motors_heading = 0;
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
#if 1
#define power_to_pwm(power, minPower) \
	(power == 0) ? 0 :	\
	(minPower + 1 +		\
		(__builtin_muluu( abs(power), MOTORS_PWM_MAX_POWER-minPower) >> 15))
#else
#define power_to_pwm(power, minPower) \
	(minPower)
#endif

static inline void motors_set_left_power( int16_t power ) 
{
	if ( power > 0 ) {
		motors_left_fordward();
	}
	else {
		motors_left_backwards();
	}
	
	uint16_t pwm = power_to_pwm(power, MOTORS_LEFT_MIN_POWER);
	OC2_SecondaryValueSet( pwm );
}


static inline void motors_set_right_power( int16_t power ) 
{
	if ( power > 0 ) {
		motors_right_fordward();
	}
	else {
		motors_right_backwards();
	}
	uint16_t pwm = power_to_pwm(power, MOTORS_RIGHT_MIN_POWER);
	OC1_SecondaryValueSet( pwm );
}


static int16_t motors_calculate_pid_steering()
{
	int16_t scaled_heading = 
			SCALE_VALUE( motors_heading, MOTORS_SPEED_BITS, PID_INPUT_BIT_SIZE );
	int16_t steering = pid_compute( &heading_data, scaled_heading ) ;
	return SCALE_VALUE( steering, PID_OUTPUT_BIT_SIZE, 16 );
}


//Adjust steering: power can't override and linear velocity (expressed by
// speed_power) must be conserved.
static int16_t motors_adjust_steering( int16_t speed, int16_t steering ) 
{
	if ( speed > 0 ) {
		if ( steering > 0 ) {	//take care about speed + steering
			return min( MOTORS_MAX_POWER - speed, steering );
		}
			////take care about speed - steering
		return max( MOTORS_MIN_POWER + speed, steering );
	}
	
	if ( steering > 0 ) {	//take care about speed - steering
		return min( MOTORS_MAX_POWER + speed, steering );
	}
				//take care about speed + steering
	return max( MOTORS_MIN_POWER - speed, steering );
}


void motors_set_power( int16_t speed_power )
{
	int16_t steering_power = motors_calculate_pid_steering();
	steering_power = motors_adjust_steering( speed_power, steering_power );
	
#if 0
	printf( "ster: %d\n", steering_power );
#endif	
	
	motors_set_left_power( speed_power - steering_power );
	motors_set_right_power( speed_power + steering_power );
}


int16_t motors_speed()
{
	int16_t right = motors_right_speed;
	motors_right_speed = 0;
	int16_t left = motors_left_speed;
	motors_left_speed = 0;
	
	motors_heading = right - left;
	
	return right + left;
}

