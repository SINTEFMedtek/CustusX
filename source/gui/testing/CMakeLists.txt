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
        cxTestCustusXController.h
        cxtestTestTransferFunctionColorWidget.h
    )

    set(CXTEST_GUI_SOURCES
        cxTestCustusXController.cpp
        cxtestSpeed.cpp
        cxtestCatchBaseWidgets.cpp
        cxtestTestTransferFunctionColorWidget.h
        cxtestTestTransferFunctionColorWidget.cpp
        cxtestTransferFunctionWidgets.cpp
        cxtestTestTransferFunctions.h
        cxtestTestTransferFunctions.cpp
        cxtestOpenCLAndGui.cpp
        cxtestProfileChange.cpp
        cxtestClippersWidget.cpp
        cxtestClipperWidget.cpp
        cxtestSelectClippersForDataWidget.cpp
        cxtestSessionStorageHelper.h
        cxtestSessionStorageHelper.cpp
        cxtestMetricsWidget.cpp
    )

    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtestGui ${CXTEST_GUI_SOURCES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtestGui)
    target_include_directories(cxtestGui
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtestGui
        PRIVATE
        cxGui
        cxtestResourceVisualization
        cxResourceVisualization
        cxGrabber
        cxtestUtilities
        cxtestResource
        ${OPENCL_LIBRARIES}
        cxLogicManager
        cxResourceWidgets
        cxGUIExtenderService
        cxPluginUtilities
        cxCatch
        cxtest_org_custusx_core_patientmodel
    )
    cx_add_tests_to_catch(cxtestGui)

endif(BUILD_TESTING)
