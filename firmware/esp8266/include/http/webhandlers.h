#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include "io/display.h"
#include "util/trace.h"
#include <ESPAsyncWebServer.h>



template<typename T>
class AsyncMethodWebHandler: public AsyncWebHandler {
public:
	typedef void (T::*RequestHandler)(AsyncWebServerRequest* request);
	typedef void (T::*ObjectBodyHandler)(AsyncWebServerRequest* request, char* body, size_t len);

	class Factory {
	public:
		Factory( T* inst, io::Display* display ): m_inst(inst), m_display(display) {}

		AsyncMethodWebHandler<T>* create( String uri,
				WebRequestMethodComposite method, RequestHandler handler ) {
			return new AsyncMethodWebHandler<T>( m_inst, m_display, uri, method, handler );
		}

		AsyncMethodWebHandler<T>* create( String uri,
				WebRequestMethodComposite method, ObjectBodyHandler handler ) {
			return new AsyncMethodWebHandler<T>( m_inst, m_display, uri, method, handler );
		}

	private:
		T* m_inst;
		io::Display* m_display;
	};

public:
	AsyncMethodWebHandler(T* inst, io::Display* display, String uri,
							WebRequestMethodComposite method, RequestHandler handler) :
			m_inst(inst), m_display(display), m_uri(uri), m_method(method),
			m_onRequest(handler), m_onBody(NULL) {
	}

	AsyncMethodWebHandler(T* inst, io::Display* display, String uri,
							WebRequestMethodComposite method, ObjectBodyHandler handler) :
				m_inst(inst), m_display(display), m_uri(uri), m_method(method),
				m_onRequest(NULL), m_onBody(handler) {
	}

	virtual bool canHandle(AsyncWebServerRequest *request) override final {
		if (!(m_method & request->method()))
			return false;

		if (m_uri.length() && m_uri != request->url() && !request->url().startsWith(m_uri + "/") )
			return false;

		request->addInterestingHeader("ANY");

		return true;
	}

	virtual void handleRequest(AsyncWebServerRequest *request) override final {
		if (!m_onRequest) {
			return;
		}
		m_display->httpRequestBegin();
		(m_inst->*m_onRequest)(request);
		m_display->httpRequestEnd();
	}

	virtual void handleBody( AsyncWebServerRequest *request, uint8_t *data,
							size_t len, size_t index, size_t total ) override final {
		if ( !m_onBody ) {
			return;
		}

		m_display->httpRequestBegin();

		if (index == 0 ) {
			request->setAppData( malloc(total) );
			request->onDisconnect( [request] {
				free( request->appData() );
			});
		}

		char* buffer = request->appData<char>();
		memcpy( buffer+index, data, len );

		if (index + len == total) {
			(m_inst->*m_onBody)( request, buffer, total );
		}

		m_display->httpRequestEnd();
	}

private:
	T* m_inst;
	io::Display* m_display;
	String m_uri;
	RequestHandler m_onRequest;
	ObjectBodyHandler m_onBody;
	WebRequestMethodComposite m_method;
};





#endif
