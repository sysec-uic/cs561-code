#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/sched.h>

#define PRINT_PREF "[SYNC_RWSPINLOCK]: "

/* shared data: */
unsigned int counter;
DEFINE_RWLOCK(counter_lock);

struct task_struct *read_thread1, *read_thread2, *read_thread3, *write_thread;

static int writer_function(void *data)
{
	while(!kthread_should_stop()) {
		write_lock(&counter_lock);
		counter++;
		write_unlock(&counter_lock);

		msleep(500);
	}

	return 0;
}

static int read_function(void *data)
{
	while(!kthread_should_stop()) {
		read_lock(&counter_lock);
		printk(PRINT_PREF "counter: %d\n", counter);
		read_unlock(&counter_lock);

		msleep(500);
	}

	return 0;
}

static int __init my_mod_init(void)
{
	printk(PRINT_PREF "Entering module.\n");
	counter = 0;

	read_thread1 = kthread_run(read_function, NULL, "read-thread1");
	read_thread2 = kthread_run(read_function, NULL, "read-thread2");
	read_thread3 = kthread_run(read_function, NULL, "read-thread3");
	write_thread = kthread_run(writer_function, NULL, "write-thread");

	return 0;
}

static void __exit my_mod_exit(void)
{
	kthread_stop(read_thread3);
	kthread_stop(read_thread2);
	kthread_stop(read_thread1);
	kthread_stop(write_thread);
	printk(KERN_INFO "Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
