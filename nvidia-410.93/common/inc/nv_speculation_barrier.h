/*
 * Copyright 2018 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef _NV_SPECULATION_BARRIER_H_
#define _NV_SPECULATION_BARRIER_H_

#define NV_SPECULATION_BARRIER_VERSION 1

/*
 * GNU-C/MSC/clang - x86/x86_64 : __x86_64, __i386, __i386__
 * GNU-C - THUMB mode       : __GNUC__, __thumb__
 * GNU-C - ARM modes        : __GNUC__, __arm__, __aarch64__
 * armclang - THUMB mode    : __ARMCC_VERSION, __thumb__
 * armclang - ARM modes     : __ARMCC_VERSION, __arm__, __aarch64__
 * GHS - THUMB mode     : __ghs__, __THUMB__
 * GHS - ARM modes      : __ghs__, __ARM__, __ARM64__
 */

#if defined(_M_IX86) || defined(__i386__) || defined(__i386) || defined(__x86_64) || defined(AMD64) || defined(_M_AMD64)

 /* All x86 */
 #define NV_SPECULATION_BARRIER_x86

#elif defined(macintosh) || defined(__APPLE__) || defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || \
      defined(__POWERPC__) || defined(__ppc) || defined(__ppc__) || defined(__ppc64__) || defined(__PPC__) || \
      defined(__PPC64__) || defined(_ARCH_PPC) || defined(_ARCH_PPC64)
 
 /* All PowerPC */
 #define NV_SPECULATION_BARRIER_PPC

#elif (defined(__GNUC__) && defined(__thumb__)) \
    || (defined(__ARMCC_VERSION) && defined(__thumb__)) \
    || (defined(__ghs__) && defined(__THUMB__))
 /* ARM-thumb mode(<=ARMv7)/T32 (ARMv8) */
 #define NV_SPECULATION_BARRIER_ARM_COMMON
 #define NV_SPEC_BARRIER_CSDB ".inst.w 0xf3af8014\n"

#elif (defined(__GNUC__) && defined(__arm__)) \
    || (defined(__ARMCC_VERSION) && defined(__arm__)) \
    || (defined(__ghs__) && defined(__ARM__))
 /* aarch32(ARMv8) / arm(<=ARMv7) mode */
 #define NV_SPECULATION_BARRIER_ARM_COMMON
 #define NV_SPEC_BARRIER_CSDB ".inst 0xe320f014\n"

#elif (defined(__GNUC__) && defined(__aarch64__)) \
    || (defined(__ARMCC_VERSION) && defined(__aarch64__)) \
    || (defined(__ghs__) && defined(__ARM64__))
 /* aarch64(ARMv8) mode */
 #define NV_SPECULATION_BARRIER_ARM_COMMON
 #define NV_SPEC_BARRIER_CSDB "HINT #20\n"

#else
 #error "Unknown compiler/chip family"
#endif

/*
 * nv_speculation_barrier -- General-purpose speculation barrier
 *
 * This approach provides full protection against variant-1 vulnerability.
 * However, the recommended approach is detailed below (See:
 * nv_array_index_no_speculate)
 *
 * Semantics:
 * Any memory read that is sequenced after a nv_speculation_barrier(),
 * and contained directly within the scope of nv_speculation_barrier() or
 * directly within a nested scope, will not speculatively execute until all
 * conditions for entering that scope have been architecturally resolved.
 *
 * Example:
 * if (untrusted_index_from_user < bound)  {
 *  ...
 *  nv_speculation_barrier();
 *  ...
 *  x = array1[untrusted_index_from_user];
 *  bit = x & 1;
 *  y = array2[0x100 * bit];
 * }
 */

#if defined(NV_SPECULATION_BARRIER_x86)
// Delete after all references are changed to nv_speculation_barrier
#define speculation_barrier() nv_speculation_barrier()

static inline void nv_speculation_barrier(void)
{

#if defined(_MSC_VER) && !defined(__clang__) 
    _mm_lfence();
#endif

#if defined(__GNUC__) || defined(__clang__)
    __asm__ __volatile__ ("lfence" : : : "memory");
#endif

}

#elif defined(NV_SPECULATION_BARRIER_PPC)

static inline void nv_speculation_barrier(void)
{
    asm volatile("ori 31,31,0");
}

#elif defined(NV_SPECULATION_BARRIER_ARM_COMMON)

/* Note: Cortex-A9 GNU-assembler seems to complain about DSB SY */
 #define nv_speculation_barrier()   \
    asm volatile            \
    (               \
        "DSB sy\n"      \
        "ISB\n"         \
        : : : "memory"      \
    )
#endif

/*
 * nv_array_index_no_speculate -- Recommended variant-1 mitigation approach
 *
 * The array-index-no-speculate approach "de-speculates" an array index that
 * has already been bounds-checked.
 *
 * This approach is preferred over nv_speculation_barrier due to the following
 * reasons:
 * - It is just as effective as the general-purpose speculation barrier.
 * - It clearly identifies what array index is being de-speculated and is thus
 *   self-commenting, whereas the general-purpose speculation barrier requires
 *   an explanation of what array index is being de-speculated.
 * - It performs substantially better than the general-purpose speculation
 *   barrier on ARM Cortex-A cores (the difference is expected to be tens of
 *   cycles per invocation). Within tight loops, this difference may become
 *   noticeable.
 *
 * Semantics:
 * Provided count is non-zero and the caller has already validated or otherwise
 * established that index < count, any speculative use of the return value will
 * use a speculative value that is less than count.
 *
 * Example:
 * if (untrusted_index_from_user < bound)  {
 *  untrusted_index_from_user = nv_array_index_no_speculate(
 *                  untrusted_index_from_user, bound);
 *  ...
 *  x = array1[untrusted_index_from_user];
 *  ...
 * }
 *
 * The use of nv_array_index_no_speculate() in the above example ensures that
 * subsequent uses of untrusted_index_from_user will not execute speculatively
 * (they will wait for the bounds check to complete).
 */

#if defined(NV_SPECULATION_BARRIER_x86)

// TODO Add implementation for _MSC_VER and PPC 
#if defined(__GNUC__) || defined(__clang__)

static inline unsigned long nv_array_index_no_speculate(unsigned long index,
                        unsigned long count)
{
    unsigned long mask;

    __asm__ __volatile__
    (
        "CMP     %2, %1          \n"
        "SBB     %0, %0          \n"
        : "=r"(mask) : "r"(index), "r"(count) : "cc"
    );

    return (index & mask);
}
#endif


#elif defined(NV_SPECULATION_BARRIER_ARM_COMMON)
static inline unsigned long nv_array_index_no_speculate(unsigned long index,
                        unsigned long count)
{
    unsigned long mask;

    asm volatile
    (
        "CMP %[ind], %[cnt] \n"
        "SBC %[res], %[cnt], %[cnt] \n"
        NV_SPEC_BARRIER_CSDB
        : [res] "=r" (mask) : [ind] "r" (index), [cnt] "r" (count): "cc"
    );

    return (index & mask);
}
#endif

#endif //_NV_SPECULATION_BARRIER_H_
