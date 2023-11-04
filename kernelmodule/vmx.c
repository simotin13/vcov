#include <asm/msr.h>
#include <linux/types.h>
#include <linux/mm.h>

#include "cpu.h"

#define VMX_PAGE_SIZE	(4096)
struct vmcs {
	u32 revision_id;
	u32 abort;
	u32 data[1];
};

struct vmcs *vmxon_region;
struct vmcs *vmcs_region;

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

static int vmxon(u64 address)
{
	u8 error;
	asm volatile("vmxon %1; setna %0"
		     : "=q"(error)
		     : "m"(address)
		     : "memory", "cc");

	return error;
}

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

int init_vmx(void)
{
    struct page *page;
    u32 vmx_msr_low, vmx_msr_high;
    int cpu;
    int node;
    int ret;
	u64 cr0, cr4, msr_tmp;
	u64 pa_vmx;

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

	ret = vmptrld(pa_vmx);
    if (ret)
    {
        printk(KERN_DEBUG "vmptrld failed...\n");
        return -1;
    }

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
