#include "io/display.h"
#include "math/util.h"
#include "motors.h"
#include "pitch.h"
#include "speed.h"
#include "heading.h"


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
	current_pitch = SCALE_VALUE( current_pitch, 16, PID_INPUT_BIT_SIZE );
	
	int16_t power = pid_compute( &pitch_data, current_pitch ) ;
		
	return SCALE_VALUE( power, PID_OUTPUT_BIT_SIZE, MOTORS_POWER_BITS );
}

//Adjust steering: power can't override and linear velocity (expressed by
// speed) must be conserved. That is, steering part can be adjusted but
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
	// Restrict pitch to fractional part and conserve sign (first bit of 
	// fractional part). That is, pitch will be restricted to 
	// [-0.5 .. 0.5] rads ([-28.6479 .. 28.6479] degrees).
	// Note: cast to int32_t needed in constant to compare as it (?)
	if ( (pitch > 0x7FFF) || (pitch < (int32_t)0xFFFF8001L) ) {
		robot_fallen();
		return false;
	}
	
		// pitch integer's part and first bit of fractional part are
		// discarded because they are out from allowed pitch range.
	int16_t pitch_fract = pitch & 0x0000FFFF;
			
	int16_t speed_power = compute_speed_power( pitch_fract );

	int16_t adjusted_steering_power = adjust_steering( speed_power, steering_power );
	
	motors_set_left_power( speed_power + adjusted_steering_power );
	motors_set_right_power( speed_power - adjusted_steering_power );
	
	return true;
}


/*
 * Updates pitch target range adjusteded to PID algorithm input.
 */
static void update_pitch_target( int16_t speed )
{
		//Maybe readed speed overflows MOTORS_SPEED_BITS -> Set it in range
		// to avoid averflows when scaling
	if ( speed > MOTORS_MAX_SPEED ) {
		speed = MOTORS_MAX_SPEED;
	}
	if ( speed < MOTORS_MIN_SPEED ) {
		speed = MOTORS_MIN_SPEED;
	}
	
	speed = SCALE_VALUE( speed, MOTORS_SPEED_BITS, PID_INPUT_BIT_SIZE );
	
	int16_t pitch = pid_compute( &speed_data, speed ) ;
	
	//Adjust to PID algorithm input and restrict target to a quarter of max angle
	pitch_data.target = SCALE_VALUE( pitch, PID_OUTPUT_BIT_SIZE,
								BOOST_PP_SUB( PID_INPUT_BIT_SIZE, 3 ) );
	if ( pitch_data.target > 0x700 ) {
		pitch_data.target = 0x700;
	}
	if ( pitch_data.target < -0x700 ) {
		pitch_data.target = -0x700;
	}
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
	update_steering_power( right - left );
}

