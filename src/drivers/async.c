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
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/debugfs.h>
#include <linux/mm.h>  // mmap related stuff
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/types.h>
//#include <linux/atomic.h>
#include <asm/atomic.h>
#include <linux/kthread.h>  // for threads

#ifdef CONFIG_ASYNCHRO_MODULE_INIT_DEBUG
#define printk_debug(...) printk(__VA_ARGS__)
#else
#define printk_debug(...) do {} while(0)
#endif

/**
 * Macro to create a string array
 */
#define STR_ITEM()
#define STR_ITEM(x)  ,#x
#define STR_ITEMS()
#define STR_ITEMS(x,...) ,#x STR_ITEM(##__VA_ARG__)
#define STR_ARRAY(x,...) { #x,STR_ITEMS(##__VA_ARG__)  }

/**
 * Static struct holding all data
 */
extern struct init_fn __async_initcall_start[], __async_initcall_end[];

extern initcall_t __initcall_start[];
extern initcall_t __initcall0_start[];
extern initcall_t __initcall1_start[];
extern initcall_t __initcall2_start[];
extern initcall_t __initcall3_start[];
extern initcall_t __initcall4_start[];
extern initcall_t __initcall5_start[];
extern initcall_t __initcall6_start[];
extern initcall_t __initcall7_start[];
extern initcall_t __initcall_end[];

static initcall_t *initcall_levels[] __initdata = {
	__initcall0_start,
	__initcall1_start,
	__initcall2_start,
	__initcall3_start,
	__initcall4_start,
	__initcall5_start,
	__initcall6_start,
	__initcall7_start,
	__initcall_end,
};

/**
 * at least one element has to be in the list in the position 0 that never executes
 */
struct task_data
{
	struct init_fn* ptr;			// ptr to static task
	struct init_fn* waiting_for;	// 0 ready,  1:end_idx waiting , > end_idx : lowest priority
};

/**
 * wait on the queue until unlocked != 0 then pick a task
 * locked == running; unlock all remaining task and scheduler
 *
 * main thread wait for unlocked !=0 or waiting == running
 * threads end when waiting == 0
 * last thread is when running became 0
 */
#define MAX_TASKS 200
struct depends_list
{
	unsigned waiting_last;	// last task waiting to be release
	unsigned running_last;	// the last task running
	unsigned end_idx;		// last element on the list
	unsigned waiting;		// how many task are ready to be executed
  //struct init_fn* stask;  // static task structure
	struct task_data task[MAX_TASKS];   //TODO use dinamic allocation
};

struct depends_list depends;
static DEFINE_SPINLOCK(list_lock);
static DECLARE_WAIT_QUEUE_HEAD(list_wait);

/**
 * Mark task as done and get
 * Get a task from the list for execution
 * nullptr - no more task available
 */
struct init_fn* TaskDone(struct init_fn* prev_task)
{
	unsigned i, j;
	unsigned waiting;    // how many task has been wakeup
	struct task_data task;
	if (depends.running_last == 0)
	  return 0;
	//lock
	spin_lock(&list_lock);
	waiting = depends.waiting;    // detect how many task has been wake up
	if (prev_task != 0)
	{
		for (i = depends.waiting_last; i < depends.running_last; ++i)
		{
			if (depends.task[i].ptr == prev_task)
			{
				// bring down a task to do
				--depends.running_last;
				depends.task[i] = depends.task[depends.running_last];
				break;
			}
		}
		// release all task that depends on the finished one and remember
		for (i = 0; i < depends.waiting_last; ++i)
		{
			if (depends.task[i].waiting_for == prev_task)
			{
				depends.task[i].waiting_for = 0;
				depends.waiting++;
			}
		}
	}
	// check if not running task and not pending one
	if (depends.running_last == depends.waiting_last && depends.waiting == 0)
	{
		// the unlock all task, maybe is better execute one by one task that we do not known what it depends on
		for (i = 0; i < depends.waiting_last; ++i)
		{
			depends.task[i].waiting_for = 0;
			depends.waiting++;
		}
	}
	// pick a new task
	if (depends.waiting != 0)
	{
		// find the lower id task available
		prev_task = 0;
		j = 0;
		for (i = 0; i < depends.waiting_last; ++i)
		{
			if ((depends.task[i].waiting_for == 0) && ((prev_task == 0) || (depends.task[i].ptr < prev_task)))
			{
				prev_task = depends.task[i].ptr;
				j = i;
			}
		}
		--depends.waiting;
		--depends.waiting_last;
		task = depends.task[j];
		depends.task[j] = depends.task[depends.waiting_last];
		depends.task[depends.waiting_last] = task;
	} else
	{
		// check end of all task
		if (depends.running_last == 0)
		{
			// clean up all memory
		}
		prev_task = 0;
	}
	// spin unlock
	spin_unlock(&list_lock);
	// allow other thread pick a task
	if ((depends.waiting > waiting + 1 ) || (depends.running_last == 0))
	  wake_up_interruptible(&list_wait);
	if ((prev_task == 0) && (depends.running_last == 0))
	{
	  free_initmem();
	}
	return prev_task;
}

