#define ASYNC_INITCALLS           \
   VMLINUX_SYMBOL(__async_initcall_start) = .;    \
   *(.async_initcall.init)        \
   VMLINUX_SYMBOL(__async_initcall_end) = .;

#define MODULES_DEPENDENCIES           \
   VMLINUX_SYMBOL(__async_modules_depends_start) = .;    \
   *(.async_modules_depends.init)        \
   VMLINUX_SYMBOL(__async_modules_depends_end) = .;


#define INIT_DATA_SECTION(initsetup_align)        \
  .init.data : AT(ADDR(.init.data) - LOAD_OFFSET) {   \
    INIT_DATA           \
    INIT_SETUP(initsetup_align)       \
    INIT_CALLS            \
    CON_INITCALL            \
    SECURITY_INITCALL         \
    ASYNC_INITCALLS   \
    MODULES_DEPENDENCIES    \
    INIT_RAM_FS           \
  }
