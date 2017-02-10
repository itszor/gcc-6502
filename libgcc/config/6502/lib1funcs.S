        .importzp _sp0, _sp1, _fp0, _fp1
        .importzp _r0, _r1, _r2, _r3, _r4, _r5, _r6, _r7
        .importzp _s0, _s1, _s2, _s3, _s4, _s5, _s6, _s7
        .importzp _e0, _e1, _e2, _e3, _e4, _e5, _e6, _e7
        .importzp _e8, _e9, _e10, _e11, _e12, _e13, _e14, _e15
        .importzp _tmp0, _tmp1
	.importzp _m65x_fpe0_mant, _m65x_fpe0_exp, _m65x_fpe0_sign
	.importzp _m65x_fpe1_mant, _m65x_fpe1_exp, _m65x_fpe1_sign

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

#ifdef L_cmpsi2
	.export __cmpsi2
__cmpsi2:
	lda _r0
	cmp _r4
	lda _r1
	sbc _r5
	lda _r2
	sbc _r6
	lda _r3
	sbc _r7
	bvc no_ov
	eor #$80
no_ov:
	bmi less
	lda _r0
	cmp _r4
	bne greater
	lda _r1
	cmp _r5
	bne greater
	lda _r2
	cmp _r6
	bne greater
	lda _r3
	cmp _r7
	bne greater
	lda #1
	sta _r0
	lda #0
	sta _r1
	rts
greater:
	lda #2
	sta _r0
	lda #0
	sta _r1
	rts
less:
	lda #0
	sta _r0
	sta _r1
	rts
#endif

#ifdef L_cmpdi2
        .export __cmpdi2
__cmpdi2:
        ldy #0
        lda _r0
        cmp (_sp0),y
        iny
        lda _r1
        sbc (_sp0),y
        iny
        lda _r2
        sbc (_sp0),y
        iny
        lda _r3
        sbc (_sp0),y
        iny
        lda _r4
        sbc (_sp0),y
        iny
        lda _r5
        sbc (_sp0),y
        iny
        lda _r6
        sbc (_sp0),y
        iny
        lda _r7
        sbc (_sp0),y
        bvc no_ov
        eor #$80
no_ov:
        bmi less
        ldy #0
samecheck:
        lda _r0,y
        cmp (_sp0),y
        bne greater
        iny
        cpy #8
        bne samecheck
        lda #1
        sta _r0
        lda #0
        sta _r1
        rts
greater:
        lda #2
        sta _r0
        lda #0
        sta _r1
        rts
less:
        lda #0
        sta _r0
        sta _r1
        rts
#endif

#ifdef L_ucmpdi2
        .export __ucmpdi2
__ucmpdi2:
        ldy #7
        lda _r7
        cmp (_sp0),y
        bcc less
        bne greater
        dey
        lda _r6
        cmp (_sp0),y
        bcc less
        bne greater
        dey
        lda _r5
        cmp (_sp0),y
        bcc less
        bne greater
        dey
        lda _r4
        cmp (_sp0),y
        bcc less
        bne greater
        dey
        lda _r3
        cmp (_sp0),y
        bcc less
        bne greater
        dey
        lda _r2
        cmp (_sp0),y
        bcc less
        bne greater
        dey
        lda _r1
        cmp (_sp0),y
        bcc less
        bne greater
        dey
        lda _r0
        cmp (_sp0),y
        bcc less
        bne greater
        lda #1
        sta _r0
        lda #0
        sta _r1
        rts
less:
        lda #0
        sta _r0
        sta _r1
        rts
greater:
        lda #2
        sta _r0
        lda #0
        sta _r1
        rts
#endif

#ifdef L_ashldi3
        .export __ashldi3
__ashldi3:
        ldy #0
        lda (_sp0),y
        tax
        beq done
        lda _r0
loop:
        asl a
        rol _r1
        rol _r2
        rol _r3
        rol _r4
        rol _r5
        rol _r6
        rol _r7
        dex
        bne loop
        sta _r0
done:
        rts
#endif

#ifdef L_lshrdi3
        .export __lshrdi3
__lshrdi3:
        ldy #0
        lda (_sp0),y
        tax
        beq done
        lda _r7
loop:
        lsr a
        ror _r6
        ror _r5
        ror _r4
        ror _r3
        ror _r2
        ror _r1
        ror _r0
        dex
        bne loop
        sta _r7
