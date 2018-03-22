#include "util/trace.h"
#include "pgmspace.h"
#include <stdio.h>
#include <stdarg.h>

namespace trace
{

std::string format( const char * format, ... )
{
	va_list arg;
	va_start(arg, format);
	char msg[128];
	vsnprintf(msg, sizeof(msg), format, arg);
	va_end(arg);

	return msg;
}

std::string format( const __FlashStringHelper* format, ... )
{
	va_list arg;
	va_start(arg, format);
	char msg[128];
	vsnprintf_P(msg, sizeof(msg), reinterpret_cast<PGM_P>(format), arg);
	va_end(arg);

	return msg;
}


void log( const char* level, const char* file, int line, const char* msg )
{
	Serial.printf( "%s (%s: %d): %s\n", level, file, line, msg );
}


}
