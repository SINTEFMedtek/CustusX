cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ Library 
# =========================================================

project( SSC )

# code coverage
option(SSC_USE_GCOV "add gcov to enable coverage testing" OFF)
if(SSC_USE_GCOV)
	message(STATUS "Building SSC with gcov code coverage support.")
	set(SSC_GCOV_LIBRARY )
	if(WIN32)
		message(ERROR "gcov not supported for WIN32")
	# needed on apple
	elseif(APPLE)
		set( SSC_GCOV_LIBRARY ${SSC_GCOV_LIBRARY} profile_rt )
	# needed on linux
	else(WIN32)
		set( SSC_GCOV_LIBRARY ${SSC_GCOV_LIBRARY} gcov)
	endif(WIN32)

	add_definitions(--coverage)
endif()

option(SSC_BUILD_PURE_CPPUNIT_TESTS "build cppunit tests unconnected to the ctest framework. Do not use when doine code coverage" OFF)

option(SSC_PRINT_CALLER_INFO "Print extra information about messages on the commandline." OFF)
if(SSC_PRINT_CALLER_INFO)
        add_definitions(-DSSC_PRINT_CALLER_INFO)
endif()

enable_testing()

include_directories( /usr/local/include )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/CMake)

include(cxCotireMacros)
CX_COTIRE_INITIALIZE()

add_subdirectory(Code)
add_subdirectory(DICOMLib)
add_subdirectory(Examples)
add_subdirectory(Testing)
