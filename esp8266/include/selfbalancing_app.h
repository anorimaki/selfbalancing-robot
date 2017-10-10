#ifndef _selfbalancing_app_H_
#define _selfbalancing_app_H_

#include "mpu/mpu9250.h"
#include "http/httpserver.h"
#include "motion/motors.h"

namespace selfbalancing
{

class Application
{
private:
	enum State {
		Boot,
		MpuInitiallization,
		MpuChecking,
		MpuError,
		MotorsInitiallization,
		MotorsError,
		Running
	};

public:
	Application(): m_state( Boot ) {}

	void init();
	void loop();


private:
	bool initMotors();
	bool initMpu();
	bool checkMpu();

	void showData( const mpu::MpuData& data );

private:
	State m_state;

	mpu::Mpu9250 m_mpu9250;
	http::Server m_httpServer;
	motion::Motors m_motors;
};






}

//Do not add code below this line
#endif /* _selfbalancing_esp8266_H_ */
