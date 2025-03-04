#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>

#define PRINT_PREF "[TIMER_TEST]: "
#define TIMER_TIMEOUT_SEC 2 /* Timeout in seconds */

/* Static to ensure it persists for the module lifetime */
static struct timer_list my_timer;

/* Timer callback function */
static void my_timer_callback(struct timer_list *t)
{
    printk(PRINT_PREF "Timer triggered after %d seconds!\n", TIMER_TIMEOUT_SEC);
}

static int __init my_mod_init(void)
{
    printk(PRINT_PREF "Entering module.\n");

    /* Initialize and set up the timer with callback */
    timer_setup(&my_timer, my_timer_callback, 0);

    /* Schedule the timer to fire after 2 seconds */
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_TIMEOUT_SEC * 1000));
    printk(PRINT_PREF "Timer scheduled to fire in %d seconds.\n", TIMER_TIMEOUT_SEC);

    return 0;
}

static void __exit my_mod_exit(void)
{
    /* Safely delete the timer and wait for any pending execution to complete */
    del_timer_sync(&my_timer);
    printk(PRINT_PREF "Timer stopped, exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LKP");
MODULE_DESCRIPTION("A simple timer test kernel module");