#ifndef INCLUDE_UTIL_TRACE_H_
#define INCLUDE_UTIL_TRACE_H_

#include <stdio.h>

#define EMPTY()
#define TRACE_ERROR_MSG_AND_RETURN(msg,v) { \
		printf( "Error (%s: %d): %s\n", __FILE__, __LINE__, msg ); \
		return v; }
#define TRACE_ERROR_AND_RETURN(v) { \
		printf( "Error (%s: %d)\n", __FILE__, __LINE__ ); \
		return v; }


#endif