done:
        rts
#endif

#ifdef L_ashrdi3
        .export __ashrdi3
__ashrdi3:
        ldy #0
        lda (_sp0),y
        tax
        beq done
        lda _r7
loop:
        cmp #$80
        ror a
        ror _r6
        ror _r5
        ror _r4
        ror _r3
        ror _r2
        ror _r1
        ror _r0
        dex
        bne loop
        sta _r7
done:
        rts
#endif

#ifdef L_muldi3
        .export __muldi3
__muldi3:
        ldy #0
        ldx #0
copyop2:
        lda (_sp0),y
        sta _e0,y
        stx _e8,y
        iny
        cpy #8
        bne copyop2
        ldx #64
loop:
        lsr _r7
        ror _r6
        ror _r5
        ror _r4
        ror _r3
        ror _r2
        ror _r1
        ror _r0
        bcc no_add
        clc
        lda _e8
        adc _e0
        sta _e8
        lda _e9
        adc _e1
        sta _e9
        lda _e10
        adc _e2
        sta _e10
        lda _e11
        adc _e3
        sta _e11
        lda _e12
        adc _e4
        sta _e12
        lda _e13
        adc _e5
        sta _e13
        lda _e14
        adc _e6
        sta _e14
        lda _e15
        adc _e7
        sta _e15
no_add:
        asl _e0
        rol _e1
        rol _e2
        rol _e3
        rol _e4
        rol _e5
        rol _e6
        rol _e7
        dex
        bne loop
        
        ldx #0
copyout:
        lda _e8,x
        sta _r0,x
        inx
        cpx #8
        bne copyout
        rts
#endif

#ifdef L_udivdi3
        .export __udivdi3
__udivdi3:
        lda #0
        ldx #0
        ; e0...e7 are the quotient.
        ; e8...e15 are the remainder.
clear:
        sta _e0,x
        inx
        cpx #16
        bne clear

        ldx #64
loop:
        asl _r0         ; shift numerator
        rol _r1
        rol _r2
        rol _r3
        rol _r4
        rol _r5
        rol _r6
        rol _r7
        rol _e8         ; left-shift remainder
        rol _e9
        rol _e10
        rol _e11
        rol _e12
        rol _e13
        rol _e14
        rol _e15
        
        ldy #0
        sec
        lda _e8
        sbc (_sp0),y
        pha
        iny
        lda _e9
        sbc (_sp0),y
        pha
        iny
        lda _e10
        sbc (_sp0),y
        pha
        iny
        lda _e11
        sbc (_sp0),y
        pha
        iny
        lda _e12
        sbc (_sp0),y
        pha
        iny
        lda _e13
        sbc (_sp0),y
        pha
        iny
        lda _e14
        sbc (_sp0),y
        pha
        iny
        lda _e15
        sbc (_sp0),y
        bcc less
        rol _e0
        rol _e1
        rol _e2
        rol _e3
        rol _e4
        rol _e5
        rol _e6
        rol _e7
        sta _e15
        pla
        sta _e14
        pla
        sta _e13
        pla
        sta _e12
        pla
        sta _e11
        pla
        sta _e10
        pla
        sta _e9
        pla
        sta _e8
        jmp next_bit
less:
        pla
        pla
        pla
        pla
        pla
        pla
        pla
        asl _e0
        rol _e1
        rol _e2
        rol _e3
        rol _e4
        rol _e5
        rol _e6
        rol _e7
next_bit:
        dex
        beq done
        ; Tsk, out of range!
        jmp loop
done:

        ; Put quotient in the right place
        ldx #0
copyout:
        lda _e0,x
        sta _r0,x
        inx
        cpx #8
        bne copyout

        rts
#endif

#ifdef L_umoddi3
        .import __udivdi3
        .export __umoddi3
__umoddi3:
        jsr __udivdi3
        ; The above function left the remainder in e8...e15.  Those aren't
        ; normally live across procedure call boundaries, but it doesn't matter
        ; here because the whole lifetime is under our control.
        ldx #0
copyout:
        lda _e8,x
        sta _r0,x
        inx
        cpx #8
        bne copyout
        rts
#endif

#ifdef L_neghi2
        .export __neghi2
