
.text

add x5, x6, x7

sub x5, x6, x7

and x5, x6, x7

or  x5, x6, x7

xor x5, x6, x7

sw x7, 8(x6)

beq x6, x7, . +8

bne x6, x7, . -8

lui x5, 0x12345

jal x5, . +16
