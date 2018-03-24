#ifndef INCLUDE_UTIL_DELAY_H_
#define INCLUDE_UTIL_DELAY_H_

#include <Arduino.h>

inline void noyield_delay_us( unsigned int us ) {
	//delayMicroseconds uses ets_delay_us
	delayMicroseconds(us);
}


inline void noyield_delay_ms( unsigned int ms ) {
	for( int i=0; i<ms; ++i ) {
		noyield_delay_us( 1000 );
	}
}


#endif /* INCLUDE_UTIL_DELAY_H_ */
