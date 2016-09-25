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
; JC reladdr

	; Carry set (jump forward)
	setb C
	jc		_ok0
	ljmp		_fail
_ok0:   sjmp		_ok1

	; Carry set (jump backwards)
_ok2:	sjmp		_ok3
	nop;
	nop;
	nop;
_ok1:	jc		_ok2
	ljmp		_fail

	; Carry not set (no jump)
_ok3:	clr C
	jc		_fail2

;--------------------------------------------------------
; JNC reladdr

	; Carry not set (jump forward)
	clr C
	jnc		_ok4
	ljmp		_fail
_ok4:   sjmp		_ok5

	; Carry not set (jump backwards)
_ok6:	sjmp		_ok7
	nop;
	nop;
	nop;
_ok5:	jnc		_ok6
	ljmp		_fail

	; Carry set (no jump)
_ok7:	setb C
	jnc		_fail2


	ljmp    	_pass

_fail2: ljmp		_fail

;--------------------------------------------------------

;       END

