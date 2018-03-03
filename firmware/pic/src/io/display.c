#include "io/display.h"
#include "tmr3.h"
#include "pin_manager.h"

/*
 * Timer inrement every 16us (256 pre-scaler)
 */

#define DISPLAY_TIMER_PERIOD		16		//In us
#define DISPLAY_TIMER_PERIOD_MS(ms)	(ms*(1000UL/DISPLAY_TIMER_PERIOD))

// 1s -> 6250
static bool display_busy = false;
static void (*display_callback)() = 0;

inline void display_timer_on( int16_t period, void (*callback)() ) 
{
	display_busy = true;
	display_callback = callback;
	TMR3_Counter16BitSet(0);
	TMR3_Period16BitSet( period );
	TMR3_Start();
}

inline void display_timer_off() 
{
	TMR3_Stop();
	display_busy = false;
}

void TMR3_CallBack(void)
{
	display_callback();
}

static void display_toggle_all() 
{
	led0_Toggle();
	led1_Toggle();
}


/******************************************************************
 * leds on
 ******************************************************************/
void display_system_initialization()
{
	display_timer_off();
	led0_SetHigh();
	led1_SetHigh();
}

/******************************************************************
 * - leds on
 * - wait 1s
 * - leds off
 ******************************************************************/
void display_system_running()
{
	display_timer_off();
	led0_SetLow();
	led1_SetLow();
}


/******************************************************************
 * - leds on
 * - wait 1s
 * - leds off
 ******************************************************************/
void display_system_starting()
{
	led0_SetHigh();
	led1_SetLow();
	display_timer_on( DISPLAY_TIMER_PERIOD_MS(200), &display_toggle_all );
}


/*****************************************************************
 * period: 1s
 * state0: l0=on, l1=off
 * state1: l0=off, l1=on
 *****************************************************************/
void display_system_paused()
{
	led0_SetHigh();
	led1_SetLow();
	display_timer_on( DISPLAY_TIMER_PERIOD_MS(1000), &display_toggle_all );
}


/******************************************************************
 * period: 500ms
 * state0: l0=on, l1=on
 * state1: l0=off, l1=off
 ******************************************************************/
void display_mpu_error()
{
	led0_SetHigh();
	led1_SetHigh();
	display_timer_on( DISPLAY_TIMER_PERIOD_MS(500), &display_toggle_all );
}


/******************************************************************
 * MPU_OK:
 *	l0: off, l1=off
 * MPU_OVERUN:
 *	l0: on, l1=off
 * MPU_DATA_CORRUPTION:
 *	l0: off, l1=on
 * MPU_ERROR:
 *	l0: on, l1=on
 ******************************************************************/
void display_mpu_result( MpuStatus status )
{
	if ( display_busy ) {
		return;
	}
	if ( status == MPU_OK ) {
		led0_SetLow();
		led1_SetLow();
		display_busy = false;
		return;
	}
	else if ( status == MPU_OVERUN ) {
		led0_SetHigh();
		led1_SetLow();
	}
	else if ( status == MPU_DATA_CORRUPTION ) {
		led0_SetLow();
		led1_SetHigh();
	}
	else if ( status == MPU_ERROR ) {
		led0_SetHigh();
		led1_SetHigh();
	}
	display_busy = true;
	display_timer_on( DISPLAY_TIMER_PERIOD_MS(500), &display_system_running );
}


void display_motor_control_overtime() 
{
	if ( display_busy ) {
		return;
	}
	led0_SetLow();
	led1_SetHigh();
	
	display_busy = true;
	display_timer_on( DISPLAY_TIMER_PERIOD_MS(500), &display_system_running );
}