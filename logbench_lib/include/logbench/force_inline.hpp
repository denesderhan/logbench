//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once

#if defined(_MSC_VER)                          // MSVC, clang-cl
#   define LOGBENCH_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)  // GCC, Clang, MinGW, ICC (Linux)
#   define LOGBENCH_FORCEINLINE inline __attribute__((__always_inline__))
#else
#   error "LOGBENCH_FORCEINLINE: unsupported compiler"
#endif
