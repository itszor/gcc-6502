        .importzp _sp0, _sp1, _fp0, _fp1
        .importzp _r0, _r1, _r2, _r3, _r4, _r5, _r6, _r7
        .importzp _s0, _s1, _s2, _s3, _s4, _s5, _s6, _s7
        .importzp _tmp0, _tmp1

	.importzp _m65x_fpe0_mant, _m65x_fpe0_exp, _m65x_fpe0_sign
	.importzp _m65x_fpe1_mant, _m65x_fpe1_exp, _m65x_fpe1_sign

	.import _m65x_renormalize_right
	.import _m65x_renormalize_left

	.segment "DATA"
;negate:
;	.byte 0
subtract:
	.byte 0

	.segment "CODE"
	.export __addsf3
__addsf3:
	.scope
	lda _r2
	and #$80
	sta _m65x_fpe0_sign
	lda _r6
	and #$80
	sta _m65x_fpe1_sign
	
	lda #0
	sta subtract
	;sta negate
	
	.scope
	lda _r2
	and #$7f
	ldx _r3
	beq a_exp_zero
	ora #$80
a_exp_zero:
	sta _m65x_fpe0_mant+3
	stx _m65x_fpe0_exp
	
	lda _r6
	and #$7f
	ldx _r7
	beq b_exp_zero
	ora #$80
b_exp_zero:
	sta _m65x_fpe1_mant+3
	stx _m65x_fpe1_exp
	.endscope

	lda _m65x_fpe0_sign
	eor _m65x_fpe1_sign
	bpl do_add_or_sub

	; -X + -Y or X + Y are both really addition. -X + Y (rewritten Y - X)
	; or X - Y are subtraction.
	lda #1
	sta subtract

	lda _m65x_fpe1_sign
	bmi do_add_or_sub

	; We have -X + Y: swap args to obtain Y - X.
	; We've already set up _m65x_fpe0_sign, _m65x_fpe1_sign,
	; _m65x_fpe0_mant+3, _m65x_fpe1_mant+3. Also need to swap the first
	; two bytes of each mantissa and the exponent.
	lda _m65x_fpe0_sign
	ldx _m65x_fpe1_sign
	sta _m65x_fpe1_sign
	stx _m65x_fpe0_sign
	
	lda _m65x_fpe0_mant+3
	ldx _m65x_fpe1_mant+3
	sta _m65x_fpe1_mant+3
	stx _m65x_fpe0_mant+3

	lda _m65x_fpe0_exp
	ldx _m65x_fpe1_exp
	sta _m65x_fpe1_exp
	stx _m65x_fpe0_exp
	
	lda _r0
	ldx _r4
	sta _r4
	stx _r0
	
	lda _r1
	ldx _r5
	sta _r5
	stx _r1
	
do_add_or_sub:
	lda _r0
	sta _m65x_fpe0_mant+1
	lda _r1
	sta _m65x_fpe0_mant+2
	
	lda _r4
	sta _m65x_fpe1_mant+1
	lda _r5
	sta _m65x_fpe1_mant+2
	
	lda #0
	sta _m65x_fpe0_mant
	sta _m65x_fpe0_mant+4
	sta _m65x_fpe1_mant
	sta _m65x_fpe1_mant+4

	; compare exponents
	lda _m65x_fpe0_exp
	cmp _m65x_fpe1_exp
	bcs a_exp_greater
	
	; The fpe1_exp is greater than the fpe0_exp. Do B +/- A instead of
	; A +/- B (because we are shifting the RHS right then
	; adding/subtracting it).
	
	;lda #$80
	;sta negate
	
	; Exchange the fpe0, fpe1 parts.
	.scope
	ldy #0
swap_mant:
	lda _m65x_fpe0_mant,y
	ldx _m65x_fpe1_mant,y
	sta _m65x_fpe1_mant,y
	stx _m65x_fpe0_mant,y
	iny
	cpy #5
	bne swap_mant
	
	lda _m65x_fpe0_exp
	ldx _m65x_fpe1_exp
	sta _m65x_fpe1_exp
	stx _m65x_fpe0_exp
	
	lda _m65x_fpe0_sign
	ldx _m65x_fpe1_sign
	sta _m65x_fpe1_sign
	stx _m65x_fpe0_sign
	.endscope
	
