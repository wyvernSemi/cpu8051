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
; MUL AB

	; 51 * 5
	mov A, #0x33
	mov 0xf0, #0x05			; B = 5
	setb C
	mul AB
	jc			_fail
	cjne A, #0xff,		_fail

	; 8 * 32
	mov A, #0x08
	mov 0xf0, #0x20
	setb C
	mul AB
	jc			_fail
	jb psw.2,		_ok	; OV = 1
	ljmp			_fail
_ok:
	cjne A, #0x00,		_fail


; -------------------------------------------------------
	ljmp    		_pass

;       END

