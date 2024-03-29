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
#define set_bit(b,v)     (*v) |= (1 << b)
#define atomic_dec_and_test(a)  (--(*a))

unsigned test_and_set_bit(unsigned b,  volatile unsigned long * v)
{
    unsigned r = (*v) & (1 <<b) != 0;
    set_bit(b,v);
    return r;
}

unsigned test_and_clear_bit(unsigned b,  volatile unsigned long * v)
{
    unsigned r = (*v) & (1 <<b) != 0;
    clear_bit(b,v);
    return r;
}

int atomic_xchg(atomic_t* v,int n)
{
    int a = *v;
    *v = n;
    return a;
}

int do_one_initcall(initcall_t fnc)
{
    printf("doing initcall %p\n",fnc);
}

#define __wake_up(...)
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
const struct init_fn_t_4 *__async_initcall_start;
const struct init_fn_t_4 *__async_initcall_end;
//struct dependency_t __async_modules_depends_start[]  = {
//        MOD_DEPENDENCY_ITEM(snd_hrtimer_init,alsa_timer_init),
//        MOD_DEPENDENCY_ITEM(alsa_mixer_oss_init,alsa_pcm_init),       //snd-mixer-oss
//        MOD_DEPENDENCY_ITEM(alsa_pcm_oss_init,alsa_mixer_oss_init),   // snd-pcm-oss
//        MOD_DEPENDENCY_ITEM(alsa_hwdep_init,alsa_pcm_init),
//        MOD_DEPENDENCY_ITEM(alsa_seq_device_init,alsa_timer_init),
//        MOD_DEPENDENCY_ITEM(alsa_seq_init,alsa_seq_device_init),
//        MOD_DEPENDENCY_ITEM(alsa_seq_midi_event_init,alsa_seq_init),
//        MOD_DEPENDENCY_ITEM(alsa_seq_dummy_init,alsa_seq_init),
//        MOD_DEPENDENCY_ITEM(alsa_seq_oss_init,alsa_seq_midi_event_init),
//        // multiple dependencie
//        MOD_DEPENDENCY_ITEM(crypto_xcbc_module_init,init_cifs),
//        MOD_DEPENDENCY_ITEM(crypto_xcbc_module_init,drm_fb_helper_modinit),
//        MOD_DEPENDENCY_ITEM(crypto_xcbc_module_init,acpi_power_meter_init),
//
//        MOD_DEPENDENCY_ITEM(init_cifs,usblp_driver_init),
//        MOD_DEPENDENCY_ITEM(init_cifs,acpi_power_meter_init)
//        };
//struct dependency_t __async_modules_depends_end[0];// = __async_modules_depends_start + 14;

#define DEFINE_SPINLOCK(a) int a
#define DECLARE_WAIT_QUEUE_HEAD(a) int a

#define module_init(...)    ;
#define __initcall(...)   ;
#define late_initcall_sync(...)   ;

#ifndef __used
#define __used
#endif

#define __u64   uint64_t

#define KERN_ERR ""
#define KERN_EMERG ""
//#define EFAULT 5

# define __user
#define loff_t  unsigned
#define ssize_t unsigned
#define copy_to_user   memcpy
#define proc_create(...)   0

#define min(a,b)    (a <b) ? a :b

#define __init

struct file
{
    const void* private_data;
};

struct file_operations
{
    int (*open)(struct inode *, struct file * );
    unsigned int (*read)(struct file*,char*,size_t,unsigned int*);
};

unsigned free_init_ref = 0;

#include "../src/drivers/async.c"


#define DOIT(x) do { printf("...\n"); Prepare(x,sizeof(x)/sizeof(*x)); WorkingThread(); } while(0)

void doit_all(init_fn_t_4* begin, init_fn_t_4* end)
{
//    //FillTasks2(begin, end);
//    tasks.type_ = asynchronized;
//    Prepare2();
//    struct task_t* t;
//    do
//    {
//        t = PeekTask();
//        if (t != NULL)
//        {
//            std::cout << "id " << t->id << " " << getName(t->id) << std::endl;
//            TaskDone2(t);
//        }
//    } while (t != NULL);
//    tasks.type_ = deferred;
//    Prepare2();
//    do
//    {
//        t = PeekTask();
//        if (t != NULL)
//        {
//            std::cout << "id " << t->id << " " << getName(t->id) << std::endl;
//            TaskDone2(t);
//        }
//    } while (t != NULL);
}

#define INI_FNC(id,...) {id ## _id, 0 },

int main(void)
{
  auto t = init_info[ssb_modinit_id];
  for ( auto *x = init_info ; x < init_info + sizeof(init_info)/sizeof(*init_info);++x)
  {
    printf("%s %s \n",getName((modules_e)(x - init_info)),x->type == asynchronized ? "async" : "def");
  }
    const static struct init_fn_t_4 list_full[] =
    {
        MODULES_ID(INI_FNC) {module_last}
    };

    // single
    const static struct init_fn_t_4 list1[] =
    {
    { rfcomm_init_id, (initcall_t)1 },
    { snd_hrtimer_init_id , (initcall_t)2},
    { alsa_pcm_oss_init_id , (initcall_t)3 },
    { alsa_seq_oss_init_id, (initcall_t)4},
    { alsa_timer_init_id, (initcall_t)5 },
    { alsa_pcm_init_id, (initcall_t)6 },
    { alsa_mixer_oss_init_id, (initcall_t)7 },
    { alsa_hwdep_init_id, (initcall_t)8 },
    { alsa_seq_device_init_id, (initcall_t)9 },
    { alsa_seq_init_id, (initcall_t)10 },
    { alsa_seq_midi_event_init_id, (initcall_t)11 },
    { alsa_seq_dummy_init_id, (initcall_t)12 },
    { alsa_seq_oss_init_id, (initcall_t)13 } };
    // multiple dependnecies
    const static struct init_fn_t_4 list2[] =
    {
    { crypto_xcbc_module_init_id, (initcall_t)1 },
    { init_cifs_id, (initcall_t)2 },
    { drm_fb_helper_modinit_id, (initcall_t)3 },
    { acpi_power_meter_init_id, (initcall_t)4 },
    {  usblp_driver_init_id, (initcall_t)5 },
    {  lz4_mod_init_id, (initcall_t)6 } };
    struct file f;
    char name[30];
    __async_initcall_start = list1;
    __async_initcall_end = list1 + sizeof(list1)/sizeof(*list1);
    device_open(nullptr,&f);
    size_t ret = 0;
    *name = 0;
    do
    {
        ret = device_read(&f,name,sizeof(name),nullptr);
        name[ret] = 0;
        printf(name);
    } while (ret != 0);

    return 0;
}