a_exp_greater:
	lda _m65x_fpe1_exp
	clc
	adc #23
	cmp _m65x_fpe0_exp
	bcs b_not_too_small
	; now b_exp+23 < a_exp.
	jmp repack_fpe0
b_not_too_small:
	; b_exp+23 >= a_exp, a_exp <= b_exp+23
	lda _m65x_fpe0_exp
	sec
	sbc _m65x_fpe1_exp
	tax
	beq no_b_shift
	lda _m65x_fpe1_mant+4
shift_right:
	lsr a
	ror _m65x_fpe1_mant+3
	ror _m65x_fpe1_mant+2
	ror _m65x_fpe1_mant+1
	ror _m65x_fpe1_mant
	dex
	bne shift_right
	sta _m65x_fpe1_mant+4
no_b_shift:
	lda subtract
	beq plus_shifted_b

	; If the fpe1 exponent was greater than the fpe0 exponent, negate the
	; result.
	;lda _m65x_fpe0_sign
	;eor negate
	;sta _m65x_fpe0_sign

	lda _m65x_fpe0_mant
	sec
	sbc _m65x_fpe1_mant
	sta _m65x_fpe0_mant
	lda _m65x_fpe0_mant+1
	sbc _m65x_fpe1_mant+1
	sta _m65x_fpe0_mant+1
	lda _m65x_fpe0_mant+2
	sbc _m65x_fpe1_mant+2
	sta _m65x_fpe0_mant+2
	lda _m65x_fpe0_mant+3
	sbc _m65x_fpe1_mant+3
	sta _m65x_fpe0_mant+3
	lda _m65x_fpe0_mant+4
	sbc _m65x_fpe1_mant+4
	sta _m65x_fpe0_mant+4
	
	bpl a_mant_positive

	; Result is less than zero: make mantissa positive, and invert sign.
	ldx #0
	txa
	sec
	sbc _m65x_fpe0_mant
	sta _m65x_fpe0_mant
	txa
	sbc _m65x_fpe0_mant+1
	sta _m65x_fpe0_mant+1
	txa
	sbc _m65x_fpe0_mant+2
	sta _m65x_fpe0_mant+2
	txa
	sbc _m65x_fpe0_mant+3
	sta _m65x_fpe0_mant+3
	txa
	sbc _m65x_fpe0_mant+4
	sta _m65x_fpe0_mant+4
	
	lda _m65x_fpe0_sign
	eor #$80
	sta _m65x_fpe0_sign
a_mant_positive:

	lda _m65x_fpe0_mant
	ora _m65x_fpe0_mant+1
	ora _m65x_fpe0_mant+2
	ora _m65x_fpe0_mant+3
	ora _m65x_fpe0_mant+4
	bne fpe0_nonzero_bits
	lda #0
	sta _m65x_fpe0_exp
	jmp repack_fpe0
fpe0_nonzero_bits:

	jsr _m65x_renormalize_left
	jmp done_subtraction

plus_shifted_b:
	lda _m65x_fpe0_mant
	clc
	adc _m65x_fpe1_mant
	sta _m65x_fpe0_mant
	lda _m65x_fpe0_mant+1
	adc _m65x_fpe1_mant+1
	sta _m65x_fpe0_mant+1
	lda _m65x_fpe0_mant+2
	adc _m65x_fpe1_mant+2
	sta _m65x_fpe0_mant+2
	lda _m65x_fpe0_mant+3
	adc _m65x_fpe1_mant+3
	sta _m65x_fpe0_mant+3
	lda _m65x_fpe0_mant+4
	adc _m65x_fpe1_mant+4
	sta _m65x_fpe0_mant+4

done_subtraction:
	jsr _m65x_renormalize_right

repack_fpe0:
	lda _m65x_fpe0_mant+1
	sta _r0
	lda _m65x_fpe0_mant+2
	sta _r1
	lda _m65x_fpe0_mant+3
	and #$7f
	ora _m65x_fpe0_sign
	sta _r2
	lda _m65x_fpe0_exp
	sta _r3

	.endscope	
	rts

