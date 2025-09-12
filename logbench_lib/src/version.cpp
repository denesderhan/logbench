//Copyright © 2025, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#include <logbench/version.hpp>

#define LOGBENCHLIB_STRINGIFY_IMPL(X) #X
#define LOGBENCHLIB_STRINGIFY(X) LOGBENCHLIB_STRINGIFY_IMPL(X)

#define LOGBENCHLIB_VERSION LOGBENCHLIB_STRINGIFY(\
    LOGBENCHLIB_HEADER_VERSION_MAJOR)"."\
    LOGBENCHLIB_STRINGIFY(LOGBENCHLIB_HEADER_VERSION_MINOR)"."\
    LOGBENCHLIB_STRINGIFY(LOGBENCHLIB_HEADER_VERSION_PATCH)

namespace logbench {
    int version_major() noexcept {
        return LOGBENCHLIB_HEADER_VERSION_MAJOR;
    }
    
    int version_minor() noexcept {
        return LOGBENCHLIB_HEADER_VERSION_MINOR;
    }
    
    int version_patch() noexcept {
        return LOGBENCHLIB_HEADER_VERSION_PATCH;
    }
    
    const char* version() noexcept {
        return LOGBENCHLIB_VERSION;
    }
}
