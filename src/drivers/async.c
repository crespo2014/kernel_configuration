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
//extern struct dependency_t __async_modules_depends_start[], __async_modules_depends_end[];

#ifdef CONFIG_ASYNCHRO_MODULE_INIT_DEBUG
#define printk_debug(...) printk(KERN_ERR  __VA_ARGS__)
#else
#define printk_debug(...) do {} while(0)
#endif

/**
 * Macros to build static data about modules from arguments.
 * until 10 dependencies available
 * (module_id,module_type,dependencies ... )
 */

#define DEPENDS_0()
#define DEPENDS_1(a)
#define DEPENDS_2(a,b)      MOD_DEPENDENCY_ITEM(a,b),
#define DEPENDS_3(a,b,...)  DEPENDS_2(a,b) DEPENDS_2(a,__VA_ARGS__)
#define DEPENDS_4(a,b,...)  DEPENDS_2(a,b) DEPENDS_3(a,__VA_ARGS__)
#define DEPENDS_5(a,b,...)  DEPENDS_2(a,b) DEPENDS_4(a,__VA_ARGS__)
#define DEPENDS_6(a,b,...)  DEPENDS_2(a,b) DEPENDS_5(a,__VA_ARGS__)
#define DEPENDS_7(a,b,...)  DEPENDS_2(a,b) DEPENDS_6(a,__VA_ARGS__)
#define DEPENDS_8(a,b,...)  DEPENDS_2(a,b) DEPENDS_7(a,__VA_ARGS__)
#define DEPENDS_9(a,b,...)  DEPENDS_2(a,b) DEPENDS_8(a,__VA_ARGS__)
#define DEPENDS_10(a,b,...) DEPENDS_2(a,b) DEPENDS_9(a,__VA_ARGS__)

#define GET_10(fnc,n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,...) fnc##n10
#define COUNT(fnc,...) GET_10(fnc,__VA_ARGS__,10,9,8,7,6,5,4,3,2,1)
#define CALL_FNC(fnc,...) COUNT(fnc,__VA_ARGS__)(__VA_ARGS__)

#define DEPENDS_BUILD(id,type,...) CALL_FNC(DEPENDS_,id,##__VA_ARGS__)

const char* getName(modules_e id)
{
#ifdef CONFIG_ASYNCHRO_MODULE_INIT_DEBUG
    static const char* const module_name[] =
    {   "",INIT_CALLS(TASK_NAME)};
    if (id > module_last)
        return "";
    return module_name[id];
#endif
    return "";
}


static const struct async_module_info_t module_info[] =
{   {disable}, INIT_CALLS(ASYNC_MODULE_INFO) {disable}};

/**
 * Dependencies list is declared next
 */
static const struct dependency_t module_depends[] =
{ INIT_CALLS(DEPENDS_BUILD) };


#define MAX_TASKS (unsigned)module_last+2

/**
 * all initcall will be enums. a tbl will store all names
 */
struct task_t
{
    task_type_t type;        //
    modules_e id;           // idx in string table
    initcall_t fnc;         // ptr to init function
    atomic_t waiting_count; // how many task does it depend on
    volatile unsigned long status;        // bit 0 1 free to get, bit 1 1 doing
    unsigned child_count;   // how many task it triggers
    struct task_t**  first_cild; // first child to be release
};

// TODO join together all task with taskid table to allow dynamic allocation
struct task_list_t
{
    task_type_t type_;              // current type in processing
    unsigned idx_list[MAX_TASKS];   // list of task to be execute
    unsigned *waiting_last;  // last task waiting to be release
    unsigned *ready_last;    // last task to be execute
    unsigned *running_last;  // the last task running
    unsigned *idx_end;      // number of idx on list
    struct task_t all[MAX_TASKS];       // full list of task
    struct task_t* task_end;
    struct task_t* current_tasks[MAX_TASKS];        // list of actived task
    struct task_t** task_last_done;      // first task to be done
    struct task_t** task_last;           // one pass last task to be done
    struct task_t* childs[sizeof(module_depends)/sizeof(*module_depends)];  //  list of child to be relase ordered by parent
};

