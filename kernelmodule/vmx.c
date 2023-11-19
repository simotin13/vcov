#include <asm/msr.h>
#include <linux/types.h>
#include <linux/mm.h>

#include "cpu.h"

#define VMX_PAGE_SIZE	(4096)

#define VMCS_ACCESS_FULL						(0)
#define VMCS_ACCESS_HIGH						(1)

#define VMCS_FIELD_TYPE_CONTROL					(0)
#define VMCS_FIELD_TYPE_VM_EXIT					(1)
#define VMCS_FIELD_TYPE_GUEST_STATE				(2)
#define VMCS_FIELD_TYPE_HOST_STATE				(3)

#define VMCS_FIELD_WIDTH_16BIT					(0)
#define VMCS_FIELD_WIDTH_64BIT					(1)
#define VMCS_FIELD_WIDTH_32BIT					(2)
#define VMCS_FIELD_WIDTH_NATURAL				(3)

#define VMCS_ENCODE_COMPONENT( access, type, width, index )     ( access ) | \
                                                                        ( ( unsigned short )( index ) << 1 ) | \
                                                                        ( ( unsigned short )( type ) << 10 ) | \
                                                                        ( ( unsigned short )( width ) << 13 )

#define VMCS_ENCODE_COMPONENT_FULL( type, width, index )    VMCS_ENCODE_COMPONENT( 0, type, width, index )
#define VMCS_ENCODE_COMPONENT_FULL_16( type, index )        VMCS_ENCODE_COMPONENT_FULL( type, VMCS_FIELD_WIDTH_16BIT, index )
#define VMCS_ENCODE_COMPONENT_FULL_32( type, index )        VMCS_ENCODE_COMPONENT_FULL( type, VMCS_FIELD_WIDTH_32BIT, index )
#define VMCS_ENCODE_COMPONENT_FULL_64( type, index )        VMCS_ENCODE_COMPONENT_FULL( type, VMCS_FIELD_WIDTH_64BIT, index )

