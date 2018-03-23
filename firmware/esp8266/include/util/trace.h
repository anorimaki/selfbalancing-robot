#ifndef INCLUDE_UTIL_TRACE_H_
#define INCLUDE_UTIL_TRACE_H_

#include "Arduino.h"
#include "error.h"

#define SerialPort Serial

namespace trace {

std::string format( const char* format, ... )  __attribute__ ((format (printf, 1, 2)));
std::string format( const __FlashStringHelper* format, ... );

void log( const char* level, const char* file, int line, const char *msg );

}


#define EMPTY()

#define TRACE_ERROR(...) \
		std::string _formated##__LINE__ = trace::format(__VA_ARGS__);						\
		error::globalStack().push( error::StackItem( __FILE__, __LINE__, _formated##__LINE__ ) ); 	\
		::trace::log( "Error", __FILE__, __LINE__, _formated##__LINE__.c_str() );

#define TRACE(...) \
		::trace::log( "Info", __FILE__, __LINE__, trace::format(__VA_ARGS__).c_str() )

#define TRACE_ERROR_MSG_AND_RETURN(msg,v) { \
		TRACE_ERROR(msg); \
		return v; }

#define TRACE_ERROR_AND_RETURN(v) { \
		TRACE_ERROR(""); \
		return v; }


#endif /* INCLUDE_UTIL_TRACE_H_ */