static struct task_list_t tasks;

struct task_t* getTask(modules_e id)
{
    struct task_t* t = tasks.all;
    while(t != tasks.task_end && t->id != id)
        ++t;
    return (t == tasks.task_end) ? NULL : t;
}

static DEFINE_SPINLOCK(list_lock);
static DECLARE_WAIT_QUEUE_HEAD( list_wait);

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
    const struct dependency_t * it_dependency;
    for (it_dependency = module_depends; it_dependency != module_depends + sizeof(module_depends)/sizeof(*module_depends); ++it_dependency)
    {
        if (it_dependency->parent_id == parent && it_dependency->task_id == child)
            return 1;
    }
    return 0;
}
/*
 * Read all information from static mmeory an expand it to dynamic memory
 */
void FillTasks(struct init_fn_t* begin, struct init_fn_t* end)
{
    const struct dependency_t *it_dependency;
    struct init_fn_t* it_init_fnc;
    struct task_t* it_task;
    struct task_t* ptask;
    tasks.task_end = tasks.all;
    for (it_init_fnc = begin; it_init_fnc < end; ++it_init_fnc, ++tasks.task_end)
    {
        tasks.task_end->id = it_init_fnc->id;
        tasks.task_end->type = module_info[it_init_fnc->id].type_;
        tasks.task_end->fnc = it_init_fnc->fnc;
        atomic_set(&tasks.task_end->waiting_count,0);
    }
    // resolve dependencies
    for (it_task = tasks.all; it_task != tasks.task_end; ++it_task)
    {
        for (it_dependency = module_depends; it_dependency != module_depends + sizeof(module_depends)/sizeof(*module_depends); ++it_dependency)
        {
            if (it_dependency->task_id == it_task->id)
            {
                // Do not register a dependency that does not exist
                ptask = getTask(it_dependency->parent_id);
                if (ptask == NULL)
                {
                    printk(KERN_ERR "async Dependency id %d %s not found for id %d %s\n",it_dependency->parent_id,getName(it_dependency->parent_id),it_dependency->task_id,getName(it_dependency->task_id));
                }
                else
                {
                    // register dependency
                    atomic_inc(&it_task->waiting_count);
                }
            }
            if (it_dependency->parent_id == it_task->id)
            {
                ++it_task->child_count;
            }
        }
        printk_debug("async registered '%pF' depends on %d tasks\n", it_task->fnc,atomic_read(&it_task->waiting_count));
    }
}
/*
 * Prepare task to be done using task pointer list
 */
void FillTasks2(struct init_fn_t* begin, struct init_fn_t* end)
{
    const struct dependency_t *it_dependency;
    struct init_fn_t* it_init_fnc;
    struct task_t* it_task;
    struct task_t* ptask;
    struct task_t** release_end;            // pointer to list of childs
    tasks.task_end = tasks.all;
    for (it_init_fnc = begin; it_init_fnc < end; ++it_init_fnc, ++tasks.task_end)
    {
        tasks.task_end->id = it_init_fnc->id;
        tasks.task_end->type = module_info[it_init_fnc->id].type_;
        tasks.task_end->fnc = it_init_fnc->fnc;
        atomic_set(&tasks.task_end->waiting_count,0);
        tasks.task_end->first_cild = NULL;
        tasks.task_end->child_count = 0;
        tasks.task_end->status = 3;
    }
    // for every task find all child to be release and store.
    release_end = tasks.childs;
    for (it_task = tasks.all; it_task != tasks.task_end; ++it_task)
    {
        it_task->first_cild = release_end;
        for (it_dependency = module_depends; it_dependency != module_depends + sizeof(module_depends)/sizeof(*module_depends); ++it_dependency)
        {
            // get all childs
            if (it_dependency->parent_id == it_task->id)
            {
                // Do not register a dependency that does not exist
                ptask = getTask(it_dependency->task_id);
                if (ptask == NULL)
                {
                    printk(KERN_ERR "async Child id %d not found for parent id %d\n",it_dependency->task_id,it_dependency->parent_id);
                }
                else
                {
                    // register dependency
                    ++it_task->child_count;
                    *release_end = ptask;
                    ++release_end;
                    if (it_task->type == deferred && ptask->type == asynchronized)
                    {
                        printk(KERN_ERR "async Child id %d %s will not released by parent id %d %s\n",it_dependency->task_id,getName(it_dependency->task_id),it_dependency->parent_id,getName(it_dependency->parent_id));
                    }
                    if (it_task > ptask)
                    {
                        printk(KERN_ERR "async child id %d %s was registered before parent id %d %s\n",it_dependency->task_id,getName(it_dependency->task_id),it_dependency->parent_id,getName(it_dependency->parent_id));
                    }
                    atomic_inc(&ptask->waiting_count);
                }
            }
        }
        printk_debug("async registered '%pF' %s release %d tasks\n", it_task->fnc,getName(it_task->id),it_task->child_count);
    }
}
/*
 * Prepare pointers to task do not use index
 */