enum vmcs_field {
	VIRTUAL_PROCESSOR_ID            = 0x00000000,
	GUEST_ES_SELECTOR               = 0x00000800,
	GUEST_CS_SELECTOR               = 0x00000802,
	GUEST_SS_SELECTOR               = 0x00000804,
	GUEST_DS_SELECTOR               = 0x00000806,
	GUEST_FS_SELECTOR               = 0x00000808,
	GUEST_GS_SELECTOR               = 0x0000080a,
	GUEST_LDTR_SELECTOR             = 0x0000080c,
	GUEST_TR_SELECTOR               = 0x0000080e,
	HOST_ES_SELECTOR                = 0x00000c00,
	HOST_CS_SELECTOR                = 0x00000c02,
	HOST_SS_SELECTOR                = 0x00000c04,
	HOST_DS_SELECTOR                = 0x00000c06,
	HOST_FS_SELECTOR                = 0x00000c08,
	HOST_GS_SELECTOR                = 0x00000c0a,
	HOST_TR_SELECTOR                = 0x00000c0c,
	IO_BITMAP_A                     = 0x00002000,
	IO_BITMAP_A_HIGH                = 0x00002001,
	IO_BITMAP_B                     = 0x00002002,
	IO_BITMAP_B_HIGH                = 0x00002003,
	MSR_BITMAP                      = 0x00002004,
	MSR_BITMAP_HIGH                 = 0x00002005,
	VM_EXIT_MSR_STORE_ADDR          = 0x00002006,
	VM_EXIT_MSR_STORE_ADDR_HIGH     = 0x00002007,
	VM_EXIT_MSR_LOAD_ADDR           = 0x00002008,
	VM_EXIT_MSR_LOAD_ADDR_HIGH      = 0x00002009,
	VM_ENTRY_MSR_LOAD_ADDR          = 0x0000200a,
	VM_ENTRY_MSR_LOAD_ADDR_HIGH     = 0x0000200b,
	TSC_OFFSET                      = 0x00002010,
	TSC_OFFSET_HIGH                 = 0x00002011,
	VIRTUAL_APIC_PAGE_ADDR          = 0x00002012,
	VIRTUAL_APIC_PAGE_ADDR_HIGH     = 0x00002013,
	APIC_ACCESS_ADDR				= 0x00002014,
	APIC_ACCESS_ADDR_HIGH			= 0x00002015,
	EPT_POINTER                     = 0x0000201a,
	EPT_POINTER_HIGH                = 0x0000201b,
	GUEST_PHYSICAL_ADDRESS          = 0x00002400,
	GUEST_PHYSICAL_ADDRESS_HIGH     = 0x00002401,
	VMCS_LINK_POINTER               = 0x00002800,
	VMCS_LINK_POINTER_HIGH          = 0x00002801,
	GUEST_IA32_DEBUGCTL             = 0x00002802,
	GUEST_IA32_DEBUGCTL_HIGH        = 0x00002803,
	GUEST_IA32_PAT					= 0x00002804,
	GUEST_IA32_PAT_HIGH				= 0x00002805,
	GUEST_IA32_EFER					= 0x00002806,
	GUEST_IA32_EFER_HIGH			= 0x00002807,
	GUEST_PDPTR0                    = 0x0000280a,
	GUEST_PDPTR0_HIGH               = 0x0000280b,
	GUEST_PDPTR1                    = 0x0000280c,
	GUEST_PDPTR1_HIGH               = 0x0000280d,
	GUEST_PDPTR2                    = 0x0000280e,
	GUEST_PDPTR2_HIGH               = 0x0000280f,
	GUEST_PDPTR3                    = 0x00002810,
	GUEST_PDPTR3_HIGH               = 0x00002811,
	HOST_IA32_PAT					= 0x00002c00,
	HOST_IA32_PAT_HIGH				= 0x00002c01,
	HOST_IA32_EFER					= 0x00002c02,
	HOST_IA32_EFER_HIGH				= 0x00002c03,
	PIN_BASED_VM_EXEC_CONTROL       = 0x00004000,
	CPU_BASED_VM_EXEC_CONTROL       = 0x00004002,
	EXCEPTION_BITMAP                = 0x00004004,
	PAGE_FAULT_ERROR_CODE_MASK      = 0x00004006,
	PAGE_FAULT_ERROR_CODE_MATCH     = 0x00004008,
	CR3_TARGET_COUNT                = 0x0000400a,
	VM_EXIT_CONTROLS                = 0x0000400c,
	VM_EXIT_MSR_STORE_COUNT         = 0x0000400e,
	VM_EXIT_MSR_LOAD_COUNT          = 0x00004010,
	VM_ENTRY_CONTROLS               = 0x00004012,
	VM_ENTRY_MSR_LOAD_COUNT         = 0x00004014,
	VM_ENTRY_INTR_INFO_FIELD        = 0x00004016,
	VM_ENTRY_EXCEPTION_ERROR_CODE   = 0x00004018,
	VM_ENTRY_INSTRUCTION_LEN        = 0x0000401a,
	TPR_THRESHOLD                   = 0x0000401c,
	SECONDARY_VM_EXEC_CONTROL       = 0x0000401e,
	PLE_GAP                         = 0x00004020,
	PLE_WINDOW                      = 0x00004022,
	VM_INSTRUCTION_ERROR            = 0x00004400,
	VM_EXIT_REASON                  = 0x00004402,
	VM_EXIT_INTR_INFO               = 0x00004404,
	VM_EXIT_INTR_ERROR_CODE         = 0x00004406,
	IDT_VECTORING_INFO_FIELD        = 0x00004408,
	IDT_VECTORING_ERROR_CODE        = 0x0000440a,
	VM_EXIT_INSTRUCTION_LEN         = 0x0000440c,
	VMX_INSTRUCTION_INFO            = 0x0000440e,
	GUEST_ES_LIMIT                  = 0x00004800,
	GUEST_CS_LIMIT                  = 0x00004802,
	GUEST_SS_LIMIT                  = 0x00004804,
	GUEST_DS_LIMIT                  = 0x00004806,
	GUEST_FS_LIMIT                  = 0x00004808,
	GUEST_GS_LIMIT                  = 0x0000480a,
	GUEST_LDTR_LIMIT                = 0x0000480c,
	GUEST_TR_LIMIT                  = 0x0000480e,
	GUEST_GDTR_LIMIT                = 0x00004810,
	GUEST_IDTR_LIMIT                = 0x00004812,
	GUEST_ES_AR_BYTES               = 0x00004814,
	GUEST_CS_AR_BYTES               = 0x00004816,
	GUEST_SS_AR_BYTES               = 0x00004818,
	GUEST_DS_AR_BYTES               = 0x0000481a,
	GUEST_FS_AR_BYTES               = 0x0000481c,
	GUEST_GS_AR_BYTES               = 0x0000481e,
	GUEST_LDTR_AR_BYTES             = 0x00004820,
	GUEST_TR_AR_BYTES               = 0x00004822,
	GUEST_INTERRUPTIBILITY_INFO     = 0x00004824,
	GUEST_ACTIVITY_STATE            = 0X00004826,
	GUEST_SYSENTER_CS               = 0x0000482A,
	HOST_IA32_SYSENTER_CS           = 0x00004c00,
	CR0_GUEST_HOST_MASK             = 0x00006000,
	CR4_GUEST_HOST_MASK             = 0x00006002,
	CR0_READ_SHADOW                 = 0x00006004,
	CR4_READ_SHADOW                 = 0x00006006,
	CR3_TARGET_VALUE0               = 0x00006008,
	CR3_TARGET_VALUE1               = 0x0000600a,
	CR3_TARGET_VALUE2               = 0x0000600c,
	CR3_TARGET_VALUE3               = 0x0000600e,
	EXIT_QUALIFICATION              = 0x00006400,
	GUEST_LINEAR_ADDRESS            = 0x0000640a,
	GUEST_CR0                       = 0x00006800,
	GUEST_CR3                       = 0x00006802,
	GUEST_CR4                       = 0x00006804,
	GUEST_ES_BASE                   = 0x00006806,
	GUEST_CS_BASE                   = 0x00006808,
	GUEST_SS_BASE                   = 0x0000680a,
	GUEST_DS_BASE                   = 0x0000680c,
	GUEST_FS_BASE                   = 0x0000680e,
	GUEST_GS_BASE                   = 0x00006810,
	GUEST_LDTR_BASE                 = 0x00006812,
	GUEST_TR_BASE                   = 0x00006814,
	GUEST_GDTR_BASE                 = 0x00006816,
	GUEST_IDTR_BASE                 = 0x00006818,
	GUEST_DR7                       = 0x0000681a,
	GUEST_RSP                       = 0x0000681c,
	GUEST_RIP                       = 0x0000681e,
	GUEST_RFLAGS                    = 0x00006820,
	GUEST_PENDING_DBG_EXCEPTIONS    = 0x00006822,
	GUEST_SYSENTER_ESP              = 0x00006824,
	GUEST_SYSENTER_EIP              = 0x00006826,
	HOST_CR0                        = 0x00006c00,
	HOST_CR3                        = 0x00006c02,
	HOST_CR4                        = 0x00006c04,
	HOST_FS_BASE                    = 0x00006c06,
	HOST_GS_BASE                    = 0x00006c08,
	HOST_TR_BASE                    = 0x00006c0a,
	HOST_GDTR_BASE                  = 0x00006c0c,
	HOST_IDTR_BASE                  = 0x00006c0e,
	HOST_IA32_SYSENTER_ESP          = 0x00006c10,
	HOST_IA32_SYSENTER_EIP          = 0x00006c12,
	HOST_RSP                        = 0x00006c14,
	HOST_RIP                        = 0x00006c16,
};


