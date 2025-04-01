/**
 * vfs_inspector.c: A simple Linux kernel module to inspect VFS data structures.
 * Usage: sudo insmod vfs_inspector.ko filepath=/path/to/file
 * */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/namei.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adv. LKP");
MODULE_DESCRIPTION("A simple module to inspect VFS data structures.");

static char *filepath = "/etc/passwd";  /* Default file to inspect */
module_param(filepath, charp, 0644);
MODULE_PARM_DESC(filepath, "Path of the file to inspect");

static int __init vfs_inspector_init(void)
{
    struct file *file;
    struct inode *inode;
    struct dentry *dentry;
    struct super_block *sb;

    printk(KERN_INFO "VFS Inspector: Initializing module...\n");

    /* Open the file using kernel's internal API */
    file = filp_open(filepath, O_RDONLY, 0);
    if (IS_ERR(file)) {
        printk(KERN_ERR "VFS Inspector: Cannot open file %s\n", filepath);
        return PTR_ERR(file);
    }

    /* Retrieve VFS data structures */
    dentry = file->f_path.dentry;
    inode = dentry->d_inode;
    sb = inode->i_sb;

    /* Log details about the file (inode, superblock, dentry and file) */
    printk(KERN_INFO "VFS Inspector: Inspecting file: %s\n", filepath);

    printk(KERN_INFO "VFS Inspector: Inode number: %lu\n", inode->i_ino);
    printk(KERN_INFO "VFS Inspector: Inode mode: %o\n", inode->i_mode);
    printk(KERN_INFO "VFS Inspector: Inode size: %lld bytes\n", inode->i_size);

    printk(KERN_INFO "VFS Inspector: Filesystem type: %s\n", sb->s_type->name);
    printk(KERN_INFO "VFS Inspector: Superblock block size: %lu\n", sb->s_blocksize);

    printk(KERN_INFO "VFS Inspector: Dentry name: %s\n", dentry->d_name.name);
    
    printk(KERN_INFO "VFS Inspector: File descriptor position: %lld\n", file->f_pos);

    /* Close the file */
    filp_close(file, NULL);

    return 0;
}

static void __exit vfs_inspector_exit(void)
{
    printk(KERN_INFO "VFS Inspector: Module unloaded.\n");
}

module_init(vfs_inspector_init);
module_exit(vfs_inspector_exit);
