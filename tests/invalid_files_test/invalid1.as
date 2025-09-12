;invalid1.as test file

; - - - - - FIRST PASS ERRORS - - - -
MAIN: mov r1,r2
      inc r7

;comma in end of line
      add #-7,

;too many operands

        jmp r1,r2

;dest operand as immediate
        jmp #-99


;invalid operand types
        lea r2, r8
        red #99




;  - - - - data - - - -
MAT1: .mat [3][5] 1,2,3,4
STR:  .string "new Strig\n"

;floating point number
      .data 12.5, 22, +23

 ;zero at mat size
      .mat [5][0]

 ;no first quotes
      .string hello wolrd"

