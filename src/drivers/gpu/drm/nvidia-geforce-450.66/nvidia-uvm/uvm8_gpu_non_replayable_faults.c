/*******************************************************************************
    Copyright (c) 2017-2019 NVIDIA Corporation

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

#include "nv_uvm_interface.h"
#include "uvm_common.h"
#include "uvm8_api.h"
#include "uvm8_gpu_non_replayable_faults.h"
#include "uvm8_gpu.h"
#include "uvm8_hal.h"
#include "uvm8_lock.h"
#include "uvm8_tools.h"
#include "uvm8_user_channel.h"
#include "uvm8_va_space_mm.h"
#include "uvm8_va_block.h"
#include "uvm8_va_range.h"
#include "uvm8_kvmalloc.h"
#include "uvm8_ats_ibm.h"
#include "uvm8_ats_faults.h"

// TODO: Bug 1881601: [uvm] Add fault handling overview for replayable and
// non-replayable faults

// There is no error handling in this function. The caller is in charge of
// calling uvm_gpu_fault_buffer_deinit_non_replayable_faults on failure.
NV_STATUS uvm_gpu_fault_buffer_init_non_replayable_faults(uvm_parent_gpu_t *parent_gpu)
{
    uvm_non_replayable_fault_buffer_info_t *non_replayable_faults = &parent_gpu->fault_buffer_info.non_replayable;

    UVM_ASSERT(parent_gpu->non_replayable_faults_supported);

    non_replayable_faults->shadow_buffer_copy = NULL;
    non_replayable_faults->fault_cache        = NULL;

    non_replayable_faults->max_faults = parent_gpu->fault_buffer_info.rm_info.nonReplayable.bufferSize /
                                        parent_gpu->fault_buffer_hal->entry_size(parent_gpu);

    non_replayable_faults->shadow_buffer_copy =
        uvm_kvmalloc_zero(parent_gpu->fault_buffer_info.rm_info.nonReplayable.bufferSize);
    if (!non_replayable_faults->shadow_buffer_copy)
        return NV_ERR_NO_MEMORY;

    non_replayable_faults->fault_cache = uvm_kvmalloc_zero(non_replayable_faults->max_faults *
                                                           sizeof(*non_replayable_faults->fault_cache));
    if (!non_replayable_faults->fault_cache)
        return NV_ERR_NO_MEMORY;

    uvm_tracker_init(&non_replayable_faults->clear_faulted_tracker);
    uvm_tracker_init(&non_replayable_faults->fault_service_tracker);

    return NV_OK;
}

void uvm_gpu_fault_buffer_deinit_non_replayable_faults(uvm_parent_gpu_t *parent_gpu)
{
    uvm_non_replayable_fault_buffer_info_t *non_replayable_faults = &parent_gpu->fault_buffer_info.non_replayable;

    if (non_replayable_faults->fault_cache) {
        UVM_ASSERT(uvm_tracker_is_empty(&non_replayable_faults->clear_faulted_tracker));
        uvm_tracker_deinit(&non_replayable_faults->clear_faulted_tracker);

        UVM_ASSERT(uvm_tracker_is_empty(&non_replayable_faults->fault_service_tracker));
        uvm_tracker_deinit(&non_replayable_faults->fault_service_tracker);
    }

    uvm_kvfree(non_replayable_faults->shadow_buffer_copy);
    uvm_kvfree(non_replayable_faults->fault_cache);
    non_replayable_faults->shadow_buffer_copy = NULL;
    non_replayable_faults->fault_cache        = NULL;
}

bool uvm_gpu_non_replayable_faults_pending(uvm_parent_gpu_t *parent_gpu)
{
    NV_STATUS status;
    NvBool has_pending_faults;

    UVM_ASSERT(parent_gpu->isr.non_replayable_faults.handling);

    status = nvUvmInterfaceHasPendingNonReplayableFaults(&parent_gpu->fault_buffer_info.rm_info,
                                                         &has_pending_faults);
    UVM_ASSERT(status == NV_OK);

    return has_pending_faults == NV_TRUE;
}

static NvU32 fetch_non_replayable_fault_buffer_entries(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    NvU32 i = 0;
    NvU32 cached_faults = 0;
    uvm_fault_buffer_entry_t *fault_cache;
    NvU32 entry_size = gpu->parent->fault_buffer_hal->entry_size(gpu->parent);
    uvm_non_replayable_fault_buffer_info_t *non_replayable_faults = &gpu->parent->fault_buffer_info.non_replayable;
    char *current_hw_entry = (char *)non_replayable_faults->shadow_buffer_copy;

    fault_cache = non_replayable_faults->fault_cache;

    UVM_ASSERT(uvm_sem_is_locked(&gpu->parent->isr.non_replayable_faults.service_lock));
    UVM_ASSERT(gpu->parent->non_replayable_faults_supported);

    status = nvUvmInterfaceGetNonReplayableFaults(&gpu->parent->fault_buffer_info.rm_info,
                                                  non_replayable_faults->shadow_buffer_copy,
                                                  &cached_faults);
    UVM_ASSERT(status == NV_OK);

    // Parse all faults
    for (i = 0; i < cached_faults; ++i) {
        uvm_fault_buffer_entry_t *fault_entry = &non_replayable_faults->fault_cache[i];

        gpu->parent->fault_buffer_hal->parse_non_replayable_entry(gpu->parent, current_hw_entry, fault_entry);

        // The GPU aligns the fault addresses to 4k, but all of our tracking is
        // done in PAGE_SIZE chunks which might be larger.
        fault_entry->fault_address = UVM_PAGE_ALIGN_DOWN(fault_entry->fault_address);

        // Make sure that all fields in the entry are properly initialized
        fault_entry->va_space = NULL;
        fault_entry->is_fatal = (fault_entry->fault_type >= UVM_FAULT_TYPE_FATAL);
        fault_entry->filtered = false;

        fault_entry->num_instances = 1;
        fault_entry->access_type_mask = uvm_fault_access_type_mask_bit(fault_entry->fault_access_type);
        INIT_LIST_HEAD(&fault_entry->merged_instances_list);
        fault_entry->non_replayable.buffer_index = i;

        if (fault_entry->is_fatal) {
            // Record the fatal fault event later as we need the va_space locked
            fault_entry->fatal_reason = UvmEventFatalReasonInvalidFaultType;
        }
        else {
            fault_entry->fatal_reason = UvmEventFatalReasonInvalid;
        }

        current_hw_entry += entry_size;
    }

    return cached_faults;
}

static NV_STATUS clear_faulted_method_on_gpu(uvm_gpu_t *gpu,
                                             uvm_user_channel_t *user_channel,
                                             const uvm_fault_buffer_entry_t *fault_entry,
                                             NvU32 batch_id,
                                             uvm_tracker_t *tracker)
{
    NV_STATUS status;
    uvm_push_t push;
    uvm_non_replayable_fault_buffer_info_t *non_replayable_faults = &gpu->parent->fault_buffer_info.non_replayable;

    UVM_ASSERT(!fault_entry->is_fatal);

    status = uvm_push_begin_acquire(gpu->channel_manager,
                                    UVM_CHANNEL_TYPE_MEMOPS,
                                    tracker,
                                    &push,
                                    "Clearing set bit for address 0x%llx",
                                    fault_entry->fault_address);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Error acquiring tracker before clearing faulted: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    gpu->parent->host_hal->clear_faulted_channel_method(&push, user_channel, fault_entry);

    uvm_tools_broadcast_replay(gpu, &push, batch_id, fault_entry->fault_source.client_type);

    uvm_push_end(&push);

    // Add this push to the GPU's clear_faulted_tracker so GPU removal can wait
    // on it.
    status = uvm_tracker_add_push_safe(&non_replayable_faults->clear_faulted_tracker, &push);

    // Add this push to the channel's clear_faulted_tracker so user channel
    // removal can wait on it instead of using the per-GPU tracker, which would
    // require a lock.
    if (status == NV_OK)
        status = uvm_tracker_add_push_safe(&user_channel->clear_faulted_tracker, &push);

    return status;
}


static NV_STATUS clear_faulted_register_on_gpu(uvm_gpu_t *gpu,
                                               uvm_user_channel_t *user_channel,
                                               const uvm_fault_buffer_entry_t *fault_entry,
                                               NvU32 batch_id,
                                               uvm_tracker_t *tracker)
{
    NV_STATUS status;

    UVM_ASSERT(!gpu->parent->has_clear_faulted_channel_method);

    // We need to wait for all pending work before writing to the channel
    // register
    status = uvm_tracker_wait(tracker);
    if (status != NV_OK)
        return status;

    gpu->parent->host_hal->clear_faulted_channel_register(user_channel, fault_entry);

    uvm_tools_broadcast_replay_sync(gpu, batch_id, fault_entry->fault_source.client_type);

    return NV_OK;
}


static NV_STATUS clear_faulted_on_gpu(uvm_gpu_t *gpu,
                                      uvm_user_channel_t *user_channel,
                                      const uvm_fault_buffer_entry_t *fault_entry,
                                      NvU32 batch_id,
                                      uvm_tracker_t *tracker)
{

    if (!gpu->parent->has_clear_faulted_channel_method)
        return clear_faulted_register_on_gpu(gpu, user_channel, fault_entry, batch_id, tracker);


    return clear_faulted_method_on_gpu(gpu, user_channel, fault_entry, batch_id, tracker);
}

static NV_STATUS service_managed_fault_in_block_locked(uvm_gpu_t *gpu,
                                                       uvm_va_block_t *va_block,
                                                       uvm_va_block_retry_t *va_block_retry,
                                                       uvm_fault_buffer_entry_t *fault_entry,
                                                       uvm_service_block_context_t *service_context)
{
    NV_STATUS status = NV_OK;
    uvm_page_index_t page_index;
    uvm_perf_thrashing_hint_t thrashing_hint;
    uvm_processor_id_t new_residency;
    bool read_duplicate;
    uvm_va_space_t *va_space;
    uvm_va_range_t *va_range = va_block->va_range;
    uvm_non_replayable_fault_buffer_info_t *non_replayable_faults = &gpu->parent->fault_buffer_info.non_replayable;

    UVM_ASSERT(!fault_entry->is_fatal);

    UVM_ASSERT(va_range);
    va_space = va_range->va_space;
    uvm_assert_rwsem_locked(&va_space->lock);
    UVM_ASSERT(va_range->type == UVM_VA_RANGE_TYPE_MANAGED);

    UVM_ASSERT(fault_entry->va_space == va_space);
    UVM_ASSERT(fault_entry->fault_address >= va_block->start);
    UVM_ASSERT(fault_entry->fault_address <= va_block->end);

    if (service_context->num_retries == 0) {
        // notify event to tools/performance heuristics. For now we use a
        // unique batch id per fault, since we clear the faulted channel for
        // each fault.
        uvm_perf_event_notify_gpu_fault(&va_space->perf_events,
                                        va_block,
                                        gpu->id,
                                        fault_entry,
                                        ++non_replayable_faults->batch_id,
                                        false);
    }

    // Check logical permissions
    status = uvm_va_range_check_logical_permissions(va_range,
                                                    gpu->id,
                                                    fault_entry->fault_access_type,
                                                    uvm_range_group_address_migratable(va_space,
                                                                                       fault_entry->fault_address));
    if (status != NV_OK) {
        fault_entry->is_fatal = true;
        fault_entry->fatal_reason = uvm_tools_status_to_fatal_fault_reason(status);
        return NV_OK;
    }

    // TODO: Bug 1880194: Revisit thrashing detection
    thrashing_hint.type = UVM_PERF_THRASHING_HINT_TYPE_NONE;

    service_context->read_duplicate_count = 0;
    service_context->thrashing_pin_count = 0;

    page_index = uvm_va_block_cpu_page_index(va_block, fault_entry->fault_address);

    // Compute new residency and update the masks
    new_residency = uvm_va_block_select_residency(va_block,
                                                  page_index,
                                                  gpu->id,
                                                  fault_entry->access_type_mask,
                                                  &thrashing_hint,
                                                  UVM_SERVICE_OPERATION_NON_REPLAYABLE_FAULTS,
                                                  &read_duplicate);

    // Initialize the minimum necessary state in the fault service context
    uvm_processor_mask_zero(&service_context->resident_processors);

    // Set new residency and update the masks
    uvm_processor_mask_set(&service_context->resident_processors, new_residency);

    // The masks need to be fully zeroed as the fault region may grow due to prefetching
    uvm_page_mask_zero(&service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency);
    uvm_page_mask_set(&service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency, page_index);

    if (read_duplicate) {
        uvm_page_mask_zero(&service_context->read_duplicate_mask);
        uvm_page_mask_set(&service_context->read_duplicate_mask, page_index);
        service_context->read_duplicate_count = 1;
    }

    service_context->access_type[page_index] = fault_entry->fault_access_type;

    service_context->region = uvm_va_block_region_for_page(page_index);

    status = uvm_va_block_service_locked(gpu->id, va_block, va_block_retry, service_context);

    ++service_context->num_retries;

    return status;
}

static NV_STATUS service_managed_fault_in_block(uvm_gpu_t *gpu,
                                                struct mm_struct *mm,
                                                uvm_va_block_t *va_block,
                                                uvm_fault_buffer_entry_t *fault_entry)
{
    NV_STATUS status, tracker_status;
    uvm_va_block_retry_t va_block_retry;
    uvm_service_block_context_t *service_context = &gpu->parent->fault_buffer_info.non_replayable.block_service_context;

    service_context->operation = UVM_SERVICE_OPERATION_NON_REPLAYABLE_FAULTS;
    service_context->num_retries = 0;
    service_context->block_context.mm = mm;

    uvm_mutex_lock(&va_block->lock);

    status = UVM_VA_BLOCK_RETRY_LOCKED(va_block, &va_block_retry,
                                       service_managed_fault_in_block_locked(gpu,
                                                                             va_block,
                                                                             &va_block_retry,
                                                                             fault_entry,
                                                                             service_context));

    tracker_status = uvm_tracker_add_tracker_safe(&gpu->parent->fault_buffer_info.non_replayable.fault_service_tracker,
                                                  &va_block->tracker);

    uvm_mutex_unlock(&va_block->lock);

    return status == NV_OK? tracker_status: status;
}

// See uvm_unregister_channel for comments on the the channel destruction
// sequence.
static void kill_channel_delayed(void *_user_channel)
{
    uvm_user_channel_t *user_channel = (uvm_user_channel_t *)_user_channel;
    uvm_va_space_t *va_space = user_channel->kill_channel.va_space;

    UVM_ASSERT(uvm_va_space_initialized(va_space) == NV_OK);

    uvm_va_space_down_read_rm(va_space);
    if (user_channel->gpu_va_space) {
        // RM handles the fault, which will do the correct fault reporting in the
        // kernel logs and will initiate channel teardown
        NV_STATUS status = nvUvmInterfaceReportNonReplayableFault(user_channel->gpu_va_space->duped_gpu_va_space,
                                                                  user_channel->kill_channel.fault_packet);
        UVM_ASSERT(status == NV_OK);
    }
    uvm_va_space_up_read_rm(va_space);

    uvm_user_channel_release(user_channel);
}

static void kill_channel_delayed_entry(void *user_channel)
{
    UVM_ENTRY_VOID(kill_channel_delayed(user_channel));
}

static void schedule_kill_channel(uvm_gpu_t *gpu,
                                  uvm_fault_buffer_entry_t *fault_entry,
                                  uvm_user_channel_t *user_channel)
{
    uvm_va_space_t *va_space = fault_entry->va_space;
    uvm_non_replayable_fault_buffer_info_t *non_replayable_faults = &gpu->parent->fault_buffer_info.non_replayable;
    void *packet = (char *)non_replayable_faults->shadow_buffer_copy +
                   (fault_entry->non_replayable.buffer_index * gpu->parent->fault_buffer_hal->entry_size(gpu->parent));

    UVM_ASSERT(gpu);
    UVM_ASSERT(va_space);
    UVM_ASSERT(user_channel);

    if (user_channel->kill_channel.scheduled)
        return;

    user_channel->kill_channel.scheduled = true;
    user_channel->kill_channel.va_space = va_space;

    // Save the packet to be handled by RM in the channel structure
    memcpy(user_channel->kill_channel.fault_packet, packet, gpu->parent->fault_buffer_hal->entry_size(gpu->parent));

    // Retain the channel here so it is not prematurely destroyed. It will be
    // released after forwarding the fault to RM in kill_channel_delayed.
    uvm_user_channel_retain(user_channel);

    // Schedule a work item to kill the channel
    nv_kthread_q_item_init(&user_channel->kill_channel.kill_channel_q_item,
                           kill_channel_delayed_entry,
                           user_channel);

    nv_kthread_q_schedule_q_item(&gpu->parent->isr.kill_channel_q,
                                 &user_channel->kill_channel.kill_channel_q_item);
}

static NV_STATUS service_non_managed_fault(uvm_gpu_va_space_t *gpu_va_space,
                                           struct mm_struct *mm,
                                           uvm_fault_buffer_entry_t *fault_entry,
                                           NV_STATUS lookup_status)
{
    uvm_gpu_t *gpu = gpu_va_space->gpu;
    uvm_non_replayable_fault_buffer_info_t *non_replayable_faults = &gpu->parent->fault_buffer_info.non_replayable;
    uvm_ats_fault_invalidate_t *ats_invalidate = &non_replayable_faults->ats_invalidate;
    NV_STATUS status = lookup_status;

    UVM_ASSERT(!fault_entry->is_fatal);

    // Avoid dropping fault events when the VA block is not found or cannot be created
    uvm_perf_event_notify_gpu_fault(&fault_entry->va_space->perf_events,
                                    NULL,
                                    gpu->id,
                                    fault_entry,
                                    ++non_replayable_faults->batch_id,
                                    false);

    if (status != NV_ERR_INVALID_ADDRESS)
        return status;

    if (uvm_can_ats_service_faults(gpu_va_space, mm)) {
        ats_invalidate->write_faults_in_batch = false;

        // The VA isn't managed. See if ATS knows about it.
        status = uvm_ats_service_fault_entry(gpu_va_space, fault_entry, ats_invalidate);

        // Invalidate ATS TLB entries if needed
        if (status == NV_OK) {
            status = uvm_ats_invalidate_tlbs(gpu_va_space,
                                             ats_invalidate,
                                             &non_replayable_faults->fault_service_tracker);
        }
    }
    else {
        UVM_ASSERT(fault_entry->fault_access_type != UVM_FAULT_ACCESS_TYPE_PREFETCH);
        fault_entry->is_fatal = true;
        fault_entry->fatal_reason = uvm_tools_status_to_fatal_fault_reason(status);

        // Do not return error due to logical errors in the application
        status = NV_OK;
    }

    return status;
}

static NV_STATUS service_fault(uvm_gpu_t *gpu, uvm_fault_buffer_entry_t *fault_entry)
{
    NV_STATUS status;
    uvm_user_channel_t *user_channel;
    uvm_va_block_t *va_block;
    uvm_va_space_t *va_space = NULL;
    struct mm_struct *mm;
    uvm_gpu_va_space_t *gpu_va_space;
    uvm_non_replayable_fault_buffer_info_t *non_replayable_faults = &gpu->parent->fault_buffer_info.non_replayable;

    status = uvm_gpu_fault_entry_to_va_space(gpu, fault_entry, &va_space);
    if (status != NV_OK) {
        // The VA space lookup will fail if we're running concurrently with
        // removal of the channel from the VA space (channel unregister, GPU VA
        // space unregister, VA space destroy, etc). The other thread will stop
        // the channel and remove the channel from the table, so the faulting
        // condition will be gone. In the case of replayable faults we need to
        // flush the buffer, but here we can just ignore the entry and proceed
        // on.
        //
        // Note that we can't have any subcontext issues here, since non-
        // replayable faults only use the address space of their channel.
        UVM_ASSERT(status == NV_ERR_INVALID_CHANNEL);
        UVM_ASSERT(!va_space);
        return NV_OK;
    }
    else {
        UVM_ASSERT(va_space);
    }

    // If an mm is registered with the VA space, we have to retain it
    // in order to lock it before locking the VA space. It is guaranteed
    // to remain valid until we release. If no mm is registered, we
    // can only service managed faults, not ATS/HMM faults.
    mm = uvm_va_space_mm_retain(va_space);
    if (mm)
        uvm_down_read_mmap_lock(mm);

    uvm_va_space_down_read(va_space);

    gpu_va_space = uvm_gpu_va_space_get_by_parent_gpu(va_space, gpu->parent);

    if (!gpu_va_space) {
        // The va_space might have gone away. See the comment above.
        status = NV_OK;
        goto exit_no_channel;
    }

    fault_entry->va_space = va_space;

    user_channel = uvm_gpu_va_space_get_user_channel(gpu_va_space, fault_entry->instance_ptr);
    if (!user_channel) {
        // The channel might have gone away. See the comment above.
        status = NV_OK;
        goto exit_no_channel;
    }

    fault_entry->fault_source.channel_id = user_channel->hw_channel_id;

    if (!fault_entry->is_fatal) {
        status = uvm_va_block_find_create(fault_entry->va_space, fault_entry->fault_address, &va_block);
        if (status == NV_OK)
            status = service_managed_fault_in_block(gpu_va_space->gpu, mm, va_block, fault_entry);
        else
            status = service_non_managed_fault(gpu_va_space, mm, fault_entry, status);

        // We are done, we clear the faulted bit on the channel, so it can be
        // re-scheduled again
        if (status == NV_OK && !fault_entry->is_fatal) {
            status = clear_faulted_on_gpu(gpu,
                                          user_channel,
                                          fault_entry,
                                          non_replayable_faults->batch_id,
                                          &non_replayable_faults->fault_service_tracker);
            uvm_tracker_clear(&non_replayable_faults->fault_service_tracker);
        }
    }

    if (fault_entry->is_fatal)
        uvm_tools_record_gpu_fatal_fault(gpu->parent->id, fault_entry->va_space, fault_entry, fault_entry->fatal_reason);

    if (status != NV_OK || fault_entry->is_fatal)
        schedule_kill_channel(gpu, fault_entry, user_channel);

exit_no_channel:
    uvm_va_space_up_read(va_space);
    if (mm) {
        uvm_up_read_mmap_lock(mm);
        uvm_va_space_mm_release(va_space);
    }

    return status;
}

void uvm_gpu_service_non_replayable_fault_buffer(uvm_gpu_t *gpu)
{
    NV_STATUS status = NV_OK;
    NvU32 cached_faults;

    // If this handler is modified to handle fewer than all of the outstanding
    // faults, then special handling will need to be added to uvm_suspend()
    // to guarantee that fault processing has completed before control is
    // returned to the RM.
    while ((cached_faults = fetch_non_replayable_fault_buffer_entries(gpu)) > 0) {
        NvU32 i;

        // Differently to replayable faults, we do not batch up and preprocess
        // non-replayable faults since getting multiple faults on the same
        // memory region is not very likely
        for (i = 0; i < cached_faults; ++i) {
            status = service_fault(gpu, &gpu->parent->fault_buffer_info.non_replayable.fault_cache[i]);
            if (status != NV_OK)
                break;
        }
    }

    if (status != NV_OK)
        UVM_DBG_PRINT("Error servicing non-replayable faults on GPU: %s\n", uvm_gpu_name(gpu));
}
