/*
 * Copyright (c) 2016, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __NVIDIA_DRM_CRTC_H__
#define __NVIDIA_DRM_CRTC_H__

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

#include "nvidia-drm-helper.h"

#if defined(NV_DRM_DRMP_H_PRESENT)
#include <drm/drmP.h>
#endif

#include <drm/drm_crtc.h>

#include "nvtypes.h"
#include "nvkms-kapi.h"

struct nv_drm_crtc {
    NvU32 head;

    /**
     * @flip_list:
     *
     * List of flips pending to get proccessed by __nv_drm_handle_flip_event().
     * Protected by @flip_list_lock.
     */
    struct list_head flip_list;

    /**
     * @flip_list_lock:
     *
     * Spinlock to protect @flip_list.
     */
    spinlock_t flip_list_lock;

    struct drm_crtc base;
};

/**
 * struct nv_drm_flip - flip state
 *
 * This state is getting used to consume DRM completion event associated
 * with each crtc state from atomic commit.
 *
 * Function nv_drm_atomic_apply_modeset_config() consumes DRM completion
 * event, save it into flip state associated with crtc and queue flip state into
 * crtc's flip list and commits atomic update to hardware.
 */
struct nv_drm_flip {
    /**
     * @event:
     *
     * Optional pointer to a DRM event to signal upon completion of
     * the state update.
     */
    struct drm_pending_vblank_event *event;

    /**
     * @list_entry:
     *
     * Entry on the per-CRTC &nv_drm_crtc.flip_list. Protected by
     * &nv_drm_crtc.flip_list_lock.
     */
    struct list_head list_entry;

    /**
     * @deferred_flip_list
     *
     * List flip objects whose processing is deferred until processing of
     * this flip object. Protected by &nv_drm_crtc.flip_list_lock.
     * nv_drm_atomic_commit() gets last flip object from
     * nv_drm_crtc:flip_list and add deferred flip objects into
     * @deferred_flip_list, __nv_drm_handle_flip_event() processes
     * @deferred_flip_list.
     */
    struct list_head deferred_flip_list;
};

struct nv_drm_crtc_state {
    /**
     * @base:
     *
     * Base DRM crtc state object for this.
     */
    struct drm_crtc_state base;

    /**
     * @head_req_config:
     *
     * Requested head's modeset configuration corresponding to this crtc state.
     */
    struct NvKmsKapiHeadRequestedConfig req_config;

    /**
     * @nv_flip:
     *
     * Flip state associated with this crtc state, gets allocated
     * by nv_drm_atomic_crtc_duplicate_state(), on successful commit it gets
     * consumed and queued into flip list by
     * nv_drm_atomic_apply_modeset_config() and finally gets destroyed
     * by __nv_drm_handle_flip_event() after getting processed.
     *
     * In case of failure of atomic commit, this flip state getting destroyed by
     * nv_drm_atomic_crtc_destroy_state().
     */
    struct nv_drm_flip *nv_flip;
};

static inline struct nv_drm_crtc_state *to_nv_crtc_state(struct drm_crtc_state *state)
{
    return container_of(state, struct nv_drm_crtc_state, base);
}

static inline struct nv_drm_crtc *to_nv_crtc(struct drm_crtc *crtc)
{
    if (crtc == NULL) {
        return NULL;
    }
    return container_of(crtc, struct nv_drm_crtc, base);
}

/*
 * CRTCs are static objects, list does not change once after initialization and
 * before teardown of device. Initialization/teardown paths are single
 * threaded, so no locking required.
 */
static inline
struct nv_drm_crtc *nv_drm_crtc_lookup(struct nv_drm_device *nv_dev, NvU32 head)
{
    struct drm_crtc *crtc;
    nv_drm_for_each_crtc(crtc, nv_dev->dev) {
        struct nv_drm_crtc *nv_crtc = to_nv_crtc(crtc);

        if (nv_crtc->head == head)  {
            return nv_crtc;
        }
    }
    return NULL;
}

/**
 * nv_drm_crtc_enqueue_flip - Enqueue nv_drm_flip object to flip_list of crtc.
 */
static inline void nv_drm_crtc_enqueue_flip(struct nv_drm_crtc *nv_crtc,
                                            struct nv_drm_flip *nv_flip)
{
    spin_lock(&nv_crtc->flip_list_lock);
    list_add(&nv_flip->list_entry, &nv_crtc->flip_list);
    spin_unlock(&nv_crtc->flip_list_lock);
}

/**
 * nv_drm_crtc_dequeue_flip - Dequeue nv_drm_flip object to flip_list of crtc.
 */
static inline
struct nv_drm_flip *nv_drm_crtc_dequeue_flip(struct nv_drm_crtc *nv_crtc)
{
    struct nv_drm_flip *nv_flip = NULL;

    spin_lock(&nv_crtc->flip_list_lock);
    nv_flip = list_first_entry_or_null(&nv_crtc->flip_list,
                                       struct nv_drm_flip, list_entry);
    if (likely(nv_flip != NULL)) {
        list_del(&nv_flip->list_entry);
    }
    spin_unlock(&nv_crtc->flip_list_lock);

    WARN_ON(nv_flip == NULL);
    return nv_flip;
}

void nv_drm_enumerate_crtcs_and_planes(
    struct nv_drm_device *nv_dev,
    const long unsigned int nvkms_primary_formats_mask,
    const long unsigned int nvkms_cursor_formats_mask,
    unsigned int num_heads,
    NvU32 validCursorCompositionModes);

int nv_drm_get_crtc_crc32_ioctl(struct drm_device *dev,
                                void *data, struct drm_file *filep);

#endif /* NV_DRM_ATOMIC_MODESET_AVAILABLE */

#endif /* __NVIDIA_DRM_CRTC_H__ */
