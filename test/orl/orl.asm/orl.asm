.area   SYS   (ABS)

; RESET VECTOR
        .org    0x0000
        ljmp    _setup

; EXTERNAL 0 Interrupt vector
	.org    0x0003
        ljmp    _halt

; TIMER 0 Interrupt vector
	.org    0x000B
        ljmp    _halt

; EXTERNAL 1 Interrupt vector
	.org    0x0013
        ljmp    _halt

; TIMER 1 Interrupt vector
	.org    0x001B
        ljmp    _halt

; SERIAL Interrupt vector
	.org    0x0023
        ljmp    _halt

; MAIN code area
        .org    0x30 

_setup: mov     SP, #0x7
        lcall   _main
_halt:  sjmp    _halt

_pass:  mov     0x7f, #0x99
        sjmp    _end
_fail:  mov     0x7f, #0xbb
_end:   ret

_main:  mov     0x7f, #0

; =======================================================
; Main test program here
; =======================================================

; -------------------------------------------------------
; ORL iram addr, A

        mov  A,		#0x55
	mov  0x60,      #0xFA
	orl  0x60,	A
	mov  A,		0x60
	cjne A, 	#0xFF, 	_fail

; -------------------------------------------------------
; ORL iram addr, #data

	mov  0x61,	#0xb3
	orl  0x61,	#0xc5
	mov  A,		0x61
	cjne A, 	#0xF7, 	_fail
	
; -------------------------------------------------------
; ORL A, #data
        mov  A, 	#0xC3
	orl  A, 	#0xE2
	cjne A, 	#0xE3, 	_fail

; -------------------------------------------------------
; ORL A, iram addr

	mov		0x70, 	#0x32
	mov A, 		#0x23
	orl A,		0x70
	cjne A,		#0x33,	_fail

; -------------------------------------------------------
; ORL A, @R0

	mov		0x71, 	#0x15
	mov		r0,	#0x71
	mov A,		#0x99
	orl A,		@r0
	cjne A,		#0x9d,	_fail

; -------------------------------------------------------
; ORL A, @R1

	mov		0x72, 	#0xa9
	mov		r1,	#0x72
	mov A,		#0x3f
	orl A,		@r1
	cjne A,		#0xbf,	_fail

; -------------------------------------------------------
; ORL A, R0

	mov		r0,	#0x25
	mov A,		#0xc1
	orl A,		r0
	cjne A,		#0xe5,	_fail

; -------------------------------------------------------
; ORL A, R1

	mov		r1,	#0xa9
	mov A,		#0x27
	orl A,		r1
	cjne A,		#0xaf,	_fail2

; -------------------------------------------------------
; ORL A, R2

	mov		r2,	#0x3C
	mov A,		#0x19
	orl A,		r2
	cjne A,		#0x3d,	_fail2

; -------------------------------------------------------
; ORL A, R3

	mov		r3,	#0x75
	mov A,		#0x96
	orl A,		r3
	cjne A,		#0xf7,	_fail2

; -------------------------------------------------------
; ORL A, R4

	mov		r4,	#0x19
	mov A,		#0x88
	orl A,		r4
	cjne A,		#0x99,	_fail2

; -------------------------------------------------------
; ORL A, R5

	mov		r5,	#0xf8
	mov A,		#0xe9
	orl A,		r5
	cjne A,		#0xf9,	_fail2

; -------------------------------------------------------
; ORL A, R6

	mov		r6,	#0x45
	mov A,		#0x46
	orl A,		r6
	cjne A,		#0x47,	_fail2

; -------------------------------------------------------
; ORL A, R7

	mov		r7,	#0x66
	mov A,		#0xbb
	orl A,		r7
	cjne A,		#0xff,	_fail2

; -------------------------------------------------------
; ORL C, bit addr

        mov		0x20, #0x80	; bit addr 0x07 set
	clr  C
	orl  C,		0x07
	jnc		_fail2
	clr  C
	orl  C,		0x06
	jc		_fail2
	setb C
	orl  C,		0x06
	jnc		_fail2

; -------------------------------------------------------
; ORL C, /bit addr

        mov		0x20, #0x7f	; bit addr 0x07 clear
	clr  C
	orl  C,		/0x07
	jnc		_fail2
	clr  C
	orl  C,		/0x06
	jc		_fail2
	setb C
	orl  C,		/0x06
	jnc		_fail2


; -------------------------------------------------------
	ljmp    		_pass
_fail2:
        ljmp    		_fail

;       END

