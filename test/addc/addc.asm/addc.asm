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
; ADDC A, #data
        ; all flags clear
	setb C
        mov  A, 	#0x02
	addc A, 	#0x20
	jc	       		_fail
	jb   		psw.7,  _fail	; PSW carry flag
	jb   		psw.6,  _fail	; PSW alternative carry
	jb   		psw.2,  _fail	; PSW overflow
	cjne A, 	#0x23, 	_fail

        ; AC set, all other flags clear
	setb C
        mov  A, 	#0x0f
	addc A, 	#0x20
	jc	       		_fail	; test carry flag clear
	jb   		psw.7,  _fail	; PSW carry flag
	jb   		psw.6,  lbl0 	; PSW alternative carry
	sjmp			_fail
lbl0:
	jb   		psw.2,  _fail	; PSW overflow
	cjne A, 	#0x30, 	_fail

        ; C set, all other flags clear
	setb C
        mov  A, 	#0x10
	addc A, 	#0xf0
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
	setb C
        mov  A, 	#0x70
	addc A, 	#0x10
	jc	       		_fail	; test carry flag set
	jb   		psw.7,  _fail	; PSW carry flag
	jb   		psw.6,  _fail	; PSW alternative carry
	jb   		psw.2,  lbl3 	; PSW overflow
	sjmp			_fail
lbl3:
	cjne A, 	#0x81, 	_fail

; -------------------------------------------------------
; ADDC A, iram addr

	setb C
	mov		0x70, 	#0x22
	mov A, 		#0x33
	addc A,		0x70
	cjne A,		#0x56,	_fail

; -------------------------------------------------------
; ADDC A, @R0

	setb C
	mov		0x71, 	#0x15
	mov		r0,	#0x71
	mov A,		#0x99
	addc A,		@r0
	cjne A,		#0xAF,	_fail

; -------------------------------------------------------
; ADDC A, @R1

	setb C
	mov		0x72, 	#0xa9
	mov		r1,	#0x72
	mov A,		#0x3f
	addc A,		@r1
	cjne A,		#0xE9,	_fail

; -------------------------------------------------------
; ADDC A, R0

	setb C
	mov		r0,	#0x25
	mov A,		#0xc1
	addc A,		r0
	cjne A,		#0xE7,	_fail2

; -------------------------------------------------------
; ADDC A, R1

	setb C
	mov		r1,	#0xa9
	mov A,		#0x17
	addc A,		r1
	cjne A,		#0xC1,	_fail2

; -------------------------------------------------------
; ADDC A, R2

	setb C
	mov		r2,	#0x08
	mov A,		#0x18
	addc A,		r2
	cjne A,		#0x21,	_fail2

; -------------------------------------------------------
; ADDC A, R3

	setb C
	mov		r3,	#0x75
	mov A,		#0x82
	addc A,		r3
	cjne A,		#0xf8,	_fail2

; -------------------------------------------------------
; ADDC A, R4

	setb C
	mov		r4,	#0x19
	mov A,		#0x88
	addc A,		r4
	cjne A,		#0xA2,	_fail2

; -------------------------------------------------------
; ADDC A, R5

	setb C
	mov		r5,	#0xf8
	mov A,		#0x06
	addc A,		r5
	cjne A,		#0xff,	_fail2

; -------------------------------------------------------
; ADDC A, R6

	setb C
	mov		r6,	#0x45
	mov A,		#0x46
	addc A,		r6
	cjne A,		#0x8C,	_fail2

; -------------------------------------------------------
; ADDC A, R7

	setb C
	mov		r7,	#0x66
	mov A,		#0xbb
	addc A,		r7
	cjne A,		#0x22,	_fail2

; -------------------------------------------------------
	ljmp    		_pass
_fail2:
        ljmp    		_fail

;       END

