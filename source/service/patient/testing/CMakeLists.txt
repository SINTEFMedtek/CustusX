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
#               cxPatientService Tests
###########################################################
# _USE_MATH_DEFINES Must be defined before math.h is included
if (CX_WINDOWS)
    add_definitions(-D_USE_MATH_DEFINES)
endif(CX_WINDOWS)

if (BUILD_TESTING)
        # Catch unit test framework stuff:
    include_directories(
        ..
	${CustusX3_SOURCE_DIR}/source/resource/core/testing
		)

    set(CXTEST_SOURCES_TO_MOC
    )
    cx_add_class_qt_moc(CXTEST_PATIENTSERVICE_SOURCES
            )
    cx_add_class(CXTEST_PATIENTSERVICE_SOURCES
        cxtestCatchPatientService.cpp
        cxtestCatchFrameMetric.cpp
        cxtestCatchToolMetric.cpp
        cxtestCatchDistanceMetric.cpp
        cxtestMetricFixture
        )

    cx_catch_add_lib_and_exe(cxPatientService "${CXTEST_PATIENTSERVICE_SOURCES}" "${CXTEST_SOURCES_TO_MOC}")

endif (BUILD_TESTING)

