        .importzp _sp0, _sp1, _fp0, _fp1
        .importzp _s0, _s1, _s2, _s3, _s4, _s5, _s6, _s7
        .importzp _tmp0, _tmp1

	.export __m65x_savestack
__m65x_savestack:
	pla
	sta _tmp1
	pla
	sta _tmp0
	lda _sp1
	pha
	lda _sp0
	pha
	lda _s7
	pha
	lda _s6
	pha
	lda _s5
	pha
	lda _s4
	pha
	lda _s3
	pha
	lda _s2
	pha
	lda _s1
	pha
	lda _s0
	pha
	jmp (_tmp0)

	.export __m65x_restorestack_return
__m65x_restorestack_return:
	pla
	sta _s0
	pla
	sta _s1
	pla
	sta _s2
	pla
	sta _s3
	pla
	sta _s4
	pla
	sta _s5
	pla
	sta _s6
	pla
	sta _s7
	pla
	sta _sp0
	pla
	sta _sp1
	rts
