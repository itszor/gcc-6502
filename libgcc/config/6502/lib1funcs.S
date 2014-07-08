        .importzp _sp0, _sp1, _fp0, _fp1
        .importzp _r0, _r1, _r2, _r3, _r4, _r5, _r6, _r7
        .importzp _s0, _s1, _s2, _s3, _s4, _s5, _s6, _s7
        .importzp _tmp0, _tmp1

#ifdef L_bswapsi2
	.export __bswapsi2
__bswapsi2:
	.scope
	lda _r3
	ldx _r0
	stx _r3
	sta _r0
	lda _r2
	ldx _r1
	stx _r2
	sta _r1
	rts
	.endscope
#endif

#ifdef L_bswapdi2
	.export __bswapdi2
__bswapdi2:
	.scope
	ldx #0
	ldy #7
loop:
	lda _r0,x
	pha
	lda _r0,y
	sta _r0,x
	pla
	sta _r0,y
	dey
	inx
	cpx #4
	bne loop
	rts
	.endscope
	rts
#endif

#ifdef L_negsi2
	.export __negsi2
__negsi2:
	lda #0
	sec
	sbc _r0
	sta _r0
	lda #0
	sbc _r1
	sta _r1
	lda #0
	sbc _r2
	sta _r2
	lda #0
	sbc _r3
	sta _r3
	rts
#endif

#ifdef L_ashlqi3
	.export __ashlqi3
__ashlqi3:
	.scope
	ldx _r1
	beq done
loop:
	asl _r0
	dex
	bne loop
done:
	rts
	.endscope
#endif

#ifdef L_lshrqi3
	.export __lshrqi3
__lshrqi3:
	.scope
	ldx _r1
	beq done
loop:
	lsr _r0
	dex
	bne loop
done:
	rts
	.endscope
#endif

#ifdef L_ashrqi3
	.export __ashrqi3
__ashrqi3:
	.scope
	ldx _r1
	beq done
	lda _r0
loop:
	cmp #$80
	ror a
	dex
	bne loop
	sta _r0
done:
	rts
	.endscope
#endif

#ifdef L_ashlhi3
	.export __ashlhi3
__ashlhi3:
	.scope
	ldx _r2
	beq done
	lda _r0
loop:
	asl a
	rol _r1
	dex
	bne loop
	sta _r0
done:
	.endscope
	rts
#endif

#ifdef L_lshrhi3
	.export __lshrhi3
__lshrhi3:
	.scope
	ldx _r2
	beq done
	lda _r1
loop:
	lsr a
	ror _r0
	dex
	bne loop
	sta _r1
done:
	.endscope
	rts
#endif

#ifdef L_ashrhi3
	.export __ashrhi3
__ashrhi3:
	.scope
	ldx _r2
	beq done
	lda _r1
loop:
	cmp #$80
	ror a
	ror _r0
	dex
	bne loop
	sta _r1
done:
	rts
	.endscope
#endif

#ifdef L_ashlsi3
	.export __ashlsi3
__ashlsi3:
	.scope
	ldx _r4
	beq done
	lda _r0
loop:
	asl a
	rol _r1
	rol _r2
	rol _r3
	dex
	bne loop
	sta _r0
done:
	rts
	.endscope
#endif

#ifdef L_lshrsi3
	.export __lshrsi3
__lshrsi3:
	.scope
	ldx _r4
	beq done
	lda _r3
loop:
	lsr a
	ror _r2
	ror _r1
	ror _r0
	dex
	bne loop
	sta _r3
done:
	rts
	.endscope
#endif

#ifdef L_ashrsi3
	.export __ashrsi3
__ashrsi3:
	.scope
	ldx _r4
	beq done
	lda _r3
loop:
	cmp #$80
	ror a
	ror _r2
	ror _r1
	ror _r0
	dex
	bne loop
	sta _r3
done:
	rts
	.endscope
#endif

