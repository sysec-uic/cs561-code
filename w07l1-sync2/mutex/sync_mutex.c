#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/mutex.h>

#define PRINT_PREF "[SYNC_MUTEX]: "

/* shared data: */
unsigned int counter;
struct mutex *mut;

struct task_struct *read_thread, *write_thread;

static int writer_function(void *data)
{
	while(!kthread_should_stop()) {
		mutex_lock(mut);
		//kfree(mut);       /* !!! */
		counter++;
		mutex_unlock(mut);
		msleep(500);
	}

	return 0;
}

static int read_function(void *data)
{
	while(!kthread_should_stop()) {
		mutex_lock(mut);
		printk(PRINT_PREF "counter: %d\n", counter);
		mutex_unlock(mut);
		msleep(500);
	}

	return 0;
}

static int __init my_mod_init(void)
{
	printk(PRINT_PREF "Entering module.\n");
	counter = 0;

	mut = kmalloc(sizeof(struct mutex), GFP_KERNEL);
	if(!mut)
		return -1;

	mutex_init(mut);

	read_thread = kthread_run(read_function, NULL, "read-thread");
	write_thread = kthread_run(writer_function, NULL, "write-thread");

	return 0;
}

static void __exit my_mod_exit(void)
{
	kthread_stop(read_thread);
	kthread_stop(write_thread);
	kfree(mut);
	printk(KERN_INFO "Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
