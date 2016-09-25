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
; CPL C
	setb C
	jnc			_fail
	cpl C
	jc			_fail
	cpl C
	jnc			_fail

; -------------------------------------------------------
; CPL A
	mov  A, #0x99
        cjne A, #0x99,		_fail
	cpl  A
        cjne A, #0x66,		_fail

; -------------------------------------------------------
; CPL bit addr (bit addressable IRAM)

	; file bit addressable IRAM with 0xff
	mov R0, #0x20
loop0:  mov @R0, #0xff
	inc R0
	cjne R0, #0x30, 	loop0

	; bit addresses 0 - 7 (byte 0x20)
        cpl 0x00
	mov A, 0x20
	cjne A, #0xfe,		_fail

        cpl 0x01
	mov A, 0x20
	cjne A, #0xfc,		_fail

        cpl 0x02
	mov A, 0x20
	cjne A, #0xf8,		_fail

        cpl 0x03
	mov A, 0x20
	cjne A, #0xf0,		_fail

        cpl 0x04
	mov A, 0x20
	cjne A, #0xe0,		_fail

        cpl 0x05
	mov A, 0x20
	cjne A, #0xc0,		_fail

        cpl 0x06
	mov A, 0x20
	cjne A, #0x80,		_fail

        cpl 0x07
	mov A, 0x20
	cjne A, #0x00,		_fail
	
	; bit addresses 0 - 7 (byte 0x20)
        cpl 0x00
	mov A, 0x20
	cjne A, #0x01,		_fail2

        cpl 0x01
	mov A, 0x20
	cjne A, #0x03,		_fail2

        cpl 0x02
	mov A, 0x20
	cjne A, #0x07,		_fail2

        cpl 0x03
	mov A, 0x20
	cjne A, #0x0f,		_fail2

        cpl 0x04
	mov A, 0x20
	cjne A, #0x1f,		_fail2

        cpl 0x05
	mov A, 0x20
	cjne A, #0x3f,		_fail2

        cpl 0x06
	mov A, 0x20
	cjne A, #0x7f,		_fail2

        cpl 0x07
	mov A, 0x20
	cjne A, #0xff,		_fail2
	
	sjmp 			_pass2

_fail2: ljmp			_fail

_pass2:

; -------------------------------------------------------
; CPL bit addr (SFRs)

	; bit addresses 0xf0 - 0xf7 (B register)
	mov 0xf0, #0xff

        cpl 0xf0
	mov A, 0xf0
	cjne A, #0xfe,		_fail3

        cpl 0xf1
	mov A, 0xf0
	cjne A, #0xfc,		_fail3

        cpl 0xf2
	mov A, 0xf0
	cjne A, #0xf8,		_fail3

        cpl 0xf3
	mov A, 0xf0
	cjne A, #0xf0,		_fail3

        cpl 0xf4
	mov A, 0xf0
	cjne A, #0xe0,		_fail3

        cpl 0xf5
	mov A, 0xf0
	cjne A, #0xc0,		_fail3

        cpl 0xf6
	mov A, 0xf0
	cjne A, #0x80,		_fail3

        cpl 0xf7
	mov A, 0xf0
	cjne A, #0x00,		_fail3

	; bit addresses 0xf0 - 0xf7 (B register)
        cpl 0xf0
	mov A, 0xf0
	cjne A, #0x01,		_fail3

        cpl 0xf1
	mov A, 0xf0
	cjne A, #0x03,		_fail3

        cpl 0xf2
	mov A, 0xf0
	cjne A, #0x07,		_fail3

        cpl 0xf3
	mov A, 0xf0
	cjne A, #0x0f,		_fail3

        cpl 0xf4
	mov A, 0xf0
	cjne A, #0x1f,		_fail3

        cpl 0xf5
	mov A, 0xf0
	cjne A, #0x3f,		_fail3

        cpl 0xf6
	mov A, 0xf0
	cjne A, #0x7f,		_fail3

        cpl 0xf7
	mov A, 0xf0
	cjne A, #0xff,		_fail3


	sjmp 			_pass3


_fail3: ljmp			_fail

_pass3:

; -------------------------------------------------------

       	ljmp    		_pass

;       END

