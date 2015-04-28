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
 *  |----------|---------|----------|------------|
 *    waiting     ready    running      done     |end
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
#include <linux/delay.h>
#include <linux/kthread.h>  // for threads

/**
 * Static struct holding all data
 */
extern struct init_fn_t __async_initcall_start[], __async_initcall_end[];
extern struct dependency_t __async_modules_depends_start[], __async_modules_depends_end[];

#ifdef CONFIG_ASYNCHRO_MODULE_INIT_DEBUG
#define printk_debug(...) printk(__VA_ARGS__)
#else
#define printk_debug(...) do {} while(0)
#endif


#ifdef CONFIG_ASYNCHRO_MODULE_INIT_DEBUG
static const char* const module_name[] =
{   "",INIT_CALLS(macro_str)};
#endif

/**
 * Dependencies list can be declare any time in any c file
 */

//ACPI
ADD_MODULE_DEPENDENCY(acpi_button_driver_init, acpi_ac_init);
ADD_MODULE_DEPENDENCY(acpi_hed_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(acpi_smb_hc_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(crb_acpi_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(acpi_smbus_cmi_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(atlas_acpi_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(smo8800_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(lis3lv02d_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(irst_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(smartconnect_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(acpi_pcc_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(pvpanic_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(acpi_topstar_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(toshiba_bt_rfkill_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(toshiba_haps_driver, acpi_ac_init);
ADD_MODULE_DEPENDENCY(xo15_ebook_driver, acpi_ac_init);


ADD_MODULE_DEPENDENCY(pci_hotplug_init, pcie_portdrv_init);
ADD_MODULE_DEPENDENCY(pcied_init, pci_hotplug_init);
ADD_MODULE_DEPENDENCY(shpcd_init, pcied_init);

//ALSA
ADD_MODULE_DEPENDENCY(snd_hda_intel,snd_hda_controller);

ADD_MODULE_DEPENDENCY(alsa_seq_init,alsa_timer_init);
ADD_MODULE_DEPENDENCY(alsa_seq_init,alsa_seq_device_init);
ADD_MODULE_DEPENDENCY(alsa_seq_midi_event_init,alsa_seq_init);
ADD_MODULE_DEPENDENCY(alsa_seq_dummy_init,alsa_seq_init);
ADD_MODULE_DEPENDENCY(alsa_seq_oss_init,alsa_seq_midi_event_init);

ADD_MODULE_DEPENDENCY(alsa_pcm_init,alsa_timer_init);
//ADD_MODULE_DEPENDENCY(snd_hda_codec,alsa_hwdep_init);
//ADD_MODULE_DEPENDENCY(snd_hda_codec,alsa_pcm_init);

ADD_MODULE_DEPENDENCY(snd_hrtimer_init,alsa_timer_init);

ADD_MODULE_DEPENDENCY(alsa_pcm_oss_init,alsa_mixer_oss_init);  // snd-pcm-oss
ADD_MODULE_DEPENDENCY(alsa_pcm_oss_init,alsa_pcm_init);  // snd-pcm-oss

/* HDA snd is exported function plus all patches */		
//ADD_MODULE_DEPENDENCY(patch_si3054_init,snd_hda_codec);
//ADD_MODULE_DEPENDENCY(patch_ca0132_init,snd_hda_codec);
//ADD_MODULE_DEPENDENCY(patch_hdmi_init,snd_hda_codec);
//ADD_MODULE_DEPENDENCY(snd_hda_controller,snd_hda_codec);
//ADD_MODULE_DEPENDENCY(snd_hda_codec_generic,snd_hda_codec);
//ADD_MODULE_DEPENDENCY(patch_sigmatel_init,snd_hda_codec_generic);
//ADD_MODULE_DEPENDENCY(patch_cirrus_init,snd_hda_codec_generic);
//ADD_MODULE_DEPENDENCY(patch_ca0110_init,snd_hda_codec_generic);
//ADD_MODULE_DEPENDENCY(patch_via_init,snd_hda_codec_generic);
//ADD_MODULE_DEPENDENCY(patch_realtek_init,snd_hda_codec_generic);
//ADD_MODULE_DEPENDENCY(patch_conexant_init,snd_hda_codec_generic);
//ADD_MODULE_DEPENDENCY(patch_cmedia_init,snd_hda_codec_generic);
//ADD_MODULE_DEPENDENCY(patch_analog_init,snd_hda_codec_generic);

ADD_MODULE_DEPENDENCY(hilscher_pci_driver_init,uio_init);  //uio - uio_cif

ADD_MODULE_DEPENDENCY(ubi_init,init_mtd);       //mtd

ADD_MODULE_DEPENDENCY(uas_driver_init,usb_storage_driver_init);
ADD_MODULE_DEPENDENCY(ene_ub6250_driver_init,usb_storage_driver_init);
ADD_MODULE_DEPENDENCY(realtek_cr_driver_init,usb_storage_driver_init);

//USB
ADD_MODULE_DEPENDENCY(ohci_pci_init,ohci_hcd_mod_init);
ADD_MODULE_DEPENDENCY(ohci_platform_init,ohci_hcd_mod_init);
ADD_MODULE_DEPENDENCY(ohci_pci_init,ehci_hcd_init);

ADD_MODULE_DEPENDENCY(uhci_hcd_init,ehci_hcd_init);

ADD_MODULE_DEPENDENCY(ehci_pci_init,ehci_hcd_init);
ADD_MODULE_DEPENDENCY(ehci_platform_init,ehci_hcd_init);

ADD_MODULE_DEPENDENCY(ohci_hcd_mod_init,ehci_platform_init);

ADD_MODULE_DEPENDENCY(hid_init,ohci_platform_init);
ADD_MODULE_DEPENDENCY(uhid_init,ohci_platform_init);


ADD_MODULE_DEPENDENCY(smsc,libphy);

ADD_MODULE_DEPENDENCY(lib80211_crypto_ccmp_init,lib80211_init);
ADD_MODULE_DEPENDENCY(lib80211_crypto_tkip_init,lib80211_init);
ADD_MODULE_DEPENDENCY(lib80211_crypto_wep_init,lib80211_init);
ADD_MODULE_DEPENDENCY(libipw_init,lib80211_init);
ADD_MODULE_DEPENDENCY(ipw2100_init,libipw_init);

// rng
ADD_MODULE_DEPENDENCY(intel_rng_mod_init,hwrng_modinit);
ADD_MODULE_DEPENDENCY(b43,hwrng_modinit);
ADD_MODULE_DEPENDENCY(b43legacy,hwrng_modinit);


// AGP DRM
ADD_MODULE_DEPENDENCY(drm_core_init,agp_init);
ADD_MODULE_DEPENDENCY(agp_nvidia_init,agp_init);
ADD_MODULE_DEPENDENCY(nvidia_frontend_init_module,drm_core_init);
ADD_MODULE_DEPENDENCY(uvm_init,nvidia_frontend_init_module);

ADD_MODULE_DEPENDENCY(algif_skcipher_init,af_alg_init);
ADD_MODULE_DEPENDENCY(algif_hash_init,af_alg_init);

ADD_MODULE_DEPENDENCY(crypto_authenc_esn_module_init,crypto_authenc_module_init);

ADD_MODULE_DEPENDENCY(cuse_init,fuse_init);

ADD_MODULE_DEPENDENCY(init_msdos_fs,init_fat_fs);
ADD_MODULE_DEPENDENCY(init_vfat_fs,init_fat_fs);

ADD_MODULE_DEPENDENCY(init_ext3_fs,journal_init);

//
//ADD_MODULE_DEPENDENCY(,);

#define MAX_TASKS 200

/**
 * all initcall will be enums. a tbl will store all names
 */
struct task_t
{
    task_type_t type;        //
    modules_e id;           // idx in string table
    initcall_t fnc;             // ptr to init function
    unsigned waiting_count; // how many task does it depend on
    unsigned child_count;   // how many task it triggers
};

// TODO join together all task with taskid table to allow dynamic allocation
struct task_list_t
{
    task_type_t type_;          // current type in processing
    unsigned idx_list[MAX_TASKS];
    unsigned *waiting_last;  // last task waiting to be release
    unsigned *ready_last;    // last task to be execute
    unsigned *running_last;  // the last task running
    unsigned *idx_end;      // number of idx on list
    struct task_t all[MAX_TASKS];
    struct task_t* task_end;
};

static struct task_list_t tasks;

struct task_t* getTask(modules_e id)
{
    struct task_t* t = tasks.all;
    while(t != tasks.task_end && t->id != id)
        ++t;
    return (t == tasks.task_end) ? NULL : t;
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
    struct init_fn_t* it_init_fnc;
    struct task_t* it_task;
    struct task_t* ptask;
    tasks.task_end = tasks.all;
    for (it_init_fnc = begin; it_init_fnc < end; ++it_init_fnc, ++tasks.task_end)
    {
        tasks.task_end->id = it_init_fnc->id;
        tasks.task_end->type = it_init_fnc->type_;
        tasks.task_end->fnc = it_init_fnc->fnc;
        tasks.task_end->waiting_count = 0;
    }
    // resolve dependencies
    for (it_task = tasks.all; it_task != tasks.task_end; ++it_task)
    {
        for (it_dependency = __async_modules_depends_start; it_dependency != __async_modules_depends_end; ++it_dependency)
        {
            if (it_dependency->task_id == it_task->id)
            {
                // Do not register a dependency that does not exist
                ptask = getTask(it_dependency->parent_id);
                if (ptask == NULL)
                {
                    printk(KERN_ERR "async Dependency id %d not found for id %d\n",it_dependency->parent_id,it_dependency->task_id);
                    printk_debug("async %s MISSIG -- %s is BROKEN\n",module_name[it_dependency->parent_id],module_name[it_dependency->task_id]);
//                    msleep(2000);
                }
                else
                {
                    // register dependency
                    ++it_task->waiting_count;
                }
            }
            if (it_dependency->parent_id == it_task->id)
            {
                ++it_task->child_count;
            }
        }
        printk_debug("async registered '%s' depends on %d tasks\n", module_name[it_task->id],it_task->waiting_count);
        //msleep(500);
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
	tasks.type_ = type;
    //
	printk_debug("async Preparing ... \n");
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
        ++it_idx1;
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
    printk_debug("async %d tasks \n",tasks.idx_end - tasks.idx_list);
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
    struct task_t* child_task;
    unsigned* it_idx;
    unsigned* it_idx2;
    unsigned idx;
     unsigned wake_count = 0;     // how many task has been wake up bool indicated a wake up is necessary
    // nothing in the list
    if (tasks.running_last == tasks.idx_list)
        return 0;
    //lock
    spin_lock(&list_lock);
    if (ptask != 0)
    {
        //tasks.task_left--;
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
        // release child tasks
        if (ptask->child_count)
        {
            // find parent task on dependency list
            for (it_dependency = __async_modules_depends_start;ptask->child_count != 0 &&  it_dependency != __async_modules_depends_end; ++it_dependency)
            {
                if (it_dependency->parent_id == ptask->id)
                {
                    child_task = getTask(it_dependency->task_id);
                    if (child_task != 0)
                    {
                    --child_task->waiting_count;
                    --ptask->child_count;
                    }
                    else
                    {
                        printk("async Failed to release task %d\n",it_dependency->task_id);
                        printk_debug("async %s not release \n",module_name[it_dependency->task_id]);
                    }
                }
            }
            //check for missing child
            if (ptask->child_count != 0)
                printk("async Failed to release all childs\n");

            // move waiting task to ready list if necessary
            for (it_idx = tasks.idx_list;it_idx != tasks.waiting_last;)
            {
                // move to running list if not waiting
                if (tasks.all[*it_idx].waiting_count == 0)
                {
                    --tasks.waiting_last;
                    idx = *tasks.waiting_last;
                    *tasks.waiting_last = *it_idx;
                    *it_idx = idx;
                    ++wake_count;
                }
                else
                    ++it_idx;
            }
        }
    }
    // check if not running task and not pending one
    if (tasks.running_last == tasks.waiting_last)
    {
        if (tasks.waiting_last != tasks.idx_list)
        {
            printk(KERN_EMERG "async Failed some tasks was not released\n");
            for (it_idx = tasks.idx_list;it_idx != tasks.waiting_last;++it_idx)
            {
                printk(KERN_EMERG "async Failed task %d waiting for %d tasks\n",tasks.all[*it_idx].id,tasks.all[*it_idx].waiting_count);
                printk_debug("async %s still waiting for %d tasks\n",module_name[tasks.all[*it_idx].id],tasks.all[*it_idx].waiting_count);
            }
        }
        tasks.waiting_last = tasks.idx_list;
    }
    // pick a new task
    ptask = 0;
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
        --tasks.ready_last;
        idx = *it_idx2;
        *it_idx2 = *tasks.ready_last;
        *tasks.ready_last = idx;
        ptask = tasks.all + *tasks.ready_last;
    }
    else
    {
        // check end of all task
        if (tasks.running_last == tasks.idx_list)
        {
            // clean up all task memory if apply
            wake_up_interruptible(&list_wait);
        }
    }
    // check for waked up tasks, more than one will trigger
    if (wake_count  < 2)
    {
        wake_count = 0;
    }
    // all tasks done
    if (tasks.running_last == tasks.idx_list)
    {
        wake_count = 1;
    }
    // spin unlock
    spin_unlock(&list_lock);
    // all task done at last stage release all data
    if (tasks.running_last == tasks.idx_list && tasks.type_ == deferred)
    {
        wake_count =1 ;
        {
        // free task structure not sure threads use it
        //free_initmem(); //do not doit until deferred
        }
    }
    if (wake_count != 0)
        wake_up_interruptible(&list_wait);
    return ptask;
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
            //msleep(2000);
            do_one_initcall(ptask->fnc);
        }
        else
        {
            printk_debug("async %d waiting ...\n", (unsigned)data);
            ret = wait_event_interruptible(list_wait, (tasks.ready_last != tasks.waiting_last || tasks.waiting_last == tasks.idx_list));
            if (ret != 0)
            {
                printk("async init wake up returned %d\n", ret);
                break;
            }
            //wait for (depends.unlocked !=0 or depends.waiting_last == 0)
        }
    } while (tasks.all != 0 && tasks.ready_last != tasks.idx_list);	// something to do
    printk_debug("async %d ends\n", (unsigned)data);
    return 0;
}

/**
 * Execute all initialization for an specific type
 * We need wait for everything done as a barrier to avoid problems
 */
int doit_type(task_type_t type)
{
    unsigned max_cpus = num_online_cpus();
    unsigned it;
    static struct task_struct *thr;
    if (max_cpus > CONFIG_ASYNCHRO_MODULE_INIT_THREADS)
    {
        max_cpus = CONFIG_ASYNCHRO_MODULE_INIT_THREADS;
    }
    // validated cpu count
    if (max_cpus == 0)
        max_cpus = 1;
    Prepare(type);
    if (tasks.idx_end == tasks.idx_list)
        return 0;
    // leave one thread free for the system on deferred mode
    if (type == deferred &&  max_cpus > 1)
    {
        --max_cpus;
    }
    printk_debug("async using %d cpus\n", max_cpus);
    for (it=0; it < max_cpus;  ++it)
    {
        //start working threads
        thr = kthread_create(WorkingThread, (void* )(it), "async thread");
        if (thr != ERR_PTR(-ENOMEM))
        {
            kthread_bind(thr, it);
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

/**
 * Second initialization USB devices, some PCI
 */
static int deferred_initialization(void)
{
    printk_debug("async started deferred\n");
    doit_type(deferred);
    return 0;
}

static ssize_t deferred_initcalls_read_proc(struct file *file, char __user *buf,size_t nbytes, loff_t *ppos)
{
   static int deferred_initcalls_done = 0;
   int len, ret;
   char tmp[3] = "1\n";

   if (*ppos >= 3)
       return 0;

   if ((! deferred_initcalls_done) && ! (*ppos)) {
       tmp[0] = '0';
       deferred_initialization();
       //do_deferred_initcalls();
       deferred_initcalls_done = 1;
   }

   len = min(nbytes, (size_t)3);
   ret = copy_to_user(buf, tmp, len);
   if (ret)
       return -EFAULT;
   *ppos += len;
   return len;
}

static const struct file_operations deferred_initcalls_fops = {
   .read           = deferred_initcalls_read_proc,
};

/**
 * First initialization of module. Disk diver and AGP  
 */
static int async_initialization(void)
{
    printk_debug("async started asynchronized\n");
    FillTasks(__async_initcall_start, __async_initcall_end);
    // register in proc filesystem
    //proc_create("deferred_initcalls", 0, NULL, &deferred_initcalls_fops);
    doit_type(asynchronized);
    wait_event_interruptible(list_wait, (tasks.running_last != tasks.idx_list));
    return 0;
}


module_init(async_initialization);
late_initcall_sync(deferred_initialization);		// Second stage, last to do before jump to high level initialization


