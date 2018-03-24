#ifndef HTTP_STREAM_RESPONSE_H
#define HTTP_STREAM_RESPONSE_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// arduino defines this macros
#undef min
#undef max

namespace http
{

/*
 * This is equivalent to AsyncStreamResponse but it doesn't support templates and consume less RAM.
 * AsyncStreamResponse derives from AsyncAbstractResponse that uses a send buffer (in AsyncAbstractResponse::_ack)
 * that can be of 2920 bytes (2*TCP_MSS defined in lwipopts.h). This implementation replaces this 3K heap buffer
 * by a 256 bytes stack buffer reducing heap stress. *
 */
class AsyncNoTemplateStreamResponse: public AsyncWebServerResponse {
private:
	String _head;
	Stream* _content;

public:
	AsyncNoTemplateStreamResponse(Stream* stream, const String& contentType, size_t len) {
		_code = 200;
		_content = stream;
		_contentLength = len;
		_contentType = contentType;
	}

	void _respond(AsyncWebServerRequest *request) {
		addHeader("Connection","close");
		_head = _assembleHead(request->version());
		_state = RESPONSE_HEADERS;
		_ack(request, 0, 0);
	}

	size_t _ack(AsyncWebServerRequest *request, size_t len, uint32_t time) {
		if (!_sourceValid()) {
			_state = RESPONSE_FAILED;
			request->client()->close();
			return 0;
		}
		_ackedLength += len;
		size_t space = request->client()->space();

		size_t headLen = _head.length();
		if (_state == RESPONSE_HEADERS) {
			if (space >= headLen) {
				_writtenLength += request->client()->add(_head.c_str(), _head.length());
				_head = String();
				space -= headLen;
				_state = RESPONSE_CONTENT;
			}
			else {
				String out = _head.substring(0, space);
				_head = _head.substring(space);
				_writtenLength += request->client()->write(out.c_str(), out.length());
				return out.length();
			}
		}

		if (_state == RESPONSE_CONTENT) {
			size_t outLen = std::min( _contentLength - _sentLength, space );

			size_t contentToWrite = outLen;
			while ( contentToWrite > 0 ) {
				char buf[256];
				size_t blockSize = std::min( sizeof(buf), contentToWrite );

				for( int i=0; i<blockSize; ++i )
					buf[i]=_content->read();

				blockSize = request->client()->add( buf, blockSize );
				_writtenLength += blockSize;
				_sentLength += blockSize;
				contentToWrite -= blockSize;
			}
			request->client()->send();

			if ( _sentLength == _contentLength ) {
				_state = RESPONSE_WAIT_ACK;
			}
			return outLen + headLen;

		}
		else if (_state == RESPONSE_WAIT_ACK) {
			if ( _ackedLength >= _writtenLength ) {
				_state = RESPONSE_END;
			}
		}
		return 0;
	}

	bool _sourceValid() const {
		return true;
	}
};

}

#endif
