#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Xiaoguang Wang");
MODULE_DESCRIPTION("A kprobe example module");

/* kprobe structure */
static struct kprobe kp;

/* Pre-handler function executed before the probed function */
static int handler_pre(struct kprobe *p, struct pt_regs *regs) {
    char filename[256];
    if (regs->si && copy_from_user(filename, (char __user *)regs->si,
				   sizeof(filename)) == 0) {
        printk(KERN_INFO "[kprobe] %s (pid: %d): File opened: %s\n",
	       current->comm, current->pid, filename);
    }
    return 0;
}

/* Module initialization */
static int __init kprobe_init(void) {
    printk(KERN_INFO "[kprobe] Loading kprobe module...\n");

    kp.symbol_name = "do_sys_openat2";
    kp.pre_handler = handler_pre;

    if (register_kprobe(&kp) < 0) {
        printk(KERN_ERR "[kprobe] Failed to register kprobe\n");
        return -1;
    }

    printk(KERN_INFO "[kprobe] kprobe registered at %s\n", kp.symbol_name);
    return 0;
}

/* Module exit function */
static void __exit kprobe_exit(void) {
    unregister_kprobe(&kp);
    printk(KERN_INFO "[kprobe] kprobe unregistered\n");
}

module_init(kprobe_init);
module_exit(kprobe_exit);
