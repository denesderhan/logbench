//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once

#include <logbench/api_def.hpp>

#define LOGBENCHLIB_HEADER_VERSION_MAJOR 0
#define LOGBENCHLIB_HEADER_VERSION_MINOR 14
#define LOGBENCHLIB_HEADER_VERSION_PATCH 6

namespace logbench {
    LOGBENCH_API int version_major() noexcept;
    LOGBENCH_API int version_minor() noexcept;
    LOGBENCH_API int version_patch() noexcept;
    LOGBENCH_API const char* version() noexcept;

    inline bool compatible() noexcept {
        if constexpr (LOGBENCHLIB_HEADER_VERSION_MAJOR == 0) {
            return logbench::version_major() == LOGBENCHLIB_HEADER_VERSION_MAJOR
                && logbench::version_minor() == LOGBENCHLIB_HEADER_VERSION_MINOR;
        }
        // backwards compatibility
        else {
            return logbench::version_major() == LOGBENCHLIB_HEADER_VERSION_MAJOR
                && logbench::version_minor() >= LOGBENCHLIB_HEADER_VERSION_MINOR;
        }
    }
}
