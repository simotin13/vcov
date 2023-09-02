#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h> // copy_from_user, copy_to_user
MODULE_LICENSE("Dual BSD/GPL");

#define DRIVER_NAME "vmm"

static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM  = 2;
static struct cdev vmm_cdev;
static unsigned int vmm_major;

// ============================================================================
// function prototype
// ============================================================================
static int vmm_init(void);
static void vmm_exit(void);
static ssize_t vmm_read(struct file *fp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t vmm_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

struct file_operations devone_fops = {
    .read = vmm_read,
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

ssize_t vmm_read(struct file *fp, char __user *buf, size_t count, loff_t *f_pos)
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

module_init(vmm_init);
module_exit(vmm_exit);