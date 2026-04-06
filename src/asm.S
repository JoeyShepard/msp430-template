.globl asm_test

asm_test:

    MOV #0x1234,&0x200
    MOV &0x200,R4

    MOV #0x1234, R4
    INC R4
    RET
