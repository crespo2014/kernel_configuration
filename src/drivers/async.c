/*
 * dependency.c
 *
 * It implements a task scheduler with dependencies.
 * Every task can depends only from another one, not multiple dependencies is allowed
 *
 * A task has this members.
 * status - block by dependency, waiting to run
 * depends - the task that depends on
 *
 * When a task is complete all dependencies are unblocked, one of them is execute in the same thread
 * if there is no more dependencies to unblock then a waiting one is pick from the list.
 *
 * every running task is move to the end of the list
 * when list size reach zero then the running thread finish.
 *
 * if there is no any task to do then the thread must wait in a waiting queue until waiting task !=0
 * ones all task are done ( list size ==0) then waiting task get the value -1 to wakeup all waiting threads
 * and release all thread because list is empty.
 * spin lock have to be use to access the list
 *
 * any module which its dependency is not found on the list will take the highest
 *
 * Picking a new task.
 * Wake up all waiting task.
 * If number of waked up task != 1 the decrement and waiting += waked up
 * if (waiting) wake_up queue
 * if list empty return null
 * if waiting == 0 then pick one, waiting = len, signal queue
 *
 *	Main thread will start others threads and pick task from the list.
 *	if all task are waiting for end_idx then wait for all threads
 *
 *	List holding task
 *
 *  |----------------|----------|------------|
 *  begin            waiting    running      done/end
 *
 *  if waiting == running then all task became ready
 *
 *  two tables are in use
 *  1 . task data
 *  2 . task index -
 *
 *  Created on: 16 Apr 2015
 *      Author: lester.crespo
 */

#include "linux/async_minit.h"

#ifdef TEST
#include <stdio.h>

#define __wake_up(...)
#define do_one_initcall(...)
#define schedule(...)
#define prepare_to_wait_for(...)
#define finish_wait(...)
#define cpu_online_mask(...) 0
#define spin_lock(...)
#define spin_unlock(...)
#define wake_up_interruptible(...) 0
#define num_online_cpus(...) 1
#define kthread_create(...) 1
#define wait_event_interruptible(...) 0
#define ERR_PTR(...) 6
#define ENOMEM 6
//#define kthread_create_on_node(...) 0
#define kthread_bind(...)
#define wake_up_process(...)
#define printk(...) printf( #__VA_ARGS__)
#define _raw_spin_lock(...)
struct init_fn_t __async_initcall_start[1], __async_initcall_end[1];
struct dependency_t __async_modules_depends_start[]  = {
        MOD_DEPENDENCY_ITEM(snd_hrtimer_init,alsa_timer_init),
        MOD_DEPENDENCY_ITEM(alsa_mixer_oss_init,alsa_pcm_init),       //snd-mixer-oss
        MOD_DEPENDENCY_ITEM(alsa_pcm_oss_init,alsa_mixer_oss_init),   // snd-pcm-oss
        MOD_DEPENDENCY_ITEM(snd_hda_codec,alsa_hwdep_init),
        MOD_DEPENDENCY_ITEM(alsa_hwdep_init,alsa_pcm_init),
        MOD_DEPENDENCY_ITEM(alsa_seq_device_init,alsa_timer_init),
        MOD_DEPENDENCY_ITEM(alsa_seq_init,alsa_seq_device_init),
        MOD_DEPENDENCY_ITEM(alsa_seq_midi_event_init,alsa_seq_init),
        MOD_DEPENDENCY_ITEM(alsa_seq_dummy_init,alsa_seq_init),
        MOD_DEPENDENCY_ITEM(alsa_seq_oss_init,alsa_seq_midi_event_init)
        };
struct dependency_t* __async_modules_depends_end = __async_modules_depends_start + 10;

#define DEFINE_SPINLOCK(a) int a
#define DECLARE_WAIT_QUEUE_HEAD(a) int a

#else

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/debugfs.h>
#include <linux/mm.h>  // mmap related stuff#include <linux/slab.h>#include <linux/types.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/types.h>
#include <asm/atomic.h>
#include <linux/kthread.h>  // for threads


