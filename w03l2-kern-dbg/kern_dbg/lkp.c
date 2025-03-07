#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* KERN_INFO */
#include <linux/init.h>     /* Init and exit macros */

static int answer = 42;
static int int_param = 42;

static int __init lkp_init(void)
{
    printk(KERN_INFO "Module loaded ...\n");

    /* TODO: Add a printk to print the lkp_init address here */

    printk(KERN_INFO "The answer is %d ...\n", answer);
    printk(KERN_INFO "The int_param is %d ...\n", int_param);
    BUG_ON(answer != int_param);

    return 0; /* return 0 on success, something else on error */
}

static void __exit lkp_exit(void)
{
    printk(KERN_INFO "Module exiting ...\n");
}

module_init(lkp_init); /* lkp_init() will be called at loading the module */
module_exit(lkp_exit); /* lkp_exit() will be called at unloading the module */

module_param(int_param, int, 0);
MODULE_PARM_DESC(int_param, "A sample integer kernel module parameter");

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Xiaoguang Wang <xgwang9@uic.edu>");
MODULE_DESCRIPTION("A simple kernel module");
