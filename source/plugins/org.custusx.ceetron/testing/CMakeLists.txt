
if(BUILD_TESTING)
    set(CX_TEST_CATCH_ORG_CUSTUSX_CEETRON_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_ORG_CUSTUSX_CEETRON_SOURCE_FILES
        ${CX_TEST_CATCH_ORG_CUSTUSX_CEETRON_MOC_SOURCE_FILES}
        cxtestCeetronPlugin.cpp
        cxtestCgeoTestFunctions.h
        cxtestExportDummyClassForLinkingOnWindowsInLibWithoutExportedClass.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ORG_CUSTUSX_CEETRON_MOC_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_CEETRON_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_ceetron ${CX_TEST_CATCH_ORG_CUSTUSX_CEETRON_SOURCE_FILES} ${CX_TEST_CATCH_ORG_CUSTUSX_CEETRON_MOC_SOURCE_FILES})

    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_ceetron)
    target_include_directories(cxtest_org_custusx_ceetron
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_ceetron
        PRIVATE
        org_custusx_ceetron
        cxResource
        cxtestUtilities
        cxResourceVisualization
        cxCatch)
    cx_add_tests_to_catch(cxtest_org_custusx_ceetron)

endif(BUILD_TESTING)
