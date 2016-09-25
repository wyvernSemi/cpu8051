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
; MOV @R0, #data

	mov r0, #0x64
	mov @r0, #0xaa
	mov A, 0x64
	cjne A, #0xaa,		_fail

;--------------------------------------------------------
; MOV @R1, #data

	mov r1, #0x55
	mov @r1, #0x32
	mov A, 0x55
	cjne A, #0x32,		_fail

;--------------------------------------------------------
; MOV @R0, A

	mov r0, #0x38
	mov A, #0xc7
	mov @r0, A
	mov r2, 0x38
	cjne r2, #0xc7,		_fail

;--------------------------------------------------------
; MOV @R1, A

	mov r1, #0x75
	mov A, #0xaf
	mov @r1, A
	mov r2, 0x75
	cjne r2, #0xaf,		_fail

;--------------------------------------------------------
; MOV @R0, iram addr

	mov r0, #0x62
	mov 0x63, #0x90
	mov @r0, 0x63
	mov r2, 0x63
	cjne r2, #0x90,		_fail

;--------------------------------------------------------
; MOV @R1, iram addr

	mov r1, #0x64
	mov 0x65, #0xb8
	mov @r1, 0x65
	mov r2, 0x65
	cjne r2, #0xb8,		_fail

;--------------------------------------------------------
; MOV A, #data

	mov A, # 0xf9
	cjne A, #0xf9,		_fail2

;--------------------------------------------------------
; MOV A, @R0

	mov r0, #0x3e
	mov 0x3e, #0x77
	mov A, @r0
	cjne A, #0x77,		_fail2
	
;--------------------------------------------------------
; MOV A, @R1

	mov r1, #0x4a
	mov 0x4a, #0x0b
	mov A, @r1
	cjne A, #0x0b,		_fail2
	
;--------------------------------------------------------
; MOV A, R0

	mov r0, #0x10
	mov A, r0
	cjne A, #0x10,		_fail2

;--------------------------------------------------------
; MOV A, R1

	mov r1, #0x11
	mov A, r1
	cjne A, #0x11,		_fail2

;--------------------------------------------------------
; MOV A, R2

	mov r2, #0x12
	mov A, r2
	cjne A, #0x12,		_fail2

;--------------------------------------------------------
; MOV A, R3

	mov r3, #0x13
	mov A, r3
	cjne A, #0x13,		_fail2

;--------------------------------------------------------
; MOV A, R4

	mov r4, #0x14
	mov A, r4
	cjne A, #0x14,		_fail2

;--------------------------------------------------------
; MOV A, R5

	mov r5, #0x15
	mov A, r5
	cjne A, #0x15,		_fail2

;--------------------------------------------------------
; MOV A, R6

	mov r6, #0x16
	mov A, r6
	cjne A, #0x16,		_fail2

;--------------------------------------------------------
; MOV A, R7

	mov r7, #0x17
	mov A, r7
	cjne A, #0x17,		_fail2

;--------------------------------------------------------
; MOV A, iram addr

	sjmp			_ok2
_fail2:	ljmp 			_fail
_ok2:

;--------------------------------------------------------
; MOV C, bit addr

	; Carry clear then set
	clr C
	mov 0x20, #0x40
	mov C, 0x06
	jnc			_fail3

	; Carry set then clear
	mov 0x20, #0xbf
	mov C, 0x06
	jc			_fail3

;--------------------------------------------------------
; MOV DPTR, #data16

	nop
	mov dptr, #0xAA55
	mov r0, 0x82
	cjne r0, #0x55,		_fail3
	mov r0, 0x83
	cjne r0, #0xAA,		_fail3

;--------------------------------------------------------
; MOV R0, #data

	mov r0, #0x30
	cjne r0, #0x30,		_fail3

;--------------------------------------------------------
; MOV R1, #data

	mov r1, #0x31
	cjne r1, #0x31,		_fail3

;--------------------------------------------------------
; MOV R2, #data

	mov r2, #0x32
	cjne r2, #0x32,		_fail3

;--------------------------------------------------------
; MOV R3, #data

	mov r3, #0x33
	cjne r3, #0x33,		_fail3

;--------------------------------------------------------
; MOV R4, #data

	mov r4, #0x34
	cjne r4, #0x34,		_fail3

;--------------------------------------------------------
; MOV R5, #data

	mov r5, #0x35
	cjne r5, #0x35,		_fail3

;--------------------------------------------------------
; MOV R6, #data

	mov r6, #0x36
	cjne r6, #0x36,		_fail3

;--------------------------------------------------------
; MOV R7, #data

	mov r7, #0x37
	cjne r7, #0x37,		_fail3

;--------------------------------------------------------
; MOV R0, A

	mov A, #0xa0
	mov r0, A
	cjne r0, #0xa0,		_fail3

;--------------------------------------------------------
; MOV R1, A

	mov A, #0xa1
	mov r1, A
	cjne r1, #0xa1,		_fail3

;--------------------------------------------------------
; MOV R2, A

	mov A, #0xa2
	mov r2, A
	cjne r2, #0xa2,		_fail3

;--------------------------------------------------------
; MOV R3, A

	mov A, #0xa3
	mov r3, A
	cjne r3, #0xa3,		_fail3

