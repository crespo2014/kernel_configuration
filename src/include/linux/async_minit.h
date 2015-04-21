/*
 * async_minit.h
 *
 *  Created on: 18 Apr 2015
 *      Author: lester
 *
 *  Async module initialization header file
 */

#ifndef ASYNC_MINIT_H_
#define ASYNC_MINIT_H_

#include <linux/init.h>

/**
 * Task type or execution priority
 * async - needs to be execute in an asynchronized way
 * deferred - it can be execute at user initialization time
 */
typedef enum { asynchronized, deferred } task_type_t;

struct init_fn
{
  task_type_t type_;    // task type
  const char* name;
  const char* depends_on;
  initcall_t fnc;
};

/*
 * TODO Disable when module build as module
 * add support for bus devices like acpi, pci, etc
*/
//
#ifdef CONFIG_ASYNCHRO_MODULE_INIT

//
#define async_module_init(fnc, ... )  \
  static struct init_fn init_fn_##fnc __used \
  __attribute__((__section__(".async_initcall.init"))) = {asynchronized,#fnc,#__VA_ARGS__,fnc};

#define deferred_module_init(fnc, ... )  \
  static struct init_fn init_fn_##fnc __used \
  __attribute__((__section__(".async_initcall.init"))) = {deferred,#fnc,#__VA_ARGS__,fnc};

// Usefull for ACPI and USB maybe PCI
#define async_module_driver(__driver, __register, __unregister,__depends, ...) \
static int __init __driver##_init(void) \
{ \
  return __register(&(__driver) , ##__VA_ARGS__); \
} \
async_module_init(__driver##_init,__depends); \
static void __exit __driver##_exit(void) \
{ \
  __unregister(&(__driver) , ##__VA_ARGS__); \
} \
module_exit(__driver##_exit);

//
#define deferred_module_driver(__driver, __register, __unregister,__depends, ...) \
static int __init __driver##_init(void) \
{ \
  return __register(&(__driver) , ##__VA_ARGS__); \
} \
deferred_module_init(__driver##_init,__depends); \
static void __exit __driver##_exit(void) \
{ \
  __unregister(&(__driver) , ##__VA_ARGS__); \
} \
module_exit(__driver##_exit);

//
#define async_module_pci_driver(__pci_driver) \
  async_module_driver(__pci_driver, pci_register_driver, \
           pci_unregister_driver,)

#define deferred_module_pci_driver(__pci_driver) \
    deferred_module_driver(__pci_driver, pci_register_driver, \
           pci_unregister_driver,)

#else

#define async_module_init(fnc, ... )      module_init(fnc);
#define deferred_module_init(fnc, ... )   module_init(fnc);

#define async_module_driver(__driver, __register, __unregister,__depends, ...)    module_driver(__driver, __register, __unregister, ##__VA_ARGS__);
#define deferred_module_driver(__driver, __register, __unregister,__depends, ...) module_driver(__driver, __register, __unregister, ##__VA_ARGS__);

#endif


#endif /* ASYNC_MINIT_H_ */
