/*******************************************************************************
    Copyright (c) 2015 NVIDIA Corporation

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

#include "uvm_linux.h"
#include "uvm8_hal_types.h"
#include "uvm8_hal.h"
#include "uvm8_push.h"
#include "clc06f.h"
#include "clc076.h"

void uvm_hal_pascal_host_membar_sys(uvm_push_t *push)
{
    NV_PUSH_4U(C06F, MEM_OP_A, 0,
                     MEM_OP_B, 0,
                     MEM_OP_C, HWCONST(C06F, MEM_OP_C, MEMBAR_TYPE, SYS_MEMBAR),
                     MEM_OP_D, HWCONST(C06F, MEM_OP_D, OPERATION, MEMBAR));
}

void uvm_hal_pascal_host_membar_gpu(uvm_push_t *push)
{
    NV_PUSH_4U(C06F, MEM_OP_A, 0,
                     MEM_OP_B, 0,
                     MEM_OP_C, HWCONST(C06F, MEM_OP_C, MEMBAR_TYPE, MEMBAR),
                     MEM_OP_D, HWCONST(C06F, MEM_OP_D, OPERATION, MEMBAR));
}

void uvm_hal_pascal_host_tlb_invalidate_all(uvm_push_t *push, uvm_gpu_phys_address_t pdb, NvU32 depth, uvm_membar_t membar)
{
    NvU32 aperture_value;
    NvU32 page_table_level;
    NvU32 pdb_lo;
    NvU32 pdb_hi;
    NvU32 ack_value = 0;
    NvU32 pdb_type_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE);
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS, "aperture: %u", pdb.aperture);

    if (pdb.aperture == UVM_APERTURE_VID)
        aperture_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);


    // TODO: Bug 2053812: This is a temporary WAR that invalidates all TLB
    // entries always. Remove when the proper RM WAR is in place.
    if (gpu->architecture == NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_TU100 && uvm_war_turing_flush_all_tlb)
        pdb_type_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB, ALL);


    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    // PDE3 is the highest level on Pascal, see the comment in uvm8_pascal_mmu.c for details.
    UVM_ASSERT_MSG(depth < NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3, "depth %u", depth);
    page_table_level = NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3 - depth;

    if (membar != UVM_MEMBAR_NONE) {
        // If a GPU or SYS membar is needed, ACK_TYPE needs to be set to
        // GLOBALLY to make sure all the pending accesses can be picked up by
        // the membar.
        ack_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);
    }

    NV_PUSH_4U(C06F, MEM_OP_A, HWCONST(C06F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS),
                     MEM_OP_B, 0,
                     MEM_OP_C, pdb_type_value |
                               HWVALUE(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                               HWVALUE(C06F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                               aperture_value |
                               ack_value,
                     MEM_OP_D, HWCONST(C06F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE) |
                               HWVALUE(C06F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));

    uvm_hal_tlb_invalidate_membar(push, membar);
}

void uvm_hal_pascal_host_tlb_invalidate_va(uvm_push_t *push, uvm_gpu_phys_address_t pdb, NvU32 depth, NvU64 base, NvU64 size, NvU32 page_size, uvm_membar_t membar)
{
    NvU32 aperture_value;
    NvU32 page_table_level;
    NvU32 pdb_lo;
    NvU32 pdb_hi;
    NvU32 ack_value = 0;
    NvU64 va;
    NvU32 va_lo;
    NvU32 va_hi;

    UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS, "aperture: %u", pdb.aperture);

    if (pdb.aperture == UVM_APERTURE_VID)
        aperture_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    // PDE3 is the highest level on Pascal, see the comment in uvm8_pascal_mmu.c for details.
    UVM_ASSERT_MSG(depth < NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3, "depth %u", depth);
    page_table_level = NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3 - depth;

    if (membar != UVM_MEMBAR_NONE) {
        // If a GPU or SYS membar is needed, ACK_TYPE needs to be set to
        // GLOBALLY to make sure all the pending accesses can be picked up by
        // the membar.
        ack_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);
    }

    UVM_ASSERT_MSG(IS_ALIGNED(page_size, 1 << 12), "page_size 0x%x\n", page_size);
    UVM_ASSERT_MSG(IS_ALIGNED(base, page_size), "base 0x%llx page_size 0x%x\n", base, page_size);
    UVM_ASSERT_MSG(IS_ALIGNED(size, page_size), "size 0x%llx page_size 0x%x\n", size, page_size);
    UVM_ASSERT_MSG(size > 0, "size 0x%llx\n", size);

    base >>= 12;
    size >>= 12;
    page_size >>= 12;

    for (va = base; va < base + size; va += page_size) {
        va_lo = va & HWMASK(C06F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
        va_hi = va >> HWSIZE(C06F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
        NV_PUSH_4U(C06F, MEM_OP_A, HWCONST(C06F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS) |
                                   HWVALUE(C06F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                         MEM_OP_B, HWVALUE(C06F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                         MEM_OP_C, HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                                   HWVALUE(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                                   HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                                   HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                                   HWVALUE(C06F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                                   aperture_value |
                                   ack_value,
                         MEM_OP_D, HWCONST(C06F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                                   HWVALUE(C06F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
    }

    uvm_hal_tlb_invalidate_membar(push, membar);
}

void uvm_hal_pascal_host_tlb_invalidate_test(uvm_push_t *push, uvm_gpu_phys_address_t pdb,
                                             UVM_TEST_INVALIDATE_TLB_PARAMS *params)
{
    NvU32 ack_value = 0;
    NvU32 invalidate_gpc_value = 0;
    NvU32 pdb_mode_value;
    NvU32 aperture_value = 0;
    NvU32 pdb_lo = 0;
    NvU32 pdb_hi = 0;
    NvU32 page_table_level = 0;
    uvm_membar_t membar;

    if (params->target_pdb_mode == UvmTargetPdbModeOne) {
        UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
        UVM_ASSERT_MSG(pdb.aperture == UVM_APERTURE_VID || pdb.aperture == UVM_APERTURE_SYS,
                       "aperture: %u", pdb.aperture);

        if (pdb.aperture == UVM_APERTURE_VID)
            aperture_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
        else
            aperture_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

        pdb.address >>= 12;

        pdb_lo = pdb.address & HWMASK(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
        pdb_hi = pdb.address >> HWSIZE(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

        pdb_mode_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE);
    }
    else {
        pdb_mode_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB, ALL);
    }

    if (params->page_table_level != UvmInvalidatePageTableLevelAll) {
        page_table_level = params->page_table_level - 1;
        // PDE3 is the highest level on Pascal, see the comment in uvm8_pascal_mmu.c for details.
        UVM_ASSERT_MSG(page_table_level <= NVC06F_MEM_OP_C_TLB_INVALIDATE_PAGE_TABLE_LEVEL_UP_TO_PDE3,
                       "level %u", page_table_level);
    }

    if (params->membar != UvmInvalidateTlbMemBarNone) {
        // If a GPU or SYS membar is needed, ack_value needs to be set to
        // GLOBALLY to make sure all the pending accesses can be picked up by
        // the membar.
        ack_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_ACK_TYPE, GLOBALLY);
    }

    if (params->disable_gpc_invalidate)
        invalidate_gpc_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_GPC, DISABLE);
    else
        invalidate_gpc_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE);

    if (params->target_va_mode == UvmTargetVaModeTargeted) {
        NvU64 va = params->va >> 12;

        NvU32 va_lo = va & HWMASK(C06F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
        NvU32 va_hi = va >> HWSIZE(C06F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO);
        NV_PUSH_4U(C06F, MEM_OP_A, HWCONST(C06F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS) |
                                   HWVALUE(C06F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                         MEM_OP_B, HWVALUE(C06F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                         MEM_OP_C, HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                                   HWVALUE(C06F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                                   pdb_mode_value |
                                   HWVALUE(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                                   invalidate_gpc_value |
                                   aperture_value |
                                   ack_value,
                         MEM_OP_D, HWCONST(C06F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                                   HWVALUE(C06F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
    }
    else {
        NV_PUSH_4U(C06F, MEM_OP_A, HWCONST(C06F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS),
                         MEM_OP_B, 0,
                         MEM_OP_C, HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_REPLAY, NONE) |
                                   HWVALUE(C06F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, page_table_level) |
                                   pdb_mode_value |
                                   HWVALUE(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                                   invalidate_gpc_value |
                                   aperture_value |
                                   ack_value,
                         MEM_OP_D, HWCONST(C06F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE) |
                                   HWVALUE(C06F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
    }

    if (params->membar == UvmInvalidateTlbMemBarSys)
        membar = UVM_MEMBAR_SYS;
    else if (params->membar == UvmInvalidateTlbMemBarLocal)
        membar = UVM_MEMBAR_GPU;
    else
        membar = UVM_MEMBAR_NONE;

    uvm_hal_tlb_invalidate_membar(push, membar);
}

void uvm_hal_pascal_replay_faults(uvm_push_t *push, uvm_fault_replay_type_t type)
{
    NvU32 aperture_value;
    NvU32 replay_value = 0;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    uvm_gpu_phys_address_t pdb;
    NvU32 va_lo = 0;
    NvU32 va_hi = 0;
    NvU32 pdb_lo;
    NvU32 pdb_hi;

    // MMU will not forward the replay to the uTLBs if the PDB is not in the MMU PDB_ID cache. If
    // we have stale entries filling the fault buffer and a context switch has happened, the new
    // context may have faulted without writing its entries into the buffer. To force a replay
    // regardless of which faults happen to be in the uTLB replay lists, we use the PDB of the
    // channel used to push the replay, which is guaranteed to be in the cache as it is required
    // to be resident for the channel to push the method. In order to minimize the performance hit
    // of the invalidation, we just invalidate PTEs for address 0x0.
    UVM_ASSERT_MSG(type == UVM_FAULT_REPLAY_TYPE_START || type == UVM_FAULT_REPLAY_TYPE_START_ACK_ALL,
                   "replay_type: %u\n", type);
    pdb = uvm_page_tree_pdb(&gpu->address_space_tree)->addr;

    if (pdb.aperture == UVM_APERTURE_VID)
        aperture_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, VID_MEM);
    else
        aperture_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_APERTURE, SYS_MEM_COHERENT);

    UVM_ASSERT_MSG(IS_ALIGNED(pdb.address, 1 << 12), "pdb 0x%llx\n", pdb.address);
    pdb.address >>= 12;

    pdb_lo = pdb.address & HWMASK(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);
    pdb_hi = pdb.address >> HWSIZE(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO);

    if (type == UVM_FAULT_REPLAY_TYPE_START)
        replay_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_REPLAY, START);
    else if (type == UVM_FAULT_REPLAY_TYPE_START_ACK_ALL)
        replay_value = HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_REPLAY, START_ACK_ALL);

    NV_PUSH_4U(C06F, MEM_OP_A, HWCONST(C06F, MEM_OP_A, TLB_INVALIDATE_SYSMEMBAR, DIS) |
                               HWVALUE(C06F, MEM_OP_A, TLB_INVALIDATE_TARGET_ADDR_LO, va_lo),
                     MEM_OP_B, HWVALUE(C06F, MEM_OP_B, TLB_INVALIDATE_TARGET_ADDR_HI, va_hi),
                     MEM_OP_C, HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PDB, ONE) |
                               HWVALUE(C06F, MEM_OP_C, TLB_INVALIDATE_PDB_ADDR_LO, pdb_lo) |
                               HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_GPC, ENABLE) |
                               HWCONST(C06F, MEM_OP_C, TLB_INVALIDATE_PAGE_TABLE_LEVEL, PTE_ONLY) |
                               aperture_value |
                               replay_value,
                     MEM_OP_D, HWCONST(C06F, MEM_OP_D, OPERATION, MMU_TLB_INVALIDATE_TARGETED) |
                               HWVALUE(C06F, MEM_OP_D, TLB_INVALIDATE_PDB_ADDR_HI, pdb_hi));
}

static NvU32 instance_ptr_aperture_type_to_hw_value(uvm_aperture_t aperture)
{
    switch (aperture)
    {
        case UVM_APERTURE_SYS:
            return NVC076_FAULT_CANCEL_A_INST_APERTURE_SYS_MEM_COHERENT;
        case UVM_APERTURE_VID:
            return NVC076_FAULT_CANCEL_A_INST_APERTURE_VID_MEM;
        default:
            UVM_ASSERT_MSG(false, "Invalid aperture_type %d\n", aperture);
    }

    return 0;
}

void uvm_hal_pascal_host_init(uvm_push_t *push)
{
    NV_PUSH_1U(C076, SET_OBJECT, GP100_UVM_SW);
}

void uvm_hal_pascal_cancel_faults_targeted(uvm_push_t *push, uvm_gpu_phys_address_t instance_ptr, NvU32 gpc_id,
                                           NvU32 client_id)
{
    NvU32 instance_ptr_lo;
    NvU32 instance_ptr_hi;

    // instance_ptr must be 4K aligned
    UVM_ASSERT_MSG(IS_ALIGNED(instance_ptr.address, 1 << 12), "instance_ptr 0x%llx\n", instance_ptr.address);
    instance_ptr.address >>= 12;

    instance_ptr_lo = instance_ptr.address & HWMASK(C076, FAULT_CANCEL_A, INST_LOW);
    instance_ptr_hi = instance_ptr.address >> HWSIZE(C076, FAULT_CANCEL_A, INST_LOW);

    NV_PUSH_3U(C076, FAULT_CANCEL_A, HWVALUE(C076, FAULT_CANCEL_A, INST_LOW, instance_ptr_lo) |
                                     HWVALUE(C076, FAULT_CANCEL_A, INST_APERTURE,
                                             instance_ptr_aperture_type_to_hw_value(instance_ptr.aperture)),
                     FAULT_CANCEL_B, HWVALUE(C076, FAULT_CANCEL_B, INST_HI, instance_ptr_hi),
                     FAULT_CANCEL_C, HWVALUE(C076, FAULT_CANCEL_C, CLIENT_ID, client_id) |
                                     HWVALUE(C076, FAULT_CANCEL_C, GPC_ID, gpc_id));
}