/**
 * Static struct holding all data
 */
extern struct init_fn_t __async_initcall_start[], __async_initcall_end[];
extern struct dependency_t __async_modules_depends_start[], __async_modules_depends_end[];

#endif
#ifdef CONFIG_ASYNCHRO_MODULE_INIT_DEBUG
#define printk_debug(...) printk(__VA_ARGS__)
#else
#define printk_debug(...) do {} while(0)
#endif


//
//extern initcall_t __initcall_start[];
//extern initcall_t __initcall0_start[];
//extern initcall_t __initcall1_start[];
//extern initcall_t __initcall2_start[];
//extern initcall_t __initcall3_start[];
//extern initcall_t __initcall4_start[];
//extern initcall_t __initcall5_start[];
//extern initcall_t __initcall6_start[];
//extern initcall_t __initcall7_start[];
//extern initcall_t __initcall_end[];
//
//static initcall_t *initcall_levels[] __initdata =
//{ //
//        __initcall0_start, //
//        __initcall1_start, //
//        __initcall2_start, //
//        __initcall3_start, //
//        __initcall4_start, //
//        __initcall5_start, //
//        __initcall6_start, //
//        __initcall7_start, //
//        __initcall_end //
//        };

#ifdef CONFIG_ASYNCHRO_MODULE_INIT_DEBUG
static const char* const module_name[] =
{   "",INIT_CALLS(macro_str)};
#endif

/**
 * Dependencies list can be declare any time in any c file
 */

//ALSA
ADD_MODULE_DEPENDENCY(alsa_seq_device_init,alsa_timer_init);
ADD_MODULE_DEPENDENCY(alsa_seq_init,alsa_seq_device_init);
ADD_MODULE_DEPENDENCY(snd_hrtimer_init,alsa_timer_init);
ADD_MODULE_DEPENDENCY(alsa_pcm_init,alsa_timer_init);

ADD_MODULE_DEPENDENCY(alsa_mixer_oss_init,alsa_pcm_init);       //snd-mixer-oss
ADD_MODULE_DEPENDENCY(alsa_pcm_oss_init,alsa_mixer_oss_init);
   // snd-pcm-oss
ADD_MODULE_DEPENDENCY(snd_hda_codec,alsa_hwdep_init);
ADD_MODULE_DEPENDENCY(alsa_hwdep_init,alsa_pcm_init);

ADD_MODULE_DEPENDENCY(alsa_seq_midi_event_init,alsa_seq_init);
ADD_MODULE_DEPENDENCY(alsa_seq_dummy_init,alsa_seq_init);	
ADD_MODULE_DEPENDENCY(alsa_seq_oss_init,alsa_seq_midi_event_init);	
/* HDA snd is exported function plus all patches */		
ADD_MODULE_DEPENDENCY(patch_si3054_init,alsa_hwdep_init);		
ADD_MODULE_DEPENDENCY(patch_ca0132_init,alsa_hwdep_init);		
ADD_MODULE_DEPENDENCY(patch_hdmi_init,alsa_hwdep_init);		
ADD_MODULE_DEPENDENCY(patch_sigmatel_init,alsa_hwdep_init);		
ADD_MODULE_DEPENDENCY(patch_cirrus_init,alsa_hwdep_init);		
ADD_MODULE_DEPENDENCY(patch_ca0110_init,alsa_hwdep_init);		
ADD_MODULE_DEPENDENCY(patch_via_init,alsa_hwdep_init);		
ADD_MODULE_DEPENDENCY(patch_realtek_init,alsa_hwdep_init);		
ADD_MODULE_DEPENDENCY(patch_conexant_init,alsa_hwdep_init);		
ADD_MODULE_DEPENDENCY(patch_cmedia_init,alsa_hwdep_init);		
ADD_MODULE_DEPENDENCY(patch_analog_init,alsa_hwdep_init);		



ADD_MODULE_DEPENDENCY(ubi_init,init_mtd);       //mtd

ADD_MODULE_DEPENDENCY(hilscher_pci_driver_init,uio_init);             //uio -> uio_cif


