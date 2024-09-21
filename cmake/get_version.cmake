#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
# Get version from logbench/version.h and put it in LOGBENCHLIB_VERSION
function(get_version)
	file(READ "${CMAKE_CURRENT_LIST_DIR}/logbench_lib/include/logbench/version.hpp" version_hpp)
	
    string(REGEX MATCH "LOGBENCHLIB_VERSION_MAJOR ([0-9]+)" _ "${version_hpp}")
    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract version.")
    endif()
    set(v_major ${CMAKE_MATCH_1})

    string(REGEX MATCH "LOGBENCHLIB_VERSION_MINOR ([0-9]+)" _ "${version_hpp}")
    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract version.")
    endif()
    set(v_minor ${CMAKE_MATCH_1})

    string(REGEX MATCH "LOGBENCHLIB_VERSION_PATCH ([0-9]+)" _ "${version_hpp}")
    if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract version.")
    endif()
    set(v_patch ${CMAKE_MATCH_1})

	set(LOGBENCHLIB_VERSION_MAJOR "${v_major}"  PARENT_SCOPE)
	set(LOGBENCHLIB_VERSION_MINOR "${v_minor}"  PARENT_SCOPE)
	set(LOGBENCHLIB_VERSION_PATCH "${v_patch}"  PARENT_SCOPE)
    set(LOGBENCHLIB_VERSION "${v_major}.${v_minor}.${v_patch}"  PARENT_SCOPE)
endfunction()
