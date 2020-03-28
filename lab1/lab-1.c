#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
// #include <linux/smp.h>
#include <linux/types.h>
#include <asm/atomic.h>

MODULE_LICENSE("GPL");
/*
Challedge: find total number of cpu cores
*/
void print_total_cpu(void) {
	// printk(KERN_INFO, "Challendge: total cpus are %d", total_cpus);

	atomic_t v;
	// 

	atomic_set(&v, 4);
	atomic_add(2, &v);
	atomic_inc(&v);

	printk("%d\n", atomic_read(&v));
}


int init_module(void)
{
	printk(KERN_INFO "Lab 1: start.\n");

	printk(KERN_INFO "Begin");
	print_total_cpu();
	printk(KERN_INFO "End");

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Lab 1: end.\n");
}
