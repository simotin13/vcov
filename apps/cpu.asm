section .text

global _cpuid_id

; CPUID
_cpuid_id:
    push rbp
    mov rbp,rsp
    mov QWORD [rbp-0x08], rdi
    xor eax, eax
    cpuid
    mov rax, QWORD [rbp-0x08]
    mov DWORD [rax], ebx
    add QWORD rax,0x04
    mov DWORD [rax], ecx
    add QWORD rax,0x04
    mov DWORD [rax], edx
    ret
