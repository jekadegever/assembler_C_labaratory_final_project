
    ; - - - -file invalid4.as - - - -

; - - - - - - macro errors - - - - -

        mcro MC_A
        mov r1,r2
        jmp r3
        stop
        mcroend



;mcro name already used
        mcro MC_A
        rst
        mcroend



;tokens after macro name
        mcro ABC abc
        jmp r3
        mcroend



;macro name missing
        mcro
        jmp r1
        mcroend



;mcro end missing
        mcro QQ
        stop
        mcroe

        jmp r3



;name after mcroend
        mcro ABF
        rst
        mcroend adl
