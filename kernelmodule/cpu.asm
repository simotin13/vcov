section .text

global _vmxon
global _vmxoff
global _vmptrld
global _vmwrite
global _vmread
global _enable_vmxe
global _disable_vmxe
global _read_cr0
global _read_cr4
global _write_cr4
global _write_cr0
global _read_msr
global _read_msr_low
global _read_msr_high
global _write_msr

global _get_reg_rax
global _get_reg_rbx
global _get_reg_cs
global _get_reg_ds
global _get_reg_es
global _get_reg_ss
global _get_reg_fs
global _get_reg_gs
global _get_reg_tr

_vmxon:
    vmxon [rdi]
    jbe _vmx_failure
    jmp _vmx_success

_vmxoff:
    vmxoff
    jbe _vmx_failure
    jmp _vmx_success

_vmwrite:
    vmwrite rdi, rsi
    jbe _vmx_failure
    jmp _vmx_success

_vmread:
    vmread rax, rdi
    ret

_vmptrld:
    vmptrld [rdi]
    jbe _vmx_failure
    jmp _vmx_success

_vmx_failure:
    mov rax, 0xFF
    ret

_vmx_success:
    mov rax, 0x00
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

_enable_vmxe:
    mov rax, cr4
    or rax, 0x2000
    mov cr4, rax
    ret

_disable_vmxe:
    mov rax, cr4
    and rax, 0xFFFFDFFF
    mov cr4, rax
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

_get_reg_rax:
    ret

_get_reg_rbx:
    mov rax, rbx
    ret

_get_reg_cs:
    mov rax, cs
    ret

_get_reg_ds:
    mov rax, ds
    ret

_get_reg_es:
    mov rax, ds
    ret

_get_reg_ss:
    mov rax, ss
    ret

_get_reg_fs:
    mov rax, ss
    ret

_get_reg_gs:
    mov rax, gs
    ret
