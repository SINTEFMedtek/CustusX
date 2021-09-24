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

if(BUILD_TESTING)

    set(CXTEST_SOURCES_TO_MOC
    )
    set(CXTEST_PLUGINALGORITHM_SOURCES
        cxtestBinaryThresholdImageFilter.cpp
        cxtestDilationFilter.cpp
        cxtestExportDummyClassForLinkingOnWindowsInLibWithoutExportedClass.cpp
        cxtestScriptFilter.cpp
				cxtestColorVariationFilter.cpp
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
        cxLogicManager
				cxtest_org_custusx_core_patientmodel
    )
    cx_add_tests_to_catch(cxtestFilter)

endif(BUILD_TESTING)
