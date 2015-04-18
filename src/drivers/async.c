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

#include <stdio.h>

/**
 * Static struct holding all data
 */
struct s_task
{
	const char* name;
	const char* depends_on;
};

/**
 * at least one element has to be in the list in the position 0 that never executes
 */
struct task_data
{
	struct s_task* ptr;			// ptr to static task
	struct s_task* waiting_for;	// 0 ready,  1:end_idx waiting , > end_idx : lowest priority
};

/**
 * wait on the queue until unlocked != 0 then pick a task
 * locked == running; unlock all remaining task and scheduler
 *
 * main thread wait for unlocked !=0 or waiting == running
 * threads end when waiting == 0
 * last thread is when running became 0
 */
#define MAX_TASKS 100
struct depends_list
{
	unsigned waiting_last;	// last task waiting to be release
	unsigned running_last;	// the last task running
	unsigned end_idx;		// last element on the list
	unsigned waiting;		// how many task are ready to be executed
	struct task_data task[MAX_TASKS];
	struct s_task* stask;	// static task structure
};

struct depends_list depends;

/**
 * Mark task as done and get
 * Get a task from the list for execution
 * nullptr - no more task available
 */
struct s_task* TaskDone(struct s_task* prev_task)
{
	unsigned i, j;
	struct task_data task;
	// spin lock
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
	if (depends.waiting)
	{
		// find the lower id task available
		prev_task = depends.stask + depends.end_idx;
		j = 0;
		for (i = 0; i < depends.waiting_last; ++i)
		{
			if (depends.task[i].waiting_for == 0 && depends.task[i].ptr < prev_task)
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
			// clean up all
		}
		prev_task = 0;
	}
	// spin unlock
	return prev_task;
}

void Prepare(struct s_task* alltask, unsigned count)
{
	// fill dependencies structure
	unsigned i, j;
	depends.end_idx = count;
	depends.waiting_last = count;
	depends.running_last = count;
	depends.waiting = 0;
	depends.stask = alltask;
	for (i = 0; i < count; ++i)
	{
		depends.task[i].ptr = alltask + i;
		depends.task[i].waiting_for = 0;
	}
	// resolve dependencies
	for (i = 0; i < count; ++i)
	{
		if (depends.task[i].ptr->depends_on != 0)
		{
			for (j = 0; j < count; ++j)
			{
				if (strcmp(alltask[j].name, depends.task[i].ptr->depends_on) == 0)
				{
					depends.task[i].waiting_for = alltask + j;
					break;
				}
			}
			// dependency not found, move task to the end
			if (depends.task[i].waiting_for == 0)
				depends.task[i].waiting_for = alltask + count;
		} else
			depends.waiting++;			// avoid bug,
	}
}

void WorkingThread()
{
	struct s_task* task = 0;
	do
	{
		task = TaskDone(task);
		if (task != 0)
		{
			// task doit
			printf("%s done\n", task->name);
		} else
		{
			//wait for (depends.unlocked !=0 or depends.waiting_last == 0)
		}
	} while (depends.waiting_last != 0);	// something to do
}

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