void Prepare2(task_type_t type)
{
    // Pick only task of type from all task
    struct task_t* it_task;

    tasks.task_last = tasks.current_tasks;
    tasks.task_last_done = tasks.current_tasks;
    tasks.type_ = type;
    //
    printk_debug("async Preparing ... \n");
    for (it_task = tasks.all; it_task != tasks.task_end; ++it_task)
    {
        if (it_task->type == type)
        {
            *tasks.task_last  = it_task;
            ++tasks.task_last;
        }
    }
}
/**
 * Mark task as done
 */
void TaskDone2(struct task_t* ptask)
{
    struct task_t** it_task;
    unsigned i;
    if (ptask != NULL)
    {
       clear_bit(1,&ptask->status);      // task done
       // release all childs
       it_task = ptask->first_cild;
       for(i = 0;i<ptask->child_count;++i,++it_task)
       {
           atomic_dec(&(*it_task)->waiting_count);
       }
       if (ptask->child_count > 1)
           wake_up_interruptible(&list_wait);
    }
    // try to update last done task
    spin_lock(&list_lock);
    while (tasks.task_last_done != tasks.task_last && (*tasks.task_last_done)->status == 0)
        ++tasks.task_last_done;
    spin_unlock(&list_lock);

    // Check for end condition
    if (tasks.task_last_done == tasks.task_last)
        wake_up_interruptible_all(&list_wait);
}

/*
 * Try to get a task ready to run
 */
struct task_t* PeekTask(void)
{
    struct task_t** it_task;
    for (it_task = tasks.task_last_done;it_task != tasks.task_last;++it_task)
    {
        if (atomic_read(&(*it_task)->waiting_count) == 0)
        {
            if (test_and_clear_bit(0,&(*it_task)->status) == 1) return *it_task;
        }
    }
    return NULL;
}

/**
 * Thread for version 2
 */

