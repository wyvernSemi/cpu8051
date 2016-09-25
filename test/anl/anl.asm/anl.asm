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
; ANL iram addr, A

        mov  A,		#0x55
	mov  0x60,      #0xFA
	anl  0x60,	A
	mov  A,		0x60
	cjne A, 	#0x50, 	_fail

; -------------------------------------------------------
; ANL iram addr, #data

	mov  0x61,	#0xb3
	anl  0x61,	#0xc5
	mov  A,		0x61
	cjne A, 	#0x81, 	_fail
	
; -------------------------------------------------------
; ANL A, #data
        mov  A, 	#0xC3
	anl  A, 	#0xE2
	cjne A, 	#0xC2, 	_fail

; -------------------------------------------------------
; ANL A, iram addr

	mov		0x70, 	#0x32
	mov A, 		#0x23
	anl A,		0x70
	cjne A,		#0x22,	_fail

; -------------------------------------------------------
; ANL A, @R0

	mov		0x71, 	#0x15
	mov		r0,	#0x71
	mov A,		#0x99
	anl A,		@r0
	cjne A,		#0x11,	_fail

; -------------------------------------------------------
; ANL A, @R1

	mov		0x72, 	#0xa9
	mov		r1,	#0x72
	mov A,		#0x3f
	anl A,		@r1
	cjne A,		#0x29,	_fail

; -------------------------------------------------------
; ANL A, R0

	mov		r0,	#0x25
	mov A,		#0xc1
	anl A,		r0
	cjne A,		#0x01,	_fail

; -------------------------------------------------------
; ANL A, R1

	mov		r1,	#0xa9
	mov A,		#0x27
	anl A,		r1
	cjne A,		#0x21,	_fail2

; -------------------------------------------------------
; ANL A, R2

	mov		r2,	#0x3C
	mov A,		#0x19
	anl A,		r2
	cjne A,		#0x18,	_fail2

; -------------------------------------------------------
; ANL A, R3

	mov		r3,	#0x75
	mov A,		#0x96
	anl A,		r3
	cjne A,		#0x14,	_fail2

; -------------------------------------------------------
; ANL A, R4

	mov		r4,	#0x19
	mov A,		#0x88
	anl A,		r4
	cjne A,		#0x08,	_fail2

; -------------------------------------------------------
; ANL A, R5

	mov		r5,	#0xf8
	mov A,		#0xe9
	anl A,		r5
	cjne A,		#0xe8,	_fail2

; -------------------------------------------------------
; ANL A, R6

	mov		r6,	#0x45
	mov A,		#0x46
	anl A,		r6
	cjne A,		#0x44,	_fail2

; -------------------------------------------------------
; ANL A, R7

	mov		r7,	#0x66
	mov A,		#0xbb
	anl A,		r7
	cjne A,		#0x22,	_fail2

; -------------------------------------------------------
; ANLC, bit addr

        mov		0x20, #0x80	; bit addr 0x07 set
	setb C
	anl C,		0x07
	jnc		_fail2
	anl C,		0x06
	jc		_fail2
	clr C
	anl C,		0x07
	jc		_fail2
	setb C
	anl C,		0x07
	jnc		_fail2

; -------------------------------------------------------
; ANLC, /bit addr

        mov		0x20, #0x7f	; bit addr 0x07 clear
	setb C
	anl C,		/0x07
	jnc		_fail2
	anl C,		/0x06
	jc		_fail2
	clr C
	anl C,		/0x07
	jc		_fail2
	setb C
	anl C,		/0x07
	jnc		_fail2

; -------------------------------------------------------
	ljmp    		_pass
_fail2:
        ljmp    		_fail

;       END

