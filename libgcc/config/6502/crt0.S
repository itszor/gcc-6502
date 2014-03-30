        .feature at_in_identifiers

        .importzp _sp0, _sp1, _fp0, _fp1
        .importzp _r0, _r1, _r2, _r3, _r4, _r5, _r6, _r7
        .importzp _s0, _s1, _s2, _s3, _s4, _s5, _s6, _s7
        .importzp _tmp0, _tmp1

	.import main
	.import __BSS_RUN__
	.import __BSS_SIZE__

	.segment "STARTUP"
	lda #<$ffe0
	sta _sp0
	lda #>$ffe0
	sta _sp1
	; Clear BSS.
	lda #<__BSS_RUN__
	sta _r0
	lda #>__BSS_RUN__
	sta _r1
	lda #<__BSS_SIZE__
	sta _r2
	ldx #>__BSS_SIZE__
	stx _r3
	ora _r3
	beq nobss
	.scope
	ldy #0
loop:
	lda #0
	sta (_r0),y
	inc _r0
	bne :+
	inc _r1
:	dec _r2
	lda _r2
	cmp #255
	bne :+
	dec _r3
:	ora _r3
	bne loop
	.endscope
nobss:
	lda #0
	sta _r0
	sta _r1
	sta _r2
	sta _r3
	jsr main
	; Retrieve exit code
	lda _r0
	; This is a special hack to exit the semi65x emulator.
	jmp 0
