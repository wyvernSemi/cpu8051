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
; ADD A, #data
        ; all flags clear
        mov  A, 	#0x03
	add  A, 	#0x20
	jc	       		_fail
	jb   		psw.7,  _fail	; PSW carry flag
	jb   		psw.6,  _fail	; PSW alternative carry
	jb   		psw.2,  _fail	; PSW overflow
	cjne A, 	#0x23, 	_fail

        ; AC set, all other flags clear
        mov  A, 	#0x0f
	add  A, 	#0x21
	jc	       		_fail	; test carry flag clear
	jb   		psw.7,  _fail	; PSW carry flag
	jb   		psw.6,  lbl0 	; PSW alternative carry
	sjmp			_fail
lbl0:
	jb   		psw.2,  _fail	; PSW overflow
	cjne A, 	#0x30, 	_fail

        ; C set, all other flags clear
        mov  A, 	#0x10
	add  A, 	#0xf1
	jc	       		lbl1	; test carry flag set
	sjmp			_fail
lbl1:
	jb   		psw.7,  lbl2 	; PSW carry flag
	sjmp			_fail
lbl2:
	jb   		psw.6,  _fail	; PSW alternative carry
	jb   		psw.2,  _fail	; PSW overflow
	cjne A, 	#0x01, 	_fail

        ; OV set, all other flags clear
        mov  A, 	#0x70
	add  A, 	#0x10
	jc	       		_fail	; test carry flag set
	jb   		psw.7,  _fail	; PSW carry flag
	jb   		psw.6,  _fail	; PSW alternative carry
	jb   		psw.2,  lbl3 	; PSW overflow
	sjmp			_fail
lbl3:
	cjne A, 	#0x80, 	_fail

; -------------------------------------------------------
; ADD A, iram addr

	mov		0x70, 	#0x22
	mov A, 		#0x33
	add A,		0x70
	cjne A,		#0x55,	_fail

; -------------------------------------------------------
; ADD A, @R0

	mov		0x71, 	#0x15
	mov		r0,	#0x71
	mov A,		#0x99
	add A,		@r0
	cjne A,		#0xAE,	_fail

; -------------------------------------------------------
; ADD A, @R1

	mov		0x72, 	#0xa9
	mov		r1,	#0x72
	mov A,		#0x3f
	add A,		@r1
	cjne A,		#0xE8,	_fail

; -------------------------------------------------------
; ADD A, R0

	mov		r0,	#0x25
	mov A,		#0xc1
	add A,		r0
	cjne A,		#0xE6,	_fail

; -------------------------------------------------------
; ADD A, R1

	mov		r1,	#0xa9
	mov A,		#0x17
	add A,		r1
	cjne A,		#0xC0,	_fail2

; -------------------------------------------------------
; ADD A, R2

	mov		r2,	#0x08
	mov A,		#0x18
	add A,		r2
	cjne A,		#0x20,	_fail2

; -------------------------------------------------------
; ADD A, R3

	mov		r3,	#0x75
	mov A,		#0x82
	add A,		r3
	cjne A,		#0xf7,	_fail2

; -------------------------------------------------------
; ADD A, R4

	mov		r4,	#0x19
	mov A,		#0x88
	add A,		r4
	cjne A,		#0xA1,	_fail2

; -------------------------------------------------------
; ADD A, R5

	mov		r5,	#0xf8
	mov A,		#0x07
	add A,		r5
	cjne A,		#0xff,	_fail2

; -------------------------------------------------------
; ADD A, R6

	mov		r6,	#0x45
	mov A,		#0x46
	add A,		r6
	cjne A,		#0x8B,	_fail2

; -------------------------------------------------------
; ADD A, R7

	mov		r7,	#0x66
	mov A,		#0xbb
	add A,		r7
	cjne A,		#0x21,	_fail2

; -------------------------------------------------------
	ljmp    		_pass
_fail2:
        ljmp    		_fail

;       END

