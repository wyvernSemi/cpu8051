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

;--------------------------------------------------------
; Main test program here


;--------------------------------------------------------
; DJNZ iram addr, reladdr

	; jump backwards
	mov 0x50, #0x6a
	sjmp 			_ok2
_ok0:   sjmp			_ok1
_ok2:	djnz 0x50,		_ok0
	ljmp			_fail
_ok1:	mov A, 0x50
	cjne A, #0x69,		_fail

	; jump forwards
	mov 0x51, #0x99
	djnz 0x51,		_ok3
	ljmp 			_fail
_ok3:   mov A, 0x51
	cjne A, #0x98,		_fail

	; no jump
	mov 0x52, #1
	djnz 0x52,		_fail
        mov A, 0x52
	cjne A, #0,		_fail

;--------------------------------------------------------
; DJNZ r0, reladdr

	; jump
	mov r0, #0x20
	djnz r0,		_ok4
	ljmp			_fail
_ok4:   cjne r0, #0x1f,		_fail

	; No jump
	mov r0, #1
	djnz r0,		_fail

;--------------------------------------------------------
; DJNZ r1, reladdr

	; jump
	mov r1, #0x67
	nop
	djnz r1,		_ok5
	ljmp			_fail
_ok5:   cjne r1, #0x66,		_fail

	; No jump
	mov r1, #1
	djnz r1,		_fail

;--------------------------------------------------------
; DJNZ r2, reladdr

	; jump
	mov r2, #0xa1
	nop
	djnz r2,		_ok6
	ljmp			_fail
_ok6:   cjne r2, #0xa0,		_fail

	; No jump
	mov r2, #1
	djnz r2,		_fail

;--------------------------------------------------------
; DJNZ r3, reladdr

	; jump
	mov r3, #0x02
	nop
	djnz r3,		_ok7
	ljmp			_fail
_ok7:   cjne r3, #0x01,		_fail

	; No jump
	mov r3, #1
	djnz r3,		_fail

;--------------------------------------------------------
; DJNZ r4, reladdr

	; jump
	mov r4, #0xff
	nop
	djnz r4,		_ok8
	ljmp			_fail
_ok8:   cjne r4, #0xfe,		_fail

	; No jump
	mov r4, #1
	djnz r4,		_fail

;--------------------------------------------------------
; DJNZ r5, reladdr

	; jump
	mov r5, #0x5c
	nop
	djnz r5,		_ok9
	ljmp			_fail2
_ok9:   cjne r5, #0x5b,		_fail2

	; No jump
	mov r5, #1
	djnz r5,		_fail2

;--------------------------------------------------------
; DJNZ r6, reladdr

	; jump
	mov r6, #0x08
	nop
	djnz r6,		_ok10
	ljmp			_fail2
_ok10:  cjne r6, #0x07,		_fail2

	; No jump
	mov r6, #1
	djnz r6,		_fail2

;--------------------------------------------------------
; DJNZ r7, reladdr

	; jump
	mov r7, #0xb4
	nop
	djnz r7,		_ok11
	ljmp			_fail2
_ok11:  cjne r7, #0xb3,		_fail2

	; No jump
	mov r7, #1
	djnz r7,		_fail2

	ljmp    _pass
_fail2: ljmp    _fail


;--------------------------------------------------------

;       END