#ifdef L_mulqi3
	.export __mulqi3
__mulqi3:
	.scope
	ldx #0
loop:
	lsr _r1
	bcc no_add
	txa
	clc
	adc _r0
	tax
no_add:
	asl _r0
	bne loop
	stx _r0
	rts
	.endscope
#endif

#ifdef L_mulsi3
	.export __mulsi3
__mulsi3:
	.scope
	lda _s0
	pha
	lda _s1
	pha
	lda _s2
	pha
	lda _s3
	pha

	lda #0
	sta _s0
	sta _s1
	sta _s2
	sta _s3

	ldx #32
loop:
	lsr _r7
	ror _r6
	ror _r5
	ror _r4
	bcc no_add
	clc
	lda _s0
	adc _r0
	sta _s0
	lda _s1
	adc _r1
	sta _s1
	lda _s2
	adc _r2
	sta _s2
	lda _s3
	adc _r3
	sta _s3
no_add:
	asl _r0
	rol _r1
	rol _r2
	rol _r3

	dex
	bne loop

	lda _s0
	sta _r0
	lda _s1
	sta _r1
	lda _s2
	sta _r2
	lda _s3
	sta _r3

	pla
	sta _s3
	pla
	sta _s2
	pla
	sta _s1
	pla
	sta _s0
	rts
	.endscope
#endif

#ifdef L_udivqi3
	.export __udivqi3
__udivqi3:
	.scope
	lda #0
	sta _r2		; quotient
	sta _r3		; remainder
	ldx #8
loop:
	asl _r0
	rol _r3
	lda _r3
	sec
	sbc _r1
	bcc less
	rol _r2
	sta _r3
	dex
	bne loop
	jmp done
less:
	rol _r2
	dex
	bne loop
done:
	lda _r2
	sta _r0
	lda _r3
	sta _r1
	rts
	.endscope
#endif

#ifdef L_umodqi3
	.import __udivqi3
	.export __umodqi3
__umodqi3:
	jsr __udivqi3
	lda _r1
	sta _r0
	rts
#endif

#ifdef L_udivsi3
	.export __udivsi3
__udivsi3:
	.scope
	lda _s0
	pha
	lda _s1
	pha
	lda _s2
	pha
	lda _s3
	pha
	lda _s4
	pha
	lda _s5
	pha
	lda _s6
	pha
	lda _s7
	pha
	
	lda #0
	sta _s0		; quotient
	sta _s1
	sta _s2
	sta _s3
	sta _s4		; remainder
	sta _s5
	sta _s6
	sta _s7
	
	ldx #32
loop:
	asl _r0		; shift numerator
	rol _r1
	rol _r2
	rol _r3
	rol _s4		; left-shift remainder
	rol _s5
	rol _s6
	rol _s7
	
	sec
	lda _s4
	sbc _r4
	pha
	lda _s5
	sbc _r5
	pha
	lda _s6
	sbc _r6
	pha
	lda _s7
	sbc _r7
	bcc less
	rol _s0
	rol _s1
	rol _s2
	rol _s3
	sta _s7
	pla
	sta _s6
	pla
	sta _s5
	pla
	sta _s4
	jmp next_bit
less:
	pla
	pla
	pla
	asl _s0
	rol _s1
	rol _s2
	rol _s3
next_bit:
	dex
	bne loop

	; Put quotient in the right place
	lda _s0
	sta _r0
	lda _s1
	sta _r1
	lda _s2
	sta _r2
	lda _s3
	sta _r3
	
	; Stash remainder too
	lda _s4
	sta _r4
	lda _s5
	sta _r5
	lda _s6
	sta _r6
	lda _s7
	sta _r7

	pla
	sta _s7
	pla
	sta _s6
	pla
	sta _s5
	pla
	sta _s4
	pla
	sta _s3
	pla
	sta _s2
	pla
	sta _s1
	pla
	sta _s0
	rts
	.endscope
