#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/rwsem.h>

#define PRINT_PREF "[SYNC_SEM]: "

/* shared data: */
unsigned int counter;
struct rw_semaphore *counter_rwsemaphore;

struct task_struct *read_thread, *read_thread2, *write_thread;

static int writer_function(void *data)
{
	while(!kthread_should_stop()) {
		down_write(counter_rwsemaphore);
		counter++;

		downgrade_write(counter_rwsemaphore);
		printk(PRINT_PREF "(writer) counter: %d\n", counter);

		up_read(counter_rwsemaphore);
		msleep(500);
	}

	return 0;
}

static int read_function(void *data)
{
	while(!kthread_should_stop()) {
		down_read(counter_rwsemaphore);
		printk(PRINT_PREF "counter: %d\n", counter);
		up_read(counter_rwsemaphore);
		msleep(500);
	}

	return 0;
}

static int __init my_mod_init(void)
{
	printk(PRINT_PREF "Entering module.\n");
	counter = 0;

	counter_rwsemaphore = kmalloc(sizeof(struct rw_semaphore), GFP_KERNEL);
	if(!counter_rwsemaphore)
		return -1;

	init_rwsem(counter_rwsemaphore);

	read_thread = kthread_run(read_function, NULL, "read-thread");
	read_thread2 = kthread_run(read_function, NULL, "read-thread2");
	write_thread = kthread_run(writer_function, NULL, "write-thread");

	return 0;
}

static void __exit my_mod_exit(void)
{
	kthread_stop(read_thread);
	kthread_stop(write_thread);
	kthread_stop(read_thread2);

	kfree(counter_rwsemaphore);

	printk(KERN_INFO "Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
