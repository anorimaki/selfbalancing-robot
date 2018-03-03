#include "io/display.h"

extern "C" {
#include "user_interface.h"
}
#include <Arduino.h>

#define LED0_PIN	14
#define LED1_PIN	16

#define INIT_ERROR_PERIOD			200
#define WIFI_INIT_ERROR_PERIOD		200
#define MOTORS_ERROR_DURATION		1000
#define MPU_OFFSET_CHANGED_PERIOD	200

namespace io
{


static void toggle_for_ever( uint8_t pin )
{
	uint8_t val = LOW;
	while( 1 ) {
		digitalWrite(pin, val);
		delay( INIT_ERROR_PERIOD );
		val = (val==LOW) ? HIGH : LOW;
	}
}

/*
 * Initialization sequence:
 * 	- systemInitialization: led0=on, led1=on
 * 	- initializingWifi: led0=toggles in each call, led1=off
 *	- initializingMpu: led1=off, led1=toggles in each call
 *
 * Error conditions in initialization:
 * 	- wifiInitError: led0=toggles each 200ms, led1=toggles each 200ms, led0=led1
 * 	- mpuInitError: led0=off, led1=toggles each 200ms for ever
 * 	- motorsInitError: led0=toggles each 200ms for ever, led1=off
 *
 * Events after initialization:
 *	- HTTP request: led0=on during process, led1=<for error conditions>
 *
 * Error conditions after initialization:
 *	- Motor communication error: led0=<for no error events>, led1=on during 1s
 *
 *
 */

Display::Display(): m_state(StIdle), m_nextChangeTime(0), m_remainingPeriods(0)
{
	pinMode(LED0_PIN, OUTPUT);
	pinMode(LED1_PIN, OUTPUT);
}

void Display::systemInitialization()
{
	digitalWrite(LED0_PIN, LOW);
	digitalWrite(LED1_PIN, LOW);
}

void Display::systemInitialized()
{
	digitalWrite(LED0_PIN, HIGH);
	digitalWrite(LED1_PIN, HIGH);
}

void Display::initializingWifi()
{
	static uint8_t val = LOW;
	digitalWrite(LED0_PIN, val);
	digitalWrite(LED1_PIN, HIGH);
	val = (val==LOW) ? HIGH : LOW;
}

void Display::initializingMpu()
{
	static uint8_t val = LOW;
	digitalWrite(LED0_PIN, HIGH);
	digitalWrite(LED1_PIN, val);
	val = (val==LOW) ? HIGH : LOW;
}

void Display::mpuInitError()
{
	digitalWrite(LED0_PIN, HIGH);
	toggle_for_ever(LED1_PIN);
}

void Display::motorsInitError()
{
	digitalWrite(LED1_PIN, HIGH);
	toggle_for_ever(LED0_PIN);
}

void Display::wifiInitError( int duration_ms )
{
	int loops = (duration_ms/WIFI_INIT_ERROR_PERIOD)+1;
	uint8_t val = LOW;
	for( int i=0; i<loops; ++i ) {
		digitalWrite(LED0_PIN, val);
		digitalWrite(LED1_PIN, val);
		val = (val==LOW) ? HIGH : LOW;
		delay(WIFI_INIT_ERROR_PERIOD);
	}
}


void Display::httpRequestBegin()
{
	digitalWrite(LED0_PIN, LOW);
}


void Display::httpRequestEnd()
{
	digitalWrite(LED0_PIN, HIGH);
}


void Display::motorsError()
{
	digitalWrite(LED1_PIN, LOW);
	m_nextChangeTime = millis() + MOTORS_ERROR_DURATION;
	m_remainingPeriods = 1;
}


void Display::mpuOffsetChanged( bool inc )
{
	digitalWrite(LED1_PIN, LOW);
	m_nextChangeTime = millis() + MPU_OFFSET_CHANGED_PERIOD;
	m_remainingPeriods = inc ? 3 : 1;
}


void Display::update()
{
	if ( m_remainingPeriods == 0 ) {
		m_state=StIdle;
		return;
	}

	unsigned long current = millis();
	if ( current < m_nextChangeTime ) {
		return;
	}

	--m_remainingPeriods;
	digitalWrite( LED1_PIN, (m_remainingPeriods & 0x01) ? LOW : HIGH );

	if ( m_state==StMpuOffsetChanged ) {
		m_nextChangeTime = current + MPU_OFFSET_CHANGED_PERIOD;
	}
}


}