//USB
ADD_MODULE_DEPENDENCY(usb_core,usb_common);
ADD_MODULE_DEPENDENCY(ohci_hcd_mod_init,usb_core);
ADD_MODULE_DEPENDENCY(uhci_hcd_init,usb_core);
ADD_MODULE_DEPENDENCY(usbmon,usb_core);
ADD_MODULE_DEPENDENCY(usb_storage_driver_init,usb_core);
ADD_MODULE_DEPENDENCY(led_driver_init,usb_core);
ADD_MODULE_DEPENDENCY(hid_init,usb_core);
ADD_MODULE_DEPENDENCY(ehci_hcd_init,usb_core);

ADD_MODULE_DEPENDENCY(ohci_pci_init,ohci_hcd_mod_init);
ADD_MODULE_DEPENDENCY(ehci_platform_init,ohci_hcd_mod_init);
ADD_MODULE_DEPENDENCY(hid_init,usb_storage_driver_init);
ADD_MODULE_DEPENDENCY(uas_driver_init,usb_storage_driver_init);
ADD_MODULE_DEPENDENCY(realtek_cr_driver_init,usb_storage_driver_init);
ADD_MODULE_DEPENDENCY(ene_ub6250_driver_init,usb_storage_driver_init);		

ADD_MODULE_DEPENDENCY(ehci_pci_init,ehci_hcd_init);
ADD_MODULE_DEPENDENCY(ehci_platform_init,ehci_hcd_init);

ADD_MODULE_DEPENDENCY(smsc,libphy);

//ADD_MODULE_DEPENDENCY(azx_driver,alsa_hwdep_init);
//ADD_MODULE_DEPENDENCY(mxm_wmi,wmi);
//ADD_MODULE_DEPENDENCY(speedstep_ich,speedstep);

//ADD_MODULE_DEPENDENCY(mmc_block,mmc_core);
//ADD_MODULE_DEPENDENCY(videodev,usb_core);
//ADD_MODULE_DEPENDENCY(v4l2_common,videodev);
//ADD_MODULE_DEPENDENCY(videobuf2_core,v4l2_common);
//ADD_MODULE_DEPENDENCY(videobuf2_memops,videobuf2_core);
//ADD_MODULE_DEPENDENCY(videobuf2_vmalloc,videobuf2_memops);

//ADD_MODULE_DEPENDENCY(uvcvideo,videobuf2_vmalloc);
//ADD_MODULE_DEPENDENCY(gspca_main,videodev);
//ADD_MODULE_DEPENDENCY(rfcomm_init,bt_init);
//ADD_MODULE_DEPENDENCY(coretemp,hwmon);
//ADD_MODULE_DEPENDENCY(gpio_fan,hwmon);
//ADD_MODULE_DEPENDENCY(acpi_processor_driver_init,hwmon);

		
		
#define MAX_TASKS 200

/**
 * all initcall will be enums. a tbl will store all names
 */
struct task_t
{
    task_type_t type;           //
    modules_e id;           // idx in string table
    initcall_t fnc;             // ptr to init function
    unsigned waiting_for_id;
    unsigned waiting_count; // how many task does it depend on
    unsigned child_count;   // how many task it triggers
};

// TODO join together all task with taskid table to allow dynamic allocation
struct task_list_t
{
    unsigned idx_list[MAX_TASKS];
    unsigned *waiting_last;  // last task waiting to be release
    unsigned *ready_last;    // last task to be execute
    unsigned *running_last;  // the last task running
    unsigned *idx_end;      // number of idx on list
    struct task_t all[MAX_TASKS];
    struct task_t* task_end;
    unsigned task_left;     // how many of global task are left to done
};

static struct task_list_t tasks;

struct task_t* getTask(modules_e id)
{
    struct task_t* t = tasks.all;
    while(t != tasks.task_end && t->id != id)
        ++t;
    if (t == tasks.task_end)
        t = NULL;
    return t;
}

/**
 * Find the waiting task on the list
 */
