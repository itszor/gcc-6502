        .feature at_in_identifiers
        .autoimport +
        .define _ah $70
        .define _ah2 $71
        .define _ah3 $72
        .define _xh $73
        .define _xh2 $74
        .define _xh3 $75
        .define _yh $76
        .define _yh2 $77
        .define _yh3 $78
        .define _sp $79
        .define _sph $7a
        .define _fp $7b
        .define _fph $7c
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

	.segment "STARTUP"
	lda #0
	sta _r0
	sta _r1
	sta _r2
	sta _r3
	jsr main
	; This is a special hack to exit the semi65x emulator.
	jmp 0