struct vmcs {
	u32 revision_id;
	u32 abort;
	u32 data[1];
};

struct vmcs *vmxon_region;
struct vmcs *vmcs_region;

typedef struct vcpu {
	uint64_t guest_rsp;
	uint64_t guest_rip;
} VCPU;

static VCPU vcpu;

#if 0
static u64 read_cr4(void)
{
	u64 cr4;
	asm volatile("movq %%cr4, %0" : "=r"(cr4));

	return cr4;
}

static void write_cr4(u64 cr4)
{
	asm volatile("movq %0, %%cr4" : : "r"(cr4));
}
#endif

static int vmxon(u64 address)
{
	u8 error;
	asm volatile("vmxon %1; setna %0"
		     : "=q"(error)
		     : "m"(address)
		     : "memory", "cc");

	return error;
}

#if 0
static int vmxoff(void)
{
	u8 error;
	asm volatile("vmxoff; setna %0" : "=q"(error));
	if (error) {
		printk(KERN_ERR "vmm: failed to vmxoff\n");
		return error;
	}

	return 0;
}
#endif

static int vmclear(u64 address)
{
	u8 error;
	asm volatile("vmclear %1; setna %0" : "=qm"(error) : "m"(address));

	return error;
}

static int vmptrld(u64 address)
{
	u8 error;
	asm volatile("vmptrld %1; setna %0" : "=qm"(error) : "m"(address));

	return error;
}

