section .text

global _vmxon
global _vmxoff
global _read_cr0
global _read_cr4
global _write_cr4
global _write_cr0
global _read_msr
global _read_msr_low
global _read_msr_high
global _write_msr

_vmxon:
    vmxon [rdi]
    jbe _vmx_failure
    jmp _vmx_success

_vmxoff:
    vmxoff
    jbe _vmx_failure
    jmp _vmx_success

_vmx_failure:
    mov rax, 0x00
    ret

_vmx_success:
    mov rax, 0xFF
    ret

; =============================================================================
; Read/Write Controll Register
; =============================================================================
_read_cr0:
    mov rax, cr0
    ret

_read_cr4:
    mov rax, cr4
    ret

_write_cr0:
    mov cr0, rdi
    ret

_write_cr4:
    mov cr4, rdi
    ret

_read_msr:
    push rcx
    mov ecx, edi
    rdmsr
    mov ebx, edx
    shl rbx, 32
    or rax, rbx
    pop rcx
    ret

_read_msr_low:
    push rcx
    mov ecx, edi
    rdmsr
    pop rcx
    ret

_read_msr_high:
    push rcx
    mov ecx, edi
    rdmsr
    mov eax, edx
    pop rcx
    ret

_write_msr:
    push rcx
    mov ecx, edi
    mov rdx, rsi
    mov rbx, 0xFFFFFFFF00000000
    and rdx, rbx
    shr rdx, 32
    mov rax, rsi
    mov rbx, 0x00000000FFFFFFFF
    and rax, rbx
    wrmsr
    pop rcx
    ret
