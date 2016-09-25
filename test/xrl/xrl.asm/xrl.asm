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
; XRL iram addr, A

        mov  A,		#0x55
	mov  0x60,      #0xFA
	xrl  0x60,	A
	mov  A,		0x60
	cjne A, 	#0xAF, 	_fail

; -------------------------------------------------------
; XRL iram addr, #data

	mov  0x61,	#0xb3
	nop
	xrl  0x61,	#0xc5
	mov  A,		0x61
	cjne A, 	#0x76, 	_fail
	
; -------------------------------------------------------
; XRL A, #data
        mov  A, 	#0xC3
	xrl  A, 	#0xE2
	cjne A, 	#0x21, 	_fail

; -------------------------------------------------------
; XRL A, iram addr

	mov		0x70, 	#0x32
	mov A, 		#0x23
	xrl A,		0x70
	cjne A,		#0x11,	_fail

; -------------------------------------------------------
; XRL A, @R0

	mov		0x71, 	#0x15
	mov		r0,	#0x71
	mov A,		#0x99
	xrl A,		@r0
	cjne A,		#0x8C,	_fail

; -------------------------------------------------------
; XRL A, @R1

	mov		0x72, 	#0xa9
	mov		r1,	#0x72
	mov A,		#0x3f
	xrl A,		@r1
	cjne A,		#0x96,	_fail

; -------------------------------------------------------
; XRL A, R0

	mov		r0,	#0x25
	mov A,		#0xc1
	xrl A,		r0
	cjne A,		#0xe4,	_fail

; -------------------------------------------------------
; XRL A, R1

	mov		r1,	#0xa9
	mov A,		#0x27
	xrl A,		r1
	cjne A,		#0x8e,	_fail2

; -------------------------------------------------------
; XRL A, R2

	mov		r2,	#0x3C
	mov A,		#0x19
	xrl A,		r2
	cjne A,		#0x25,	_fail2

; -------------------------------------------------------
; XRL A, R3

	mov		r3,	#0x75
	mov A,		#0x96
	xrl A,		r3
	cjne A,		#0xe3,	_fail2

; -------------------------------------------------------
; XRL A, R4

	mov		r4,	#0x19
	mov A,		#0x88
	xrl A,		r4
	cjne A,		#0x91,	_fail2

; -------------------------------------------------------
; XRL A, R5

	mov		r5,	#0xf8
	mov A,		#0xe9
	xrl A,		r5
	cjne A,		#0x11,	_fail2

; -------------------------------------------------------
; XRL A, R6

	mov		r6,	#0x45
	mov A,		#0x46
	xrl A,		r6
	cjne A,		#0x03,	_fail2

; -------------------------------------------------------
; XRL A, R7

	mov		r7,	#0x66
	mov A,		#0xbb
	xrl A,		r7
	cjne A,		#0xdd,	_fail2

; -------------------------------------------------------
	ljmp    		_pass
_fail2:
        ljmp    		_fail

;       END

