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

#cmake_minimum_required( VERSION 2.4 )

# http://www.cmake.org/cmake/help/v2.8.10/cmake.html#policy%3aCMP0003
#if(COMMAND cmake_policy)
#  cmake_policy(SET CMP0003 NEW)
#endif(COMMAND cmake_policy)

if (BUILD_TESTING)
    include_directories(
        ${CustusX3_SOURCE_DIR}/source/ThirdParty/VTKMultiVolume
        )

    cx_add_class(CX_CPPUNIT_VISUALIZATIONSERVICE_SOURCES
        cxtestCatchMultiVolume3DRepProducer.cpp
        cxtestMultiVolume3DRepProducerFixture
        cxtestCatchViewRenderSpeed.cpp
        cxtestCatchVolumeReps.cpp
        cxtestCatchVtkOpenGLGPUMultiVolumeRayCastMapper.cpp
		cxtestDataTypeSort.cpp
        cxTestRenderSpeed.h
        cxTestRenderSpeed.cpp
        )

    cx_catch_add_lib_and_exe(cxVisualizationService "${CXTEST_VISUALIZATIONSERVICE_SOURCES}" "${CXTEST_SOURCES_TO_MOC}")


endif (BUILD_TESTING)
