/*
 * mtest.cpp
 *
 *  Created on: 27 Apr 2015
 *      Author: lester
 */


#define CONFIG_ASYNCHRO_MODULE_INIT_THREADS 1
#define CONFIG_ASYNCHRO_MODULE_INIT_DEBUG 1
#define TEST


#include "linux/async_minit.h"

#include <stdio.h>
#include <stdint.h>
#include <ostream>
#include <iostream>

#define ATOMIC_INIT(a)      a
#define atomic_t unsigned

#define atomic_set(a,b)  *a = b
#define atomic_read(a)   *a
#define atomic_inc(a)    ++(*a)
#define atomic_dec(a)    --(*a)
#define clear_bit(b,v)   (*v) &= ~(1 << b)

unsigned test_and_set_bit(unsigned b,  volatile unsigned long * v)
{
    unsigned r = (*v) & (1 <<b) != 0;
    (*v) |= (1 << b);
    return r;
}

int atomic_xchg(atomic_t* v,int n)
{
    int a = *v;
    *v = n;
    return a;
}

#define __wake_up(...)
#define do_one_initcall(...)
#define schedule(...)
#define prepare_to_wait_for(...)
#define finish_wait(...)
#define cpu_online_mask(...) 0
#define spin_lock(...)
#define spin_unlock(...)
#define wake_up_interruptible(...) 0
#define wake_up_interruptible_all(...)  0
#define num_online_cpus(...) 1
#define kthread_create(...) NULL
#define wait_event_interruptible(...) 0
#define free_initmem(...)
#define ERR_PTR(...) NULL
//#define ENOMEM 6
//#define kthread_create_on_node(...) 0
#define kthread_bind(...)
#define wake_up_process(...)
#define printk(...) printf( __VA_ARGS__ )
#define _raw_spin_lock(...)
struct init_fn_t __async_initcall_start[1], __async_initcall_end[1];
struct dependency_t __async_modules_depends_start[]  = {
        MOD_DEPENDENCY_ITEM(snd_hrtimer_init,alsa_timer_init),
        MOD_DEPENDENCY_ITEM(alsa_mixer_oss_init,alsa_pcm_init),       //snd-mixer-oss
        MOD_DEPENDENCY_ITEM(alsa_pcm_oss_init,alsa_mixer_oss_init),   // snd-pcm-oss
        MOD_DEPENDENCY_ITEM(alsa_hwdep_init,alsa_pcm_init),
        MOD_DEPENDENCY_ITEM(alsa_seq_device_init,alsa_timer_init),
        MOD_DEPENDENCY_ITEM(alsa_seq_init,alsa_seq_device_init),
        MOD_DEPENDENCY_ITEM(alsa_seq_midi_event_init,alsa_seq_init),
        MOD_DEPENDENCY_ITEM(alsa_seq_dummy_init,alsa_seq_init),
        MOD_DEPENDENCY_ITEM(alsa_seq_oss_init,alsa_seq_midi_event_init),
        // multiple dependencie
        MOD_DEPENDENCY_ITEM(crypto_xcbc_module_init,init_cifs),
        MOD_DEPENDENCY_ITEM(crypto_xcbc_module_init,drm_fb_helper_modinit),
        MOD_DEPENDENCY_ITEM(crypto_xcbc_module_init,acpi_power_meter_init),

        MOD_DEPENDENCY_ITEM(init_cifs,usblp_driver_init),
        MOD_DEPENDENCY_ITEM(init_cifs,acpi_power_meter_init)
        };
struct dependency_t __async_modules_depends_end[0];// = __async_modules_depends_start + 14;

#define DEFINE_SPINLOCK(a) int a
#define DECLARE_WAIT_QUEUE_HEAD(a) int a

#define module_init(...)    ;
#define __initcall(...)   ;
#define late_initcall_sync(...)   ;
#define __used
#define __u64   uint64_t

#define KERN_ERR ""
#define KERN_EMERG ""
//#define EFAULT 5

# define __user
#define loff_t  unsigned
#define ssize_t unsigned
#define copy_to_user(...)   0
#define proc_create(...)   0

#define min(a,b)    (a <b) ? a :b

struct file
{

};

struct file_operations
{
    unsigned int (*read)(struct file*,char*,size_t,unsigned int*);
};

#include "../src/drivers/async.c"


#define DOIT(x) do { printf("...\n"); Prepare(x,sizeof(x)/sizeof(*x)); WorkingThread(); } while(0)

int main(void)
{
    // single
    struct init_fn_t list1[] =
    {
    { rfcomm_init_id, 0 },
    { snd_hrtimer_init_id , 0},
    { alsa_pcm_oss_init_id ,0 },
    { alsa_seq_oss_init_id, 0},
    { alsa_timer_init_id, 0 },
    { alsa_pcm_init_id, 0 },
    { alsa_mixer_oss_init_id, 0 },
    { alsa_hwdep_init_id, 0 },
    { alsa_seq_device_init_id, 0 },
    { alsa_seq_init_id, 0 },
    { alsa_seq_midi_event_init_id, 0 },
    { alsa_seq_dummy_init_id, 0 },
    { alsa_seq_oss_init_id, 0 } };
    // multiple dependnecies
    struct init_fn_t list2[] =
    {
    { crypto_xcbc_module_init_id, 0 },
    { init_cifs_id, 0 },
    { drm_fb_helper_modinit_id, 0 },
    { acpi_power_meter_init_id, 0 },
    {  usblp_driver_init_id, 0 },
    {  lz4_mod_init_id, 0 } };

    FillTasks2(list1,list1+13);
    Prepare2(deferred);
    struct task_t* t;

    do
    {
        t = PeekTask();
        if (t != NULL)
        {
            std::cout << "id" << t->id << std::endl;
            TaskDone2(t);
        }
    }
    while ( t != NULL);

    FillTasks(list1,list1+9);
    Prepare(asynchronized);
    WorkingThread(0);

//    std::cout << "idx" << tasks.idx_list << std::endl;
//    std::cout << "waiting" << tasks.waiting_last << std::endl;
//    std::cout << "ready" << tasks.ready_last << std::endl;
//    std::cout << "running" << tasks.running_last << std::endl;

//    doit_type(asynchronized);
//    doit_type(deferred);


    FillTasks(list2,list2+6);


    return 0;
}

