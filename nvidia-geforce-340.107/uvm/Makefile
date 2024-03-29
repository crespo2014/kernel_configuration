################################################################################
#   Copyright (c) 2013 NVidia Corporation
#
#   Permission is hereby granted, free of charge, to any person obtaining a copy
#   of this software and associated documentation files (the "Software"), to
#   deal in the Software without restriction, including without limitation the
#   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#   sell copies of the Software, and to permit persons to whom the Software is
#   furnished to do so, subject to the following conditions:
#
#       The above copyright notice and this permission notice shall be
#       included in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#
################################################################################

#
# UVM kernel module makefile
#

# The following UVM_BUILD_TYPEs are supported: debug, release, and develop.
# The first two are nearly self-explanatory, but "develop" deserves a little
# more explanation (below).
#
# Description of the UVM build types (UVM_BUILD_TYPE):
#
# debug builds: these get debug symbols, no optimization (in order to allow
# easier stepping through of functions), and debug printing.
#
# develop builds: these are "printing" builds, normally without debug symbols.
# These builds must run at close to normal speeds, so they are optimized, and
# do not have any debug symbols. However, they do have debug printing output.
# This allows a convenient way for QA (or anyone) to get a slightly deeper look
# at what the driver is doing, without the hassle of setting up kernel
# debugging.
#
# release builds: these are just the usual speed-optimized, no-debug,
# no-printing builds that you would expect most users to run.
#
UVM_BUILD_TYPE ?= release

ifeq ($(UVM_BUILD_TYPE),debug)
  EXTRA_CFLAGS += -DDEBUG -O0 -g
else
    ifeq ($(UVM_BUILD_TYPE),develop)
	  # -DDEBUG is required, in order to allow pr_devel() print statements to
	  # work:
      EXTRA_CFLAGS += -DDEBUG -O2
	else
      EXTRA_CFLAGS += -O2
    endif
endif

NV_HOST_OS := $(shell uname)
NV_HOST_ARCH := $(shell uname -m)

NV_TARGET_DEFINES ?=
NV_DEFINES=

# In addition to the UVM_BUILD_TYPE (release, debug) choice, there is also a
# UVM_PROJECT choice. Choices include UVM_NEXT and UVM_LITE, and ALL. The
# default is UVM_LITE.
#
# Unless you are working (inside NVIDIA) on the "next" version of UVM, you want
# to use the default choice here.
UVM_PROJECT ?= UVM_LITE

UVM_LITE ?= 0
UVM_NEXT ?= 0
RM_ENABLED ?= 1

ifeq ($(UVM_PROJECT),ALL)
    UVM_LITE = 1
    UVM_NEXT = 1
else
    ifeq ($(UVM_PROJECT),UVM_LITE)
        UVM_LITE = 1
    else
        ifeq ($(UVM_PROJECT),UVM_NEXT)
            UVM_NEXT = 1
        else
            $(error "Invalid UVM_PROJECT: $(UVM_PROJECT)")
        endif
    endif
endif

ifeq ($(UVM_LITE),0)
    ifeq ($(UVM_NEXT),0)
        $(error "Cannot build with both UVM_LITE and UVM_NEXT disabled")
    endif
endif

ifneq ($(UVM_LITE),0)
    NV_DEFINES += -DNVIDIA_UVM_LITE_ENABLED
endif
ifneq ($(UVM_NEXT),0)
    NV_DEFINES += -DNVIDIA_UVM_NEXT_ENABLED
endif

NV_DEFINES += -D$(NV_HOST_OS)

ifneq ($(findstring $(NV_HOST_OS),"Linux"),)
  NV_HOST_OS_FAMILY = Unix
  NV_HOST_OS_FAMILY_LOWER_CASE = unix
  NV_DEFINES+= -D__linux__
endif

ifneq ($(findstring $(NV_HOST_OS),"macosx"),)
  NV_HOST_OS_FAMILY = MacOSX
  NV_HOST_OS_FAMILY_LOWER_CASE = macosx
endif

ifeq ($(NV_HOST_OS_FAMILY),)
  $(error "Unrecognized NV_HOST_OS: $(NV_HOST_OS)")
endif

