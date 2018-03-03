.text
.global _count_equal_leading_bits
_count_equal_leading_bits:
    fbcl    w0,w3
    fbcl    w1,w0
    bra	    NC,single_word
    add	    w3,w0,w0
single_word:
    return
