
   ; - - - -file invalid3.as - - - -

; - - - - - - - - - LABEL ERRORS - - - - - - - - -
;label name too long
LABAAAAAAAAAAAAAAAAAAAAAAAAAAAA: mov r1,r2

;macro declaration

;error be printed on invalid line defined in macro body on macro call

        mcro MC_A
        jmp r2
        jmp r1
        rts
        move r10
        mcroend



;invalid label name
_LAB: jmp r2

K!2: jmp r3



;no line after label
LABEL2:



;declaration with used name
MAT1: jmp r5

MAT1: inc r1

jmp: jmp r2

data: .mat [1][1] 1




;error using macro with invalid line
MC_A
jmp r5
stop
MC_A

;extern label already declared in current file
      .extern MAT1

;extra tokens after extern label
       .extern MAT 21

;no extern label
       .extern