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
; RL A
	mov A, #0x85
	rl A
	jc			_fail
	cjne A, #0x0B,		_fail

;--------------------------------------------------------
; RLC A
	; carry clear
        clr C
	mov A, #0x77
	rlc A
	jc			_fail
	cjne A, #0xee,		_fail

	; carry clear then set
        clr C
	mov A, #0xbf
	rlc A
	jnc			_fail
	cjne A, #0x7e,		_fail

	; carry set then clear
	setb C
	mov A, #0x62
	rlc A
	jc			_fail
	cjne A, #0xc5,		_fail

	; carry set 
	setb C
	mov A, #0xc4
	rlc A
	jnc			_fail
	cjne A, #0x89,		_fail

;--------------------------------------------------------
; RR A

	mov A, #0x71
	rr A
	jc			_fail
	cjne A, #0xb8,		_fail

;--------------------------------------------------------
; RRC A

	; carry clear
        clr C
	mov A, #0xee
	rrc A
	jc			_fail
	cjne A, #0x77,		_fail

	; carry clear then set
        clr C
	mov A, #0x7f
	rrc A
	jnc			_fail
	cjne A, #0x3f,		_fail

	; carry set then clear
	setb C
	mov A, #0xca
	rrc A
	jc			_fail
	cjne A, #0xe5,		_fail

	; carry set 
	setb C
	mov A, #0x89
	rrc A
	jnc			_fail
	cjne A, #0xc4,		_fail


;--------------------------------------------------------

	ljmp    _pass

;       END

