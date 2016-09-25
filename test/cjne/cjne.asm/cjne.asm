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
; CJNE A, #data, reladdr

	; op1 > op2 (carry clear)
	setb C
	mov  A, #0x22
	cjne A, #0x21, lbl1
lbl0:	ljmp 			_fail
lbl1:   jc			lbl0

	; op1 < op2 (carry set)
	clr  C
	mov  A, #0xb0
	cjne A, #0xb1, lbl3
lbl2:	ljmp 			_fail
lbl3:   jnc			lbl2

	; op1 == op2 (carry clear)
	setb C
	mov  A, #0x2c
	cjne A, #0x2c,		_fail
	jc                      lbl2

; -------------------------------------------------------
; CJNE A, iram addr, reladdr

       ; op1 != op2
       mov  A, #0x45
       mov  0x55, #0x7f
       cjne A, 0x55, 		lbl4
       ljmp			_fail

lbl4:  ; op1 == op2
	mov  A, #0xb3
	mov 0x56, #0xb3
	cjne A, 0x56,           _fail

; -------------------------------------------------------
; CJNE @R0, #data, reladdr

	; op1 != op2
	mov  0x57, #0x10
	mov  r0, #0x57
	cjne @r0, #0x39,	lbl6
lbl5:	ljmp			_fail

lbl6:  ; op1 == op2
	mov  0x58, #0x80
	mov  r0, #0x58
	cjne @r0, #0x80,	lbl5

; -------------------------------------------------------
; CJNE @R1, #data, reladdr

	; op1 != op2
	mov  0x58, #0xf7
	mov  r1, #0x58
	cjne @r1, #0x1c,	lbl8
lbl7:	ljmp			_fail

lbl8:  ; op1 == op2
	mov  0x58, #0x7f
	mov  r1, #0x58
	cjne @r1, #0x7f,	lbl7

; -------------------------------------------------------
; CJNE R0, #data, reladdr

	; op1 != op2
	mov  r0, #0x45
	cjne r0, #0x46,		lbl10
lbl9:	ljmp			_fail

lbl10:  ; op1 == op2
	mov  r0, #0x66
	cjne r0, #0x66,		lbl9

; -------------------------------------------------------
; CJNE R1, #data, reladdr

	; op1 != op2
	mov  r1, #0xa8
	cjne r1, #0x32,		lbl12
lbl11:	ljmp			_fail

lbl12:  ; op1 == op2
	mov  r1, #0x40
	cjne r1, #0x40,		lbl11

; -------------------------------------------------------
; CJNE R2, #data, reladdr

	; op1 != op2
	mov  r2, #0x00
	cjne r2, #0xff,		lbl14
lbl13:	ljmp			_fail

lbl14:  ; op1 == op2
	mov  r2, #0xce
	cjne r2, #0xce,		lbl13

; -------------------------------------------------------
; CJNE R3, #data, reladdr

	; op1 != op2
	mov  r3, #0x10
	cjne r3, #0x77,		lbl16
lbl15:	ljmp			_fail

lbl16:  ; op1 == op2
	mov  r3, #0x00
	cjne r3, #0x00,		lbl15

; -------------------------------------------------------
; CJNE R4, #data, reladdr

	; op1 != op2
	mov  r4, #0x01
	cjne r4, #0x22,		lbl18
lbl17:	ljmp			_fail

lbl18:  ; op1 == op2
	mov  r4, #0xd6
	cjne r4, #0xd6,		lbl17

; -------------------------------------------------------
; CJNE R5, #data, reladdr

	; op1 != op2
	mov  r5, #0xa8
	cjne r5, #0x51,		lbl20
lbl19:	ljmp			_fail

lbl20:  ; op1 == op2
	mov  r5, #0x09
	cjne r5, #0x09,		lbl19

; -------------------------------------------------------
; CJNE R6, #data, reladdr

	; op1 != op2
	mov  r6, #0x19
	cjne r6, #0x88,		lbl22
lbl21:	ljmp			_fail

lbl22:  ; op1 == op2
	mov  r6, #0x2c
	cjne r6, #0x2c,		lbl21

; -------------------------------------------------------
; CJNE R7, #data, reladdr

	; op1 != op2
	mov  r7, #0xdd
	cjne r7, #0x09,		lbl24
lbl23:	ljmp			_fail

lbl24:  ; op1 == op2
	mov  r7, #0xfe
	cjne r7, #0xfe,		lbl23

; -------------------------------------------------------
	ljmp    		_pass

;       END

