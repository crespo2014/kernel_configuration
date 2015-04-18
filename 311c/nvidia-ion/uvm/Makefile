
MODULE_NAME := nvidia-uvm


ISYSTEM=`$(CC) -print-file-name=include 2> /dev/null`

options := CONFIG_DEBUG_SECTION_MISMATCH=y

$(MODULE_NAME)-y := nvidia_uvm_linux.o \
                        nvidia_uvm_common.o \
                        nvidia_uvm_utils.o
                        
ifeq ($(CONFIG_NVIDIA_ION_UVM_LITE),y) 

$(MODULE_NAME)-y += nvidia_uvm_lite.o \
                        nvidia_uvm_page_cache.o \
                        nvidia_uvm_lite_api.o \
                        nvidia_uvm_lite_counters.o \
                        nvidia_page_migration.o \
                        nvidia_page_migration_kepler.o \
                        nvidia_uvm_lite_events.o \
                        uvm_gpu_ops_tests.o
endif                      

obj-$(CONFIG_NVIDIA_UVM) += $(MODULE_NAME).o  

#compiler flags
ccflags-y := -DNVRM -DNV_VERSION_STRING=\"340.65\" -Wno-unused-function -Wuninitialized -fno-strict-aliasing -DNV_UVM_ENABLE  -DNV_DEV_NAME=\"$(MODULE_NAME)\"

#os system
ccflags-y += -D__linux__
ccflags-y += -DNV_TARGET_ARCH=x86

ccflags-y += -nostdinc -isystem $(ISYSTEM)
ccflags-y += -DNV_BUILD_MODULE_INSTANCES=0 -DNV_MODULE_INSTANCE=0
ccflags-y += -I$(obj) -I$(src)/.. 

ifeq ($(CONFIG_NVIDIA_ION_UVM_ALL),y)
ccflags-y += -DNVIDIA_UVM_LITE_ENABLED -DNVIDIA_UVM_NEXT_ENABLED
else
ifeq ($(CONFIG_NVIDIA_ION_UVM_LITE),y)
ccflags-y += -DNVIDIA_UVM_LITE_ENABLED
else
ifeq ($(CONFIG_NVIDIA_ION_UVM_NEXT),y)
ccflags-y += -DNVIDIA_UVM_NEXT_ENABLED
else 
#    $(error "Cannot build with both UVM_LITE and UVM_NEXT disabled")
endif
endif
endif

ifeq ($(CONFIG_NVIDIA_UVM_RM),y)
ccflags-y += -DNVIDIA_UVM_RM_ENABLED
endif

clean-files := $(CONF_TEST) $(VERSION_HEADER) $(CONF_TEST_PATCHES) 
clean-dirs := $(obj)/conftest

		