inline struct task_t* getWaitingTask(modules_e id)
{
    unsigned *it_idx;
    for (it_idx = tasks.idx_list;it_idx != tasks.waiting_last;++it_idx)
    {
        if (tasks.all[*it_idx].id == id)
            return tasks.all + *it_idx;
    }
    return NULL;
}

/**
 * Check if one task depends on another
 * 0 - false
 * 1 - true
 */
unsigned doesDepends(modules_e child, modules_e parent)
{
    struct dependency_t * it_dependency;
    for (it_dependency = __async_modules_depends_start; it_dependency != __async_modules_depends_end; ++it_dependency)
    {
        if (it_dependency->parent_id == parent && it_dependency->task_id == child)
            return 1;
    }
    return 0;
}

void FillTasks(struct init_fn_t* begin, struct init_fn_t* end)
{
    struct dependency_t *it_dependency;
    //unsigned* it_idx;
    struct init_fn_t* it_init_fnc;
    struct task_t* it_task;
    struct task_t* ptask;
    tasks.task_end = tasks.all;
    for (it_init_fnc = begin; it_init_fnc < end; ++it_init_fnc, ++tasks.task_end)
    {
        tasks.task_end->id = it_init_fnc->id;
        tasks.task_end->type = it_init_fnc->type_;
        tasks.task_end->fnc = it_init_fnc->fnc;
        tasks.task_end->waiting_for_id = 0;
        tasks.task_end->waiting_count = 0;
    }
    tasks.task_left = tasks.task_end - tasks.all;
    // resolve dependencies
    for (it_task = tasks.all; it_task != tasks.task_end; ++it_task)
    {
        for (it_dependency = __async_modules_depends_start; it_dependency != __async_modules_depends_end; ++it_dependency)
        {
            // at the moment only one dependency is supported
            if (it_dependency->task_id == it_task->id)
            {
                // Do not register a dependency that is not in the current list
                ptask = getTask(it_dependency->parent_id);
                if (ptask == NULL || ptask->type != it_task->type)
                    printk("async Dependency %d not found for id %d",it_dependency->parent_id,it_dependency->task_id);
                else
                {
                    // register dependency
                    ++it_task->waiting_count;
                    it_task->waiting_for_id = it_dependency->parent_id;
                }
            }
            if (it_dependency->parent_id == it_task->id)
            {
                ++it_task->child_count;
            }
        }
        printk_debug("async registered '%s' depends on '%s'\n", module_name[it_task->id], it_task->waiting_count != 0 ? module_name[it_task->waiting_for_id]: "");
    }
}
/**
 * Prepare dependencies structure to process an specific type of task
 */
void Prepare(task_type_t type)
{
    // Pick only task of type from all task
    struct task_t* it_task;
	unsigned *it_idx1;
	unsigned *it_idx2;
	unsigned id;
	tasks.idx_end = tasks.idx_list;
    //
    for (it_task = tasks.all; it_task != tasks.task_end; ++it_task)
    {
        if (it_task->type == type)
        {
			*tasks.idx_end  = it_task - tasks.all;
            ++tasks.idx_end;
        }
    }
    tasks.ready_last = tasks.idx_end;
    tasks.waiting_last = tasks.idx_end;
    tasks.running_last = tasks.idx_end;
    // jump waiting task
    it_idx1 = tasks.idx_list;
    while (it_idx1 != tasks.idx_end && tasks.all[*it_idx1].waiting_count != 0)
        ++it_task;
    for (it_idx2 = it_idx1 + 1; it_idx2 < tasks.idx_end; ++it_idx2)
    {
        //move waiting task to front
        if (tasks.all[*it_idx2].waiting_count != 0)
        {
            id = *it_idx2;
            *it_idx2 = *it_idx1;
            *it_idx1 = id;
            ++it_idx1;
        }
    }
    tasks.waiting_last = it_idx1;
}

/**
 * wait on the queue until unlocked != 0 then pick a task
 * locked == running; unlock all remaining task and scheduler
 *
 * main thread wait for unlocked !=0 or waiting == running
 * threads end when waiting == 0
 * last thread is when running became 0
 */

