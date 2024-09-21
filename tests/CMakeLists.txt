#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).

set(src
    "main.cpp"
)
source_group("Source Files" FILES ${src})

set(unit_tests
    "unit_tests/to_hexstring.cpp"
	"unit_tests/rdtsc_frequency.cpp"
)

source_group("unit_tests" FILES ${unit_tests})

set(ALL_FILES
    ${src}
    ${unit_tests}
)

add_executable(logbench_tests ${ALL_FILES})
set_target_properties(logbench_tests PROPERTIES 
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(logbench_tests PRIVATE
	"../logbench_lib/src"
)

################################################################################
# Dependencies
################################################################################

find_package(Catch2 REQUIRED)
target_link_libraries(logbench_tests
    PRIVATE logbench::logbench
	PRIVATE Catch2::Catch2
)