int ProcessThread2(void *data)
{
    int ret;
    struct task_t* ptask = NULL;
    printk_debug("async %d starts\n", (unsigned )data);
    do
    {
        ret = wait_event_interruptible(list_wait, (ptask = PeekTask()) != NULL || tasks.task_last_done == tasks.task_last);
        if (ret != 0)
        {
            printk("async init wake up returned %d\n", ret);
            break;
        }
        if (ptask != NULL)
        {
            printk_debug("async %d %pF %s\n", (unsigned)data, ptask->fnc,getName(ptask->id));
            do_one_initcall(ptask->fnc);
            TaskDone2(ptask);
        }
    } while (ptask != 0);
    printk_debug("async %d ends\n", (unsigned)data);
    return 0;
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
    while (it_idx1 != tasks.idx_end && atomic_read(&tasks.all[*it_idx1].waiting_count) != 0)
        ++it_idx1;
    for (it_idx2 = it_idx1 + 1; it_idx2 < tasks.idx_end; ++it_idx2)
    {
        //move waiting task to front
        if (atomic_read(&tasks.all[*it_idx2].waiting_count) != 0)
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
 * Mark task as done and get
 * Get a task from the list for execution
 * nullptr - no more task available
 */
struct task_t* TaskDone(struct task_t* ptask)
{
    const struct dependency_t * it_dependency;
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
            for (it_dependency = module_depends;ptask->child_count != 0 &&  it_dependency != module_depends + sizeof(module_depends)/sizeof(*module_depends); ++it_dependency)
            {
                if (it_dependency->parent_id == ptask->id)
                {
                    child_task = getTask(it_dependency->task_id);
                    if (child_task != 0)
                    {
                    atomic_dec(&child_task->waiting_count);
                    --ptask->child_count;
                    }
                    else
                    {
                        printk("async Failed %pF does not release task %d %s\n",ptask->fnc,it_dependency->task_id,getName(it_dependency->task_id));
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
                if (atomic_read(&tasks.all[*it_idx].waiting_count) == 0)
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
                printk(KERN_EMERG "async Failed id %d task %pF waiting for %d tasks\n",*it_idx,tasks.all[*it_idx].fnc,atomic_read(&tasks.all[*it_idx].waiting_count));
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
    // check for waked up tasks, more than one will trigger
    if (wake_count  < 2)
    {
        wake_count = 0;
    }
    // all tasks done
    if (tasks.running_last == tasks.idx_list)
    {
        printk_debug("async all done\n");
        wake_count = 1;
    }
    // spin unlock
    spin_unlock(&list_lock);
    // all task done at last stage release all data
    if (tasks.running_last == tasks.idx_list && tasks.type_ == deferred)
    {
        // free task structure not sure threads use it
        //free_initmem(); //do not doit until deferred
    }
    if (wake_count != 0)
        wake_up_interruptible_all(&list_wait);
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
            printk_debug("async %d %pF\n", (unsigned)data, ptask->fnc);
            //msleep(2000);
            do_one_initcall(ptask->fnc);
        }
        else
        {
            printk_debug("async %d waiting ...\n", (unsigned)data);
            ret = wait_event_interruptible(list_wait, (tasks.ready_last != tasks.waiting_last || tasks.ready_last == tasks.idx_list));
            if (ret != 0)
            {
                printk("async init wake up returned %d\n", ret);
                break;
            }
        }
        // loop again if a task was done or something can be done
    } while (ptask != 0 || tasks.ready_last != tasks.idx_list);
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
    // leave one thread free
    if (type == deferred &&  max_cpus > 1)
    {
        //--max_cpus;
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
 *
 *     int test_and_clear_bit(int nr, void *addr)
 *
 */
static atomic_t deferred_initcalls_done = ATOMIC_INIT(0);
static int deferred_initialization(void)
{
    int old = atomic_xchg(&deferred_initcalls_done,1);
    if (old == 0)
    {
        // wait for async completion
        wait_event_interruptible(list_wait, (tasks.running_last == tasks.idx_list));
        printk_debug("async started deferred\n");
        doit_type(deferred);
    }
    return 0;
}

static ssize_t deferred_initcalls_read_proc(struct file *file, char __user *buf,size_t nbytes, loff_t *ppos)
{
   int len, ret;
   char tmp[3] = "1\n";

   if (*ppos >= 3)
       return 0;

   // if offset != 0
   if (!(*ppos)) {
       tmp[0] = '0';
       deferred_initialization();
       // wait for completion, any process that depends on driver can wait for this to complete
       wait_event_interruptible(list_wait, (tasks.running_last == tasks.idx_list));
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
    proc_create("deferred_initcalls", 0, NULL, &deferred_initcalls_fops);
    doit_type(asynchronized);
    return 0;
}


__initcall(async_initialization);
//late_initcall_sync(deferred_initialization);		// Second stage, last to do before jump to high level initialization


