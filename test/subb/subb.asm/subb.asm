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
; SUBB A, #data
        ; all flags clear
        mov  A, 	#0x24
	subb  A, 	#0x03
	jc	       		_fail
	jb   		psw.7,  _fail	; PSW carry flag
	jb   		psw.6,  _fail	; PSW alternative carry
	jb   		psw.2,  _fail	; PSW overflow
	cjne A, 	#0x21, 	_fail

        ; AC set, all other flags clear
        mov  A, 	#0x21
	subb  A, 	#0x0f
	jc	       		_fail	; test carry flag clear
	jb   		psw.7,  _fail	; PSW carry flag
	jb   		psw.6,  lbl0 	; PSW alternative carry
	sjmp			_fail
lbl0:
	jb   		psw.2,  _fail	; PSW overflow
	cjne A, 	#0x12, 	_fail

        ; C set, all other flags clear
        mov  A, 	#0x15
	subb  A, 	#0xf1
	jc	       		lbl1	; test carry flag set
	sjmp			_fail
lbl1:
	jb   		psw.7,  lbl2 	; PSW carry flag
	sjmp			_fail
lbl2:
	jb   		psw.6,  _fail	; PSW alternative carry
	jb   		psw.2,  _fail	; PSW overflow
	cjne A, 	#0x24, 	_fail

        ; OV set, all other flags clear
        mov  A, 	#0x80
	subb  A, 	#0x10
	jc	       		_fail	; test carry flag set
	jb   		psw.7,  _fail	; PSW carry flag
	jb   		psw.6,  _fail	; PSW alternative carry
	jb   		psw.2,  lbl3 	; PSW overflow
	sjmp			_fail
lbl3:
	cjne A, 	#0x70, 	_fail

; -------------------------------------------------------
; SUBB A, iram addr

	mov		0x70, 	#0x23
	mov A, 		#0x33
	subb A,		0x70
	cjne A,		#0x10,	_fail

; -------------------------------------------------------
; SUBB A, @R0

	mov		0x71, 	#0x55
	mov		r0,	#0x71
	mov A,		#0x73
	subb A,		@r0
	cjne A,		#0x1e,	_fail

; -------------------------------------------------------
; SUBB A, @R1

	mov		0x72, 	#0xa9
	mov		r1,	#0x72
	mov A,		#0xa9
	subb A,		@r1
	cjne A,		#0x00,	_fail

; -------------------------------------------------------
; SUBB A, R0

	mov		r0,	#0x25
	mov A,		#0xc1
	subb A,		r0
	cjne A,		#0x9c,	_fail

; -------------------------------------------------------
; SUBB A, R1

	mov		r1,	#0x17
	mov A,		#0xa9
	subb A,		r1
	cjne A,		#0x92,	_fail2

; -------------------------------------------------------
; SUBB A, R2

	mov		r2,	#0x08
	mov A,		#0x18
	subb A,		r2
	cjne A,		#0x10,	_fail2

; -------------------------------------------------------
; SUBB A, R3

	mov		r3,	#0x72
	mov A,		#0x75
	subb A,		r3
	cjne A,		#0x03,	_fail2

; -------------------------------------------------------
; SUBB A, R4

	mov		r4,	#0x18
	mov A,		#0x19
	nop
	subb A,		r4
	cjne A,		#0x01,	_fail2

; -------------------------------------------------------
; SUBB A, R5

	mov		r5,	#0x08
	mov A,		#0xf7
	subb A,		r5
	cjne A,		#0xef,	_fail2

; -------------------------------------------------------
; SUBB A, R6

	mov		r6,	#0x44
	mov A,		#0x45
	subb A,		r6
	cjne A,		#0x01,	_fail2

; -------------------------------------------------------
; SUBB A, R7

	mov		r7,	#0x45
	mov A,		#0x66
	subb A,		r7
	cjne A,		#0x21,	_fail2

; -------------------------------------------------------
	ljmp    		_pass
_fail2:
        ljmp    		_fail

;       END

