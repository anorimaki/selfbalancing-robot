#include "math/fixdec16.h"


extern uint64_t umul32_32( uint32_t x, uint32_t y );

uint32_t fix16_isqrt_update( uint32_t est, uint32_t mant2 )
{
	return (est + (est >> 1)) -
			(umul32_32( 
				umul32_32( mant2, est ) >> 31, 
				umul32_32( est, est )  >> 31 )  >> 31);
}


/**
 *  Compute the inverse square root for a normalized mantissa.
 *  iter  error
 *  0      9 bits
 *  1     19 bits
 *  2     31 bits (full precision)
 */
static inline uint32_t fix16_core_isqrt( uint32_t mant, int expn )
{
    static const uint8_t isqrt_lut[256] =
        {150,149,148,147,146,145,144,143,142,141,140,139,138,137,136,135,
         134,133,132,131,131,130,129,128,127,126,125,124,124,123,122,121,
         120,119,119,118,117,116,115,114,114,113,112,111,110,110,109,108,
         107,107,106,105,104,104,103,102,101,101,100, 99, 98, 98, 97, 96,
          95, 95, 94, 93, 93, 92, 91, 91, 90, 89, 88, 88, 87, 86, 86, 85,
          84, 84, 83, 82, 82, 81, 80, 80, 79, 79, 78, 77, 77, 76, 75, 75,
          74, 74, 73, 72, 72, 71, 70, 70, 69, 69, 68, 67, 67, 66, 66, 65,
          65, 64, 63, 63, 62, 62, 61, 61, 60, 59, 59, 58, 58, 57, 57, 56,
          56, 55, 54, 54, 53, 53, 52, 52, 51, 51, 50, 50, 49, 49, 48, 48,
          47, 47, 46, 46, 45, 45, 44, 44, 43, 43, 42, 42, 41, 41, 40, 40,
          39, 39, 38, 38, 37, 37, 36, 36, 35, 35, 34, 34, 33, 33, 33, 32,
          32, 31, 31, 30, 30, 29, 29, 28, 28, 28, 27, 27, 26, 26, 25, 25,
          25, 24, 24, 23, 23, 22, 22, 22, 21, 21, 20, 20, 19, 19, 19, 18,
          18, 17, 17, 17, 16, 16, 15, 15, 15, 14, 14, 13, 13, 13, 12, 12,
          11, 11, 11, 10, 10,  9,  9,  9,  8,  8,  8,  7,  7,  6,  6,  6,
           5,  5,  5,  4,  4,  3,  3,  3,  2,  2,  2,  1,  1,  1,  0,  0};

    uint32_t m2 = mant >> 1; /* Half mantissa temporary      */
    uint32_t est;           /* Inverse square root estimate */

    /* Look up the initial guess from mantissa MSBs */
    est = (uint32_t)(isqrt_lut[(mant >> 23) & 0xff] + 362) << 22;

    /* Iterate newton step: est = est*(3 - mant*est*est)/2 */
    est = fix16_isqrt_update(est, m2);
    est = fix16_isqrt_update(est, m2);

    /* Adjust estimate by 1/sqrt(2) if exponent is odd */
    if (expn & 1) {
        est = umul32_32( est, 0xb504f334UL ) >> 31;
    }

    return est;
}

static inline uint8_t fix16_clz(uint32_t val)
{
	int nz;
	asm ("ff1l  %[in], %[in]\n\t"
		"ff1l   %d[in], %d[in]\n\t"
		"bra    NC,clz_end\n\t"
		"add    %[in], %d[in], %d[in]\n\t"
		"clz_end:\n"
	    "dec    %d[in], %[res]"
			: [res] "=&r"(nz), [in] "+r"(val) );
	return nz;

}


/**
 *  Fixed-point inverse square root.
 */
fix16_t fix16_isqrt( fix16_t xval )
{
    uint32_t est;   /* Estimated value         */
    uint32_t mant;  /* Floating-point mantissa */
    int      expn;  /* Floating-point exponent */

    /* Handle illegal values */
 //   if ( xval <= 0 ) {
  //      return -1;
  //  }

    /* Convert fixed-point number to floating-point with 32-bit mantissa */
	int nz = fix16_clz(xval); 
    mant = (uint32_t)(xval) << nz;
    expn = 15 - nz;

    /* Call inverse square root core function */
    est = fix16_core_isqrt(mant, expn);

    /* The square root of the exponent */
    expn = -expn >> 1;

    /* Convert back to fixed-point */
    return est >> (15 - expn);
}


fix16_t fix16_sqrt(fix16_t inValue)
{
	uint8_t  neg = (inValue < 0);
	uint32_t num = (neg ? -inValue : inValue);
	uint32_t result = 0;
	uint32_t bit;
	uint8_t  n;
	
	// Many numbers will be less than 15, so
	// this gives a good balance between time spent
	// in if vs. time spent in the while loop
	// when searching for the starting value.
	if (num & 0xFFF00000)
		bit = (uint32_t)1 << 30;
	else
		bit = (uint32_t)1 << 18;
	
	while (bit > num) bit >>= 2;
	
	// The main part is executed twice, in order to avoid
	// using 64 bit values in computations.
	for (n = 0; n < 2; n++)
	{
		// First we get the top 24 bits of the answer.
		while (bit)
		{
			if (num >= result + bit)
			{
				num -= result + bit;
				result = (result >> 1) + bit;
			}
			else
			{
				result = (result >> 1);
			}
			bit >>= 2;
		}
		
		if (n == 0)
		{
			// Then process it again to get the lowest 8 bits.
			if (num > 65535)
			{
				// The remainder 'num' is too large to be shifted left
				// by 16, so we have to add 1 to result manually and
				// adjust 'num' accordingly.
				// num = a - (result + 0.5)^2
				//	 = num + result^2 - (result + 0.5)^2
				//	 = num - result - 0.5
				num -= result;
				num = (num << 16) - 0x8000;
				result = (result << 16) + 0x8000;
			}
			else
			{
				num <<= 16;
				result <<= 16;
			}
			
			bit = 1 << 14;
		}
	}

#ifndef FIXMATH_NO_ROUNDING
	// Finally, if next bit would have been 1, round the result upwards.
	if (num > result)
	{
		result++;
	}
#endif
	
	return (neg ? -(fix16_t)result : (fix16_t)result);
}
