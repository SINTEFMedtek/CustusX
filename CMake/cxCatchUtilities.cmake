# =========================================================================
# This file is part of CustusX, an Image Guided Therapy Application.
#
# Copyright (c) SINTEF Department of Medical Technology.
# All rights reserved.
#
# CustusX is released under a BSD 3-Clause license.
#
# See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
# =========================================================================

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
    if(CX_LINUX)
        cx_install_target(${SHARED_LIBRARY_WITH_TESTS})
    endif(CX_LINUX)
endfunction(cx_add_tests_to_catch)

