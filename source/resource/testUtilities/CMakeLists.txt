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
	
add_subdirectory(cisst_code)

if(BUILD_TESTING)

    set(CX_TESTUTILITIES_FILES_TO_MOC
        cxtestQueuedSignalListener.h
        cxtestDirectSignalListener.h
    )

    set(CX_TESTUTILITIES_SOURCE_FILES
        cxtestUtilities.h
        cxtestUtilities.cpp
        cxtestJenkinsMeasurement.h
        cxtestJenkinsMeasurement.cpp
        cxtestQueuedSignalListener.cpp
        cxtestDirectSignalListener.cpp
        cxtestSyntheticVolumeComparer.h
        cxtestSyntheticVolumeComparer.cpp
        cxtestSphereSyntheticVolume.h
        cxtestSphereSyntheticVolume.cpp
        cxtestSyntheticReconstructInput.h
        cxtestSyntheticReconstructInput.cpp
        cxSyntheticVolume.h
        cxSyntheticVolume.cpp
        cxSimpleSyntheticVolume.h
        cxSimpleSyntheticVolume.cpp
        cxtestProbeFixture.h
        cxtestProbeFixture.cpp
    )

    qt5_wrap_cpp(CX_TESTUTILITIES_FILES_TO_MOC ${CX_TESTUTILITIES_FILES_TO_MOC})
    add_library(cxtestUtilities ${CX_TESTUTILITIES_SOURCE_FILES} ${CX_TESTUTILITIES_FILES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtestUtilities)
    target_include_directories(cxtestUtilities
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CustusX_SOURCE_DIR}/source/resource/core/usReconstructionTypes
    )
    target_link_libraries(cxtestUtilities
        PRIVATE
        Qt5::Test
        cxResource
        cxCatch
    )
    cx_add_tests_to_catch(cxtestUtilities)

endif(BUILD_TESTING)
