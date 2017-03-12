/*
 * trace.h
 *
 *  Created on: 11 mar. 2017
 *      Author: David
 */

#ifndef INCLUDE_UTIL_TRACE_H_
#define INCLUDE_UTIL_TRACE_H_

#include "Arduino.h"


#define EMPTY
#define TRACE() \
		Serial.printf( "%s: %d\n", __FILE__, __LINE__ );

#define RETURN_ERROR(a) { \
		Serial.printf( "Error (%s: %d)\n", __FILE__, __LINE__ ); \
		return a; }


#endif /* INCLUDE_UTIL_TRACE_H_ */
