#include "io/display.h"
#include "math/util.h"
#include "motors.h"
#include "pitch.h"
#include "speed.h"
#include "heading.h"

//In Q16 format and RADs, it is equivalent to [-14.2º .. +14.2º] range
#define MAX_PITCH_ANGLE_BITS	15		//In signed fomat
#define MAX_PITCH_ANGLE         ((1 << (MAX_PITCH_ANGLE_BITS-1))-1)	//Q16 format
#define MIN_PITCH_ANGLE			(-MAX_PITCH_ANGLE)					//Q16 format

static int16_t steering_power;


void ctrl_init()
{
	steering_power = 0;
	
	pitch_init();
	speed_init();
	heading_init();
	motors_init();
}


static void robot_fallen() 
{
	pid_clear( &pitch_data );
	pid_clear( &speed_data );
	pid_clear( &heading_data );
	steering_power = 0;
	motors_set_left_power(0);
	motors_set_right_power(0);
}


static int16_t compute_speed_power( int16_t current_pitch )
{
	current_pitch = SCALE_VALUE( current_pitch, MAX_PITCH_ANGLE_BITS,
								PID_INPUT_BIT_SIZE );
	
	int16_t power = pid_compute( &pitch_data, current_pitch ) ;
		
	return SCALE_VALUE( power, PID_OUTPUT_BIT_SIZE, MOTORS_POWER_BITS );
}

//Adjust steering: power can't override and linear velocity (expressed by
// speed) must be conserved. That is, steering part can be ajusted but
// speed must be conserved.
static int16_t adjust_steering( int16_t speed, int16_t steering ) 
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

/*
 * return false if motors can't be updated.
 */
bool ctrl_update_motors_power( fix16_t pitch )
{
		//Limit angle range to:
		//	[-14.2 .. +14.2] degrees
	if ( (pitch > MAX_PITCH_ANGLE) || (pitch < MIN_PITCH_ANGLE) ) {
		robot_fallen();
		return false;
	}
				//current_pitch integer's part is discarded
				// because it's out from allowed pitch range.
	int16_t speed_power = compute_speed_power( pitch );

	int16_t adjusted_steering_power = adjust_steering( speed_power, steering_power );
	
	motors_set_left_power( speed_power - adjusted_steering_power );
	motors_set_right_power( speed_power + adjusted_steering_power );
	
	return true;
}


/*
 * Updates pitch target range adjusteded to PID algorithm input.
 */
static void update_pitch_target( int16_t speed )
{
	speed = SCALE_VALUE( speed, MOTORS_SPEED_BITS, PID_INPUT_BIT_SIZE );
	
	int16_t pitch = pid_compute( &speed_data, speed ) ;
	
	//Adjust to PID algorithm input and restrict target to the half of max angle
	pitch_data.target = SCALE_VALUE( pitch, PID_OUTPUT_BIT_SIZE,
								BOOST_PP_SUB( PID_INPUT_BIT_SIZE, 1 ) );
}


static void update_steering_power( int16_t heading )
{
	heading = SCALE_VALUE( heading, MOTORS_SPEED_BITS, PID_INPUT_BIT_SIZE );
	int16_t steering = pid_compute( &heading_data, heading ) ;
	steering_power = SCALE_VALUE( steering, PID_OUTPUT_BIT_SIZE, MOTORS_POWER_BITS );
}


void ctrl_update_pitch_target_and_steering()
{
	int16_t right = motors_right_speed;
	motors_right_speed = 0;
	int16_t left = motors_left_speed;
	motors_left_speed = 0;
	
	update_pitch_target( right + left );
	update_steering_power( left - right );
}

