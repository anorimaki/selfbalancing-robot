
#ifndef MATH_UTIL_H
#define	MATH_UTIL_H

#include "boost/preprocessor/control/if.hpp"
#include "boost/preprocessor/comparison/less.hpp"

#define SCALE_VALUE(in, in_bits, out_bits) \
      BOOST_PP_IF( BOOST_PP_LESS( out_bits, in_bits ), \
                (in >> (in_bits-out_bits)),  \
                 (in << (out_bits-in_bits)) )

#ifndef max
#define max(a,b) ((a>b) ? a : b)
#endif

#ifndef min
#define min(a,b) ((a>b) ? b : a)
#endif

#endif	

