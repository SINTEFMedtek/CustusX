# This file is part of CustusX, an Image Guided Therapy Application.
#
# Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
#
# CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
# code and binaries can only be used by SMT and those with explicit permission
# from SMT. CustusX shall not be distributed to anyone else.
#
# CustusX is a research tool. It is NOT intended for use or certified for use
# in a normal clinical setting. SMT does not take responsibility for its use
# in any way.
#
# See CustusX_License.txt for more information.

###############################################################################
# Initialize the catch build framework.
# Call before any other calls in this module.
###############################################################################
function(cx_catch_initialize)
	# used as a global variable: clear at start of run
	unset(CX_TEST_CATCH_GENERATED_LIBRARIES CACHE)
	unset(CX_TEST_CATCH_SOURCES CACHE)
	unset(CX_TEST_CATCH_INCLUDE_DIRS CACHE)
	unset(CX_TEST_CATCH_LINKER_LIBS CACHE)
	unset(CX_TEST_CATCH_MOC_SOURCES CACHE)
endfunction()

###############################################################################
# private
###############################################################################
macro(cx_catch__private_define_platform_specific_linker_options)
    # need --whole-archive and STATIC libs in order to link in the static register test code in Catch.
    # see http://stackoverflow.com/questions/805555/ld-linker-question-the-whole-archive-option
    # and http://stackoverflow.com/questions/14347107/how-to-put-compiler-command-line-args-in-specific-places-using-cmake
    # for background
    if(CX_LINUX)
        set(CX_CATCH_SHARED_LIB_TYPE STATIC)
        set(CX_CATCH_PRE_WHOLE_ARCHIVE "-Wl,--whole-archive")
        set(CX_CATCH_POST_WHOLE_ARCHIVE "-Wl,--no-whole-archive")
    else()
    endif()
endmacro()

###############################################################################
# Add targets for the Catch unit testing framework.
#
# A library containing all the tests is created. This can be linked
# in by other catch libs. 
# 
# A catch executable for running all the tests are also created.  
#
# Input variables:
#    LIB_TO_TEST : Name of the library to create tests for. 
#    SOURCES     : List of test source files.
#
###############################################################################
function(cx_catch_add_lib_and_exe LIB_TO_TEST SOURCES MOC_SOURCES)
	set(ADDITIONAL_LIBS ${ARGV3}) # emulating optional argument
#message(STATUS "========= " ${ADDITIONAL_LIBS} )

    if(CX_WINDOWS)
		_cx_catch_save_info_in_globals(${LIB_TO_TEST} "${SOURCES}" "${MOC_SOURCES}" "${ADDITIONAL_LIBS}")
    else()
		_cx_catch_add_lib_and_exe(${LIB_TO_TEST} "${SOURCES}" "${MOC_SOURCES}" "${ADDITIONAL_LIBS}")
    endif()

endfunction()

###############################################################################
# Add a master exe target for the Catch unit testing framework.
#
# The target contains all the libraries added using the
# cx_catch_add_lib_and_exe() macro.
###############################################################################
function(cx_catch_add_master_exe)

    include_directories(
            .
            ${CustusX_SOURCE_DIR}/source/resource/testUtilities
            ${CustusX_SOURCE_DIR}/source/ThirdParty/catch)

    set(TEST_EXE_NAME "Catch")
    set(cxtest_MAIN ${CustusX_SOURCE_DIR}/source/resource/testUtilities/cxtestCatchMain.cpp)

    if(CX_WINDOWS)
		_cx_catch_generate_master_catch_using_sources(${TEST_EXE_NAME} ${cxtest_MAIN})
    else()
        _cx_catch_generate_master_catch_using_libs(${TEST_EXE_NAME} ${cxtest_MAIN})
    endif()
    
    cx_install_target(${TEST_EXE_NAME})

endfunction()

###############################################################################
# PRIVATE:
# Create master Catch using source files. 
###############################################################################
function(_cx_catch_generate_master_catch_using_sources EXE_NAME PATH_TO_MAIN)
    message(STATUS "Generating master Catch exe.")
    include_directories(
        ${CX_TEST_CATCH_INCLUDE_DIRS}
    )
    
    QT4_WRAP_CPP(MOCCED ${CX_TEST_CATCH_MOC_SOURCES})
    
    add_executable(${EXE_NAME} ${PATH_TO_MAIN} ${CX_TEST_CATCH_SOURCES} ${MOCCED})
    target_link_libraries(${EXE_NAME} ${CX_TEST_CATCH_LINKER_LIBS} cxtestUtilities)
    
endfunction()

###############################################################################
# PRIVATE:
# Create master Catch using testing libraries.
###############################################################################
function(_cx_catch_generate_master_catch_using_libs EXE_NAME PATH_TO_MAIN)
    message(STATUS "Generating master Catch exe containing libs:")
    foreach( NAME ${CX_TEST_CATCH_GENERATED_LIBRARIES})
            message(STATUS "    ${NAME}")
    endforeach()
    
    cx_catch__private_define_platform_specific_linker_options()
    add_executable(${EXE_NAME} ${PATH_TO_MAIN} )
    target_link_libraries(${EXE_NAME} ${CX_CATCH_PRE_WHOLE_ARCHIVE} ${CX_TEST_CATCH_GENERATED_LIBRARIES} cxtestUtilities ${CX_CATCH_POST_WHOLE_ARCHIVE}  )
