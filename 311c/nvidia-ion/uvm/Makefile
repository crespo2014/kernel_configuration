
MODULE_NAME := nvidia-uvm

#obj ?= $(CURDIR)
#src ?= $(CURDIR)

ISYSTEM=`$(CC) -print-file-name=include 2> /dev/null`

options := CONFIG_DEBUG_SECTION_MISMATCH=y

$(MODULE_NAME)-objs := nvidia_uvm_linux.o \
                        nvidia_uvm_common.o \
                        nvidia_uvm_utils.o
                        
ifeq ($(CONFIG_NVIDIA_ION_UVM_LITE),y) 

$(MODULE_NAME)-objs += nvidia_uvm_lite.o \
                        nvidia_uvm_page_cache.o \
                        nvidia_uvm_lite_api.o \
                        nvidia_uvm_lite_counters.o \
                        nvidia_page_migration.o \
                        nvidia_page_migration_kepler.o \
                        nvidia_uvm_lite_events.o \
                        uvm_gpu_ops_tests.o
endif                      

obj-$(CONFIG_NVIDIA_UVM) += $(MODULE_NAME).o                         

ccflags-y := -D__KERNEL__ -DMODULE -DNVRM -DNV_VERSION_STRING=\"340.65\" -Wno-unused-function -Wuninitialized -fno-strict-aliasing -DNV_UVM_ENABLE  -DNV_DEV_NAME=\"$(MODULE_NAME)\"
#os system
ccflags-y += -D__linux__
ccflags-y += -DNV_TARGET_ARCH=x86

ccflags-y += -nostdinc -isystem $(ISYSTEM)
ccflags-y += -DNV_BUILD_MODULE_INSTANCES=0 -DNV_MODULE_INSTANCE=0
ccflags-y += -I$(src)/.. -I$(src) 

ifeq ($(CONFIG_NVIDIA_ION_UVM_LITE),y)
    NV_DEFINES += -DNVIDIA_UVM_LITE_ENABLED
endif
ifeq ($(NVIDIA_ION_UVM_NEXT),y)
    NV_DEFINES += -DNVIDIA_UVM_NEXT_ENABLED
endif

ifdef CONFIG_DRM
ifdef CONFIG_DRM_MODULE
ccflags-y += -DNV_DRM_DRMP_H_PRESENT -DNV_DRM_AVAILABLE -DNV_DRM_DRM_GEM_H_PRESENT
endif
endif

VERSION_HEADER := $(obj)/nv_compiler.h
CONF_TEST := $(obj)/conftest.h
CONF_TEST_PATCHES := $(obj)/conftest/patches.h


$(obj)/conftest: 
	@mkdir -p $@

$(VERSION_HEADER):
	@echo \#define NV_COMPILER \"`$(CC) -v 2>&1 | tail -n 1`\" > $@
	
$(CONF_TEST_PATCHES): $(CONF_TEST)
	@echo " static struct { const char *short_description; const char *description; } __nv_patches[] = { { NULL, NULL } }; " > $@

$(obj)/nvidia_uvm_linux.o: $(CONF_TEST) $(VERSION_HEADER) $(CONF_TEST_PATCHES) 
	
# Conftest header file creation 	

$(CONF_TEST): | $(obj)/conftest
	@echo "typedef unsigned long nv_acpi_integer_t;" > $@	
	
	@echo "// $(obj) $(src) $(CONFIG_EFI) $(CONFIG_SMP)" >> $@
	
ifeq ($(CONFIG_EFI),TODO)
	@echo "#define NV_EFI_ENABLED_PRESENT" >> $@
	@echo "#define NV_EFI_ENABLED_ARGUMENT_COUNT 1" >> $@
endif
ifdef ($(CONFIG_SMP),y)
	@echo "#define NV_ON_EACH_CPU_PRESENT " >> $@
	@echo "#define NV_ON_EACH_CPU_ARGUMENT_COUNT 3" >> $@  
