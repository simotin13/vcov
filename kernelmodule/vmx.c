#include <asm/msr.h>
#include <linux/types.h>
#include <linux/mm.h>

typedef struct vmcs
{
	uint32_t revisionId;
	uint32_t abort;
	uint32_t *data;
} VMCS;

int init_vmx(void)
{
    VMCS *vmcs;
	struct page *page;

    printk(KERN_DEBUG "init_vmx in...\n");

	page = alloc_pages(GFP_KERNEL, 0);
	if (!page) {
		printk(KERN_ERR "Failed to alloc_pages\n");
		return -1;
	}

	vmcs = page_address(page);
	memset(vmcs, 0, 4096);

    printk(KERN_DEBUG "init_vmx out...\n");
	return 0;
}