void Prepare(struct init_fn* begin, struct init_fn* end,task_type_t type)
{
	// fill dependencies structure
	struct init_fn* it_task;
	unsigned i;
	depends.waiting = 0;
	depends.end_idx = 0;
	for (it_task = begin; it_task < end; ++it_task)
	{
		if (it_task->type_ == type)
		{
			depends.task[depends.end_idx].ptr = it_task;
			depends.task[depends.end_idx].waiting_for = 0;
			++depends.end_idx;
		}
	}
	depends.waiting_last = depends.end_idx;
	depends.running_last = depends.end_idx;
	// resolve dependencies
	for (i = 0; i < depends.end_idx; ++i)
	{
		if ((depends.task[i].ptr->depends_on != 0) && (*depends.task[i].ptr->depends_on != 0))
		{
			for (it_task = begin; it_task < end; ++it_task)
			{
				if (strcmp(it_task->name, depends.task[i].ptr->depends_on) == 0)
				{
					if (it_task->type_ != type)
					{
						printk("async cross type dependency detected %s -> %s\n",depends.task[i].ptr->name,it_task->name);
						it_task = end;
					}
					break;
				}
			}	
			depends.task[i].waiting_for = it_task;    // it will point to the end if task not found
			printk_debug("async registered '%s' --> '%s'\n", depends.task[i].ptr->name,(it_task != end) ? it_task->name : "not found");
		} else
		{
			depends.waiting++;			// avoid bug,
			printk_debug("async registered '%s'\n", depends.task[i].ptr->name);
		}
	}
}

int WorkingThread(void *data)
{
  int ret;
  //struct task_struct *kthread = *(struct task_struct**)data;
	struct init_fn* task = 0;
	printk_debug("async %d starts\n",(unsigned)data);
	do
	{
		task = TaskDone(task);
		if (task != 0)
		{
		  printk_debug("async %d %s\n",(unsigned)data, task->name);
		  do_one_initcall(task->fnc);
		} else
		{
		  printk_debug("async %d waiting ...\n",(unsigned)data);
		  ret = wait_event_interruptible(list_wait, (depends.waiting !=0 || depends.waiting_last == 0));
		  if (ret != 0)
		  {
		    printk("async init wake up returned %d\n",ret);
		    break;
		  }
			//wait for (depends.unlocked !=0 or depends.waiting_last == 0)
		}
	} while (depends.waiting_last != 0);	// something to do
	printk_debug("async %d ends\n",(unsigned)data);
	return 0;
}

/**
 * Execute all initialization for an specific type
 */
int doit_type(task_type_t type)
{
  unsigned max_threads = CONFIG_ASYNCHRO_MODULE_INIT_THREADS;
  unsigned max_cpus = num_online_cpus();
  static struct task_struct *thr;
  Prepare(__async_initcall_start, __async_initcall_end, type);
  if (max_threads == 0) WorkingThread(0);
  for (; max_threads != 0; --max_threads)
  {
    //start working threads
    thr = kthread_create(WorkingThread, (void* )(max_threads), "async thread");
    if (thr != ERR_PTR(-ENOMEM))
    {
      kthread_bind(thr, max_threads % max_cpus);
      wake_up_process(thr);
    } else
    {
      printk("Async module initialization thread failed .. fall back to normal mode");
      //WorkingThread(NULL);
    }
  }
  return 0;
}

void traceInitCalls(void)
{
    initcall_t *fn;
    int level;
    for (level = 0; level < ARRAY_SIZE(initcall_levels) - 1; level++)
    {
        for (fn = initcall_levels[level]; fn < initcall_levels[level + 1]; fn++)
        {
            printk_debug("initcall %d , %pF ", level, fn);
        }
    }
}
/**
 * First initialization of module. Disk diver and AGP  
 */
static int async_initialization(void)
{
    traceInitCalls();
    printk_debug("async started asynchronized\n");
    return doit_type(asynchronized);
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

static char* d = STR_ARRAY(a,b,c,d,e,f);

#ifdef TEST
#define DOIT(x) do { printf("...\n"); Prepare(x,sizeof(x)/sizeof(*x)); WorkingThread(); } while(0)

int main()
{
	// single
	struct s_task list1[] =
	{
		{	"a", 0},
		{	"b", "a"},
		{	"c", "b"},
		{	"d", "b"}};
	// keep order for single thread but can be all together
	struct s_task list2[] =
	{
		{	"a", 0},
		{	"b",0},
		{	"c", 0},
		{	"d", 0}};
	// order is keep because dependences are resolved on time
	struct s_task list3[] =
	{
		{	"a",0},
		{	"b","a"},
		{	"c","b"},
		{	"d",0},
		{	"e", 0}};

	struct s_task list4[] =
	{
		{	"a",0},
		{	"d","c"},
		{	"e",0},
		{	"b","a"},
		{	"c","b"}

	};

	//
	struct s_task list5[] =
	{
		{	"a", 0}};

	DOIT(list1);
	DOIT(list2);
	DOIT(list3);
	DOIT(list4);
//	Prepare(list1,sizeof(list1)/sizeof(*list1));
//	WorkingThread();
//	Prepare(list2,sizeof(list2)/sizeof(*list2));
//	WorkingThread();
//	Prepare(list3,sizeof(list3)/sizeof(*list));
//	WorkingThread();

}
#endif

