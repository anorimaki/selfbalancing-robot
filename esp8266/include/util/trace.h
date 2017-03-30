/*
 * trace.h
 *
 *  Created on: 11 mar. 2017
 *      Author: David
 */

#ifndef INCLUDE_UTIL_TRACE_H_
#define INCLUDE_UTIL_TRACE_H_

#include "Arduino.h"

#define EMPTY()
#define TRACE_ERROR_MSG_AND_RETURN(msg,v) { \
		Serial.printf( "Error (%s: %d): %s\n", __FILE__, __LINE__, msg ); \
		return v; }
#define TRACE_ERROR_AND_RETURN(v) { \
		Serial.printf( "Error (%s: %d)\n", __FILE__, __LINE__ ); \
		return v; }



#endif /* INCLUDE_UTIL_TRACE_H_ */