static DEFINE_SPINLOCK(list_lock);
static DECLARE_WAIT_QUEUE_HEAD( list_wait);

/**
 * Mark task as done and get
 * Get a task from the list for execution
 * nullptr - no more task available
 */
struct task_t* TaskDone(struct task_t* ptask)
{
    struct dependency_t * it_dependency;
    unsigned* it_idx;
    unsigned* it_idx2;
    unsigned idx;
    unsigned child_count = 0;    // how many task has been wakeup
    if (tasks.running_last == 0)
        return 0;
    //lock
    spin_lock(&list_lock);
    if (ptask < tasks.task_end)
    {
        tasks.task_left--;
        // move to done list
        for (it_idx = tasks.ready_last; it_idx < tasks.running_last; ++it_idx)
        {
            if (tasks.all + *it_idx == ptask)
            {
                // bring down a task to do
                --tasks.running_last;
                *it_idx = *tasks.running_last;
                break;
            }
        }
        // release waiting tasks
        child_count = ptask->child_count;
        if (child_count)
        {
            // find parent task on dependency list
            for (it_dependency = __async_modules_depends_start; it_dependency != __async_modules_depends_end; ++it_dependency)
            {
                if (it_dependency->parent_id == ptask->id)
                {
                    //find child task on waiting list
                    for (it_idx = tasks.idx_list;it_idx != tasks.waiting_last;)
                    {
                        if (tasks.all[*it_idx].id == it_dependency->task_id)
                        {
                            --tasks.all[*it_idx].waiting_count;
                            // move to running list
                            --tasks.waiting_last;
                            idx = *tasks.waiting_last;
                            *tasks.waiting_last = *it_idx;
                            *it_idx = idx;
                        }
                        else
                            ++it_idx;
                    }
                    --ptask->child_count;
                    if (ptask->child_count == 0)
                        break;
                }
            }
            //check for missing child
            if (ptask->child_count != 0)
                printk("async Failed to release all childs");
        }

    }
    // check if not running task and not pending one
    if (tasks.running_last == tasks.waiting_last)
    {
        tasks.waiting_last = 0;
    }
    // pick a new task
    ptask = tasks.task_end;
    if (tasks.waiting_last != tasks.ready_last)
    {
        // find the lower id task available
        it_idx2 = tasks.waiting_last;
        // check ready list
        for (it_idx = tasks.waiting_last+1;it_idx != tasks.ready_last;++it_idx)
        {
            // pick the lower index to task table
            if (*it_idx < *it_idx2)
            {
                it_idx2 = it_idx;
            }
        }
        --tasks.waiting_last;
        idx = *tasks.waiting_last;
        *tasks.waiting_last = *it_idx2;
        *it_idx2 = idx;
    }
    else
    {
        // check end of all task
        if (tasks.running_last == 0)
        {
            // clean up all memory if apply
            wake_up_interruptible(&list_wait);
        }
    }
    // spin unlock
    spin_unlock(&list_lock);
    // allow other thread pick a task
    if (child_count > 1)
        wake_up_interruptible(&list_wait);
    if (tasks.running_last == 0 && (tasks.task_left == 0))
    {
        //free_initmem(); do not doit until deferred
    }
    return (ptask == tasks.task_end ? NULL : ptask);
}

int WorkingThread(void *data)
{
    int ret;
    struct task_t* ptask = NULL;
    printk_debug("async %d starts\n", (unsigned)data);
    do
    {
        ptask = TaskDone(ptask);
        if (ptask != NULL)
        {
            printk_debug("async %d %s\n", (unsigned)data, module_name[ptask->id]);
            do_one_initcall(ptask->fnc);
        }
        else
        {
            printk_debug("async %d waiting ...\n", (unsigned)data);
            ret = wait_event_interruptible(list_wait, (tasks.ready_last != tasks.waiting_last || tasks.waiting_last == 0));
            if (ret != 0)
            {
                printk("async init wake up returned %d\n", ret);
                break;
            }
            //wait for (depends.unlocked !=0 or depends.waiting_last == 0)
        }
    } while (tasks.waiting_last != 0);	// something to do
    printk_debug("async %d ends\n", (unsigned)data);
    return 0;
}

