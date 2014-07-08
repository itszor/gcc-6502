	.importzp _m65x_fpe0_mant, _m65x_fpe0_exp

	.segment "CODE"
	.export _m65x_renormalize_left
_m65x_renormalize_left:
	.scope
	lda _m65x_fpe0_mant
loop:
	ldx _m65x_fpe0_mant+4
	bne done
	ldx _m65x_fpe0_mant+3
	bmi done
	asl a
	rol _m65x_fpe0_mant+1
	rol _m65x_fpe0_mant+2
	rol _m65x_fpe0_mant+3
	rol _m65x_fpe0_mant+4
	dec _m65x_fpe0_exp
	jmp loop
done:
	sta _m65x_fpe0_mant
	rts
	.endscope
