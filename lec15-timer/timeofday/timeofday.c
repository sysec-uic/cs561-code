#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timekeeping.h>
#include <linux/ktime.h>
#include <asm-generic/delay.h>

#define PRINT_PREF	"[TIMEOFDAY]: " 

extern void getboottime64(struct timespec64 *ts);

static int __init my_mod_init(void)
{
	unsigned long seconds;
	struct timespec64 ts, start, stop;
	ktime_t kt, start_kt, stop_kt;

	printk(PRINT_PREF "Entering module.\n");

	/* Number of seconds since the epoch (01/01/1970) */
	seconds = get_seconds();
	printk("get_seconds() returns %lu\n", seconds);

	/* Same thing with seconds + nanoseconds using struct timespec */
	ts = current_kernel_time64();
	printk(PRINT_PREF "current_kernel_time64() returns: %lu (sec),"
		"i %lu (nsec)\n", ts.tv_sec, ts.tv_nsec);

	/* Get the boot time offset */
	getboottime64(&ts);
	printk(PRINT_PREF "getboottime64() returns: %lu (sec),"
		"i %lu (nsec)\n", ts.tv_sec, ts.tv_nsec);

	/* The correct way to print a struct timespec as a single value: */
	printk(PRINT_PREF "Boot time offset: %lu.%09lu secs\n", ts.tv_sec, ts.tv_nsec);
	/* Otherwise, just using %lu.%lu transforms this:
	 * ts.tv_sec  == 10
	 * ts.tv_nsec == 42
	 * into: 10.42 rather than 10.000000042
	 */
	
	/* another interface using ktime_t */
	kt = ktime_get();
	printk(PRINT_PREF "ktime_get() returns %llu\n", kt.tv64);

	/* Subtract two struct timespec */
	getboottime64(&start);
	stop = current_kernel_time64();
	ts = timespec64_sub(stop, start);
	printk(PRINT_PREF "Uptime: %lu.%09lu secs\n", ts.tv_sec, ts.tv_nsec);

	/* measure the execution time of a piece of code */
	start_kt = ktime_get();
	udelay(100);
	stop_kt = ktime_get();
	
	kt = ktime_sub(stop_kt, start_kt);
	printk(PRINT_PREF "Measured execution time: %llu usecs\n", (kt.tv64)/1000);
	
	return 0;
}

static void __exit my_mod_exit(void)
{
	printk(PRINT_PREF "Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