__neghi2:
        lda #0
        sec
        sbc _r0
        sta _r0
        lda #0
        sbc _r1
        sta _r1
        rts
#endif

#ifdef L_negsi2
	.export __negsi2
__negsi2:
	ldx #0
	txa
	sec
	sbc _r0
	sta _r0
	txa
	sbc _r1
	sta _r1
	txa
	sbc _r2
	sta _r2
	txa
	sbc _r3
	sta _r3
	rts
#endif

#ifdef L_negdi2
        .export __negdi2
__negdi2:
        ldx #0
        txa
        sec
        sbc _r0
        sta _r0
        txa
        sbc _r1
        sta _r1
        txa
        sbc _r2
        sta _r2
        txa
        sbc _r3
        sta _r3
        txa
        sbc _r4
        sta _r4
        txa
        sbc _r5
        sta _r5
        txa
        sbc _r6
        sta _r6
        txa
        sbc _r7
        sta _r7
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

#ifdef L_mulhi3
        .export __mulhi3
__mulhi3:
        ldx #0
        ldy #0
loop:
        lsr _r1
        ror _r0
        bcc no_add
        txa
        clc
        adc _r2
        tax
        tya
        adc _r3
        tay
no_add:
        asl _r2
        rol _r3
        lda _r2
        ora _r3
        bne loop
        stx _r0
        sty _r1
        rts
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

#ifdef L_udivhi3
        .export __udivhi3
__udivhi3:
        ; (_r1, _r0) / (_r3, _r2)
        ; quotient in _r5, _r4
        ; remainder in _r7, _r6
        lda #0
        sta _r4
        sta _r5
        sta _r6
        sta _r7
        ldx #16
loop:
        asl _r0         ; left-shift numerator
        rol _r1
        rol _r6         ; shift high-order bit into remainder
        rol _r7
        lda _r6         ; compare remainder with denominator
        sec
        sbc _r2
        tay
        lda _r7
        sbc _r3
        bcc notgreater
        rol _r4
        rol _r5
        sty _r6
        sta _r7
        jmp next
notgreater:
        asl _r4
        rol _r5
next:
        dex
        bne loop
        lda _r4
        sta _r0
        lda _r5
        sta _r1
        rts
#endif

#ifdef L_umodhi3
        .import __udivhi3
        .export __umodhi3
__umodhi3:
        jsr __udivhi3
        lda _r6
        sta _r0
        lda _r7
        sta _r1
        rts
#endif

#ifdef L_divhi3
        .import __udivhi3
        .import __neghi2
        .export __divhi3
__divhi3:
        lda _r1
        eor _r3
        pha

        lda _r1
        bpl a_positive
        jsr __neghi2
a_positive:

        lda _r3
        bpl b_positive
        lda #0
        sec
        sbc _r2
        sta _r2
        lda #0
        sbc _r3
        sta _r3
b_positive:

        jsr __udivhi3

        pla
        bpl res_positive
        ; tailcall
        jmp __neghi2
res_positive:

        rts
#endif

#ifdef L_modhi3
        .import __umodhi3
        .import __neghi2
        .export __modhi3
__modhi3:
        lda _r1
        pha
        
        bpl numerator_positive
        jsr __neghi2
numerator_positive:
        lda _r3
        bpl denominator_positive
        lda #0
        sec
        sbc _r2
        sta _r2
        lda #0
        sbc _r3
        sta _r3
denominator_positive:
        jsr __umodhi3
        pla
        bpl result_positive
        ; tailcall
        jmp __neghi2
result_positive:
        rts
#endif

#ifdef L_udivsi3
        ; This might as well use the _eN registers instead of clobbering the
        ; _sN registers. FIXME!
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

#ifdef L_divsi3
	.import __udivsi3
	.import __negsi2
	.export __divsi3
__divsi3:
	lda _r3
	eor _r7
	pha
	
	lda _r3
	bpl numerator_positive
	jsr __negsi2
numerator_positive:
	lda _r7
	bpl denominator_positive
	ldx #0
	txa
	sec
	sbc _r4
	sta _r4
	txa
	sbc _r5
	sta _r5
	txa
	sbc _r6
	sta _r6
	txa
	sbc _r7
	sta _r7
denominator_positive:
	jsr __udivsi3
	pla
	bpl result_positive
	; tailcall
	jmp __negsi2
result_positive:
	rts
