#define ASYNC_INITCALLS           \
   VMLINUX_SYMBOL(__async_initcall_start) = .;    \
   *(.async_initcall.init)        \
   VMLINUX_SYMBOL(__async_initcall_end) = .;


#define INIT_DATA_SECTION(initsetup_align)        \
  .init.data : AT(ADDR(.init.data) - LOAD_OFFSET) {   \
    INIT_DATA           \
    INIT_SETUP(initsetup_align)       \
    INIT_CALLS            \
    ASYNC_INITCALLS   \
    CON_INITCALL            \
    SECURITY_INITCALL         \
    INIT_RAM_FS           \
  }
