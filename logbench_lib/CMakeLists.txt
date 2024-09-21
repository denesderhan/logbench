#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).

find_package(Threads REQUIRED)

set(include_logbench
	"include/logbench/api_def.hpp"
	"include/logbench/bench_data.hpp"
	"include/logbench/do_work.hpp"
	"include/logbench/fnv_hash.hpp"
	"include/logbench/force_inline.hpp"
	"include/logbench/formatter_typ.hpp"
	"include/logbench/is_exec.hpp"
	"include/logbench/is_pow2.hpp"
	"include/logbench/latch.hpp"
	"include/logbench/logger_thread.hpp"
	"include/logbench/logtest.hpp"
	"include/logbench/logtest1.hpp"
	"include/logbench/pin_thread.hpp"
	"include/logbench/precision_timer.hpp"
	"include/logbench/proc_high_prio.hpp"
	"include/logbench/proc_max_power.hpp"
	"include/logbench/proc_run_stats.hpp"
	"include/logbench/rdtsc_frequency.hpp"
	"include/logbench/sink_typ.hpp"
	"include/logbench/small_string.hpp"
	"include/logbench/store_path.hpp"
	"include/logbench/test_in_param.hpp"
	"include/logbench/test_out_param.hpp"
	"include/logbench/test_result_thread.hpp"
	"include/logbench/test_run.hpp"
	"include/logbench/thread_high_prio.hpp"
	"include/logbench/timestamp_nano.hpp"
	"include/logbench/timestamp_tsc.hpp"
	"include/logbench/to_hexstring.hpp"
	"include/logbench/version.hpp"
)

set(src
	"src/bench_data.cpp"
	"src/ipc_data_posix.cpp"
	"src/ipc_data_posix.hpp"
	"src/ipc_data_win.cpp"
	"src/ipc_data_win.hpp"
	"src/is_exec_posix.cpp"
	"src/is_exec_win.cpp"
	"src/max_power_impl_posix.hpp"
	"src/max_power_impl_win.hpp"
	"src/pin_thread.cpp"
	"src/pin_thread_posix.cpp"
	"src/pin_thread_win.cpp"
	"src/precision_timer.cpp"
	"src/precision_timer_impl_posix.hpp"
	"src/precision_timer_impl_win.hpp"
	"src/proc_high_prio_posix.cpp"
	"src/proc_high_prio_win.cpp"
	"src/proc_max_power.cpp"
	"src/rdtsc_frequency.cpp"
	"src/run_proc.hpp"
	"src/run_proc_posix.cpp"
	"src/run_proc_win.cpp"
	"src/test_run.cpp"
	"src/thread_high_prio_posix.cpp"
	"src/thread_high_prio_win.cpp"
)

set(ALL_FILES 
	${include_logbench}
	${src}
)

source_group("include/logbench" FILES ${include_logbench})
source_group("src" FILES ${src})

add_library(logbench ${ALL_FILES})
add_library(logbench::logbench ALIAS logbench)
target_link_libraries(logbench PUBLIC Threads::Threads)
set_target_properties(logbench PROPERTIES VERSION ${LOGBENCHLIB_VERSION} SOVERSION  ${LOGBENCHLIB_VERSION_MAJOR}.${LOGBENCHLIB_VERSION_MINOR})

if(BUILD_SHARED_LIBS)
	target_compile_definitions(logbench
		PUBLIC "LOGBENCH_SHARED"
		PRIVATE "LOGBENCH_EXPORT"
	)
endif()

if(NOT(MSVC) OR LOGBENCH_DEBUG)
	target_compile_definitions(logbench 
		PUBLIC $<$<CONFIG:Debug>:LOGBENCH_DEBUG>
	)
endif()

#--------------------
# Include directories
#--------------------
target_include_directories(logbench
	PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
	PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>"
	PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

#--------
# Install
#--------
install(
	TARGETS logbench
	EXPORT logbenchTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(FILES ${include_logbench} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/logbench")

install(EXPORT logbenchTargets
        FILE logbenchTargets.cmake
        NAMESPACE logbench::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/logbench
)

# ---------------------
# Package Configuration
# ---------------------
include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/logbenchConfig.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/logbenchConfig.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/logbench
)

set_property(
	TARGET logbench PROPERTY
	INTERFACE_logbench_MAJOR_VERSION ${LOGBENCHLIB_VERSION_MAJOR}
)
set_property(
	TARGET logbench APPEND PROPERTY
	COMPATIBLE_INTERFACE_STRING logbench_MAJOR_VERSION
)

# generate the version file for the config file
if(LOGBENCHLIB_VERSION_MAJOR EQUAL 0)
	write_basic_package_version_file(
	  "${CMAKE_CURRENT_BINARY_DIR}/logbenchConfigVersion.cmake"
	  VERSION "${LOGBENCHLIB_VERSION}"
	  COMPATIBILITY ExactVersion
	)
else()
	write_basic_package_version_file(
	  "${CMAKE_CURRENT_BINARY_DIR}/logbenchConfigVersion.cmake"
	  VERSION "${LOGBENCHLIB_VERSION}"
	  COMPATIBILITY SameMajorVersion
	)
endif()

install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/logbenchConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/logbenchConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/logbench
)


if(CMAKE_BUILD_TYPE)
# ----------
# pkg-config
# ----------
	set(PKG_LIBNAME "logbench")
	string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPER)
	set(PKG_DEFINES "")
	if (BUILD_TYPE_UPPER STREQUAL "DEBUG")
		set(PKG_LIBNAME "${PKG_LIBNAME}${CMAKE_DEBUG_POSTFIX}")
		set(PKG_DEFINES "-DLOGBENCH_DEBUG")
	endif ()
	if(IS_ABSOLUTE "${CMAKE_INSTALL_INCLUDEDIR}")
			set(PKG_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}")
	else()
		set(PKG_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
	endif()
	if(IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
		set(PKG_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
	else()
		set(PKG_LIBDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
	endif()

	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/liblogbench.pc.in"
		"${CMAKE_CURRENT_BINARY_DIR}/lib${PKG_LIBNAME}.pc" 
		@ONLY)
	install(FILES
		"${CMAKE_CURRENT_BINARY_DIR}/lib${PKG_LIBNAME}.pc"
		DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
	)
endif()
# -------------------------------------
# Exporting Targets from the Build Tree
# -------------------------------------
export(EXPORT logbenchTargets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/logbenchTargets.cmake"
       NAMESPACE logbench::
)