#endif

#ifdef L_modsi3
	.import __umodsi3
	.import __negsi2
	.export __modsi3
__modsi3:
	lda _r3
	pha
	
	bpl numerator_positive
	jsr __negsi2
numerator_positive:
	lda _r7
	bpl denominator_positive
	ldx #0
	txa
	sec
	sbc _r4
	sta _r4
	txa
	sbc _r5
	sta _r5
	txa
	sbc _r6
	sta _r6
	txa
	sbc _r7
	sta _r7
denominator_positive:
	jsr __umodsi3
	pla
	bpl result_positive
	; tailcall
	jmp __negsi2
result_positive:
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
	lda _r2
	and #$7f
	ora _r3
	ora _r1
	ora _r0
	bne not_plusminus_zero

	lda _r6
	and #$7f
	ora _r7
	ora _r5
	ora _r4
	beq eq

not_plusminus_zero:
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
eq:
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

#ifdef L_mulsf3
	.import _m65x_renormalize_right
	.export __mulsf3
__mulsf3:
	lda _s2
	pha
	lda _s1
	pha
	lda _s0
	pha

	lda _r2
	eor _r6
	and #$80
	sta _m65x_fpe0_sign
	
	.scope
	lda _r2
	and #$7f
	ldx _r3
	beq a_exp_zero
	ora #$80
a_exp_zero:
	sta _r2
	stx _m65x_fpe0_exp
	
	lda _r6
	and #$7f
	ldx _r7
	beq b_exp_zero
	ora #$80
b_exp_zero:
	stx _m65x_fpe1_exp
	tax
	.endscope
	
	lda _r4
	sta _r3
	lda _r5
	sta _r4
	stx _r5
	
	; Do the actual multiplication.
	.scope
	
	lda #0
	sta _r6
	sta _r7
	sta _m65x_fpe0_mant
	sta _m65x_fpe0_mant+1
	sta _m65x_fpe0_mant+2
	sta _m65x_fpe0_mant+3
	sta _m65x_fpe0_mant+4
	
	; s2,s1,s0,r2,r1,r0
	sta _s0
	sta _s1
	sta _s2
	
	ldx #24
loop:
	lsr _r5
	ror _r4
	ror _r3
	bcc no_add
	clc
	lda _r6
	adc _r0
	sta _r6
	lda _r7
	adc _r1
	sta _r7
	lda _m65x_fpe0_mant
	adc _r2
	sta _m65x_fpe0_mant
	lda _m65x_fpe0_mant+1
	adc _s0
	sta _m65x_fpe0_mant+1
	lda _m65x_fpe0_mant+2
	adc _s1
	sta _m65x_fpe0_mant+2
	lda _m65x_fpe0_mant+3
	adc _s2
	sta _m65x_fpe0_mant+3
	bcc :+
	inc _m65x_fpe0_mant+4
	:
no_add:
	asl _r0
	rol _r1
	rol _r2
	rol _s0
	rol _s1
	rol _s2
	dex
	bne loop

	asl _r6
	rol _r7
	rol _m65x_fpe0_mant
	rol _m65x_fpe0_mant+1
	rol _m65x_fpe0_mant+2
	rol _m65x_fpe0_mant+3
	rol _m65x_fpe0_mant+4

	.endscope
	
	ldx #0
	
	lda _m65x_fpe0_exp
	clc
	adc _m65x_fpe1_exp
	sta _m65x_fpe0_exp
	bcc :+
	inx
	:
	
	lda _m65x_fpe0_exp
	sec
	sbc #127
	sta _m65x_fpe0_exp
	bcs :+
	dex
	:
	; FIXME: clamping to zero/max_float goes here.

	.scope
	lda _m65x_fpe0_mant+1
	ora _m65x_fpe0_mant+2
	ora _m65x_fpe0_mant+3
	bne not_zero
	sta _m65x_fpe0_exp
	jmp done
not_zero:
	jsr _m65x_renormalize_right
done:
	.endscope

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

	pla
	sta _s0
	pla
	sta _s1
	pla
	sta _s2

	rts
#endif

