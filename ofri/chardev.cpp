#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "encrypted_memory"
#define BUF_SIZE 4096

static dev_t dev_num;
static struct cdev cdev;
static struct class *dev_class;
static int key = 0;  // Initial key value

static int device_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static ssize_t device_read(struct file *file, char __user *buffer, size_t length, loff_t *offset)
{
    // Read operation is not supported for this device
    return -EINVAL;
}

static ssize_t device_write(struct file *file, const char __user *buffer, size_t length, loff_t *offset)
{
    // Write operation is not supported for this device
    return -EINVAL;
}

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
    int new_key;
    char *data = (char *)ioctl_param;

    switch (ioctl_num) {
        case 0:  // Set the encryption key
            if (copy_from_user(&new_key, data, sizeof(new_key))) {
                return -EFAULT;
            }
            key = new_key;
            break;

        default:
            return -EINVAL;
    }

    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = device_ioctl,
};

static int __init chardev_init(void)
{
    // Allocate device numbers
    if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_ALERT "Failed to allocate device numbers\n");
        return -1;
    }

    // Create char device structure
    cdev_init(&cdev, &fops);
    cdev.owner = THIS_MODULE;

    // Add char device to the system
    if (cdev_add(&cdev, dev_num, 1) < 0) {
        printk(KERN_ALERT "Failed to add char device to the system\n");
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    // Create class for the device
    dev_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(dev_class)) {
        printk(KERN_ALERT "Failed to create device class\n");
        cdev_del(&cdev);
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    // Create device node
    if (device_create(dev_class, NULL, dev_num, NULL, DEVICE_NAME) == NULL) {
        printk(KERN_ALERT "Failed to create device node\n");
        class_destroy(dev_class);
        cdev_del(&cdev);
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    printk(KERN_INFO "Char device '%s' initialized\n", DEVICE_NAME);
    return 0;
}

static void __exit chardev_exit(void)
{
    // Destroy device node
    device_destroy(dev_class, dev_num);

    // Destroy device class
    class_destroy(dev_class);

    // Remove char device from the system
    cdev_del(&cdev);

    // Release device numbers
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO "Char device '%s' exited\n", DEVICE_NAME);
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Encrypted Memory Char Device");
