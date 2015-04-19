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

struct init_fn
{
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

#define async_init(fnc, ... )  \
  static struct init_fn init_fn_##fnc __used \
  __attribute__((__section__(".async_initcall.init"))) = {#fnc,#__VA_ARGS__,fnc};

#define async_module_init(fnc, ... )  \
  static struct init_fn init_fn_##fnc __used \
  __attribute__((__section__(".async_initcall.init"))) = {#fnc,#__VA_ARGS__,fnc};

//#define async_init(fnc) async_init(fnc,);

#else

#define async_init(fnc, ... ) module_init(fnc);

#endif


#endif /* ASYNC_MINIT_H_ */
