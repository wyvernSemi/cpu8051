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
; INC A

	mov A, #0x66
	inc A
	cjne A, #0x67, _fail

; -------------------------------------------------------
; INC iram addr

	mov 0x70, # 0xff
	inc 0x70
	mov A, 0x70
	cjne A, #0x00, _fail

; -------------------------------------------------------
; INC @r0

	mov 0x71, #0xdf
	mov r0, #0x71
	inc @r0
	mov A, 0x71
	cjne A, #0xe0, _fail

; -------------------------------------------------------
; INC @r1

	mov 0x72, # 0xa8
	mov r1, #0x72
	inc @r1
	mov A, 0x72
	cjne A, #0xa9, _fail

; -------------------------------------------------------
; INC r0

	mov r0, #0x80
	inc r0
	cjne r0, #0x81, _fail

; -------------------------------------------------------
; INC r1

	mov r1, #0x81
	inc r1
	cjne r1, #0x82, _fail

; -------------------------------------------------------
; INC r2

	mov r2, #0x82
	inc r2
	cjne r2, #0x83, _fail

; -------------------------------------------------------
; INC r3

	mov r3, #0x83
	inc r3
	cjne r3, #0x84, _fail

; -------------------------------------------------------
; INC r4

	mov r4, #0x84
	inc r4
	cjne r4, #0x85, _fail

; -------------------------------------------------------
; INC r5

	mov r5, #0x85
	inc r5
	cjne r5, #0x86, _fail

; -------------------------------------------------------
; INC r6

	mov r6, #0x86
	inc r6
	cjne r6, #0x87, _fail

; -------------------------------------------------------
; INC r7

	mov r7, #0x87
	inc r7
	cjne r7, #0x88, _fail

; -------------------------------------------------------
; INC DPTR
	mov 0x82, #0xff			; DPL = 0xff
	mov 0x83, #0x28			; DPH = 0x28
	inc dptr
	mov A, 0x82
	cjne A, #0x00,		_fail
	mov A, 0x83
	cjne A, #0x29,		_fail
	
; -------------------------------------------------------
	ljmp    		_pass
_fail2:
        ljmp    		_fail

;       END

