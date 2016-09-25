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

	mov 0x70, #0xe4
	nop
	mov 0x71, 0x81			; Store stack pointer at 0x71
	push 0x70
	mov A, 0x71
	inc A
	cjne A, 0x81,		_fail
	mov r0, 0x81
	mov A, @r0
	cjne A, #0xe4,		_fail

	pop 0x75
	mov A, 0x75
	cjne A, #0xe4,		_fail
	mov A, 0x71
	cjne A, 0x81,		_fail

	ljmp    _pass

;--------------------------------------------------------

;       END

