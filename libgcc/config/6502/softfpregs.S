	.segment "ZEROPAGE"

	; The stored mantissa consists of a byte for rounding, three bytes for
	; the actual value (with explicit leading bit), and one byte for
	; temporarily holding denormalized high bits during operations.

	.exportzp _m65x_fpe0_mant, _m65x_fpe0_exp, _m65x_fpe0_sign

_m65x_fpe0_mant:
	.res 5,0
_m65x_fpe0_exp:
	.word 0
_m65x_fpe0_sign:
	.byte 0

	; A secondary (unpacked) floating point number, for use as the second
	; operand in calculations.

	.exportzp _m65x_fpe1_mant, _m65x_fpe1_exp, _m65x_fpe1_sign

_m65x_fpe1_mant:
	.res 5,0
_m65x_fpe1_exp:
	.byte 0
_m65x_fpe1_sign:
	.byte 0

