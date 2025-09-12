;invalid8.as test file

; - - - - - FIRST PASS ERRORS - - - -
MAIN: mov r1,r2
      inc r7

;comma in end of line
      add #-7,

;comma in start of line
      cmp ,#12, r2

;extra comma between operands
        mov r1,,MAIN
        lea MAT1,,, r7


;too many operands
        stop r3
        rts #22
        jmp r1,r2

;dest operand as immediate
        jmp #-99
        not #+2

        bne r7
        rts

;invalid operand types
        lea r2, r8
        red #99

; not enough operands
        mov r1
        dec
        clr r2


;undefined directives
.numbers 1,2,3
.char 'c'


;  - - - - data - - - -
MAT1: .mat [3][5] 1,2,3,4
STR:  .string "new Strig\n"

;floating point number
      .data 12.5, 22, +23

 ;zero at mat size
      .mat [5][0]

 ;no first quotes
      .string hello wolrd"

 ;no second quotes
      .string "shalom