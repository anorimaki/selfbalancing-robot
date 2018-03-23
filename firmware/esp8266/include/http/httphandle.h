#ifndef INCLUDE_HTTP_HANDLE_H_
#define INCLUDE_HTTP_HANDLE_H_

#include "io/display.h"
#include "util/trace.h"
#include <ESP8266WebServer.h>
#include <functional>


template<typename T>
class MethodHttpHandlerFactory {
public:
	typedef void (T::*MethodHandler)();

public:
	MethodHttpHandlerFactory(T* inst, io::Display* display) :
			m_inst(inst), m_display(display) {
	}

	ESP8266WebServer::THandlerFunction operator()( MethodHandler handler ) {
		return std::bind(  &handleRequest, m_display, m_inst, handler );
	}

private:
	static void handleRequest( io::Display* display, T* inst, MethodHandler handler ) {
		display->httpRequestBegin();
		(inst->*handler)();
		display->httpRequestEnd();
	}

private:
	T* m_inst;
	io::Display* m_display;
};


#endif