;--------------------------------------------------------
; MOV R4, A

	mov A, #0xa4
	mov r4, A
	cjne r4, #0xa4,		_fail3

;--------------------------------------------------------
; MOV R5, A

	mov A, #0xa5
	mov r5, A
	cjne r5, #0xa5,		_fail3

;--------------------------------------------------------
; MOV R6, A

	mov A, #0xa6
	mov r6, A
	cjne r6, #0xa6,		_fail3

;--------------------------------------------------------
; MOV R7, A

	mov A, #0xa7
	mov r7, A
	cjne r7, #0xa7,		_fail3

	sjmp		_ok3
_fail3:	ljmp 		_fail
_ok3:

;--------------------------------------------------------
; MOV R0, iram addr

	mov 0x40, #0xe0
	mov r0, 0x40
	cjne r0, #0xe0,		_fail4

;--------------------------------------------------------
; MOV R1, iram addr

	mov 0x41, #0xe1
	mov r1, 0x41
	cjne r1, #0xe1,		_fail4

;--------------------------------------------------------
; MOV R2, iram addr

	mov 0x42, #0xe2
	mov r2, 0x42
	cjne r2, #0xe2,		_fail4

;--------------------------------------------------------
; MOV R3, iram addr

	mov 0x43, #0xe3
	mov r3, 0x43
	cjne r3, #0xe3,		_fail4

;--------------------------------------------------------
; MOV R4, iram addr

	mov 0x44, #0xe4
	mov r4, 0x44
	cjne r4, #0xe4,		_fail4

;--------------------------------------------------------
; MOV R5, iram addr

	mov 0x45, #0xe5
	mov r5, 0x45
	cjne r5, #0xe5,		_fail4

;--------------------------------------------------------
; MOV R6, iram addr

	mov 0x46, #0xe6
	mov r6, 0x46
	cjne r6, #0xe6,		_fail4

;--------------------------------------------------------
; MOV R7, iram addr

	mov 0x47, #0xe7
	mov r7, 0x47
	cjne r7, #0xe7,		_fail4

;--------------------------------------------------------
; MOV bit addr, C

	; Set a bit with C
	mov 0x24, #0xaa
	setb C
	mov 0x20, C
	mov r0, 0x24
	cjne r0, #0xab,		_fail4

	; Clear a bit with C
	clr C
	mov 0x21, C
	mov r0, 0x24
	cjne r0, #0xa9,		_fail4

	sjmp		_ok4
_fail4:	ljmp 		_fail
_ok4:

;--------------------------------------------------------
; MOV iram addr, #data

	mov 0x77, #0x38
	mov r0, 0x77
	cjne r0, #0x38,		_fail5

;--------------------------------------------------------
; MOV iram addr, @R0

	mov r0, #0x62
	mov 0x62, #0xee
	mov 0x63, @r0
	mov r2, 0x63
	cjne r2, #0xee,		_fail5

;--------------------------------------------------------
; MOV iram addr, @R1

	mov r1, #0x65
	mov 0x65, #0x71
	mov 0x66, @r1
	mov r2, 0x66
	cjne r2, #0x71,		_fail5

;--------------------------------------------------------
; MOV iram addr, R0

	mov r0, #0xa0
	mov 0x50, r0
	mov A, 0x50
	cjne A, #0xa0,		_fail5

;--------------------------------------------------------
; MOV iram addr, R1

	mov r1, #0xa1
	mov 0x51, r1
	mov A, 0x51
	cjne A, #0xa1,		_fail5

;--------------------------------------------------------
; MOV iram addr, R2

	mov r2, #0xa2
	mov 0x52, r2
	mov A, 0x52
	cjne A, #0xa2,		_fail5

;--------------------------------------------------------
; MOV iram addr, R3

	mov r3, #0xa3
	mov 0x53, r3
	mov A, 0x53
	cjne A, #0xa3,		_fail5

;--------------------------------------------------------
; MOV iram addr, R4

	mov r4, #0xa4
	mov 0x54, r4
	mov A, 0x54
	cjne A, #0xa4,		_fail5

;--------------------------------------------------------
; MOV iram addr, R5

	mov r5, #0xa5
	mov 0x55, r5
	mov A, 0x55
	cjne A, #0xa5,		_fail5

;--------------------------------------------------------
; MOV iram addr, R6

	mov r6, #0xa6
	mov 0x56, r6
	mov A, 0x56
	cjne A, #0xa6,		_fail5

;--------------------------------------------------------
; MOV iram addr, R7

	mov r7, #0xa7
	mov 0x57, r7
	mov A, 0x57
	cjne A, #0xa7,		_fail5

;--------------------------------------------------------
; MOV iram addr, A

	mov A, #0xd9
	mov 0x5e, A
	mov r0, 0x5e
	cjne r0, #0xd9,		_fail5

;--------------------------------------------------------
; MOV iram addr, iram addr

	mov 0x3b, #0x47
	mov 0x76, 0x3b
	mov r0, 0x76
	cjne r0, #0x47,		_fail5

	ljmp    _pass

_fail5:	ljmp 		_fail


;       END

