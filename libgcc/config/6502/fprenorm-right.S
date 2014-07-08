	.importzp _m65x_fpe0_mant, _m65x_fpe0_exp

	.segment "CODE"
	.export _m65x_renormalize_right
_m65x_renormalize_right:
	.scope
	lda _m65x_fpe0_mant+4
loop:
	cmp #0
	beq done
	lsr a
	ror _m65x_fpe0_mant+3
	ror _m65x_fpe0_mant+2
	ror _m65x_fpe0_mant+1
	ror _m65x_fpe0_mant
	inc _m65x_fpe0_exp
	jmp loop
done:
	sta _m65x_fpe0_mant+4
	rts
	.endscope