/**
 * Execute all initialization for an specific type
 * We need wait for everything done as a barrier to avoid problems
 */
int doit_type(task_type_t type)
{
    unsigned max_threads = CONFIG_ASYNCHRO_MODULE_INIT_THREADS;
    unsigned max_cpus = num_online_cpus();
    static struct task_struct *thr;
    Prepare(type);
    if (max_threads == 0)
        WorkingThread(0);
    for (; max_threads != 0; --max_threads)
    {
        //start working threads
        thr = kthread_create(WorkingThread, (void* )(max_threads), "async thread");
        if (thr != ERR_PTR(-ENOMEM))
        {
            kthread_bind(thr, max_threads % max_cpus);
            wake_up_process(thr);
        }
        else
        {
            printk("Async module initialization thread failed .. fall back to normal mode");
            WorkingThread(NULL);
        }
    }

    return 0;
}

//void traceInitCalls(void)
//{
//    initcall_t *fn;
//    int level;
//    for (level = 0; level < ARRAY_SIZE(initcall_levels) - 1; level++)
//    {
//        for (fn = initcall_levels[level]; fn < initcall_levels[level + 1]; fn++)
//        {
//            printk_debug("initcall %d , %pF ", level, fn);
//        }
//    }
//}
/**
 * First initialization of module. Disk diver and AGP  
 */
static int async_initialization(void)
{
    int ret;
    FillTasks(__async_initcall_start, __async_initcall_end);
    //traceInitCalls();
    printk_debug("async started asynchronized\n");
    doit_type(asynchronized);
    ret = wait_event_interruptible(list_wait, (tasks.running_last != tasks.idx_list));
    return 0;
}
/**
 * Second initialization USB devices, some PCI
 */
static int deferred_initialization(void)
{
    printk_debug("async started deferred\n");
    //return doit_type(deferred);
    return 0;
}

module_init(async_initialization);
late_initcall_sync(deferred_initialization);		// Second stage, last to do before jump to high level initialization

#ifdef TEST
#define DOIT(x) do { printf("...\n"); Prepare(x,sizeof(x)/sizeof(*x)); WorkingThread(); } while(0)

int main(void)
{
    // single
    struct init_fn_t list1[] =
    {
        {   asynchronized,rfcomm_init_id,0},
        {   asynchronized,alsa_pcm_init_id, 0},
        {   asynchronized,alsa_seq_init_id, 0},
        {   deferred,snd_hda_codec_id, 0}};
    FillTasks(list1,list1+4);
    doit_type(asynchronized);
    /*
    // keep order for single thread but can be all together
    struct init_fn_t list2[] =
    {
        {   asynchronized,"a", 0},
        {   asynchronized,"b",0},
        {   asynchronized,"c", 0},
        {   asynchronized,"d", 0}};
    // order is keep because dependences are resolved on time
    struct init_fn_t list3[] =
    {
        {   asynchronized,"a",0},
        {   asynchronized,"b","a"},
        {   asynchronized,"c","b"},
        {   asynchronized,"d",0},
        {   asynchronized,"e", 0}};

    struct init_fn_t list4[] =
    {
        {   asynchronized,"a",0},
        {   asynchronized,"d","c"},
        {   asynchronized,"e",0},
        {   asynchronized,"b","a"},
        {   asynchronized,"c","b"}

    };


    //
    struct init_fn_t list5[] =
    {
        {   "a", 0}};

    DOIT(list1);
    DOIT(list2);
    DOIT(list3);
    DOIT(list4);
    */
//	Prepare(list1,sizeof(list1)/sizeof(*list1));
//	WorkingThread();
//	Prepare(list2,sizeof(list2)/sizeof(*list2));
//	WorkingThread();
//	Prepare(list3,sizeof(list3)/sizeof(*list));
//	WorkingThread();

}
#endif

