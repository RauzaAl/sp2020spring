#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#define TAG "lab8_1"
#define MAX_NUM 1000000

int unsorted[MAX_NUM];
int sorted[MAX_NUM];

static struct task_struct *t1;
static struct task_struct *t2;
static struct task_struct *t3;
static struct task_struct *t4;

DEFINE_SPINLOCK(lock);
struct mergeParams {
    int begin;
    int mid;
    int end;
};

struct insertionSortParams {
    int start;
    int end;
};

int gen_int(void) {
    int n;
    get_random_bytes(&n, sizeof(n));
    n = n < 0 ? n * -1 : n;
    n = n % MAX_NUM;
    return n;
}

void fillArray(int size) {
    int i;
    for (i=0; i<size; i++)
        unsorted[i] = gen_int();
}
void printArray(int *list, int size) {
    int i;
     for (i=0; i<10; i++)
        printk("%d\n", list[i]); 
    printk(".\n.\n.\n");
    for (i=size-11; i<size-1; i++)
        printk("%d\n", list[i]);  
    printk("\n");
}

static int insertionSort(void *args) {
    spin_lock(&lock);
    struct insertionSortParams *params = (struct insertionSortParams*) args;
    int start = params->start, end = params->end;
   
    int i = start, j, itemToInsert;

    while (i <= end) {
        itemToInsert = unsorted[i]; 
        j = i-1;
        while (j >= start && itemToInsert < unsorted[j])
        {
            unsorted[j+1] = unsorted[j];
            j--;
        }
        unsorted[j+1] = itemToInsert;
        i++;
    }
    spin_unlock(&lock);
    return 0;
}
static int merge(void *args) {
    spin_lock(&lock);
    struct mergeParams *params = (struct mergeParams*) args;
    int begin = params->begin, mid = params->mid, end = params->end;
    int i = begin, j = mid, tpos = begin;

    while (i < mid && j <= end)
    {
        if (unsorted[i] < unsorted[j])
            sorted[tpos++] = unsorted[i++];
        else
            sorted[tpos++] = unsorted[j++];
    }

    while (i < mid)
        sorted[tpos++] = unsorted[i++];
    while (j <= end)
        sorted[tpos++] = unsorted[j++];
    spin_unlock(&lock);
    printArray(sorted, MAX_NUM);
    return 0;
}


int init_module(void)
{
    printk(KERN_INFO "START %s\n", TAG);
    fillArray(MAX_NUM);
    printArray(unsorted, MAX_NUM);

    struct insertionSortParams sArgs[2];
    sArgs[0].start = 0;
    sArgs[0].end = MAX_NUM/2;
    sArgs[1].start = MAX_NUM/2 + 1;
    sArgs[1].end = MAX_NUM -1;

    struct mergeParams mArgs;
    mArgs.begin = 0;
    mArgs.mid = MAX_NUM/2 + 1;
    mArgs.end = MAX_NUM - 1;
    t1 = kthread_run(insertionSort, &sArgs[0], "first");
    printk("---1----");
    t2 = kthread_run(insertionSort, &sArgs[1], "second");
    printk("---2----");
    t3 = kthread_run(merge, &mArgs, "third");
    printk("---3----");
    t4 = kthread_run(merge, &mArgs, "forth");
    kthread_stop(t1);
    kthread_stop(t2);
    kthread_stop(t3);
    kthread_stop(t4);
    return 0;
}

void cleanup_module(void)
{
        printk(KERN_DEBUG "END %s\n", TAG);
}
