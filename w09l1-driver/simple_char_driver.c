/**
 * A simple char device driver.
 * Execute the following command to create a node /dev/simple_char_dev
 * sudo mknod /dev/simple_char_dev c 241 0
 * sudo chmod 666 /dev/simple_char_dev
 * */
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h> // for copy_to_user and copy_from_user
#include <linux/slab.h>    // for kmalloc and kfree

#define DEVICE_NAME "simple_char_dev"
#define BUFFER_SIZE 1024

static int major_number;
static char *device_buffer;
static struct cdev simple_cdev;

static int device_open(struct inode *inode, struct file *file) {
    pr_info("Device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    pr_info("Device closed\n");
    return 0;
}

static ssize_t device_read(struct file *file, char __user *user_buffer,
                           size_t length, loff_t *offset) {
    size_t bytes_read = BUFFER_SIZE - *offset;

    if (bytes_read == 0)
        return 0;  // End of file

    if (bytes_read > length)
        bytes_read = length;

    if (copy_to_user(user_buffer, device_buffer + *offset, bytes_read))
        return -EFAULT;

    *offset += bytes_read;
    pr_info("Read %zu bytes from device\n", bytes_read);

    return bytes_read;
}

static ssize_t device_write(struct file *file, const char __user *user_buffer,
                            size_t length, loff_t *offset) {
    size_t bytes_to_write = BUFFER_SIZE - *offset;

    if (bytes_to_write == 0)
        return -ENOSPC;  // No space left in buffer

    if (bytes_to_write > length)
        bytes_to_write = length;

    if (copy_from_user(device_buffer + *offset, user_buffer, bytes_to_write))
        return -EFAULT;

    *offset += bytes_to_write;
    pr_info("Wrote %zu bytes to device\n", bytes_to_write);

    return bytes_to_write;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
};

static int __init simple_char_driver_init(void) {
    // Allocate device number
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        pr_err("Failed to register character device\n");
        return major_number;
    }

    // Allocate memory for the device buffer
    device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!device_buffer) {
        unregister_chrdev(major_number, DEVICE_NAME);
        pr_err("Failed to allocate device buffer\n");
        return -ENOMEM;
    }

    pr_info("Simple char driver registered with major number %d\n", major_number);
    return 0;
}

static void __exit simple_char_driver_exit(void) {
    kfree(device_buffer);
    unregister_chrdev(major_number, DEVICE_NAME);
    pr_info("Simple char driver unregistered\n");
}

module_init(simple_char_driver_init);
module_exit(simple_char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CS594 LKP");
MODULE_DESCRIPTION("A simple character device driver");