endif	
ifeq ($(CONFIG_DRM),y)
	@echo "#define NV_DRM_DRMP_H_PRESENT " >> $@	
	@echo "#define NV_DRM_AVAILABLE " >> $@	
	@echo "#define NV_DRM_DRM_GEM_H_PRESENT   " >> $@
endif		

	@echo "#define NV_FILE_HAS_INODE" >> $@
	@echo "#define NV_SET_MEMORY_UC_PRESENT   " >> $@
	@echo "#define NV_SET_MEMORY_ARRAY_UC_PRESENT" >> $@ 			
	@echo "#define NV_SET_PAGES_UC_PRESENT" >> $@		   
	@echo "#define NV_OUTER_FLUSH_ALL_PRESENT" >> $@				
	@echo "#define NV_CHANGE_PAGE_ATTR_PRESENT" >> $@					
	@echo "#define NV_PCI_GET_CLASS_PRESENT" >> $@					  
	@echo "#define NV_PCI_GET_DOMAIN_BUS_AND_SLOT_PRESENT" >> $@		
	@echo "#define NV_PCI_SAVE_STATE_ARGUMENT_COUNT 1" >> $@			
	@echo "#define NV_REMAP_PFN_RANGE_PRESENT" >> $@					
	@echo "#define NV_VMAP_PRESENT" >> $@								
	@echo "#define NV_VMAP_ARGUMENT_COUNT 4" >> $@
	@echo "#define NV_PM_MESSAGE_T_PRESENT" >> $@						
	@echo "#define NV_PM_MESSAGE_T_HAS_EVENT" >> $@					   
	@echo "#define NV_PCI_CHOOSE_STATE_PRESENT" >> $@					
	@echo "#define NV_VM_INSERT_PAGE_PRESENT" >> $@					   
	@echo "#define NV_IRQ_HANDLER_T_PRESENT" >> $@					   
	@echo "#define NV_IRQ_HANDLER_T_ARGUMENT_COUNT 3" >> $@
	@echo "#define NV_ACPI_DEVICE_OPS_REMOVE_ARGUMENT_COUNT 1" >> $@	
	@echo "#define NV_ACPI_DEVICE_ID_HAS_DRIVER_DATA" >> $@
	@echo "#define NV_CONSOLE_LOCK_PRESENT" >> $@				
	@echo "#define NV_KMEM_CACHE_CREATE_PRESENT" >> $@
	@echo "#define NV_KMEM_CACHE_CREATE_ARGUMENT_COUNT 5" >> $@		   
	@echo "#define NV_SMP_CALL_FUNCTION_PRESENT" >> $@ 
	@echo "#define NV_SMP_CALL_FUNCTION_ARGUMENT_COUNT 3" >> $@	   			
	@echo "#define NV_ACPI_EVALUATE_INTEGER_PRESENT" >> $@		   
	@echo "#define NV_ACPI_WALK_NAMESPACE_PRESENT" >> $@				
	@echo "#define NV_ACPI_WALK_NAMESPACE_ARGUMENT_COUNT 7" >> $@ 
	@echo "#define NV_IOREMAP_CACHE_PRESENT" >> $@					   
	@echo "#define NV_IOREMAP_WC_PRESENT" >> $@						   
	@echo "#define NV_PROC_DIR_ENTRY_HAS_OWNER" >> $@				   
	@echo "#define NV_INIT_WORK_PRESENT" >> $@					   
	@echo "#define NV_INIT_WORK_ARGUMENT_COUNT 2" >> $@				   
	@echo "#define NV_PCI_DMA_MAPPING_ERROR_PRESENT" >> $@			   
	@echo "#define NV_PCI_DMA_MAPPING_ERROR_ARGUMENT_COUNT 2" >> $@
	@echo "#define NV_PCI_DOMAIN_NR_PRESENT" >> $@
	@echo "#define NV_FILE_OPERATIONS_HAS_UNLOCKED_IOCTL" >> $@		   
	@echo "#define NV_FILE_OPERATIONS_HAS_COMPAT_IOCTL" >> $@		   
	@echo "#define NV_SG_INIT_TABLE_PRESENT" >> $@					   
	@echo "#define NV_SG_TABLE_PRESENT" >> $@						   
	@echo "#define NV_SG_ALLOC_TABLE_PRESENT" >> $@					   
	@echo "#define NV_SG_ALLOC_TABLE_FROM_PAGES_PRESENT" >> $@
	@echo "#define NV_GENERATED_AUTOCONF_H_PRESENT" >> $@				
	@echo "#define NV_LINUX_SEMAPHORE_H_PRESENT" >> $@
	@echo "#define NV_DRM_AVAILABLE" >> $@							   
	@echo "#define NV_PROC_CREATE_DATA_PRESENT" >> $@				   
	@echo "#define NV_PDE_DATA_PRESENT" >> $@						   
	@echo "#define NV_GET_NUM_PHYSPAGES_PRESENT" >> $@				   
	@echo "#define NV_PROC_REMOVE_PRESENT" >> $@						
	@echo "#define NV_VM_OPERATIONS_STRUCT_HAS_FAULT" >> $@			   
	@echo "#define NV_TASK_STRUCT_HAS_CRED" >> $@					   
	@echo "#define NV_ADDRESS_SPACE_HAS_RWLOCK_TREE_LOCK" >> $@		   
	@echo "#define NV_ADDRESS_SPACE_INIT_ONCE_PRESENT" >> $@			
	@echo "#define NV_KBASENAME_PRESENT" >> $@						   
	@echo "#define NV_FATAL_SIGNAL_PENDING_PRESENT" >> $@			   
	@echo "#define NV_KUID_T_PRESENT" >> $@			 	   
	@echo "#define NV_PM_VT_SWITCH_REQUIRED_PRESENT" >> $@	

