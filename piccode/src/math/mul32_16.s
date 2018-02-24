;	Paramters: 
;		AB: 32 bit Integer in w1-w0 registers
;		C: 16 bit Integer in w2 registers
;	Return:
;		AB*C: 48 bit integer in w3-w0 registers
;
;	Input paramters are splited in 16 bit blocs:
;			w0: AB 16 LSB (B)
;			w1: AB 16 MSB (A)
;			w2: C 
;
;			A	B
;		*		C
;	--------------
;			BC1	BC0
;		AC1	AC0

.text
.global _umul32_16
_umul32_16:
    mul.uu 	w1, w2, w4		;w4 = AC0, w5 = AC1
    mul.uu 	w0, w2, w0		;w0 = BC0, w1 = BC1
    add		w1, w4, w1		;w1 = BC1 + AC0
    addc	w5, #0, w3		;w3 = AC1 + c
    return
.end



