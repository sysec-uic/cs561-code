#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/sched.h>

#define PRINT_PREF "[SYNC_SPINLOCK]: "

unsigned int counter;	/* shared data: */
DEFINE_SPINLOCK(counter_lock);
struct task_struct *read_thread, *write_thread;

static int writer_function(void *data)
{
	while(!kthread_should_stop()) {
		spin_lock(&counter_lock);
		counter++;
		spin_unlock(&counter_lock);

		msleep(500);
	}
	return 0;
}

static int read_function(void *data)
{
	while(!kthread_should_stop()) {
		spin_lock(&counter_lock);
		printk(PRINT_PREF "counter: %d\n", counter);
		spin_unlock(&counter_lock);

		msleep(500);
	}
	return 0;
}

static int __init my_mod_init(void)
{
	printk(PRINT_PREF "Entering module.\n");
	counter = 0;

	read_thread = kthread_run(read_function, NULL, "read-thread");
	write_thread = kthread_run(writer_function, NULL, "write-thread");

	return 0;
}

static void __exit my_mod_exit(void)
{
	kthread_stop(read_thread);
	kthread_stop(write_thread);
	printk(KERN_INFO "Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
