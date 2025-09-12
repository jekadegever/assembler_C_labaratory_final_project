
;- - - -  file: invalid2.as - - - - -

;- - - - - - FIRST PASS ERRORS - - - - - -

        jmp r3
        inc r9

;unknown operations
        remove data

;matrix index not register
        jmp MAT1[2][3]
        inc MAT1[r12][A]

;immediate operand floating point
        add #22.3, r0



MAT1:   .mat [3][4] 1,2,3,4

;no mat size provided
MAT2:   .mat

 ;missing numbers in data
 DATA2:  .data

 ;data number invalid range
         .data +12345678,-1,5
         .mat [2][4] -1999999

 ;matrix format error
         .mat [r[r2] 1
         ;reg instead numeber
         .mat [5][r2] 3,4,5

 ;memory overflow
        .mat [30][6] 1,2,3


 ;too much numbers in mat
         .mat [2][2] 1,2,3,4,5,6

 ;chars at data declaration
         .data 1A,2,3
