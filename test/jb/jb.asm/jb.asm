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
; JB bit addr, reladdr

	; Bit addressable RAM
	mov 0x29, #0xfb			; bit 2 not set
	jb 0x4A,		_fail
	jb 0x4B,		_ok0
	ljmp			_fail
_ok0:	mov A, 0x29
        cjne A, #0xfb,	_fail

	; Bit addressable SFR 
	mov 0x2f, #0xef			; bit 4 not set
	jb 0x7c,		_fail
	jb 0x78,		_ok1
	ljmp			_fail
_ok1:	mov A, 0x2f
        cjne A, #0xef,	_fail

;--------------------------------------------------------
; JBC bit addr, reladdr


	; Bit addressable RAM
	mov 0x29, #0xfb			; bit 2 not set
	jbc 0x4A,		_fail
	jbc 0x4B,		_ok2
	ljmp			_fail
_ok2:	mov A, 0x29
        cjne A, #0xf3,	_fail


	; Bit addressable SFR 
	mov 0x2f, #0xef			; bit 4 not set
	jbc 0x7c,		_fail
	jbc 0x78,		_ok3
	ljmp			_fail
_ok3:	mov A, 0x2f
        cjne A, #0xee,	_fail

;--------------------------------------------------------
; JNB bit addr, reladdr


	; Bit addressable RAM
	mov 0x29, #0xfb			; bit 2 not set
	jnb 0x4B,		_fail
	jnb 0x4A,		_ok4
	ljmp			_fail
_ok4:	mov A, 0x29
        cjne A, #0xfb,	_fail

	; Bit addressable SFR 
	mov 0x2f, #0xef			; bit 4 not set
	jnb 0x78,		_fail
	jnb 0x7c,		_ok5
	ljmp			_fail
_ok5:	mov A, 0x2f
        cjne A, #0xef,	_fail




	ljmp    _pass

;--------------------------------------------------------

;       END