#ifdef L_divsf3
	; A plain implementation of the "Integer division (unsigned) with
	; remainder" algorithm given on Wikipedia:
	;   [if D == 0 then error(DivisionByZeroException) end]
	;   Q := 0                 -- initialize quotient and remainder to zero
	;   R := 0                     
	;   for i = n-1...0 do     -- where n is number of bits in N
	;     R := R << 1          -- left-shift R by 1 bit
	;     R(0) := N(i)         -- set the least-significant bit of R equal
	;     if R >= D then	      to bit i of the numerator
	;       R = R - D
	;       Q(i) := 1
	;     end
	;   end
	.import _m65x_renormalize_left
	.export __divsf3
__divsf3:
	lda _s2
	pha
	lda _s1
	pha
	lda _s0
	pha

	lda _r2
	eor _r6
	and #$80
	sta _m65x_fpe0_sign
	
	.scope
	lda _r2
	and #$7f
	ldx _r3
	beq a_exp_zero
	ora #$80
a_exp_zero:
	sta _r2
	stx _m65x_fpe0_exp
	
	lda _r6
	and #$7f
	ldx _r7
	beq b_exp_zero
	ora #$80
b_exp_zero:
	sta _r6
	stx _m65x_fpe1_exp
	.endscope
	
	lda #0
	sta _s2
	sta _s1
	sta _s0
	
	; Result goes here. We want the eventual answer in fpe0_mant+[1,2,3].
	sta _m65x_fpe0_mant
	sta _m65x_fpe0_mant+1
	sta _m65x_fpe0_mant+2
	sta _m65x_fpe0_mant+3
	sta _m65x_fpe0_mant+4
	sta _r3
	
	; The remainder
	sta _m65x_fpe1_mant
	sta _m65x_fpe1_mant+1
	sta _m65x_fpe1_mant+2
	sta _m65x_fpe1_mant+3
	sta _m65x_fpe1_mant+4
	sta _r7
	
	; r2,r1,r0,s2,s1,s0 = a_mant << 23
	
	lsr _r2
	ror _r1
	ror _r0
	ror _s2
	ror _s1
	ror _s0
	
	; result[47:0] = r2,r1,r0,s2,s1,s0 / r6,r5,r4
	.scope
	ldx #48
loop:
	; get the i'th bit of N (starting from highest-order bit).
	asl _s0
	rol _s1
	rol _s2
	rol _r0
	rol _r1
	rol _r2
	; R = (R << 1) | (N[i] ? 1 : 0)
	rol _m65x_fpe1_mant
	rol _m65x_fpe1_mant+1
	rol _m65x_fpe1_mant+2
	rol _m65x_fpe1_mant+3
	rol _m65x_fpe1_mant+4
	rol _r7
	
	; Test D <= R
	sec
	lda _m65x_fpe1_mant
	sbc _r4
	pha
	lda _m65x_fpe1_mant+1
	sbc _r5
	pha
	lda _m65x_fpe1_mant+2
	sbc _r6
	pha
	lda _m65x_fpe1_mant+3
	sbc #0
	pha
	lda _m65x_fpe1_mant+4
	sbc #0
	pha
	lda _r7
	sbc #0
	bcc less
	rol _m65x_fpe0_mant+1
	rol _m65x_fpe0_mant+2
	rol _m65x_fpe0_mant+3
	rol _m65x_fpe0_mant+4
	rol _m65x_fpe0_mant
	rol _r3
	sta _r7
	pla
	sta _m65x_fpe1_mant+4
	pla
	sta _m65x_fpe1_mant+3
	pla
	sta _m65x_fpe1_mant+2
	pla
	sta _m65x_fpe1_mant+1
	pla
	sta _m65x_fpe1_mant
	jmp next_bit
less:
	pla
	pla
	pla
	pla
	pla
	asl _m65x_fpe0_mant+1
	rol _m65x_fpe0_mant+2
	rol _m65x_fpe0_mant+3
	rol _m65x_fpe0_mant+4
	rol _m65x_fpe0_mant
	rol _r3
next_bit:
	dex
	bne loop
	.endscope
	
	; Remove junk in high-order & low-order bits.
	lda #0
	sta _m65x_fpe0_mant
	sta _m65x_fpe0_mant+4
	
	; High-order exponent byte.
	ldx #0
	
	lda _m65x_fpe0_exp
	sec
	sbc _m65x_fpe1_exp
	sta _m65x_fpe0_exp
	bcs :+
	dex
	:
	
	lda _m65x_fpe0_exp
	clc
	adc #127
	sta _m65x_fpe0_exp
	bcc :+
	inx
	:
	
	; FIXME: Handle exponent overflow/underflow here.
	
	.scope
	lda _m65x_fpe0_mant+1
	ora _m65x_fpe0_mant+2
	ora _m65x_fpe0_mant+3
	bne not_zero
	sta _m65x_fpe0_exp
	jmp done
