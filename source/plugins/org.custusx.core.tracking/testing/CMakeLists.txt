
if(BUILD_TESTING)

    set(CX_TEST_CATCH_org_custusx_core_tracking_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_org_custusx_core_tracking_SOURCE_FILES
        cxtestTrackingPlugin.cpp
        cxtestToolFiles.cpp
        cxtestTestToolMesh.h
        cxtestTestToolMesh.cpp
        cxtestCatchTrackingService.cpp
        cxtestCatchProbe.cpp
        )

    qt5_wrap_cpp(CX_TEST_CATCH_org_custusx_core_tracking_MOC_SOURCE_FILES ${CX_TEST_CATCH_org_custusx_core_tracking_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_core_tracking ${CX_TEST_CATCH_org_custusx_core_tracking_SOURCE_FILES} ${CX_TEST_CATCH_org_custusx_core_tracking_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_core_tracking)
    target_include_directories(cxtest_org_custusx_core_tracking
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(
        cxtest_org_custusx_core_tracking
        PRIVATE
        cxtestUtilities
        cxResource
        org_custusx_core_tracking
        cxCatch
    )
    cx_add_tests_to_catch(cxtest_org_custusx_core_tracking)
        
endif(BUILD_TESTING)
