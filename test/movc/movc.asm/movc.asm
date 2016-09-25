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
; MOVC A, @A+DPTR

	mov dptr, #_data0
	mov a, #0
	movc a, @a+dptr
	cjne a, #0x55, 		_fail

	mov a, #1
	movc a, @a+dptr
	cjne a, #0xaa, 		_fail

;-------------------------------------------------------
; MOVC A, @A+PC

	mov A, #0x0A
	lcall _code0
	cjne a, #0x99,		_fail

	mov A, #0x0b
	lcall _code0
	cjne a, #0x33,		_fail

;-------------------------------------------------------
; MOVX @DPTR, A ;  MOVX A, @DPTR

	mov dptr, #0x0000
	mov A, #0xb6
	movx @dptr, a
	mov A, #0
	movx a, @dptr
	cjne a, #0xb6,		_fail

	mov dptr, #0xffff
	mov A, #0x89
	movx @dptr, a
	mov A, #0
	movx a, @dptr
	cjne a, #0x89,		_fail

;-------------------------------------------------------
; MOVX @R0, A ;  MOVX A, @R0

	mov r0, #0
	movx a, @r0
	cjne a, #0xb6,		_fail

	mov r0, #0x78
	mov A, #0xf4
	movx @r0, a
	mov a, #0
	movx a, @r0
	cjne a, #0xf4,		_fail

;-------------------------------------------------------
; MOVX @R1, A ;  MOVX A, @R1

	mov r1, #0x9a
	mov A, #0x39
	movx @r1, a
	mov a, #0
	movx a, @r1
	cjne a, #0x39,		_fail

	ljmp    _pass

;--------------------------------------------------------

	.org 0x1ff7
_code0:
	movc a, @a+pc
	ret

	.org 0x2000
_data0:
	.db 0x55, 0xAA, 0x99, 0x33

;--------------------------------------------------------

;       END

