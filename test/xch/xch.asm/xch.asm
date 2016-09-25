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
; SWAP A

	mov A, #0x5a
	swap A
	cjne A, #0xa5,		_fail

;--------------------------------------------------------
; XCH A, @R0

	mov A, #0x8e
	mov 0x55, #0x3f
	mov r0, #0x55
	xch A, @r0
	cjne A, #0x3f,		_fail
	mov r2, 0x55
	cjne r2, #0x8e,		_fail

;--------------------------------------------------------
; XCH A, @R1

	mov A, #0xcc
	mov 0x56, #0x42
	mov r1, #0x56
	xch A, @r1
	cjne A, #0x42,		_fail
	mov r2, 0x56
	cjne r2, #0xcc,		_fail

;--------------------------------------------------------
; XCH A, R0

	mov A, #0x20
	mov r0, #0xb0
	xch A, r0
	cjne A, #0xb0,		_fail
	cjne r0, #0x20,		_fail

;--------------------------------------------------------
; XCH A, R1

	mov A, #0x21
	mov r1, #0xb1
	xch A, r1
	cjne A, #0xb1,		_fail
	cjne r1, #0x21,		_fail

;--------------------------------------------------------
; XCH A, R2

	mov A, #0x22
	mov r2, #0xb2
	xch A, r2
	cjne A, #0xb2,		_fail
	cjne r2, #0x22,		_fail

;--------------------------------------------------------
; XCH A, R3

	mov A, #0x23
	mov r3, #0xb3
	xch A, r3
	cjne A, #0xb3,		_fail
	cjne r3, #0x23,		_fail

;--------------------------------------------------------
; XCH A, R0

	mov A, #0x24
	mov r4, #0xb4
	xch A, r4
	cjne A, #0xb4,		_fail
	cjne r4, #0x24,		_fail

;--------------------------------------------------------
; XCH A, R0

	mov A, #0x25
	mov r5, #0xb5
	xch A, r5
	cjne A, #0xb5,		_fail
	cjne r5, #0x25,		_fail

;--------------------------------------------------------
; XCH A, R0

	mov A, #0x26
	mov r6, #0xb6
	xch A, r6
	cjne A, #0xb6,		_fail
	cjne r6, #0x26,		_fail

;--------------------------------------------------------
; XCH A, R0

	mov A, #0x27
	mov r7, #0xb7
	xch A, r7
	cjne A, #0xb7,		_fail2
	cjne r7, #0x27,		_fail2

;--------------------------------------------------------
; XCH A, iram addr

	mov a, #0x2e
	mov 0x44, #0x90
	xch A, 0x44
	mov r7, 0x44
	cjne a, #0x90,		_fail2
	cjne r7, #0x2e,		_fail2

;--------------------------------------------------------
; XCHD A, @R0

	mov r0, #0x3a
	mov 0x3a, #0xb5
	mov A, #0x7c
	xchd A, @r0
	mov r7, 0x3a
	cjne A, #0x75,		_fail2
	cjne r7, #0xbc,		_fail2

;--------------------------------------------------------
; XCHD A, @R1

	mov r1, #0x63
	mov 0x63, #0xe9
	mov A, #0x04
	xchd A, @r1
	mov r7, 0x63
	cjne A, #0x09,		_fail2
	cjne r7, #0xe4,		_fail2

	ljmp    _pass

_fail2: ljmp 			_fail

;--------------------------------------------------------

;       END

