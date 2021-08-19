
.set IRQ_BASE, 0x20
.section .text
.extern __ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj

.macro HandleInterruptRequest num
.global __ZN4myos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev
__ZN4myos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptNumber)
    pushl $0 # 获取硬件码
    jmp int_bottom
.endm

.macro HandleException num
.global  __ZN4myos21hardwarecommunication16InterruptManager19HandleException\num\()Ev
__ZN4myos21hardwarecommunication16InterruptManager19HandleException\num\()Ev:
    movb $\num, (interruptNumber)
    jmp int_bottom  # jmp 跳转到
.endm



HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x02
HandleInterruptRequest 0x03
HandleInterruptRequest 0x04
HandleInterruptRequest 0x05
HandleInterruptRequest 0x06
HandleInterruptRequest 0x07
HandleInterruptRequest 0x08
HandleInterruptRequest 0x09
HandleInterruptRequest 0x0A
HandleInterruptRequest 0x0B
HandleInterruptRequest 0x0C
HandleInterruptRequest 0x0D
HandleInterruptRequest 0x0E
HandleInterruptRequest 0x0F
HandleInterruptRequest 0x31

HandleException 0x00
HandleException 0x01
HandleException 0x02
HandleException 0x03
HandleException 0x04
HandleException 0x05
HandleException 0x06
HandleException 0x07
HandleException 0x08
HandleException 0x09
HandleException 0x0A
HandleException 0x0B
HandleException 0x0C
HandleException 0x0D
HandleException 0x0E
HandleException 0x0F
HandleException 0x10
HandleException 0x11
HandleException 0x12
HandleException 0x13

int_bottom:
    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    # pushl %ds
    # pushl %es
    # pushl %fs
    # pushl %gs

    pushl %esp
    push (interruptNumber)
    call __ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj

    movl %eax, %esp

    # popl %gs 这才是注释
    # popl %fs
    # popl %es
    # popl %ds
    popl %eax
    popl %ebx
    popl %ecx
    popl %edx

    popl %esi
    popl %edi
    popl %ebp

    add $4, %esp

.global __ZN4myos21hardwarecommunication16InterruptManager15InterruptIgnoreEv
__ZN4myos21hardwarecommunication16InterruptManager15InterruptIgnoreEv:

    iret

.data 
    interruptNumber: .byte 0



