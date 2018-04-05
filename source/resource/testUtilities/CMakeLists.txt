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
        cxtestVtkPolyDataTree.h
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
