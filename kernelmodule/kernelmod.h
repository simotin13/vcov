#ifndef _KERNELMOD_H_
#define _KERNELMOD_H_
#include <linux/ioctl.h>

// ioctl I/F
typedef struct _VmmCtrl{
    unsigned long long addr;
    unsigned long long val;
} VmmCtrl;

// ioctl command
// dir:2bit, size:14bit, type:8bit, nr:8bit
#define IOC_MAGIC 'v'
#define VMM_READ_CR0        _IOR(IOC_MAGIC, 0x00, VmmCtrl)
#define VMM_READ_CR4        _IOR(IOC_MAGIC, 0x04, VmmCtrl)
#define VMM_READ_MSR        _IOR(IOC_MAGIC, 0x05, VmmCtrl)
#define VMM_WRITE_CR0       _IOW(IOC_MAGIC, 0x10, VmmCtrl)
#define VMM_WRITE_CR4       _IOW(IOC_MAGIC, 0x14, VmmCtrl)
#define VMM_WRITE_MSR       _IOW(IOC_MAGIC, 0x15, VmmCtrl)
#define VMM_VMXOFF          _IOW(IOC_MAGIC, 0x0100, VmmCtrl)
#define VMM_VMXON           _IOW(IOC_MAGIC, 0x0101, VmmCtrl)

#define VMM_ENABLE_VMXE     _IO(IOC_MAGIC, 0x0200)
#define VMM_DISABLE_VMXE    _IO(IOC_MAGIC, 0x0201)

// msr vmx related addrs
#define MSR_IA32_FEATURE_CONTROL                    (58)
#define MSR_REG_ADDR_IA32_VMX_BASIC                 (1152)
#define MSR_REG_ADDR_IA32_VMX_PINBASED_CTLS         (1153)
#define MSR_REG_ADDR_IA32_VMX_PROCBASED_CTLS        (1154)
#define MSR_REG_ADDR_IA32_VMX_EXIT_CTLS             (1155)
#define MSR_REG_ADDR_IA32_VMX_ENTRY_CTLS            (1156)
#define MSR_REG_ADDR_IA32_VMX_MISC                  (1157)
#define MSR_REG_ADDR_IA32_VMX_CR0_FIXED0            (1158)
#define MSR_REG_ADDR_IA32_VMX_CR0_FIXED1            (1159)
#define MSR_REG_ADDR_IA32_VMX_CR4_FIXED0            (1160)
#define MSR_REG_ADDR_IA32_VMX_CR4_FIXED1            (1161)
#define MSR_REG_ADDR_IA32_VMX_VMCS_ENUM             (1162)
#define MSR_REG_ADDR_IA32_VMX_VMCS_PROCBASED_CTLS2  (1163)

// msr
#define MSR_MASK_LOCK_IA32_FEATURE_CONTROL          (0x01)
#define MSR_MASK_FEATURE_CONTROL_VMX_EN             (0x04)
#endif  // _KERNELMOD_H_
