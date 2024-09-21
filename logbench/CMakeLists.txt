#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).

set(src
    "main.cpp"
)
source_group("Source Files" FILES ${src})

set(includes
    "parse_par.hpp"
    "version.hpp"
	"clipp.h"
)

source_group("Header Files" FILES ${includes})

set(ALL_FILES
    ${src}
    ${includes}
)

add_executable(logbench_run ${ALL_FILES})
set_target_properties(logbench_run PROPERTIES 
	OUTPUT_NAME "logbench"
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(logbench_run PRIVATE
	"./"
	"../logbench_lib/include"
)

################################################################################
# Dependencies
################################################################################

target_link_libraries(logbench_run
    PRIVATE logbench::logbench
)

install(
	TARGETS logbench_run
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
