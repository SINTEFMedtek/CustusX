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

###########################################################
#               cxTestUtilities
#
# Utilities library for testing.
# Contains the catch unit test framework.
# Add all system-wide test utilities here.
###########################################################

cmake_minimum_required( VERSION 2.6 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_TESTING)
        # link statically on linux, see explanation in cx_catch__private_define_platform_specific_linker_options
        cx_catch__private_define_platform_specific_linker_options()

        QT4_WRAP_CPP( CX_TESTUTILITIES_SOURCE_FILES
            cxtestSignalListener.h
            cxtestUtilities.h
            )
	set(CX_TESTUTILITIES_SOURCE_FILES
            cxtestUtilities.h
            cxtestUtilities.cpp
	    cxtestSignalListener.h
	    cxtestSignalListener.cpp
	    cxtestCatchImpl.h
	    cxtestCatchImpl.cpp
	    cxtestCatchExamples.cpp
            ${CX_TESTUTILITIES_SOURCE_FILES}
            )
	include_directories(
	    .
	    ${CustusX3_SOURCE_DIR}/source/ThirdParty/catch
	    )
        add_library( cxtestUtilities ${CX_CATCH_SHARED_LIB_TYPE} ${CX_TESTUTILITIES_SOURCE_FILES} )
	target_link_libraries(cxtestUtilities SSC)
endif (BUILD_TESTING)
