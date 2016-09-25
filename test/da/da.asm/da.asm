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

	
	; No adjustment
        mov A, #0x09

	; make sure CY and AC flags are clear
	add A, #0
	da A
	jc			_fail

	cjne A, #0x09,		_fail

	; Adjust only low nibble on nibble > 9
        mov A, #0x0A
	add A, #0
	da A
	; jb  psw.cy,		_fail
	jb  psw.7,		_fail
	cjne A, #0x10,		_fail

	; Adjust only low nibble on AC
        mov A, #0x08
	add A, #00
	setb psw.6			; set AC bit
	da A
	jc           		_fail
	cjne A, #0x0e,		_fail

	; Adjust only high nibble on nibble > 9
	mov A, #0xB1
	add A, #0
	da A
	jnc			_fail
        cjne A, #0x11,		_fail
	
        ; Adjust only on high nibble, with carry set

	mov A, #0x92
	add A, #0
	setb psw.7			; set CY bit
	da A
	jnc			_fail
	cjne A, #0xf2,		_fail

	sjmp    _pass

;--------------------------------------------------------

;       END

