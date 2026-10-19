//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#if defined(_M_ARM64) && defined(_MSC_VER)
#   error timestamp_tsc not implemented
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#   define LOGBENCH_TSC_X86 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#   define LOGBENCH_TSC_A64 1
#else
#   error timestamp_tsc not implemented for this architecture.
#endif

#if defined(LOGBENCH_TSC_X86)
#   if defined(_MSC_VER)
#       include <intrin.h>
#   else
#       include <cpuid.h>
#       include <x86intrin.h>
#   endif
#endif

#include <atomic>
#include <cstdint>
#include <logbench/force_inline.hpp>

namespace logbench {

    /*
     * timestamp_tsc() pairs are intended to measure latency between two points.
     *
     * x86:
     *   - On x86, RDTSCP returns a TSC value and an auxiliary value (TSC_AUX),
     *     which we expose via the `core` parameter.
     *   - If two timestamps in a pair have different `core` values, the thread
     *     has likely migrated, and the measurement should be treated as invalid.
     *
     * AArch64 (ARMv8-A) without virtualization:
     *   - CNTVCT_EL0 is a system-wide counter visible from all cores and runs at
     *     a constant frequency. A read from any core is comparable to a read
     *     from any other core.
     *   - There is no architectural equivalent of TSC_AUX, so `core` is always 0
     *     and should be ignored for ARM.
     *
     * AArch64 under virtualization:
     *   - A hypervisor may present per-vCPU counters that are not synchronized
     *     across physical cores. In such environments, cross-core comparisons
     *     can be invalid even though `core` is always 0 and tsc_ok() returns true.
     */

#if defined(LOGBENCH_TSC_X86)

     // tsc_ok() must be true for this to be used. (for performance reasons, we don't check it here)
    LOGBENCH_FORCEINLINE std::uint64_t timestamp_tsc(std::uint32_t& core) noexcept {
        std::atomic_signal_fence(std::memory_order_seq_cst);//prevents compiler reordering of instructions across this point
        unsigned int aux;
        const std::uint64_t t = __rdtscp(&aux); // RDTSCP serializes previous instructions
        // Prevents CPU from reordering later loads 
        // (and, on recent CPUs, effectively all later instructions) before the timestamp
        _mm_lfence();
        core = aux;
        std::atomic_signal_fence(std::memory_order_seq_cst);//prevents compiler reordering of instructions across this point
        return t;
    }

#elif defined(LOGBENCH_TSC_A64) // LOGBENCH_TSC_A64

    LOGBENCH_FORCEINLINE std::uint64_t timestamp_tsc(std::uint32_t& core) noexcept {
        core = 0;
        std::atomic_signal_fence(std::memory_order_seq_cst);
        // instruction synchronization barrier; 
        // it ensures that all previous instructions have completed 
        // and the pipeline is flushed before subsequent instructions are fetched.
        asm volatile("isb" ::: "memory");
        std::uint64_t v;
        asm volatile("mrs %0, cntvct_el0" : "=r"(v) :: "memory");
        asm volatile("isb" ::: "memory");
        std::atomic_signal_fence(std::memory_order_seq_cst);
        return v;
    }    
#endif

#if defined(LOGBENCH_TSC_X86)
    LOGBENCH_FORCEINLINE bool tsc_ok() noexcept {
#   if defined(_MSC_VER)
        int cpu_features[4];
        __cpuid(cpu_features, 0x80000000);
        if (static_cast<unsigned>(cpu_features[0]) < 0x80000001u)
            return false;
        __cpuid(cpu_features, 0x80000001);
        return (cpu_features[3] & (1 << 27)) != 0;
#   else
        unsigned a, b, c, d;
        if (!__get_cpuid(0x80000000, &a, &b, &c, &d) || a < 0x80000001u)
            return false;
        __get_cpuid(0x80000001, &a, &b, &c, &d);
        return (d & (1u << 27)) != 0;
#   endif
    }

    LOGBENCH_FORCEINLINE bool tsc_invariant() noexcept {
#   if defined(_MSC_VER)
        int cpu_features[4];
        __cpuid(cpu_features, 0x80000000);
        if (static_cast<unsigned>(cpu_features[0]) < 0x80000007u)
            return false;
        __cpuid(cpu_features, 0x80000007);
        return (cpu_features[3] & (1 << 8)) != 0;
#   else
        unsigned a, b, c, d;
        if (!__get_cpuid(0x80000000, &a, &b, &c, &d) || a < 0x80000007u)
            return false;
        __get_cpuid(0x80000007, &a, &b, &c, &d);
        return (d & (1u << 8)) != 0;
#   endif
    }
#else
    LOGBENCH_FORCEINLINE bool tsc_ok() noexcept {
        return true;
    }
    LOGBENCH_FORCEINLINE bool tsc_invariant() noexcept {
        return true;
    }
#endif
} // namespace logbench
