
;======= file: valid1.as =======

;entry/ external declaration

    .entry LOOP
    .entry LENGTH
    .extern L3
    .extern W

; ----- macros ------
    mcro MC_1
    inc r1
    inc r2
    inc r3
    inc r4
    lea MAIN,r2
    mcroend

    mcro MC_2
    mov r2,r6
    jmp r1
    mcroend

;----- main ------
    MAIN: mov M1[r2][r7],W
    add r2,STR

    MC_1

    LOOP: jmp W
    prn #-5
    sub r1, r4
    inc K

    MC_2
    MC_1

    mov M1[r3][r3],r3
    bne L3
    END: stop

; ----- data declaration -----
    STR: .string "abcdef"
    LENGTH: .data 6,-9,15
    K: .data 22
    M1: .mat [2][2] 1,2,3,4
