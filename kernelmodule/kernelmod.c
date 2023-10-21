#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h> // copy_from_user, copy_to_user
#include <linux/mm.h>
#include "cpu.h"
#include "kernelmod.h"

MODULE_LICENSE("Dual BSD/GPL");

#define DRIVER_NAME "vmm"

#define VMM_PAGE_SIZE   4096

typedef struct vmm_mmap_info {
    char *data;
    int ref;
} VMM_MMAP_INFO;

static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM  = 2;
static struct cdev vmm_cdev;
static unsigned int vmm_major;
static uint8_t vmxon_region[VMM_PAGE_SIZE];

// ============================================================================
// function prototype
// ============================================================================
static int vmm_init(void);
static void vmm_exit(void);
static int vmm_open(struct inode *inode, struct file *filep);
static int vmm_release(struct inode *inode, struct file *filep);
static ssize_t vmm_read(struct file *filep, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t vmm_write(struct file *filep, const char __user *buf, size_t count, loff_t *f_pos);
static int vmm_mmap(struct file *filep, struct vm_area_struct *vma);
static long vmm_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);

static void vm_open(struct vm_area_struct *vma);
static void vm_close(struct vm_area_struct *vma);
static vm_fault_t vm_fault(struct vm_fault *vmf);

static int enable_vmx(void)
static uint32_t read_vmx_revision(void);

struct file_operations devone_fops = {
    .open = vmm_open,
    .release = vmm_release,
    .read = vmm_read,
    .write = vmm_write,
    .mmap = vmm_mmap,
    .unlocked_ioctl = vmm_ioctl,
};

static struct vm_operations_struct vm_ops =
{
    .close = vm_close,
    .fault = vm_fault,
    .open = vm_open,
};

static int vmm_init(void)
{
    dev_t dev;
    int alloc_ret = 0;
    int cdev_err = 0;

    printk(KERN_DEBUG "vmm_init called.");
    alloc_ret = alloc_chrdev_region(&dev, MINOR_BASE, MINOR_NUM, DRIVER_NAME);
    if (alloc_ret != 0) {
        printk(KERN_ERR  "alloc_chrdev_region = %d\n", alloc_ret);
        return -1;
    }

    vmm_major = MAJOR(dev);

    cdev_init(&vmm_cdev, &devone_fops);
    vmm_cdev.owner = THIS_MODULE;

    cdev_err = cdev_add(&vmm_cdev, dev, MINOR_NUM);
    if (cdev_err) {
        printk(KERN_ERR  "cdev_add = %d\n", cdev_err);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }

    printk(KERN_DEBUG "%s driver(major %d) installed\n", "vmm", vmm_major);
    return 0;
}

static void vmm_exit(void)
{
    dev_t dev = MKDEV(vmm_major, MINOR_BASE);

    printk(KERN_DEBUG "%s called.", __FUNCTION__);

    cdev_del(&vmm_cdev);
    unregister_chrdev_region(dev, MINOR_NUM);

    printk(KERN_DEBUG "vmm driver removed.\n");
}

static int vmm_open(struct inode *inode, struct file *filep)
{
    VMM_MMAP_INFO *vmm_mmap_info;
    printk(KERN_DEBUG "vmm_open called...\n");

    vmm_mmap_info = kmalloc(sizeof(VMM_MMAP_INFO), GFP_KERNEL);
    vmm_mmap_info->data = (char *)get_zeroed_page(GFP_KERNEL);

    memcpy(vmm_mmap_info->data, "hello from kernel this is file: ", 32);
    filep->private_data = vmm_mmap_info;

    return 0;
}

static int vmm_release(struct inode *inode, struct file *filep)
{
    printk(KERN_DEBUG "vmm_close call...\n");
    return 0;
}

ssize_t vmm_read(struct file *filep, char __user *buf, size_t count, loff_t *f_pos)
{
    int i;
    unsigned char val = 0xff;
    int retval;

    for (i = 0 ; i < count ; i++) {
        retval = copy_to_user(&buf[i], &val, 1);
        if (retval != 0) {
            break;
        }
    }
    if (retval == 0) {
        retval = count;
    }

    return retval;
}
static ssize_t vmm_write(struct file *filep, const char __user *buf, size_t count, loff_t *f_pos)
{
    printk(KERN_DEBUG "vmm_write called...\n");
    return 0;
}


static int vmm_mmap(struct file *filep, struct vm_area_struct *vma)
{
    printk(KERN_DEBUG "vmm_mmap called...\n");
    vma->vm_ops = &vm_ops;
    vma->vm_flags |= VM_DONTEXPAND | VM_DONTDUMP;
    vma->vm_private_data = filep->private_data;
    vm_open(vma);
    return 0;
}

static void vm_open(struct vm_area_struct *vma)
{
    VMM_MMAP_INFO *vmm_mmap_info;
    printk(KERN_DEBUG "vm_open called In...\n");
    vmm_mmap_info = (VMM_MMAP_INFO *)vma->vm_private_data;
    vmm_mmap_info->ref++;
    printk(KERN_DEBUG "vm_open called Out...\n");
}

