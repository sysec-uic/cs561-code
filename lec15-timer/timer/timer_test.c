#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>

#define PRINT_PREF	"[TIMER_TEST]: " 

struct timer_list my_timer;

static void my_handler(unsigned long data)
{
	printk(PRINT_PREF "handler executed!\n");
}

static int __init my_mod_init(void)
{
	printk(PRINT_PREF "Entering module.\n");

	/* initializes the timer data structure internal values: */
	init_timer(&my_timer);

	/* fill out the interesting fields: */
	my_timer.data = 0;
	my_timer.function = my_handler;
	my_timer.expires = jiffies + 2*HZ; /* timeout == 2secs */
	
	add_timer(&my_timer);
	printk(PRINT_PREF "Timer started\n");

	return 0;
}

static void __exit my_mod_exit(void)
{
	del_timer(&my_timer);
	printk(PRINT_PREF "Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);
