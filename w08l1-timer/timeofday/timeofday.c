#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timekeeping.h> /* For ktime_get_real_ts64, ktime_get_boottime_ts64 */
#include <linux/delay.h>       /* For udelay() */

#define PRINT_PREF "[TIMEOFDAY]: "

static int __init my_mod_init(void)
{
    struct timespec64 ts, start, stop;
    ktime_t kt, start_kt, stop_kt;

    printk(PRINT_PREF "Entering module.\n");

    /* Number of seconds since the epoch (01/01/1970) */
    unsigned long seconds = ktime_get_real_seconds();
    printk(PRINT_PREF "ktime_get_real_seconds() returns %lu\n", seconds);

    /* Seconds + nanoseconds using struct timespec64 */
    ktime_get_real_ts64(&ts);
    printk(PRINT_PREF "ktime_get_real_ts64() returns: %lld (sec), %ld (nsec)\n",
           (s64)ts.tv_sec, ts.tv_nsec);

    /* Get the boot time offset */
    ktime_get_boottime_ts64(&ts);
    printk(PRINT_PREF "ktime_get_boottime_ts64() returns: %lld (sec), %ld (nsec)\n",
           (s64)ts.tv_sec, ts.tv_nsec);

    /* Another interface using ktime_t */
    kt = ktime_get();
    printk(PRINT_PREF "ktime_get() returns %lld (nsec)\n", ktime_to_ns(kt));

    /* Subtract two struct timespec64 to get uptime */
    ktime_get_boottime_ts64(&start);
    ktime_get_real_ts64(&stop);
    ts = timespec64_sub(stop, start);
    printk(PRINT_PREF "Uptime: %lld.%09ld secs\n", (s64)ts.tv_sec, ts.tv_nsec);

    /* Measure execution time of a piece of code */
    start_kt = ktime_get();
    udelay(100); /* Delay for 100 microseconds */
    stop_kt = ktime_get();

    kt = ktime_sub(stop_kt, start_kt);
    printk(PRINT_PREF "Measured execution time: %lld usecs\n", ktime_to_us(kt));

    return 0;
}

static void __exit my_mod_exit(void)
{
    printk(PRINT_PREF "Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LKP");
MODULE_DESCRIPTION("A kernel module to demonstrate timekeeping functions");