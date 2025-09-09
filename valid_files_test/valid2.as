MAIN:   mov #-1,r2          
        cmp r3,#5           
        add r6,r7           
        sub r1,r4           

        mcro MC_1
        inc r1
        inc r2
        inc r3
        dec r4
        mcroend

        lea STR,r6          
        clr r2             
        not r2             
        inc r2             
        dec Count          
        
        jmp Line           
        bne Line           
        jsr SUBR           
        red r1             
        prn #-5            
        prn r1              
        
        mov M1[r1][r7],LENGTH
        add r2,STR              
        
LOOP:   mov M1[r3][r3],r3      
        bne LOOP               
        
        lea HELLO,r1           
        cmp A,r1               
        
       
        mov A,r1               
        cmp A,r1                 
        add A,r0               
        sub #-4,r3
        
     
        .data 2,-51,+13,9
        
   
        .string "shalom\n"

        MC_1

        .mat [2][3]             
        .mat [2][2] 4,-5,7,9    
        
        .entry HELLO            
        .extern L3      
        
SUBR:   clr r2          
        inc r2
        mov A,r1
        red r1                  
        prn r1
        rts

        MC_1

        stop       

; Labels and data section
STR:    .string "abcdef"
LENGTH: .data 6,-9,15
K:      .data 22
M1:     .mat [2][5] -1,22,32
Count:  .data 0
HELLO:  add r7,B
A:      mov XYZ,r4
B:      bne XYZ
        stop
XYZ:    lea XYZ,r1
Line:   jmp Line