#	@echo "#define NV_I2C_ADAPTER_HAS_CLIENT_REGISTER" >> $@			   
#	@echo "#define NV_ACPI_DEVICE_OPS_HAS_MATCH" >> $@			   
#	@echo "#define NV_ACQUIRE_CONSOLE_SEM_PRESENT" >> $@
#	@echo "#define NV_SCATTERLIST_HAS_PAGE" >> $@			   
#	@echo "#define NV_FILE_OPERATIONS_HAS_IOCTL" >> $@		
#	@echo "#define NV_DOM0_KERNEL_PRESENT" >> $@		


ldflags-y := $(obj)/$(CORE_OBJ)

modules: $(CONF_TEST)

311c:
	make -C `pwd`/../linux-3.19.2 M=`pwd` ${options}  O=`pwd`/../linux-3.19.2/build/311c KCONFIG_CONFIG=`pwd`/../linux/311c.config modules
				
default:
	make -C /usr/src/linux-headers-`uname -r` M=$$PWD ${options} modules
		
k3_19_3:
	make V=1 -C `pwd`/../linux-3.19.3 M=$(CURDIR) ${options} modules
	
oi520:
	make -C `pwd`/../linux-3.19.2 M=`pwd` ${options}  O=`pwd`/../linux-3.19.2/build/oi520 KCONFIG_CONFIG=`pwd`/../linux/zbox-oi520.config modules

clean:
	rm -f *.o
	rm -f $(MODULE_GLUE_OBJS) $(KERNEL_GLUE_OBJS)
	rm -f build-in.o nv-linux*.o *.d .*.cmd .*.flags
	rm -f $(MODULE_NAME)*.o $(MODULE_NAME)*.ko*
	rm -f $(MODULE_NAME)*.mod* $(VERSION_HEADER) *~
	rm -f $(CONF_TEST) $(CONF_TEST_PATCHES)
	rm -rf Module*.symvers .tmp_versions modules.order $(obj)/conftest
	make V=1 -C `pwd`/../linux-3.19.3 M=$(CURDIR) clean
		
