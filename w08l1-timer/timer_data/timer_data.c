#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CS594 LKP");
MODULE_DESCRIPTION("A simple kernel module with a timer");

struct my_timer_data {
    struct timer_list timer;
    int counter;
    // Add other data members as needed
    // ...
};
static struct my_timer_data my_data;

void my_timer_callback(struct timer_list *t);

void my_timer_callback(struct timer_list *t) {
    struct my_timer_data *my_data = from_timer(my_data, t, timer);
    my_data->counter++;

    // Perform actions when the timer expires
    pr_info("Timer expired! counter = %d\n", my_data->counter);

    // Restart the timer if needed
    mod_timer(&my_data->timer, jiffies + msecs_to_jiffies(1000));
}

static int __init my_module_init(void) {
    pr_info("Initializing my_module\n");

    // Initialize the timer
    timer_setup(&my_data.timer, my_timer_callback, 0);
    my_data.counter = 0;

    // Set the timer to expire after 1000 milliseconds (1 second)
    mod_timer(&my_data.timer, jiffies + msecs_to_jiffies(1000));

    return 0;
}

static void __exit my_module_exit(void) {
    pr_info("Exiting my_module\n");

    // Delete the timer when unloading the module
    del_timer(&my_data.timer);
}

module_init(my_module_init);
module_exit(my_module_exit);


