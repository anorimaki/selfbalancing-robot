;	Paramters: 
;		AB: 32 bit Integer in w1-w0 registers
;		CD:	32 bit Integer in w3-w2 registers
;	Return:
;		AB*CD: 64 bit integer in w3-w0 registers
;
;	Input paramters are splited in 16 bit blocs:
;			w0: AB 16 LSB (B)
;			w1: AB 16 MSB (A)
;			w2: CD 16 LSB (D)
;			w3: CD 16 MSB (C)
;
;			A	B
;		*	C	D
;	--------------
;			BD1	BD0
;		AD1	AD0
;		BC1	BC0
;	AC1	AC0

.text
.global _umul32_32
_umul32_32:
    mul.uu 	w0, w3, w4		;w4 = BC0, w5 = BC1
    mul.uu 	w1, w3, w6		;w6 = AC0, w7 = AC1
    mov		w1, w3			;w3 = A
    mul.uu 	w0, w2, w0		;w0 = BD0, w1 = BD1
    mul.uu 	w3, w2, w2		;w2 = AD0, w3 = AD1
    add		w1, w2, w1		;w1 = BD1 + AD0
    addc	w3, w5, w3		;w3 = AD1 + BC1
    addc	w7, #0, w7		;
    add		w1, w4, w1		;w1 = BD1 + AD0 + BC0
    addc	w3, w6, w2		;w2 = AD1 + BC1 + AC0
    addc	w7, #0, w3		;
    return
.end
