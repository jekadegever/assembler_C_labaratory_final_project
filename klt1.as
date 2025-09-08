mov r1,r2
HELLO: .data 1
.entry HELLO

.extern A
jmp A
mcro ABC
stop
jmpb
stop
mcroend
rts
rts
.extern HEL
jmp HEL
mcro qqq
bne r2
jmp r4
movr r1, HEL
mcroend
qqq
jmp r4
ABC
bne r5
qqq
ABC