static void vm_close(struct vm_area_struct *vma)
{
    VMM_MMAP_INFO *vmm_mmap_info;
    printk(KERN_DEBUG "vm_close called...\n");
    vmm_mmap_info = (VMM_MMAP_INFO *)vma->vm_private_data;
    vmm_mmap_info->ref--;
}

static vm_fault_t vm_fault(struct vm_fault *vmf)
{
    struct page *page;
    VMM_MMAP_INFO *vmm_mmap_info;
    unsigned long address = (unsigned long)vmf->address;

    printk(KERN_DEBUG "vm_fault called...\n");

    if (address > vmf->vma->vm_end) {
        return VM_FAULT_SIGBUS;
    }
    vmm_mmap_info = (VMM_MMAP_INFO *)vmf->vma->vm_private_data;
    if (!vmm_mmap_info->data) {
        return VM_FAULT_SIGBUS;
    }

    page = virt_to_page(vmm_mmap_info->data);

    get_page(page);
    vmf->page = page;
    return 0;
}

static long vmm_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    static VmmCtrl vmmCtrl;
    int ret;
    unsigned long long val = 0;
    unsigned int b32Val;

    printk(KERN_DEBUG "vmm_ioctl in, cmd:[0x%02X]\n", cmd);
    memset(&vmmCtrl, 0, sizeof(VmmCtrl));

    switch(cmd) {
    case VMM_READ_CR0:
    {

        val = _read_cr0();
        printk(KERN_DEBUG "_read_cr0 val:[0x%llX]\n", val);
        vmmCtrl.val = val;
        ret = raw_copy_to_user((void __user *)arg, &vmmCtrl, sizeof(VmmCtrl));
        if (ret != 0) {
            ret = -EFAULT;
        }
    }
    break;
    case VMM_READ_CR4:
    {
        val = _read_cr4();
        vmmCtrl.val = val;
        ret = raw_copy_to_user((void __user *)arg, &vmmCtrl, sizeof(VmmCtrl));
        if (ret != 0) {
            ret = -EFAULT;
        }
    }
    break;
    case VMM_WRITE_CR4:
    {
        ret = raw_copy_from_user(&vmmCtrl, (void __user *)arg, sizeof(VmmCtrl));
        if (ret < 0) {
            return -EFAULT;
        }
        b32Val = vmmCtrl.val;
        printk(KERN_DEBUG "_write_cr4 b32Val:[0x%X]\n", b32Val);
        _write_cr4(b32Val);
        ret = 0;
    }
    break;
    case VMM_READ_MSR:
    {
        val = _read_msr(vmmCtrl.addr);
        vmmCtrl.val = val;
        ret = raw_copy_to_user((void __user *)arg, &vmmCtrl, sizeof(VmmCtrl));
        if (ret != 0) {
            ret = -EFAULT;
        }
    }
    break;
    case VMM_WRITE_MSR:
    {
        ret = raw_copy_from_user(&vmmCtrl, (void __user *)arg, sizeof(VmmCtrl));
        if (ret < 0) {
            return -EFAULT;
        }
        val = _write_msr(vmmCtrl.addr, vmmCtrl.val);
        vmmCtrl.val = val;
        ret = raw_copy_to_user((void __user *)arg, &vmmCtrl, sizeof(VmmCtrl));
        if (ret != 0) {
            ret = -EFAULT;
        }
    }
        break;
    case VMM_VMXOFF:
    {
        ret = _vmxoff();
    }
    break;
    case VMM_VMXON:
    {
        ret = _vmxon();
    }
    break;
    case VMM_ENABLE_VMXE:
    {
        _enable_vmxe();
        ret = 0;
    }
    break;
    case VMM_DISABLE_VMXE:
    {
        _disable_vmxe();
        ret = 0;
    }
    break;
    default:
        printk(KERN_DEBUG "Unknown cmd:[0x%X] called.\n", cmd);
        break;
    }

    return ret;
}

static int enable_vmx(void)
{
    int ret;
    uint64_t wVal;
    uint64_t reg = _read_msr(MSR_IA32_FEATURE_CONTROL);
    printk(KERN_DEBUG, "check_vmx feature_control:[%x]\n", reg);
    if ((reg & MSR_MASK_LOCK_IA32_FEATURE_CONTROL) == 1 &&
        (reg & MSR_MASK_FEATURE_CONTROL_VMX_EN) == 0)
        {
            printk(KERN_DEBUG "check failed...\n");
        }
        return -1;
    }

    wVal = reg | MSR_MASK_LOCK_IA32_FEATURE_CONTROL | MSR_MASK_FEATURE_CONTROL_VMX_EN;
    wrmsr(MSR_IA32_FEATURE_CONTROL, wVal);

    load_cr4(rcr4() | CR4_VMXE);

    uint32_t vmx_rev = read_vmx_revision();
    *(uint32_t *)vmxon_region = vmx_rev
    ret = vmxon(vmxon_region);
    if (ret != 0)
    {
        return -1;
    }
    return 0;
}

static uint32_t read_vmx_revision(void)
{
    uint64_t reg = rdmsr(MSR_VMX_BASIC);
	return (reg & 0xffffffff);
}


module_init(vmm_init);
module_exit(vmm_exit);
