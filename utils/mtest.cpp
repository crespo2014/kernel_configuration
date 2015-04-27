/*
 * mtest.cpp
 *
 *  Created on: 27 Apr 2015
 *      Author: lester
 */


#define CONFIG_ASYNCHRO_MODULE_INIT_THREADS 1
#define TEST


#include "linux/async_minit.h"

#include <stdio.h>
#include <stdint.h>

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
#define kthread_create(...) NULL
#define wait_event_interruptible(...) 0
#define free_initmem(...)
#define ERR_PTR(...) NULL
#define ENOMEM 6
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
#define late_initcall_sync(...)   ;
#define __used
#define __u64   uint64_t

#include "../src/drivers/async.c"


#define DOIT(x) do { printf("...\n"); Prepare(x,sizeof(x)/sizeof(*x)); WorkingThread(); } while(0)

int main(void)
{
    // single
    struct init_fn_t list1[] =
    {
    { asynchronized, rfcomm_init_id, 0 },
    { asynchronized, alsa_timer_init_id, 0 },
    { asynchronized, alsa_pcm_init_id, 0 },
    { asynchronized, alsa_mixer_oss_init_id, 0 },
    { asynchronized, alsa_hwdep_init_id, 0 },
    { asynchronized, alsa_seq_device_init_id, 0 },
    { asynchronized, alsa_seq_init_id, 0 },
    { asynchronized, alsa_seq_midi_event_init_id, 0 },
    { asynchronized, alsa_seq_dummy_init_id, 0 },
    { asynchronized, alsa_seq_oss_init_id, 0 } };
    // multiple dependnecies
    struct init_fn_t list2[] =
    {
    { asynchronized, crypto_xcbc_module_init_id, 0 },
    { asynchronized, init_cifs_id, 0 },
    { asynchronized, drm_fb_helper_modinit_id, 0 },
    { asynchronized, acpi_power_meter_init_id, 0 },
    { asynchronized, usblp_driver_init_id, 0 },
    { asynchronized, lz4_mod_init_id, 0 } };

    FillTasks(list1,list1+9);
    doit_type(asynchronized);
    doit_type(deferred);


    FillTasks(list2,list2+6);
    doit_type(asynchronized);
    //unmeet dependnency force to do all modules in order
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
//  Prepare(list1,sizeof(list1)/sizeof(*list1));
//  WorkingThread();
//  Prepare(list2,sizeof(list2)/sizeof(*list2));
//  WorkingThread();
//  Prepare(list3,sizeof(list3)/sizeof(*list));
//  WorkingThread();
    return 0;
}

