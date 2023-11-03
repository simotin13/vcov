#include <asm/msr.h>
#include <linux/types.h>
#include <linux/mm.h>

#include "cpu.h"

#define VMX_PAGE_SIZE	(4096)
typedef struct vmcs
{
    uint32_t revision_id;
    uint32_t abort;
    uint32_t *data;
} VMCS;

typedef struct vm_state
{
    void *vmxon_region;
    VMCS *vmcs_region;
} VM_STATE;

static VM_STATE vm_state;

int init_vmx(void)
{
    struct page *page;
    uint32_t vmx_msr_low, vmx_msr_high;
    int cpu;
    int node;
    int ret;

    printk(KERN_DEBUG "init_vmx in...\n");

    rdmsr(MSR_IA32_VMX_BASIC, vmx_msr_low, vmx_msr_high);
    printk(KERN_DEBUG "vmx_msr_low:[%x], vmx_msr_high:[%x]\n", vmx_msr_low, vmx_msr_high);

    cpu = raw_smp_processor_id();
    printk(KERN_ERR "smp processor id:[%d]\n", cpu);
    node = cpu_to_node(cpu);

    page = alloc_pages_node(node, GFP_KERNEL, 0);
    if (!page) {
        printk(KERN_ERR "Failed to alloc_pages_node\n");
        return -1;
    }
    vm_state.vmxon_region = page_address(page);
    memset(vm_state.vmxon_region, 0, VMX_PAGE_SIZE);
    memcpy(vm_state.vmxon_region,  &vmx_msr_low, 4);

    page = alloc_pages_node(node, GFP_KERNEL, 0);
    if (!page) {
        printk(KERN_ERR "Failed to alloc_pages_node\n");
        return -1;
    }
    vm_state.vmcs_region = page_address(page);
    memset(vm_state.vmcs_region, 0, VMX_PAGE_SIZE);
    vm_state.vmcs_region->revision_id = vmx_msr_low;

    // vmxon
    printk(KERN_DEBUG "before vmxon...\n");
    ret = _vmxon(vm_state.vmxon_region);
    if (ret)
    {
        printk(KERN_DEBUG "vmxon failed...\n");
        return -1;
    }

    printk(KERN_DEBUG "init_vmx out...\n");
    return 0;
}
