/*
 * display.h
 *
 *  Created on: 29 oct. 2017
 *      Author: David
 */

#ifndef INCLUDE_DISPLAY_H_
#define INCLUDE_DISPLAY_H_

namespace io
{

class Display
{
private:
	enum State { StIdle, StMotorsError, StMpuOffsetChanged };

public:
	Display();

	void systemInitialization();
	void systemInitialized();
	void initializingWifi();
	void initializingMpu();

	void mpuInitError();
	void motorsInitError();
	void wifiInitError( int duration_ms );

	void httpRequestBegin();
	void httpRequestEnd();

	void motorsError();

	void mpuOffsetChanged( bool inc );

	/*
	 * Use of os_timer resets ESP8266 randomly. So, call this method from main application loop.
	 */
	void update();

private:
	State m_state;
	unsigned long m_nextChangeTime;
	int m_remainingPeriods;
};

}

#endif /* INCLUDE_DISPLAY_H_ */
