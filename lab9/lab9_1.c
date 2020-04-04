#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>

MODULE_LICENSE("GPL");

#define INTERVAL_BETWEEN_CALLBACKS (100 * 10000000LL)

int arr[10] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
int counter = 1;
struct timespec tv; 


static struct hrtimer timer;
static ktime_t kt;

static s64 starttime_ns;

void print_current_time(void) {
   getnstimeofday(&tv);
   long long hours = (tv.tv_sec/3600 + 6)%24;
   long long minutes = (tv.tv_sec/60)%60;
   long long seconds = tv.tv_sec%60; 
   pr_info("Current time: %lld:%lld:%lld", hours, minutes, seconds);
}

static enum hrtimer_restart my_hrtimer_callback( struct hrtimer *timer )
{
    pr_info("---------------------------------------------------------");
	printk("I'm in hrtimer handler. Counter now is %d\r\n", counter);
    print_current_time();
    printk("Current fibonacci number: %d", arr[counter]);
    s64 now_ns = ktime_to_ns(ktime_get());
    kt = ktime_set(0, arr[counter]*INTERVAL_BETWEEN_CALLBACKS); 
    hrtimer_forward(timer, timer->_softexpires, kt);

    if(counter <= 9) {
        counter++;
    }
    return HRTIMER_RESTART;
}
 
static int init_module_hrtimer( void )
{
	printk("mod_hrtimer: installing module...\n");
	kt = ktime_set(0, INTERVAL_BETWEEN_CALLBACKS);
    hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    hrtimer_start(&timer, kt, HRTIMER_MODE_REL);
    timer.function = my_hrtimer_callback;
    return 0;
}
 

static void cleanup_module_hrtimer( void )
{
	int ret;
	ret = hrtimer_cancel( &timer );
	if (ret)
	printk("mod_hrtimer: The timer was still in use...\n");
	printk("mod_hrtimer: HR Timer module uninstalling\n");
}
 
module_init(init_module_hrtimer);
module_exit(cleanup_module_hrtimer);