#if 0
static uint64_t read_vmcs_field(uint32_t field)
{
	uint64_t sel = field;
	return _vmread(sel);
}
#endif

static int write_vmcs_field(uint32_t field, uint64_t value)
{
	uint64_t sel = field;
	return _vmwrite(sel, value);
}

#define IA32_DEBUGCTL					(0x01d9)
#define IA32_SYSENTER_CS				(0x0174)
#define IA32_SYSENTER_ESP				(0x0175)
#define IA32_SYSENTER_EIP				(0x0176)
#define IA32_FS_BASE				(0xC0000100)
#define IA32_GS_BASE				(0xc0000101)

static int setup_vmcs(VCPU *vcpu)
{
	unsigned long cr0 = _read_cr0();
	unsigned long cr3 = _read_cr3();
	unsigned long cr4 = _read_cr4();
	unsigned long dr7 = _read_dr7();
	uint64_t rflag = _read_rflag();

	write_vmcs_field(GUEST_CR0, cr0);
	write_vmcs_field(GUEST_CR3, cr3);
	write_vmcs_field(GUEST_CR4, cr4);
	write_vmcs_field(GUEST_DR7, dr7);

	write_vmcs_field(GUEST_RSP, vcpu->guest_rsp);
	write_vmcs_field(GUEST_RIP, vcpu->guest_rip);
	write_vmcs_field(GUEST_RFLAGS, rflag);
	write_vmcs_field(GUEST_IA32_DEBUGCTL, _read_msr(IA32_DEBUGCTL));
	write_vmcs_field(GUEST_SYSENTER_ESP, _read_msr(IA32_SYSENTER_ESP));
	write_vmcs_field(GUEST_SYSENTER_EIP, _read_msr(IA32_SYSENTER_EIP));
	write_vmcs_field(GUEST_SYSENTER_CS, _read_msr(IA32_SYSENTER_CS));
	write_vmcs_field(VMCS_LINK_POINTER, 0xFFFFFFFFFFFFFFFF);
	write_vmcs_field(GUEST_FS_BASE, _read_msr(IA32_FS_BASE));
	write_vmcs_field(GUEST_GS_BASE, _read_msr(IA32_GS_BASE));

#if 0
	reg = _get_reg_es();
	write_vmcs_field(GUEST_ES_SELECTOR, reg);

	reg = _get_reg_cs();
	write_vmcs_field(GUEST_CS_SELECTOR, reg);

	reg = _get_reg_ss();
	write_vmcs_field(GUEST_SS_SELECTOR, reg);

	reg = _get_reg_ds();
	write_vmcs_field(GUEST_SS_SELECTOR, reg);

	reg = _get_reg_fs();
	write_vmcs_field(GUEST_SS_SELECTOR, reg);

	reg = _get_reg_gs();
	write_vmcs_field(GUEST_GS_SELECTOR, reg);
#endif

	return 0;
}

