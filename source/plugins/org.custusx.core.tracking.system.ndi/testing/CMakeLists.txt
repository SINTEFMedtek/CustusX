if(BUILD_TESTING)

    set(CXTEST_SOURCES
        cxtestNdiFakeDevice.h
        cxtestNdiTool.cpp
        cxtestNdiTransform.cpp
        cxtestNdiToolSetup.cpp
        cxtestNdiTrackerManager.cpp
        cxtestNdiCapiDevice.cpp
        cxtestNdiPositionQuality.cpp
        cxtestExportDummyClassForLinkingOnWindowsInLibWithoutExportedClass.cpp
    )

    add_library(cxtest_org_custusx_core_tracking_system_ndi ${CXTEST_SOURCES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_core_tracking_system_ndi)

    target_include_directories(cxtest_org_custusx_core_tracking_system_ndi
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
        PRIVATE
        $<TARGET_PROPERTY:cx_ndicapi,INTERFACE_INCLUDE_DIRECTORIES>
    )

    target_link_libraries(cxtest_org_custusx_core_tracking_system_ndi
        PRIVATE
        org_custusx_core_tracking_system_ndi
        cxResource
        cxtestUtilities
        cxCatch
    )

    cx_add_tests_to_catch(cxtest_org_custusx_core_tracking_system_ndi)

endif(BUILD_TESTING)
