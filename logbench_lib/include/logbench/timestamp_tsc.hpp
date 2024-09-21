//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstdint>

#if defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#include <logbench/force_inline.hpp>

namespace logbench {
	LOGBENCH_FORCEINLINE auto timestamp_tsc(unsigned int &id) noexcept {
		_mm_mfence();
		_mm_lfence();
		auto tsc = __rdtscp(&id);
		_mm_lfence();
		return tsc;
	}
}
#elif defined(__ARM_ARCH_ISA_A64)
#include <sched.h>
#include <logbench/force_inline.hpp>
namespace logbench {
	LOGBENCH_FORCEINLINE auto timestamp_tsc(unsigned int &id) noexcept {
		unsigned int node;
		getcpu(&id, &node);
		std::uint64_t cntvct;
		asm volatile ("isb; mrs %0, cntvct_el0; isb; " : "=r"(cntvct) :: "memory");
		return cntvct;
	}
}
#else
#error timestamp_tsc() not implemented for this architecture.
#endif