ifneq ($(findstring $(NV_HOST_OS_FAMILY),"Unix MacOSX"),)
  # For Unix and MacOS, use the host platform as the default target platform.
  NV_TARGET_OS ?= $(NV_HOST_OS)
  NV_TARGET_ARCH ?= $(NV_HOST_ARCH)
endif

#
# Determine NV_TARGET_OS_FAMILY
#
NV_TARGET_OS_FAMILY =

ifneq ($(findstring $(NV_TARGET_OS),"Linux"),)
  NV_TARGET_OS_FAMILY = Unix
  NV_TARGET_OS_FAMILY_LOWER_CASE = unix
endif

ifneq ($(findstring $(NV_TARGET_OS),"macosx"),)
  NV_TARGET_OS_FAMILY = MacOSX
  NV_TARGET_OS_FAMILY_LOWER_CASE = macosx
endif

ifeq ($(NV_TARGET_OS_FAMILY),)
  $(error "NV_TARGET_OS_FAMILY has not been set.")
endif

src ?= .
obj ?= $(src)

# In both internal builds, and installation (public) builds, the UVM open-source
# files (that's all of them, for UVM) are in a subdirectory below the RM
# open-source files.
RM_OUT_DIR ?= $(src)/..

VERSION_HEADER := $(RM_OUT_DIR)/nv_compiler.h

COMPILE_TESTS = \
	remap_page_range \
	remap_pfn_range \
	irq_handler_t \
	vm_insert_page \
	kmem_cache_create \
	outer_flush_all \
	vm_operations_struct \
	file_operations \
	task_struct \
	address_space \
	address_space_init_once \
	kbasename \
	fatal_signal_pending \
	kuid_t \
	vm_fault_has_address

MODULE_NAME:= nvidia-uvm

MODULE_GLUE_OBJS :=     nvidia_uvm_linux.o \
                        nvidia_uvm_common.o \
                        nvidia_uvm_utils.o

ifneq ($(UVM_LITE),0)
    MODULE_GLUE_OBJS += nvidia_uvm_lite.o \
                        nvidia_uvm_page_cache.o \
                        nvidia_uvm_lite_api.o \
                        nvidia_uvm_lite_counters.o \
                        nvidia_page_migration.o \
                        nvidia_page_migration_kepler.o \
                        nvidia_uvm_lite_events.o \
                        uvm_gpu_ops_tests.o
endif

NV_INCLUDES:= -I$(PWD) -I$(RM_OUT_DIR)

ifneq ($(UVM_NEXT),0)
    include $(src)/uvmnext.mk
endif

ifneq ($(RM_ENABLED),0)
    NV_DEFINES += -DNVIDIA_UVM_RM_ENABLED
endif

EXTRA_CFLAGS += $(NV_DEFINES) $(NV_TARGET_DEFINES) $(NV_INCLUDES)

ccflags-y += $(EXTRA_CFLAGS)

RM_MODULE_SYMVERS:= $(RM_OUT_DIR)/Module.symvers
UVM_MODULE_SYMVERS:= $(obj)/Module.symvers

module $(MODULE_NAME).ko: $(UVM_MODULE_SYMVERS) debug_diagnostics_printing

$(MODULE_NAME)-y := $(MODULE_GLUE_OBJS)

include $(RM_OUT_DIR)/nvidia-modules-common.mk

$(eval $(call BUILD_MODULE_RULE, $(MODULE_OBJECT),"uvm",))
module: $(MODULE_OBJECT)

$(RM_MODULE_SYMVERS):
	cd $(RM_OUT_DIR); make module SYSSRC=$(KERNEL_SOURCES) SYSOUT=$(KERNEL_OUTPUT) KBUILD_EXTMOD=$(RM_OUT_DIR)

$(UVM_MODULE_SYMVERS): $(RM_MODULE_SYMVERS)
	cp $< $@

debug_diagnostics_printing:
ifeq ($(NV_VERBOSE),1)
	@echo "NVIDIA UVM: CC=$(CC), HOST_CC=$(HOST_CC) NV_TARGET_ARCH=$(NV_TARGET_ARCH)"
	@echo "NVIDIA UVM: CONFTEST=$(CONFTEST)"
	@echo "KERNEL_SOURCES: $(KERNEL_SOURCES)"
	@echo "EXTRA_CFLAGS: $(EXTRA_CFLAGS)"
endif

.PHONY: debug_diagnostics_printing
