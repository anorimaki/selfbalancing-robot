#include "math/quaternation.h"

#define max(x,y) ((x>y)?x:y)

extern int8_t count_equal_leading_bits( int32_t );

void quat_normalize( Quaternation* q ) {
	int w_bits = count_equal_leading_bits(q->w);
    int x_bits = count_equal_leading_bits(q->x);
    int y_bits = count_equal_leading_bits(q->y);
    int z_bits = count_equal_leading_bits(q->z);
    
    int max_bits = max( max( max( w_bits, x_bits ), y_bits), z_bits);
    int to_shift = 8 + max_bits;

    fix16_t w_fx;
    fix16_t x_fx;
    fix16_t y_fx;
    fix16_t z_fx;
	if ( to_shift>0 ) {
		w_fx = q->w >> to_shift;
		x_fx = q->x >> to_shift;
		y_fx = q->y >> to_shift;
		z_fx = q->z >> to_shift;
	}
	else {
		to_shift = -to_shift;
		w_fx = q->w << to_shift;
		x_fx = q->x << to_shift;
		y_fx = q->y << to_shift;
		z_fx = q->z << to_shift;
	}
	
	int32_t sum = fix16_mul( w_fx, w_fx );
	sum += fix16_mul( x_fx, x_fx );
	sum += fix16_mul( y_fx, y_fx );
	sum += fix16_mul( z_fx, z_fx );
		
	fix16_t sum_isqrt = fix16_isqrt( sum );
	
	q->w = fix16_mul( w_fx, sum_isqrt );
	q->x = fix16_mul( x_fx, sum_isqrt );
	q->y = fix16_mul( y_fx, sum_isqrt );
	q->z = fix16_mul( z_fx, sum_isqrt );
}


fix16_t quat_pitch( Quaternation* q ) {
	fix16_t t2 = (fix16_mul( q->w, q->y ) - fix16_mul( q->z, q->x ));
	t2 <<= 1;
	t2 = t2 > 0x10000 ? 0x10000 : t2;
	t2 = t2 < (long)0xFFFF0000 ? 0xFFFF0000 : t2;
	return fix16_asin(t2);
}


fix16_t quat_roll( Quaternation* q ) {
	fix16_t t0 = (fix16_mul( q->w ,q->x ) + fix16_mul( q->y, q->z ));
	t0 <<= 1;
	
	fix16_t t1 = (fix16_mul( q->x, q->x ) + fix16_mul( q->y, q->y ));
	t1 <<= 1;
	t1 = 0x00010000 - t1;
	
	return fix16_atan2(t0, t1);
}