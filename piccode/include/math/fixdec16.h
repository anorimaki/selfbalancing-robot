#ifndef _FIXDEC_16_H
#define _FIXDEC_16_H

#include <stdint.h>
#include <libq.h>

typedef _Q16 fix16_t;

static inline fix16_t fix16_mul( fix16_t x, fix16_t y ) {
    return _Q16mpy( x, y );
}

static inline fix16_t fix16_div( fix16_t x, fix16_t y ) {
    return _Q16div( x, y );
}

static inline fix16_t fix16_asin( fix16_t x ) {
    return _Q15asin(x);
}

fix16_t fix16_isqrt( fix16_t x );

fix16_t fix16_sqrt(fix16_t x);

#endif


