#ifndef _selfbalancing_app_H_
#define _selfbalancing_app_H_

#include "mpu/mpu9250.h"
#include "http/httpserver.h"
#include "motion/motors.h"
#include "io/display.h"

namespace selfbalancing
{

class Application
{
public:
	Application(): m_httpServer(NULL) {}

	void init();
	void loop();


private:
	void initMotors();
	bool mpuInitialization();
	bool initMpu();
	bool initWifi();
	bool checkMpu();

	void doInput();
	bool changeMpuOffset( bool inc );

	void showData( const mpu::MpuData& data );

private:
	mpu::Mpu9250 m_mpu9250;
	http::Server* m_httpServer;
	motion::Motors m_motors;
	io::Display m_display;
};






}

//Do not add code below this line
#endif /* _selfbalancing_esp8266_H_ */
