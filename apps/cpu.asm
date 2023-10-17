section .text

global _cpuid_id
global _is_support_vmx

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
    mov DWORD [rax], edx
    add QWORD rax,0x04
    mov DWORD [rax], ecx
    pop rbp
    ret

_is_support_vmx:
    mov eax, 0x01
    cpuid
    bt ecx, 0x05
    mov eax, 0x00
    jc vmx_support
    ret
vmx_support:
    mov eax, 0x01
    ret
