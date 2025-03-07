#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>

void my_tasklet_handler(struct tasklet_struct *ts);

static DECLARE_TASKLET(my_tasklet, my_tasklet_handler);

void my_tasklet_handler(struct tasklet_struct *ts)
{
	pr_info("tasklet executing.\ncount: %d, state: %ld",
		atomic_read(&ts->count), ts->state);
}

static int __init my_mod_init(void)
{
	pr_info("Entering module.\n");
	pr_info("Scheduling tasklet.\n");

	tasklet_schedule(&my_tasklet);

	return 0;
}

static void __exit my_mod_exit(void)
{
	tasklet_disable(&my_tasklet);
	pr_info("Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);
MODULE_LICENSE("GPL");
