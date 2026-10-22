
if(BUILD_TESTING)

    set(CX_TEST_CATCH_org_custusx_bronchoscopynavigation_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_org_custusx_bronchoscopynavigation_SOURCE_FILES
        cxtestTrackingSystemBronchoscopyService.cpp
        cxtestExportDummyClassForLinkingOnWindowsInLibWithoutExportedClass.cpp
        )

    qt5_wrap_cpp(CX_TEST_CATCH_org_custusx_bronchoscopynavigation_MOC_SOURCE_FILES ${CX_TEST_CATCH_org_custusx_bronchoscopynavigation_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_bronchoscopynavigation ${CX_TEST_CATCH_org_custusx_bronchoscopynavigation_SOURCE_FILES} ${CX_TEST_CATCH_org_custusx_bronchoscopynavigation_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_bronchoscopynavigation)
    target_include_directories(cxtest_org_custusx_bronchoscopynavigation
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
        ${org_custusx_bronchoscopynavigation_BINARY_DIR}
    )
    target_link_libraries(
        cxtest_org_custusx_bronchoscopynavigation
        PRIVATE
        cxtestUtilities
        cxResource
        org_custusx_bronchoscopynavigation
        cxCatch
    )
    cx_add_tests_to_catch(cxtest_org_custusx_bronchoscopynavigation)

endif(BUILD_TESTING)
