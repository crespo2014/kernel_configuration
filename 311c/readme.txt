Nvidia module for 311c.

Makefile to integrate nvidia kernel module into kernel sources

uncompress nvida source files.

./NVIDIA-Linux-x86-340.76.run -x

rename Makefile in kernel directory

mv Makefile Makefile.orig

Copy kernel directory into folder containing Makefile and KBuild

Apply patch to nvidia source code

patch < nvidia.patch

Not accepted arguments
--kernel-module-only

Steps.
./NVIDIA-Linux-x86-340.76.run -x
cd NVIDIA-Linux-x86-340.76
./nvidia-installer --no-x-check --accept-license "--kernel-source-path=/mnt/data/users/lester/projects/linux-3.19.2" --kernel-output-path=/mnt/data/users/lester/projects/linux-3.19.2/build/oi520 --kernel-install-path=/tmp/nvidia --no-nvidia-modprobe --no-precompiled-interface --no-rpms --no-recursion  --no-nouveau-check  --no-cc-version-check --no-distro-scripts --no-opengl-files --kernel-module-source-dir=output/nvidia --no-check-for-alternate-installs
cd kernel
rm conftest.h
rm -r conftest/
rm gcc-version-check.c
cp nv-kernel.o nv-kernel.o.src

Next variables are used by nvidia makefile
SYSSRC	-    kernel source dir

Configuration test will defined many macros

XEN_PRESENT=0  if !CONFIG_XEN  || CONFIG_PARAVIRT

