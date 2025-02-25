#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

struct work_item {
	struct work_struct ws;
	int parameter;
};

struct work_item *wi1, *wi2;
struct workqueue_struct *my_wq;

static void handler( struct work_struct *work)
{
	struct work_item *wi = container_of(work, struct work_item, ws);
	pr_info("doing some work ...\n");
	pr_info("parameter is: %d\n", wi->parameter);
}

static int __init my_mod_init(void)
{
	pr_info("Entering module.\n");

	my_wq = create_workqueue("lkp_wq");
	wi1 = kmalloc(sizeof(struct work_item), GFP_KERNEL);
	wi2 = kmalloc(sizeof(struct work_item), GFP_KERNEL);

	INIT_WORK(&wi1->ws, handler);
	wi1->parameter = 51;
	INIT_WORK(&wi2->ws, handler);
	wi2->parameter = 52;

	/* Uses a system-wide shared workqueue, system_wq. */
	schedule_work(&wi1->ws);
	/* Uses a custom workqueue. */
	queue_work(my_wq, &wi2->ws);

	return 0;
}

static void __exit my_mod_exit(void)
{
	flush_workqueue(my_wq);
	kfree(wi1);
	kfree(wi2);
	destroy_workqueue(my_wq);
	pr_info("Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);
MODULE_LICENSE("GPL");
