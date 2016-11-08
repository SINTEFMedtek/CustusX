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
#               cxPluginAlgorithm Tests
###########################################################

if(BUILD_TESTING)
    cx_add_class(CXTEST_SOURCES ${CXTEST_SOURCES}
        cxtestAccusurfFilter.cpp
    )
    set(CXTEST_SOURCES_TO_MOC
    )

    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtest_org_custusx_filter_accusurf ${CXTEST_SOURCES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_filter_accusurf)
    target_include_directories(cxtest_org_custusx_filter_accusurf
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_filter_accusurf
        PRIVATE
        org_custusx_filter_accusurf
		cxLogicManager
        cxtestUtilities
        cxCatch
    )
    cx_add_tests_to_catch(cxtest_org_custusx_filter_accusurf)

endif(BUILD_TESTING)
