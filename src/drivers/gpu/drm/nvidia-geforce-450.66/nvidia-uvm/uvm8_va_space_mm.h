/*******************************************************************************
    Copyright (c) 2018-2019 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#ifndef __UVM8_VA_SPACE_MM_H__
#define __UVM8_VA_SPACE_MM_H__

#include "uvm_linux.h"
#include "uvm8_forward_decl.h"
#include "uvm8_test_ioctl.h"
#include "nv-kref.h"

#include <linux/mmu_notifier.h>

struct uvm_va_space_mm_struct
{
    // The mm currently associated with this VA space. Most callers shouldn't
    // access this directly, but should instead use uvm_va_space_mm_retain()/
    // uvm_va_space_mm_release().
    //
    // The pointer itself is valid between uvm_va_space_mm_register() and
    // uvm_va_space_mm_unregister(), but should only be considered usable when
    // retained or current.
    struct mm_struct *mm;

#if UVM_CAN_USE_MMU_NOTIFIERS()
    struct mmu_notifier mmu_notifier;
#endif

    // Lock protecting the alive and retained_count fields.
    uvm_spinlock_t lock;

    // Whether the mm is usable. uvm_va_space_mm_register() marks the mm as
    // alive and uvm_va_space_mm_shutdown() marks it as dead.
    bool alive;

    // Refcount for uvm_va_space_mm_retain()/uvm_va_space_mm_release()
    NvU32 retained_count;

    // Wait queue for threads waiting for retainers to finish (retained_count
    // going to 0 when not alive).
    wait_queue_head_t last_retainer_wait_queue;

    // State which is only injected by test ioctls
    struct
    {
        // Whether uvm_va_space_mm_shutdown() should do a timed wait for other
        // threads to arrive.
        bool delay_shutdown;

        bool verbose;

        // Number of threads which have called uvm_va_space_mm_shutdown(). Only
        // used when delay_shutdown is true.
        atomic_t num_mm_shutdown_threads;
    } test;
};

// Whether the system can support creating an association between a VA space and
// an mm.
bool uvm_va_space_mm_enabled_system(void);

// Whether this VA space is associated with an mm. This must not be called
// before uvm_va_space_initialize().
bool uvm_va_space_mm_enabled(uvm_va_space_t *va_space);

// Registers current->mm with the va_space. A reference is taken on the mm,
// meaning that until uvm_va_space_mm_unregister() is called the mm will remain
// a valid object in memory (mm_count), but is not guaranteed to remain alive
// (mm_users).
//
// Use uvm_va_space_mm_retain() to retrieve the mm.
//
// Locking: mmap_lock and the VA space lock must both be held for write.
NV_STATUS uvm_va_space_mm_register(uvm_va_space_t *va_space);

// De-associate the mm from the va_space. This function won't return until all
// in-flight retainers have called uvm_va_space_mm_release(). Subsequent calls
// to uvm_va_space_mm_retain() will return NULL.
//
// This function may invoke uvm_va_space_mm_shutdown() so the caller must not
// hold either mmap_lock or the VA space lock. Since this API must provide the
// same guarantees as uvm_va_space_mm_shutdown(), the caller must also guarantee
// prior to calling this function that all GPUs in this VA space have stopped
// making accesses under this mm and will not be able to start again under that
// VA space.
//
// Locking: This function may take both mmap_lock and the VA space lock.
void uvm_va_space_mm_unregister(uvm_va_space_t *va_space);

// Retains the current mm registered with this VA space. If no mm is currently
// registered, NULL is returned. Otherwise, the returned mm will remain valid
// for normal use (locking mmap_lock, find_vma, get_user_pages, etc) until
// uvm_va_space_mm_release() is called.
//
// It is NOT necessary to hold the VA space lock when calling this function.
struct mm_struct *uvm_va_space_mm_retain(uvm_va_space_t *va_space);

// Counterpart to uvm_va_space_mm_retain(). After this call, the mm must not be
// used again without another call to uvm_va_space_mm_retain().
void uvm_va_space_mm_release(uvm_va_space_t *va_space);

// Handles the va_space's mm being torn down while the VA space still exists.
// This function won't return until all in-flight retainers have called
// uvm_va_space_mm_release(). Subsequent calls to uvm_va_space_mm_retain() will
// return NULL.
//
// uvm_va_space_mm_unregister() must still be called. It is guaranteed that
// uvm_va_space_mm_shutdown() will not be called after
// uvm_va_space_mm_unregister() returns, though they may execute concurrently.
// If so, uvm_va_space_mm_unregister() will not return until
// uvm_va_space_mm_shutdown() is done.
//
// After this call returns the VA space is essentially dead. GPUs cannot make
// any new memory accesses in registered GPU VA spaces, and no more GPU faults
// which are attributed to this VA space will arrive. Additionally, no more
// registration within the VA space is allowed (GPU, GPU VA space, or channel).
void uvm_va_space_mm_shutdown(uvm_va_space_t *va_space);

NV_STATUS uvm8_test_va_space_mm_retain(UVM_TEST_VA_SPACE_MM_RETAIN_PARAMS *params, struct file *filp);
NV_STATUS uvm8_test_va_space_mm_delay_shutdown(UVM_TEST_VA_SPACE_MM_DELAY_SHUTDOWN_PARAMS *params, struct file *filp);

#endif // __UVM8_VA_SPACE_MM_H__