int init_vmx(void)
{
    struct page *page;
    u32 vmx_msr_low, vmx_msr_high;
    int cpu;
    int node;
    int ret;
	u64 pa_vmx;
	// u64 cr0, cr4, msr_tmp;

    printk(KERN_DEBUG "init_vmx in...\n");

    rdmsr(MSR_IA32_VMX_BASIC, vmx_msr_low, vmx_msr_high);
    printk(KERN_DEBUG "vmx_msr_low:[%x], vmx_msr_high:[%x]\n", vmx_msr_low, vmx_msr_high);
	printk(KERN_DEBUG "vmm: vmcs_size:[%x]\n", (vmx_msr_high & 0x1ffff));

    cpu = raw_smp_processor_id();
    printk(KERN_DEBUG "smp processor id:[%d]\n", cpu);
    node = cpu_to_node(cpu);

    page = __alloc_pages_node(node, GFP_KERNEL, 0);
    if (!page) {
        printk(KERN_ERR "Failed to alloc_pages_node\n");
        return -1;
    }

    vmxon_region = page_address(page);
    memset(vmxon_region, 0, VMX_PAGE_SIZE);
    vmxon_region->revision_id = vmx_msr_low;

    // vmxon
    pa_vmx = __pa(vmxon_region);
    printk(KERN_DEBUG "vmxon, vmxon_region: %p, pa_vmx:%08lx\n", vmxon_region, (uintptr_t)pa_vmx);

	// TODO _vmxon だと page faultになることがある
	// ret = _vmxon(pa_vmx);
	ret = vmxon(pa_vmx);
    if (ret)
    {
        printk(KERN_ERR "vmxon failed...\n");
        return -1;
    }

    page = __alloc_pages_node(node, GFP_KERNEL, 0);
    if (!page) {
        printk(KERN_ERR "Failed to alloc_pages_node\n");
        return -1;
    }

    vmcs_region = page_address(page);
    memset(vmcs_region, 0, VMX_PAGE_SIZE);
    vmcs_region->revision_id = vmx_msr_low;
    pa_vmx = __pa(vmcs_region);
    printk(KERN_DEBUG "vmptrld, vmcs_region: %p, pa_vmx:%08lx\n", vmcs_region, (uintptr_t)pa_vmx);

    ret = vmclear(pa_vmx);
    if (ret)
    {
        printk(KERN_DEBUG "vmptrld failed...\n");
        return -1;
    }

    ret = vmptrld(pa_vmx);
    if (ret)
    {
        printk(KERN_DEBUG "vmptrld failed...\n");
        return -1;
    }

	// TODO
	setup_vmcs(&vcpu);

	// TODO 要確認
#if 0
	cr0 = read_cr0();
	rdmsrl(MSR_IA32_VMX_CR0_FIXED1, msr_tmp);
	cr0 &= msr_tmp;
	rdmsrl(MSR_IA32_VMX_CR0_FIXED0, msr_tmp);
	cr0 |= msr_tmp;
	write_cr0(cr0);

	cr4 = read_cr4();
	rdmsrl(MSR_IA32_VMX_CR4_FIXED1, msr_tmp);
	cr4 &= msr_tmp;
	rdmsrl(MSR_IA32_VMX_CR4_FIXED0, msr_tmp);
	cr4 |= msr_tmp;
	write_cr4(cr4);
#endif

    printk(KERN_DEBUG "init_vmx out...\n");
    return 0;
}
