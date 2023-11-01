#include <asm/msr.h>
#include <linux/types.h>
#include <linux/mm.h>

struct vmcs {
	uint32_t revisionId;
	uint32_t abort;
	uint32_t *data;
};

static struct vmcs *alloc_vmcs_region(void)
{
	struct vmcs *vmcs;
	struct page *page;

	page = alloc_pages(GFP_KERNEL, 0);
	if (!page) {
		return NULL;
	}

	vmcs = page_address(page);
	memset(vmcs, 0, 4096);
	return vmcs;
}
