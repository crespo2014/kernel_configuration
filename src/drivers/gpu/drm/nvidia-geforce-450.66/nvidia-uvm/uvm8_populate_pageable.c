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

#include "uvm_common.h"
#include "uvm_ioctl.h"
#include "uvm_linux.h"
#include "uvm8_lock.h"
#include "uvm8_api.h"
#include "uvm8_va_range.h"
#include "uvm8_va_space.h"
#include "uvm8_populate_pageable.h"

NV_STATUS uvm_populate_pageable_vma(struct vm_area_struct *vma,
                                    unsigned long start,
                                    unsigned long length,
                                    int min_prot)
{
    unsigned long vma_size;
    unsigned long vma_num_pages;
    unsigned long outer = start + length;
    const bool is_writable = (vma->vm_flags) & VM_WRITE;
    const bool min_prot_ok = (vma->vm_flags & min_prot) == min_prot;
    struct mm_struct *mm = vma->vm_mm;
    bool uvm_managed_vma;
    long ret;

    UVM_ASSERT(PAGE_ALIGNED(start));
    UVM_ASSERT(PAGE_ALIGNED(outer));
    UVM_ASSERT(vma->vm_end > start);
    UVM_ASSERT(vma->vm_start < outer);
    uvm_assert_mmap_lock_locked(mm);

    if (!min_prot_ok)
        return NV_ERR_INVALID_ADDRESS;

    // Adjust to input range boundaries
    start = max(start, vma->vm_start);
    outer = min(outer, vma->vm_end);

    vma_size = outer - start;
    vma_num_pages = vma_size / PAGE_SIZE;

    // If the input vma is managed by UVM, temporarily remove the record
    // associated with the locking of mmap_lock, in order to avoid a "locked
    // twice" validation error triggered when also acquiring mmap_lock in the
    // page fault handler. The page fault is caused by get_user_pages.
    uvm_managed_vma = uvm_file_is_nvidia_uvm(vma->vm_file);
    if (uvm_managed_vma)
        uvm_record_unlock_mmap_lock_read(mm);

    ret = NV_GET_USER_PAGES(start, vma_num_pages, is_writable, 0, NULL, NULL);

    if (uvm_managed_vma)
        uvm_record_lock_mmap_lock_read(mm);

    if (ret < 0)
        return errno_to_nv_status(ret);

    // We couldn't populate all pages, return error
    if (ret < vma_num_pages)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

NV_STATUS uvm_populate_pageable(struct mm_struct *mm,
                                const unsigned long start,
                                const unsigned long length,
                                int min_prot)
{
    struct vm_area_struct *vma;
    const unsigned long outer = start + length;
    unsigned long prev_outer = outer;

    UVM_ASSERT(PAGE_ALIGNED(start));
    UVM_ASSERT(PAGE_ALIGNED(length));
    uvm_assert_mmap_lock_locked(mm);

    vma = find_vma_intersection(mm, start, outer);

    // VMAs are validated and populated one at a time, since they may have
    // different protection flags
    // Validation of VM_SPECIAL flags is delegated to get_user_pages
    for (; vma && (vma->vm_start <= prev_outer); vma = vma->vm_next) {
        NV_STATUS status = uvm_populate_pageable_vma(vma, start, outer - start, min_prot);

        if (status != NV_OK)
            return status;

        if (vma->vm_end >= outer)
            return NV_OK;

        prev_outer = vma->vm_end;
    }

    // Input range not fully covered by VMAs.
    return NV_ERR_INVALID_ADDRESS;
}

NV_STATUS uvm_api_populate_pageable(const UVM_POPULATE_PAGEABLE_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    bool allow_managed;
    bool skip_prot_check;
    int min_prot;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    if (params->flags & ~UVM_POPULATE_PAGEABLE_FLAGS_ALL)
        return NV_ERR_INVALID_ARGUMENT;

    if ((params->flags & UVM_POPULATE_PAGEABLE_FLAGS_TEST_ALL) && !uvm_enable_builtin_tests) {
        UVM_INFO_PRINT("Test flag set for UVM_POPULATE_PAGEABLE. Did you mean to insmod with uvm_enable_builtin_tests=1?\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Population of managed ranges is only allowed for test purposes. The goal
    // is to validate that it is possible to populate pageable ranges backed by
    // VMAs with the VM_MIXEDMAP or VM_DONTEXPAND special flags set. But since
    // there is no portable way to force allocation of such memory from user
    // space, and it is not safe to change the flags of an already created
    // VMA from kernel space, we take advantage of the fact that managed ranges
    // have both special flags set at creation time (see uvm_mmap)
    allow_managed = params->flags & UVM_POPULATE_PAGEABLE_FLAG_ALLOW_MANAGED;

    skip_prot_check = params->flags & UVM_POPULATE_PAGEABLE_FLAG_SKIP_PROT_CHECK;
    if (skip_prot_check)
        min_prot = 0;
    else
        min_prot = VM_READ | VM_WRITE;

    // Check size, alignment and overflow. VMA validations are performed by
    // populate_pageable
    if (uvm_api_range_invalid(params->base, params->length))
        return NV_ERR_INVALID_ADDRESS;

    // mmap_lock is needed to traverse the vmas in the input range and call
    // into get_user_pages
    uvm_down_read_mmap_lock(current->mm);

    if (allow_managed || uvm_va_space_range_empty(va_space, params->base, params->base + params->length - 1))
        status = uvm_populate_pageable(current->mm, params->base, params->length, min_prot);
    else
        status = NV_ERR_INVALID_ADDRESS;

    uvm_up_read_mmap_lock(current->mm);

    return status;
}
