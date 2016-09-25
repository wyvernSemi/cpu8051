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
; DEC A

	mov A, #0x66
	dec A
	cjne A, #0x65, _fail

; -------------------------------------------------------
; DEC iram addr

	mov 0x70, # 0x00
	dec 0x70
	mov A, 0x70
	cjne A, #0xff, _fail

; -------------------------------------------------------
; DEC @r0

	mov 0x71, #0xe0
	mov r0, #0x71
	dec @r0
	mov A, 0x71
	cjne A, #0xdf, _fail

; -------------------------------------------------------
; DEC @r1

	mov 0x72, # 0xa8
	mov r1, #0x72
	dec @r1
	mov A, 0x72
	cjne A, #0xa7, _fail

; -------------------------------------------------------
; DEC r0

	mov r0, #0x80
	dec r0
	cjne r0, #0x7f, _fail

; -------------------------------------------------------
; DEC r1

	mov r1, #0x81
	dec r1
	cjne r1, #0x80, _fail

; -------------------------------------------------------
; DEC r2

	mov r2, #0x82
	dec r2
	cjne r2, #0x81, _fail

; -------------------------------------------------------
; DEC r3

	mov r3, #0x83
	dec r3
	cjne r3, #0x82, _fail

; -------------------------------------------------------
; DEC r4

	mov r4, #0x84
	dec r4
	cjne r4, #0x83, _fail

; -------------------------------------------------------
; DEC r5

	mov r5, #0x85
	dec r5
	cjne r5, #0x84, _fail

; -------------------------------------------------------
; DEC r6

	mov r6, #0x86
	dec r6
	cjne r6, #0x85, _fail

; -------------------------------------------------------
; DEC r7

	mov r7, #0x87
	dec r7
	cjne r7, #0x86, _fail

; -------------------------------------------------------
	ljmp    		_pass
_fail2:
        ljmp    		_fail

;       END