not_zero:
	jsr _m65x_renormalize_left
done:
	.endscope
	
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
	
	pla
	sta _s0
	pla
	sta _s1
	pla
	sta _s2
	
	rts
#endif

#ifdef L_fixsfsi
	.import __fixunssfsi
	.export __fixsfsi
__fixsfsi:
	lda _r2
	and #$80
	sta _m65x_fpe0_sign
	
	jsr __fixunssfsi

	lda _m65x_fpe0_sign
	bpl not_negative
	ldx #0
	txa
	sec
	sbc _r0
	sta _r0
	txa
	sbc _r1
	sta _r1
	txa
	sbc _r2
	sta _r2
	txa
	sbc _r3
	sta _r3
not_negative:

	rts
#endif

#ifdef L_fixunssfsi
	.export __fixunssfsi
__fixunssfsi:
	lda _r2
	and #$7f
	ldx _r3
	beq a_exp_zero
	ora #$80
a_exp_zero:
	sta _r2
	stx _m65x_fpe0_exp
	
	cpx #127
	bcs over_one
	lda #0
	sta _r0
	sta _r1
	sta _r2
	sta _r3
	rts
over_one:
	
	; r3 is part of the result now.
	lda #0
	sta _r3
	
	.scope
	lda #150
	sec
	sbc _m65x_fpe0_exp
	bcc shift_left
	; shifting right by 'A' places
	tax
	beq exit
right_shift_loop:
	lsr _r3
	ror _r2
	ror _r1
	ror _r0
	dex
	bne right_shift_loop
exit:
	rts
shift_left:
	eor #$ff
	tax
	inx
left_shift_loop:
	asl _r0
	rol _r1
	rol _r2
	rol _r3
	dex
	bne left_shift_loop
	.endscope
	
	rts
#endif

#ifdef L_floatunsisf
	.import _m65x_renormalize_left
	.import _m65x_renormalize_right
	.export __floatunsisf
__floatunsisf:
	; All zero, just return zero.
	lda _r0
	ora _r1
	ora _r2
	ora _r3
	bne notzero
	rts
notzero:

	lda #150
	sta _m65x_fpe0_exp
	lda #0
	sta _m65x_fpe0_mant

	lda _r0
	sta _m65x_fpe0_mant+1
	lda _r1
	sta _m65x_fpe0_mant+2
	ldx _r2
	stx _m65x_fpe0_mant+3
	lda _r3
	sta _m65x_fpe0_mant+4

	bne rightshift
	cpx #$80
	bcs rightshift
	; We need to shift left until the mantissa is normalized.
	jsr _m65x_renormalize_left
	jmp repack
rightshift:
	jsr _m65x_renormalize_right
repack:

	bit _m65x_fpe0_mant
	bpl no_rounding
	inc _m65x_fpe0_mant+1
	bne no_rounding
	inc _m65x_fpe0_mant+2
	bne no_rounding
	inc _m65x_fpe0_mant+3
	bne no_rounding
	inc _m65x_fpe0_mant+4
	jsr _m65x_renormalize_right
no_rounding:
	
	lda _m65x_fpe0_mant+1
	sta _r0
	lda _m65x_fpe0_mant+2
	sta _r1
	lda _m65x_fpe0_mant+3
	and #$7f
	sta _r2
	lda _m65x_fpe0_exp
	sta _r3

	rts
#endif

#ifdef L_floatsisf
	.import __floatunsisf
	.export __floatsisf
__floatsisf:
	lda _r3
	and #$80
	sta _m65x_fpe0_sign
	bpl not_negative
	ldx #0
	txa
	sec
	sbc _r0
	sta _r0
	txa
	sbc _r1
	sta _r1
	txa
	sbc _r2
	sta _r2
	txa
	sbc _r3
	sta _r3
not_negative:
	jsr __floatunsisf
	
	lda _r2
	ora _m65x_fpe0_sign
	sta _r2

	rts
#endif
