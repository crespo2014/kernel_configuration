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

#include "nvtypes.h"

#ifndef _cla0b5_h_
#define _cla0b5_h_

#ifdef __cplusplus
extern "C" {
#endif

#define KEPLER_DMA_COPY_A                                                            (0x0000A0B5)

#define NVA0B5_SET_SEMAPHORE_A                                                  (0x00000240)
#define NVA0B5_SET_SEMAPHORE_A_UPPER                                            7:0
#define NVA0B5_SET_SEMAPHORE_B                                                  (0x00000244)
#define NVA0B5_SET_SEMAPHORE_B_LOWER                                            31:0
#define NVA0B5_SET_SEMAPHORE_PAYLOAD                                            (0x00000248)
#define NVA0B5_SET_SRC_PHYS_MODE                                                (0x00000260)
#define NVA0B5_SET_SRC_PHYS_MODE_TARGET                                         1:0
#define NVA0B5_SET_SRC_PHYS_MODE_TARGET_LOCAL_FB                                (0x00000000)
#define NVA0B5_SET_SRC_PHYS_MODE_TARGET_COHERENT_SYSMEM                         (0x00000001)
#define NVA0B5_SET_DST_PHYS_MODE                                                (0x00000264)
#define NVA0B5_SET_DST_PHYS_MODE_TARGET                                         1:0
#define NVA0B5_SET_DST_PHYS_MODE_TARGET_LOCAL_FB                                (0x00000000)
#define NVA0B5_SET_DST_PHYS_MODE_TARGET_COHERENT_SYSMEM                         (0x00000001)
#define NVA0B5_LAUNCH_DMA                                                       (0x00000300)
#define NVA0B5_LAUNCH_DMA_DATA_TRANSFER_TYPE                                    1:0
#define NVA0B5_LAUNCH_DMA_DATA_TRANSFER_TYPE_PIPELINED                          (0x00000001)
#define NVA0B5_LAUNCH_DMA_FLUSH_ENABLE                                          2:2
#define NVA0B5_LAUNCH_DMA_FLUSH_ENABLE_FALSE                                    (0x00000000)
#define NVA0B5_LAUNCH_DMA_FLUSH_ENABLE_TRUE                                     (0x00000001)
#define NVA0B5_LAUNCH_DMA_SEMAPHORE_TYPE                                        4:3
#define NVA0B5_LAUNCH_DMA_SEMAPHORE_TYPE_NONE                                   (0x00000000)
#define NVA0B5_LAUNCH_DMA_SEMAPHORE_TYPE_RELEASE_ONE_WORD_SEMAPHORE             (0x00000001)
#define NVA0B5_LAUNCH_DMA_SRC_MEMORY_LAYOUT                                     7:7
#define NVA0B5_LAUNCH_DMA_SRC_MEMORY_LAYOUT_PITCH                               (0x00000001)
#define NVA0B5_LAUNCH_DMA_DST_MEMORY_LAYOUT                                     8:8
#define NVA0B5_LAUNCH_DMA_DST_MEMORY_LAYOUT_PITCH                               (0x00000001)
#define NVA0B5_LAUNCH_DMA_REMAP_ENABLE                                          10:10
#define NVA0B5_LAUNCH_DMA_REMAP_ENABLE_FALSE                                    (0x00000000)
#define NVA0B5_LAUNCH_DMA_REMAP_ENABLE_TRUE                                     (0x00000001)
#define NVA0B5_LAUNCH_DMA_SRC_TYPE                                              12:12
#define NVA0B5_LAUNCH_DMA_SRC_TYPE_VIRTUAL                                      (0x00000000)
#define NVA0B5_LAUNCH_DMA_SRC_TYPE_PHYSICAL                                     (0x00000001)
#define NVA0B5_LAUNCH_DMA_DST_TYPE                                              13:13
#define NVA0B5_LAUNCH_DMA_DST_TYPE_VIRTUAL                                      (0x00000000)
#define NVA0B5_LAUNCH_DMA_DST_TYPE_PHYSICAL                                     (0x00000001)
#define NVA0B5_OFFSET_IN_UPPER                                                  (0x00000400)
#define NVA0B5_OFFSET_IN_UPPER_UPPER                                            7:0
#define NVA0B5_OFFSET_IN_LOWER                                                  (0x00000404)
#define NVA0B5_OFFSET_IN_LOWER_VALUE                                            31:0
#define NVA0B5_OFFSET_OUT_UPPER                                                 (0x00000408)
#define NVA0B5_OFFSET_OUT_UPPER_UPPER                                           7:0
#define NVA0B5_OFFSET_OUT_LOWER                                                 (0x0000040C)
#define NVA0B5_OFFSET_OUT_LOWER_VALUE                                           31:0
#define NVA0B5_LINE_LENGTH_IN                                                   (0x00000418)
#define NVA0B5_LINE_COUNT                                                       (0x0000041C)
#define NVA0B5_SET_REMAP_CONST_A                                                (0x00000700)
#define NVA0B5_SET_REMAP_CONST_A_V                                              31:0
#define NVA0B5_SET_REMAP_CONST_B                                                (0x00000704)
#define NVA0B5_SET_REMAP_CONST_B_V                                              31:0
#define NVA0B5_SET_REMAP_COMPONENTS                                             (0x00000708)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_X                                       2:0
#define NVA0B5_SET_REMAP_COMPONENTS_DST_X_SRC_X                                 (0x00000000)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_X_SRC_Y                                 (0x00000001)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_X_SRC_Z                                 (0x00000002)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_X_SRC_W                                 (0x00000003)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_X_CONST_A                               (0x00000004)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_X_CONST_B                               (0x00000005)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_X_NO_WRITE                              (0x00000006)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Y                                       6:4
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Y_SRC_X                                 (0x00000000)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Y_SRC_Y                                 (0x00000001)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Y_SRC_Z                                 (0x00000002)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Y_SRC_W                                 (0x00000003)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Y_CONST_A                               (0x00000004)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Y_CONST_B                               (0x00000005)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Y_NO_WRITE                              (0x00000006)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Z                                       10:8
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Z_SRC_X                                 (0x00000000)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Z_SRC_Y                                 (0x00000001)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Z_SRC_Z                                 (0x00000002)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Z_SRC_W                                 (0x00000003)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Z_CONST_A                               (0x00000004)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Z_CONST_B                               (0x00000005)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_Z_NO_WRITE                              (0x00000006)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_W                                       14:12
#define NVA0B5_SET_REMAP_COMPONENTS_DST_W_SRC_X                                 (0x00000000)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_W_SRC_Y                                 (0x00000001)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_W_SRC_Z                                 (0x00000002)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_W_SRC_W                                 (0x00000003)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_W_CONST_A                               (0x00000004)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_W_CONST_B                               (0x00000005)
#define NVA0B5_SET_REMAP_COMPONENTS_DST_W_NO_WRITE                              (0x00000006)
#define NVA0B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE                              17:16
#define NVA0B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_ONE                          (0x00000000)
#define NVA0B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_TWO                          (0x00000001)
#define NVA0B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_THREE                        (0x00000002)
#define NVA0B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_FOUR                         (0x00000003)
#define NVA0B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS                          21:20
#define NVA0B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_ONE                      (0x00000000)
#define NVA0B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_TWO                      (0x00000001)
#define NVA0B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_THREE                    (0x00000002)
#define NVA0B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_FOUR                     (0x00000003)
#define NVA0B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS                          25:24
#define NVA0B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS_ONE                      (0x00000000)
#define NVA0B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS_TWO                      (0x00000001)
#define NVA0B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS_THREE                    (0x00000002)
#define NVA0B5_SET_REMAP_COMPONENTS_NUM_DST_COMPONENTS_FOUR                     (0x00000003)

#ifdef __cplusplus
};     /* extern "C" */
#endif
#endif // _cla0b5_h

