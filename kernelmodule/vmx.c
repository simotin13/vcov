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

static int zzzzzz(u64 address)
{
	u8 error;
	asm volatile("vmxon %1; setna %0"
		     : "=q"(error)
		     : "m"(address)
		     : "memory", "cc");

	return error;
}

static struct vmcs *alloc_vmcs_region(int cpu)
{
	struct vmcs *vmcs;
	struct page *page;
	uint32_t vmx_msr_low, vmx_msr_high;
	size_t vmcs_size;

	printk(KERN_DEBUG "vmm: alloc_vmcs_region called\n");

	int node = cpu_to_node(cpu);

	rdmsr(MSR_IA32_VMX_BASIC, vmx_msr_low, vmx_msr_high);
	printk(KERN_DEBUG "vmm: vmcs_size:[%x]\n", vmcs_size);

	page = __alloc_pages_node(node, GFP_KERNEL, 0);
	if (!page) {
		return NULL;
	}

	vmcs = page_address(page);
	memset(vmcs, 0, VMX_PAGE_SIZE);

	return vmcs;
}

static void vmx_enable(void)
{
	int r;

	u64 msr_vmx_basic;
	u64 pa_vmx;
	u64 cr0, cr4, msr_tmp;

    int cpu = raw_smp_processor_id();

	printk(KERN_DEBUG "vmm: vmx_enable called\n");

    rdmsrl(MSR_IA32_VMX_BASIC, msr_vmx_basic);

	vmxon_region->revision_id = (u32)msr_vmx_basic;
	printk(KERN_DEBUG "revision_id:[%x]\n", vmxon_region->revision_id);

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

	pa_vmx = __pa(vmxon_region);

	printk("pa_vmx: %p %08lx\n", vmxon_region, (uintptr_t)pa_vmx);

	r = zzzzzz(pa_vmx);
	if (r) {
		printk(KERN_ERR "vmm: failed to vmxon [%d]\n", r);
	}

	return;
}

int init_vmx(void)
{
    int cpu = raw_smp_processor_id();

	vmxon_region = alloc_vmcs_region(cpu);
	if(!vmxon_region)
	{
		return -ENOMEM;
	}

    vmx_enable();
    return 0;
#if 0
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

    // vmxon
    pa_vmx = __pa(vmxon_region);

    printk("pa_vmx: %p %08lx\n", vmxon_region, (uintptr_t)pa_vmx);

    printk(KERN_DEBUG "before vmxon...\n");
    ret = _vmxon(pa_vmx);
    if (ret)
    {
        printk(KERN_DEBUG "vmxon failed...\n");
        return -1;
    }

    printk(KERN_DEBUG "init_vmx out...\n");
    return 0;
#endif
#if 0
    vmxon_region = page_address(page);
    memset(vmxon_region, 0, VMX_PAGE_SIZE);
    vmxon_region->revision_id = vmx_msr_low;

    page = __alloc_pages_node(node, GFP_KERNEL, 0);
    if (!page) {
        printk(KERN_ERR "Failed to alloc_pages_node\n");
        return -1;
    }
    vmcs_region = page_address(page);
    memset(vmcs_region, 0, VMX_PAGE_SIZE);
    vmcs_region->revision_id = vmx_msr_low;
#endif
}
