        .importzp _sp0, _sp1, _fp0, _fp1
        .importzp _r0, _r1, _r2, _r3, _r4, _r5, _r6, _r7
        .importzp _s0, _s1, _s2, _s3, _s4, _s5, _s6, _s7
        .importzp _tmp0, _tmp1

	.segment "DATA"
a_sign:
	.byte 0
b_sign:
	.byte 0
subtract:
	.byte 0

	.segment "CODE"
	.export __addsf3
__addsf3:
	.scope
	lda _r2
	and #$80
	sta a_sign
	lda _r6
	and #$80
	sta b_sign
	
	lda #0
	sta subtract
	
	.scope
	lda _r3
	beq a_exp_zero
	lda _r2
	ora #$80
	sta _r2
a_exp_zero:
	
	lda _r7
	beq b_exp_zero
	lda _r6
	ora #$80
	sta _r6
b_exp_zero:
	.endscope
	
	lda a_sign
	eor b_sign
	bpl signs_equal
	
	lda b_sign
	bpl swap_args
	
	lda #1
	sta subtract
	lda #0
	sta b_sign
	jmp do_addition
swap_args:
	lda _r0
	ldx _r4
	sta _r4
	stx _r0
	lda _r1
	ldx _r5
	sta _r5
	stx _r1
	lda _r2
	ldx _r6
	sta _r6
	stx _r2
	lda _r3
	ldx _r7
	sta _r7
	stx _r3
	jmp __addsf3
signs_equal:
do_addition:
	; compare exponents
	lda _r7
	cmp _r3
	bcc a_exp_less
	lda _r7
	clc
	adc #24
	cmp _r3
	bcs b_not_too_small
	; reconstruct first operand to return
	lda _r2
	and #$7f
	ora a_sign
	sta _r2
	rts
b_not_too_small:
	lda a_exp
	sec
	sbc b_exp
	tax
	beq no_b_shift
	lda b_mant+3
shift_right:
	lsr a
	ror b_mant+2
	ror b_mant+1
	ror b_mant
	dex
	bne shift_right
	sta b_mant+3
no_b_shift:
	lda subtract
	beq plus_shifted_b
	lda a_mant
	sec
	sbc b_mant
	sta a_mant
	lda a_mant+1
	sbc b_mant+1
	sta a_mant+1
	lda a_mant+2
	sbc b_mant+2
	sta a_mant+2
	lda a_mant+3
	sbc b_mant+3
	sta a_mant+3
	
	
	
plus_shifted_b:
	lda a_mant
	clc
	adc b_mant
	sta a_mant
	lda a_mant+1
	adc b_mant+1
	sta a_mant+1
	lda a_mant+2
	adc b_mant+2
	sta a_mant+2
	lda a_mant+3
	adc b_mant+3
	sta a_mant+3

a_mant_plus:
	
	
a_exp_less:

	.endscope	
	rts

	.export __subsf3
__subsf3:
	lda _r6
	eor #$80
	sta _r6
	jmp __addsf3
