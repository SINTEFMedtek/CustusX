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

if(BUILD_TESTING)

    set(CXTEST_SOURCES_TO_MOC
    )
    set(CXTEST_PLUGINALGORITHM_SOURCES
        cxtestBinaryThresholdImageFilter.cpp
        cxtestDilationFilter.cpp
        cxtestExportDummyClassForLinkingOnWindowsInLibWithoutExportedClass.cpp
    )

    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtestFilter ${CXTEST_PLUGINALGORITHM_SOURCES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtestFilter)
    target_include_directories(cxtestFilter
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtestFilter
        PRIVATE
        cxResource
		cxtestResource
        cxResourceFilter
		cxResourceVisualization
        cxCatch
        cxtestUtilities
    )
    cx_add_tests_to_catch(cxtestFilter)

endif(BUILD_TESTING)
