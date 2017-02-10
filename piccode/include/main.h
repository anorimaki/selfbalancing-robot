#ifndef MAIN_H
#define	MAIN_H


/*******************************************************/
// TARGET HW
/*******************************************************/
#define USE_PIC24FJ32GA002
//#define USE_PIC24FV16KM202

#include "picconfig.h"

/*******************************************************/
//   INPUT/OUPUT PINS
/*******************************************************/
#if defined(USE_PIC24FJ32GA002)



#endif

#use delay(clock=CLOCK_HZ,internal=8000000)
#use fast_io( A )
#use fast_io( B )

#endif

