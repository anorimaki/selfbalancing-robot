#ifndef QUATERNATION_H
#define	QUATERNATION_H

#include "math/fixdec16.h"

typedef struct {
    fix16_t w;
    fix16_t x;
    fix16_t y;
    fix16_t z;
} Quaternation;

void quat_normalize( Quaternation* q );

fix16_t quat_to_pitch( Quaternation* q );
fix16_t quat_roll( Quaternation* q );

#endif	/* QUATERNATION_H */

