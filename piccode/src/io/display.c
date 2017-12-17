#include "io/display.h"
#include "tmr3.h"
#include "pin_manager.h"

/*
 * Timer inrement every 16us (256 pre-scaler)
 */

#define DISPLAY_TIMER_PERIOD		16		//In us
#define DISPLAY_TIMER_PERIOD_MS(ms)	(ms*(1000UL/DISPLAY_TIMER_PERIOD))

// 1s -> 6250
static bool display_mpu_busy = false;
static void (*display_callback)() = 0;

inline void display_timer_on( int16_t period, void (*callback)() ) 
{
	display_mpu_busy = true;
	display_callback = callback;
	TMR3_Counter16BitSet(0);
	TMR3_Period16BitSet( period );
	TMR3_Start();
}

inline void display_timer_off() 
{
	TMR3_Stop();
	display_mpu_busy = false;
}

void TMR3_CallBack(void)
{
	display_callback();
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


/*****************************************************************
 * period: 1s
 * state0: l0=on, l1=off
 * state1: l0=off, l1=on
 *****************************************************************/
static void display_system_paused_0() 
{
	led0_Toggle();
	led1_Toggle();
}

void display_system_paused()
{
	led0_SetHigh();
	led1_SetLow();
	display_timer_on( DISPLAY_TIMER_PERIOD_MS(1000), &display_system_paused_0 );
}


/******************************************************************
 * period: 500ms
 * state0: l0=on, l1=on
 * state1: l0=off, l1=off
 ******************************************************************/
static void display_mpu_error_0()
{
	led0_Toggle();
	led1_Toggle();
}

void display_mpu_error()
{
	led0_SetHigh();
	led1_SetHigh();
	display_timer_on( DISPLAY_TIMER_PERIOD_MS(500), &display_mpu_error_0 );
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
static void display_mpu_result_0()
{
	led0_SetLow();
	led1_SetLow();
	display_timer_off();
}

void display_mpu_result( MpuStatus status )
{
	if ( display_mpu_busy ) {
		return;
	}
	if ( status == MPU_OK ) {
		led0_SetLow();
		led1_SetLow();
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
	display_mpu_busy = true;
	display_timer_on( DISPLAY_TIMER_PERIOD_MS(500), &display_mpu_result_0 );
}