NV_SET_MEMORY_UC_PRESENT  					set_memory_uc  OK
NV_SET_MEMORY_ARRAY_UC_PRESENT   			OK
NV_SET_PAGES_UC_PRESENT  					OK
NV_OUTER_FLUSH_ALL_PRESENT					struct outer_cache_fns has flush_all  OK
NV_CHANGE_PAGE_ATTR_PRESENT					change_page_attr 	OK
NV_PCI_GET_CLASS_PRESENT					pci_get_class		OK
NV_PCI_GET_DOMAIN_BUS_AND_SLOT_PRESENT		pci_get_domain_bus_and_slot
NV_PCI_SAVE_STATE_ARGUMENT_COUNT 2			pci_save_state  from 2.6.9  otherwise 1
NV_REMAP_PFN_RANGE_PRESENT					remap_pfn_range
NV_VMAP_PRESENT								vmap
NV_VMAP_ARGUMENT_COUNT 4					or 2
NV_I2C_ADAPTER_HAS_CLIENT_REGISTER			struct i2c_adapter has client_register
NV_PM_MESSAGE_T_PRESENT						struct pm_message_t
NV_PM_MESSAGE_T_HAS_EVENT					struct pm_message_t has event
NV_PCI_CHOOSE_STATE_PRESENT					pci_choose_state
NV_VM_INSERT_PAGE_PRESENT					vm_insert_page
NV_IRQ_HANDLER_T_PRESENT					conftest_isr
NV_IRQ_HANDLER_T_ARGUMENT_COUNT 3			or 2
NV_ACPI_DEVICE_OPS_HAS_MATCH				struct acpi_device_ops has match
NV_ACPI_DEVICE_OPS_REMOVE_ARGUMENT_COUNT 2	acpi_op_remove
NV_ACPI_DEVICE_ID_HAS_DRIVER_DATA			struct acpi_device_id has driver_data
NV_ACQUIRE_CONSOLE_SEM_PRESENT				acquire_console_sem
NV_CONSOLE_LOCK_PRESENT						console_lock
NV_KMEM_CACHE_CREATE_PRESENT				kmem_cache_create
NV_KMEM_CACHE_CREATE_ARGUMENT_COUNT 6		or 5
NV_SMP_CALL_FUNCTION_PRESENT				smp_call_function
NV_SMP_CALL_FUNCTION_ARGUMENT_COUNT 4		or 3
NV_ON_EACH_CPU_PRESENT						on_each_cpu
 NV_ON_EACH_CPU_ARGUMENT_COUNT 4			or 3
 #undef HAVE_NV_ANDROID
 NV_ACPI_EVALUATE_INTEGER_PRESENT			acpi_evaluate_integer
 typedef unsigned long long nv_acpi_integer_t;	4 function argument
 NV_ACPI_WALK_NAMESPACE_PRESENT				acpi_walk_namespace
 NV_ACPI_WALK_NAMESPACE_ARGUMENT_COUNT 7	or 6
 NV_IOREMAP_CACHE_PRESENT					ioremap_cache
 NV_IOREMAP_WC_PRESENT						ioremap_wc
 NV_PROC_DIR_ENTRY_HAS_OWNER				struct proc_dir_entry has owner
 NV_INIT_WORK_PRESENT						INIT_WORK() MACRO
 NV_INIT_WORK_ARGUMENT_COUNT 3				or 2
 NV_PCI_DMA_MAPPING_ERROR_PRESENT			pci_dma_mapping_error
 NV_PCI_DMA_MAPPING_ERROR_ARGUMENT_COUNT 2	or 1
 NV_SCATTERLIST_HAS_PAGE					struct scatterlist has page_link
 NV_PCI_DOMAIN_NR_PRESENT					pci_domain_nr
 NV_FILE_OPERATIONS_HAS_IOCTL				struct file_operations has ioctl
 NV_FILE_OPERATIONS_HAS_UNLOCKED_IOCTL		struct file_operations, unlocked_ioctl
 NV_FILE_OPERATIONS_HAS_COMPAT_IOCTL		struct file_operations, compat_ioctl
 NV_SG_INIT_TABLE_PRESENT					sg_init_table
 NV_SG_TABLE_PRESENT						struct sg_table
 NV_SG_ALLOC_TABLE_PRESENT					sg_alloc_table
 NV_SG_ALLOC_TABLE_FROM_PAGES_PRESENT		sg_alloc_table_from_pages
 NV_EFI_ENABLED_PRESENT						efi_enabled
 NV_EFI_ENABLED_ARGUMENT_COUNT 1
 NV_DOM0_KERNEL_PRESENT						?? VGX_BUILD
 NV_DRM_AVAILABLE							struct drm_driver and defined(CONFIG_DRM) && defined(CONFIG_DRM_MODULE)
 NV_PROC_CREATE_DATA_PRESENT				proc_create_data
 NV_PDE_DATA_PRESENT						PDE_DATA()
 NV_GET_NUM_PHYSPAGES_PRESENT				get_num_physpages
 NV_PROC_REMOVE_PRESENT						proc_remove
 NV_VM_OPERATIONS_STRUCT_HAS_FAULT			struct vm_operations_struct, fault
 NV_TASK_STRUCT_HAS_CRED					struct task_struct, cred
 NV_ADDRESS_SPACE_HAS_RWLOCK_TREE_LOCK		struct address_space, tree_lock
 NV_ADDRESS_SPACE_INIT_ONCE_PRESENT			address_space_init_once
 NV_KBASENAME_PRESENT						kbasename
 NV_FATAL_SIGNAL_PENDING_PRESENT			fatal_signal_pending
 NV_KUID_T_PRESENT							kuid_t
 NV_PM_VT_SWITCH_REQUIRED_PRESENT			pm_vt_switch_required
 
Extra Definitions
NVCPU_X86
NV_HOST_OS
NV_HOST_ARCH
NVIDIA_UVM_LITE_ENABLED		if UVM_LITE
NVIDIA_UVM_NEXT_ENABLED


for linux
NV_HOST_OS_FAMILY = Unix
NV_HOST_OS_FAMILY_LOWER_CASE = unix
NV_DEFINES+= -D__linux__

NV_TARGET_OS_FAMILY = Unix
NV_TARGET_OS_FAMILY_LOWER_CASE = unix

Two parts, module and libraries.

Module requirements
nvidia.ko
nvidia-frontend.ko
nvidia[0-7].ko
nvidia-uvm.ko


nvidia_drv.so used by X11
libglx.so.x.y.z
libwfb.so
libGL.so.x.y.z
libnvidia-glcore.so.x.y.z
libvdpau.so.x.y.z
libcuda.so.x.y.z, libcuda.la
/usr/lib/libOpenCL.so.1.0.0, /usr/lib/libnvidia-opencl.so.x.y.z


/usr/lib/libnvidia-tls.so.x.y.z and /usr/lib/tls/libnvidia-tls.so.x.y.z
/usr/lib/libnvidia-ml.so.x.y.z
