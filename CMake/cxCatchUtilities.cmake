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
macro(cx_catch_initialize)
	#message(STATUS "CX_TEST_CATCH_GENERATED_LIBRARIES: " ${CX_TEST_CATCH_GENERATED_LIBRARIES})
	# used as a global variable: clear at start of run
	unset(CX_TEST_CATCH_GENERATED_LIBRARIES CACHE)
endmacro()

###############################################################################
# private
###############################################################################
macro(cx_catch__private_define_platform_specific_linker_options)
    # need --whole-archive and STATIC libs in order to link in the static register test code in Catch.
    # see http://stackoverflow.com/questions/805555/ld-linker-question-the-whole-archive-option
    # and http://stackoverflow.com/questions/14347107/how-to-put-compiler-command-line-args-in-specific-places-using-cmake
    # for background
    if(CX_LINUX)
        set(SHARED_LIB_TYPE STATIC)
        #set(SHARED_LIB_TYPE "")
        set(PRE_WHOLE_ARCHIVE "-Wl,--whole-archive")
        set(POST_WHOLE_ARCHIVE "-Wl,--no-whole-archive")
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
macro(cx_catch_add_lib_and_exe LIB_TO_TEST SOURCES)
	message(STATUS "Adding catch test targets based on: ${LIB_TO_TEST}")

	include_directories(
	    .
	    ${CustusX3_SOURCE_DIR}/source/resource/testUtilities
	    ${CustusX3_SOURCE_DIR}/source/ThirdParty/catch)

        cx_catch__private_define_platform_specific_linker_options()
	set(TEST_LIB_NAME "cxtestCatch${LIB_TO_TEST}")
        add_library(${TEST_LIB_NAME} ${SHARED_LIB_TYPE} ${SOURCES} )
        message(STATUS "          Lib name : ${TEST_LIB_NAME}")
	target_link_libraries(${TEST_LIB_NAME} ${LIB_TO_TEST} cxtestUtilities )

	set(CX_TEST_CATCH_GENERATED_LIBRARIES
		"${TEST_LIB_NAME}" "${CX_TEST_CATCH_GENERATED_LIBRARIES}"
		CACHE INTERNAL
		"List of all catch unit test libs that should be added to the master test exe.")
	
	set(TEST_EXE_NAME "Catch${LIB_TO_TEST}")
	message(STATUS "          Exe name : ${TEST_EXE_NAME}")

	set(cxtest_MAIN ${CustusX3_SOURCE_DIR}/source/resource/testUtilities/cxtestCatchMain.cpp)
        add_executable(${TEST_EXE_NAME} ${cxtest_MAIN} )
        target_link_libraries(${TEST_EXE_NAME} ${PRE_WHOLE_ARCHIVE} ${TEST_LIB_NAME} ${POST_WHOLE_ARCHIVE})

# alternative where the lib is omitted
#        add_executable(${TEST_EXE_NAME} ${cxtest_MAIN} ${SOURCES})
#        target_link_libraries(${TEST_EXE_NAME} ${LIB_TO_TEST} cxtestUtilities)
endmacro()


###############################################################################
# Add a master exe target for the Catch unit testing framework.
#
# The target contains all the libraries added using the
# cx_catch_add_lib_and_exe() macro.
###############################################################################
macro(cx_catch_add_master_exe)
        message(STATUS "Generating master Catch exe containing libs:")
	foreach( NAME ${CX_TEST_CATCH_GENERATED_LIBRARIES})
		message(STATUS "    ${NAME}")
	endforeach()

        cx_catch__private_define_platform_specific_linker_options()

	include_directories(
		.
                ${CustusX3_SOURCE_DIR}/source/resource/testUtilities
		${CustusX3_SOURCE_DIR}/source/ThirdParty/catch)

	set(TEST_EXE_NAME "Catch")
        set(cxtest_MAIN ${CustusX3_SOURCE_DIR}/source/resource/testUtilities/cxtestCatchMain.cpp)

        add_executable(${TEST_EXE_NAME} ${cxtest_MAIN} )
        target_link_libraries(${TEST_EXE_NAME} ${PRE_WHOLE_ARCHIVE} ${CX_TEST_CATCH_GENERATED_LIBRARIES} ${POST_WHOLE_ARCHIVE} cxtestUtilities )
        cx_install_target(${TEST_EXE_NAME})
endmacro()