endfunction()

###############################################################################
# PRIVATE:
# Save needed information about source files to be able to add them to
# a master Catch.
###############################################################################
function(_cx_catch_save_info_in_globals LIB_TO_TEST SOURCES MOC_SOURCES ADDITIONAL_LIBS)
    foreach( SOURCE_FILE ${MOC_SOURCES})
        cx_make_path_absolute(${SOURCE_FILE} RESULT)
        set(ABS_MOC_SOURCES
            ${ABS_MOC_SOURCES}
            ${RESULT}
            )
    endforeach()
    foreach( SOURCE_FILE ${SOURCES})
        cx_make_path_absolute(${SOURCE_FILE} RESULT)
        set(ABS_SOURCES
            ${ABS_SOURCES}
            ${RESULT}
            )
    endforeach()
    set(CX_TEST_CATCH_SOURCES
        "${CX_TEST_CATCH_SOURCES}"
        "${ABS_SOURCES}"
        CACHE INTERNAL
        "List of all Catch sources.")
    set(CX_TEST_CATCH_MOC_SOURCES
        "${CX_TEST_CATCH_MOC_SOURCES}"
        "${ABS_MOC_SOURCES}"
        CACHE INTERNAL
        "List of all sources that needs to be mocced.")
    get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
    set(CX_TEST_CATCH_INCLUDE_DIRS
        "${CX_TEST_CATCH_INCLUDE_DIRS}"
        "${CMAKE_CURRENT_LIST_DIR}"
        ${inc_dirs}
        CACHE INTERNAL
        "List of all Catch include directories."
        )
     set(CX_TEST_CATCH_LINKER_LIBS
        "${CX_TEST_CATCH_LINKER_LIBS}"
        ${LIB_TO_TEST}
		${ADDITIONAL_LIBS}
        CACHE INTERNAL
        "List of all libraries Catch needs to link to."
        )
endfunction()

###############################################################################
#
# Replacement for add_library().
# Usage: cx_add_test_library(LIB_NAME source1 source2 ...)
#
# Creates a library that can be used within the catch framework.
# I.e, it is created statically if needed and catch tests can thus
# be found by the exe.
#
###############################################################################
macro(cx_add_test_library LIB_NAME)
    set(SOURCE_FILES ${ARGV})
    list(REMOVE_AT SOURCE_FILES 0) # SOURCE_FILES is now all but the first argument
    cx_catch__private_define_platform_specific_linker_options()
    add_library(${LIB_NAME} ${CX_CATCH_SHARED_LIB_TYPE} ${SOURCE_FILES})
endmacro()


###############################################################################
# PRIVATE:
# Create testing lib and small catch executable for the incoming lib.
###############################################################################
function(_cx_catch_add_lib_and_exe LIB_TO_TEST SOURCES MOC_SOURCES ADDITIONAL_LIBS)
	set(TEST_LIB_NAME "cxtestCatch${LIB_TO_TEST}")

	message(STATUS "Adding catch test targets based on: ${LIB_TO_TEST}")
#    message(STATUS "          Lib name : ${TEST_LIB_NAME}")

	include_directories(
        .
        ${CustusX_SOURCE_DIR}/source/resource/testUtilities
        ${CustusX_SOURCE_DIR}/source/ThirdParty/catch)

    cx_catch__private_define_platform_specific_linker_options()

    QT4_WRAP_CPP(MOCCED ${MOC_SOURCES})


    cx_add_test_library(${TEST_LIB_NAME} ${SOURCES} ${MOCCED})
#    add_library(${TEST_LIB_NAME} ${CX_CATCH_SHARED_LIB_TYPE} ${SOURCES} ${MOCCED})
#    message(STATUS "          Lib name : ${TEST_LIB_NAME}")
	target_link_libraries(${TEST_LIB_NAME} LINK_PUBLIC ${LIB_TO_TEST} cxtestUtilities ${ADDITIONAL_LIBS})
    
    set(CX_TEST_CATCH_GENERATED_LIBRARIES
        "${TEST_LIB_NAME}" "${CX_TEST_CATCH_GENERATED_LIBRARIES}"
        CACHE INTERNAL
        "List of all catch unit test libs that should be added to the master test exe.")

#    set(TEST_EXE_NAME "Catch${LIB_TO_TEST}")
#    message(STATUS "          Exe name : ${TEST_EXE_NAME}")

#    set(cxtest_MAIN ${CustusX_SOURCE_DIR}/source/resource/testUtilities/cxtestCatchMain.cpp)
#    add_executable(${TEST_EXE_NAME} ${cxtest_MAIN})
#    target_link_libraries(${TEST_EXE_NAME} ${CX_CATCH_PRE_WHOLE_ARCHIVE} ${TEST_LIB_NAME} ${CX_CATCH_POST_WHOLE_ARCHIVE})
endfunction()


