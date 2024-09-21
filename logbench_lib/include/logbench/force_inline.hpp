//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#if defined(_WIN32)
	#define LOGBENCH_FORCEINLINE __forceinline
#elif defined(__GNUC__)
	#define LOGBENCH_FORCEINLINE inline __attribute__((__always_inline__))
#else
	#define LOGBENCH_FORCEINLINE Macro Error force inline failed!
#endif
