/*******************************************************************************
    Copyright (c) 2013 NVidia Corporation

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

#include "nvidia_uvm_linux.h"

//
// nvidia_uvm_linux.c
//
// This file, along with conftest.h and nvidia_umv_linux.h, helps to insulate
// the (out-of-tree) UVM driver from changes to the upstream Linux kernel.
//

#if !defined(NV_ADDRESS_SPACE_INIT_ONCE_PRESENT)
void address_space_init_once(struct address_space *mapping)
{
    memset(mapping, 0, sizeof(*mapping));
    INIT_RADIX_TREE(&mapping->page_tree, GFP_ATOMIC);

#if defined(NV_ADDRESS_SPACE_HAS_RWLOCK_TREE_LOCK)
    //
    // The .tree_lock member variable was changed from type rwlock_t, to
    // spinlock_t, on 25 July 2008, by mainline commit
    // 19fd6231279be3c3bdd02ed99f9b0eb195978064.
    //
    rwlock_init(&mapping->tree_lock);
#else
    spin_lock_init(&mapping->tree_lock);
#endif

    spin_lock_init(&mapping->i_mmap_lock);
    INIT_LIST_HEAD(&mapping->private_list);
    spin_lock_init(&mapping->private_lock);
    INIT_RAW_PRIO_TREE_ROOT(&mapping->i_mmap);
    INIT_LIST_HEAD(&mapping->i_mmap_nonlinear);
}
#endif

