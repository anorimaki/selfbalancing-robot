/* 
 * File:   mul.h
 * Author: David
 *
 * Created on 29 de diciembre de 2017, 7:31
 */

#ifndef MUL_H
#define	MUL_H

#ifdef	__cplusplus
extern "C" {
#endif

uint64_t umul32_32( uint32_t x, uint32_t y );
uint64_t umul32_16( uint32_t x, uint16_t y );

#ifdef	__cplusplus
}
#endif

#endif	/* MUL_H */

