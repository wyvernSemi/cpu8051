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
; ACALL

lbl0:	mov	a, #0xff
        acall   _page0
        cjne    a, #0x00, _fail

lbl1:   mov	a, #0xff
        acall   _page1
        cjne    a, #0x01, _fail

lbl2:   mov	a, #0xff
        acall   _page2
        cjne    a, #0x02, _fail

lbl3:   mov	a, #0xff
        acall   _page3
        cjne    a, #0x03, _fail

lbl4:   mov	a, #0xff
        acall   _page4
        cjne    a, #0x04, _fail

lbl5:   mov	a, #0xff
        acall   _page5
        cjne    a, #0x05, _fail

lbl6:   mov	a, #0xff
        acall   _page6
        cjne    a, #0x06, _fail

lbl7:   mov	a, #0xff
        acall   _page7
        cjne    a, #0x07, _fail

;--------------------------------------------------------
; LCALL

	mov	a, #0xff
	lcall	_dest0
	cjne	a, #0x08, _fail


	sjmp    _pass

;--------------------------------------------------------

_page0: mov	a, #0x00
        ret            

        .org    0x100
_page1: mov	a, #0x01
        ret

        .org    0x200
_page2: mov	a, #0x02
        ret

        .org    0x300
_page3: mov	a, #0x03
        ret

        .org    0x400
_page4: mov	a, #0x04
        ret

        .org    0x500
_page5: mov	a, #0x05
        ret

        .org    0x600
_page6: mov	a, #0x06
        ret

        .org    0x700
_page7: mov	a, #0x07
        ret

	.org 	0x800
_dest0:	mov     a, #0x08
	ret


;--------------------------------------------------------

;       END