#endif

#ifdef L_umodsi3
	.import __udivsi3
	.export __umodsi3
__umodsi3:
	jsr __udivsi3
	lda _r4
	sta _r0
	lda _r5
	sta _r1
	lda _r6
	sta _r2
	lda _r7
	sta _r3
	rts
#endif

#ifdef L_ltsf2
	.export __m65x_fpcmp
	; Helper routine used by ltsf/lesf/gtsf/gesf.  This has to go
	; somewhere, so put it here.
	; On entry the Y register has
	;   - zero for a regular comparison
	;   - nonzero for a reversed comparison
	; On exit the accumulator has
	;   - 0x80 for "less than" result
	;   - zero for "greater than or equal" result.
__m65x_fpcmp:
	.scope
	lda _r2
	tax
	and #$80
	sta _tmp0
	txa
	and #$7f
	sta _r2
	
	lda _r6
	tax
	and #$80
	sta _tmp1
	txa
	and #$7f
	sta _r6
	
	; -X < -Y == X > Y == Y < X
	lda _tmp0
	and _tmp1
	bpl not_both_negative
	jmp maybe_reverse_cmp
not_both_negative:
	lda _tmp0
	eor _tmp1
	bpl not_one_only
	; Now we have one of:
	; -X <  Y
	;  X < -Y
	; -X >  Y  (reversed)
	;  X > -Y
	cpy #0
	bne reverse
	lda _tmp0
	rts
reverse:
	lda _tmp1
	rts
not_one_only:

	cpy #0
	bne reverse_cmp
forward_cmp:
	.scope
	; mantissa
	lda _r0
	cmp _r4
	lda _r1
	sbc _r5
	lda _r2
	sbc _r6
	; exponent
	lda _r3
	sbc _r7
	bcc less
	lda #0
	rts
less:
	lda #$80
	rts
	.endscope

maybe_reverse_cmp:
	cpy #0
	bne forward_cmp
reverse_cmp:
	.scope
	; mantissa
	lda _r4
	cmp _r0
	lda _r5
	sbc _r1
	lda _r6
	sbc _r2
	; exponent
	lda _r7
	sbc _r3
	bcc less
	lda #0
	rts
less:
	lda #$80
	rts
	.endscope
	.endscope

	.export __ltsf2
__ltsf2:
	ldy #0
	jsr __m65x_fpcmp
	sta _r0
	rts
#endif

#ifdef L_gesf2
	.import __m65x_fpcmp
	.export __gesf2
__gesf2:
	ldy #0
	jsr __m65x_fpcmp
	eor #$80
	sta _r0
	rts
#endif

#ifdef L_gtsf2
	.import __m65x_fpcmp
	.export __gtsf2
__gtsf2:
	ldy #1
	jsr __m65x_fpcmp
	sta _r0
	rts
#endif

#ifdef L_lesf2
	.import __m65x_fpcmp
	.export __lesf2
__lesf2:
	ldy #1
	jsr __m65x_fpcmp
	eor #$80
	sta _r0
	rts
#endif

#ifdef L_eqsf2
	.export __eqsf2
__eqsf2:
	.scope
	lda _r0
	cmp _r4
	bne ne
	lda _r1
	cmp _r5
	bne ne
	lda _r2
	cmp _r6
	bne ne
	lda _r3
	cmp _r7
	bne ne
	lda #1
	sta _r0
	rts
ne:
	lda #0
	sta _r0
	rts
	.endscope
#endif

#ifdef L_nesf2
	.import __eqsf2
	.export __nesf2
__nesf2:
	jsr __eqsf2
	lda _r0
	eor #1
	sta _r0
	rts
#endif

#ifdef L_addsf3
#include "addsf3.S"
#endif

#ifdef L_subsf3
	.import __addsf3
	.export __subsf3
__subsf3:
	lda _r6
	eor #$80
	sta _r6
	jmp __addsf3
#endif
