        .define _ah $70
        .define _ah2 $71
        .define _ah3 $72
        .define _xh $73
        .define _xh2 $74
        .define _xh3 $75
        .define _yh $76
        .define _yh2 $77
        .define _yh3 $78
        .define _sp0 $79
        .define _sp1 $7a
        .define _fp0 $7b
        .define _fp1 $7c
        .define _r0 $7d
        .define _r1 $7e
        .define _r2 $7f
        .define _r3 $80
        .define _r4 $81
        .define _r5 $82
        .define _r6 $83
        .define _r7 $84
        .define _s0 $85
        .define _s1 $86
        .define _s2 $87
        .define _s3 $88
        .define _s4 $89
        .define _s5 $8a
        .define _s6 $8b
        .define _s7 $8c
        .define _tmp0 $8d
        .define _tmp1 $8e

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
