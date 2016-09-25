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
; JMP @A+DPTR

	mov 0x82, #0x7f
	mov 0x83, #0x01
	mov A,	  #0x01
	jmp @A+DPTR
	ljmp		_fail
ret8:	
	
;--------------------------------------------------------
; SJMP reladdr

	; jump forward
	sjmp		_ok3
	ljmp		_fail

	; jump backward
_ok1:	sjmp		_ok2
_ok3:   sjmp		_ok1
	ljmp		_fail
_ok2:

;--------------------------------------------------------
; LJMP code addr

	nop
	ljmp		_ok4  
	nop
	nop
	ljmp		_fail

_ok4:

;--------------------------------------------------------
; AJMP page<n>

lbl0:	mov	a, #0xff
        ajmp    _page0
ret0:   cjne    a, #0x00, _fail

lbl1:   mov	a, #0xff
        ajmp    _page1
ret1:   cjne    a, #0x01, _fail

lbl2:   mov	a, #0xff
        ajmp    _page2
ret2:   cjne    a, #0x02, _fail

lbl3:   mov	a, #0xff
        ajmp    _page3
ret3:   cjne    a, #0x03, _fail

lbl4:   mov	a, #0xff
        ajmp    _page4
ret4:   cjne    a, #0x04, _fail

lbl5:   mov	a, #0xff
        ajmp    _page5
ret5:   cjne    a, #0x05, _fail

lbl6:   mov	a, #0xff
        ajmp    _page6
ret6:   cjne    a, #0x06, _fail

lbl7:   mov	a, #0xff
        ajmp    _page7
ret7:   cjne    a, #0x07, _fail


	sjmp    _pass

_page0: mov	a, #0x00
        ljmp    ret0   

        .org    0x100
_page1: mov	a, #0x01
        ljmp    ret1

        .org    0x200
_page2: mov	a, #0x02
        ljmp    ret2

        .org    0x300
_page3: mov	a, #0x03
        ljmp    ret3

        .org    0x400
_page4: mov	a, #0x04
        ljmp    ret4

        .org    0x500
_page5: mov	a, #0x05
        ljmp    ret5

        .org    0x600
_page6: mov	a, #0x06
        ljmp    ret6

        .org    0x700
_page7: mov	a, #0x07
        ljmp    ret7


	.org    0x180
_dest0:	ljmp	ret8

;--------------------------------------------------------

;       END

