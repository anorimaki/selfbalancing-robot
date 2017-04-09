#ifndef _FIXDEC_16_H
#define _FIXDEC_16_H

#include <stdint.h>
#include <libq.h>

#define RAD_TO_DEG_FACTOR 0x00394BB8        //180/PI = 57.29577

typedef _Q16 fix16_t;

static inline fix16_t fix16_mul( fix16_t x, fix16_t y ) {
    return _Q16mpy( x, y );
}

static inline fix16_t fix16_div( fix16_t x, fix16_t y ) {
    return _Q16div( x, y );
}

static inline fix16_t rad_to_deg( fix16_t r ) {
    return fix16_mul( r, RAD_TO_DEG_FACTOR );
}

static inline fix16_t fix16_asin( fix16_t x ) {
    return _Q16asin(x);
}

static inline fix16_t fix16_atan2( fix16_t y, fix16_t x ) {
    return _Q16atanYByX(x, y);
}

fix16_t fix16_isqrt( fix16_t x );

fix16_t fix16_sqrt(fix16_t x);

//1 sign
//4 integer part
//1 '.'
//4 decimal part
//1 for last '\0'
#define MAX_FIX16_STR_SIZE  11  

void fix16_to_str( fix16_t value, char *buf ) ;

#endif


