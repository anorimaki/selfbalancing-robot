#ifndef INCLUDE_HTTP_HTTPSERVER_H_
#define INCLUDE_HTTP_HTTPSERVER_H_

#include <ESPAsyncWebServer.h>
#include "motion/motors.h"
#include "mpu/mpu9250.h"

namespace http
{


class PidService
{
public:
	PidService( AsyncWebServer* impl, const std::string& path,
				motion::PidEngine* pidEngine, io::Display* display );

private:
	void handleState( AsyncWebServerRequest *request );
	void handleSettings( AsyncWebServerRequest *request );
	void handleSetSettings( AsyncWebServerRequest* request, char* data, size_t len );

private:
	AsyncWebServer* m_impl;
	motion::PidEngine* m_pidEngine;
	io::Display* m_display;
};


class Server
{
private:
	class AsyncWebServerImpl: public AsyncWebServer {
	public:
		AsyncWebServerImpl( uint16_t port ): AsyncWebServer(port) {}

		void begin() {
			_server.setNoDelay(true);
			AsyncWebServer::begin();
		}
	};

public:
	Server( motion::Motors* motors, mpu::Mpu9250* m_mpu, io::Display* display );

	AsyncWebServer& impl() {
		return m_impl;
	}

private:
	void handleOptionsRequest( AsyncWebServerRequest *request );
	void handleDefault( AsyncWebServerRequest *request );

	void handleGetMpuSettings( AsyncWebServerRequest *request );
	void handlePutMpuSettings( AsyncWebServerRequest* request, char* data, size_t len );

	void handleGetMpuCalibration( AsyncWebServerRequest *request );
	void handlePutMpuCalibration( AsyncWebServerRequest* request );

	void handlePutTargets( AsyncWebServerRequest* request, char* data, size_t len );

private:
	AsyncWebServerImpl m_impl;
	motion::Motors* m_motors;
	io::Display* m_display;
	mpu::Mpu9250* m_mpu;
	PidService* m_speedService;
	PidService* m_pitchService;
	PidService* m_headingService;
};

}



#endif /* INCLUDE_HTTP_HTTPSERVER_H_ */
