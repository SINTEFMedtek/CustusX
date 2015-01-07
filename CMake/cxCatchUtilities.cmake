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
# Create the Catch executable
#
# NOTE: Should only be called once.
#
###############################################################################
function(cx_add_executable_catch CX_CATCH_LIB)

    message(STATUS "Generating master Catch exe.")
    message(STATUS "Adding tests from:")
    foreach(LIBRARY ${CX_SHARED_TEST_LIBRARIES})
        message(STATUS "        ${LIBRARY}")
    endforeach()

    configure_file(
      "${CustusX_SOURCE_DIR}/source/testing/cxImportTests.h.in"
      "${CustusX_BINARY_DIR}/source/testing/cxImportTests.h"
      )

    set(TEST_EXE_NAME "Catch")
    set(cxtest_MAIN ${CustusX_SOURCE_DIR}/source/testing/cxtestCatchMain.cpp)


    set(BUNDLE_TYPE "")
    if(CX_APPLE)
        set(BUNDLE_TYPE "MACOSX_BUNDLE")
    endif()

    add_executable(${TEST_EXE_NAME} ${BUNDLE_TYPE} ${cxtest_MAIN})
    target_link_libraries(${TEST_EXE_NAME} PRIVATE cxResource ${CX_SHARED_TEST_LIBRARIES} ${CX_CATCH_LIB})

    cx_install_target(${TEST_EXE_NAME})

endfunction(cx_add_executable_catch)

###############################################################################
# Clear the internal Catch cache
#
# NOTE: Should only be called once, before adding tests to Catch
#
###############################################################################
function(cx_clear_catch_cache)
    unset(CX_SHARED_TEST_LIBRARIES CACHE)
endfunction(cx_clear_catch_cache)

###############################################################################
# Add shared library with tests to Catch
#
# Usage: cx_add_tests_to_catch(cxtestShareLibrary)
#
###############################################################################
function(cx_add_tests_to_catch SHARED_LIBRARY_WITH_TESTS)
    set(CX_SHARED_TEST_LIBRARIES
        ${CX_SHARED_TEST_LIBRARIES}
        ${SHARED_LIBRARY_WITH_TESTS}
        CACHE INTERNAL
        "List of all shared libraries containing catch tests."
    )
endfunction(cx_add_tests_to_catch)

