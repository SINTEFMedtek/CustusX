
if(BUILD_TESTING)
    set(CX_TEST_CATCH_ORG_CUSTUSX_WEBSERVER_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_ORG_CUSTUSX_WEBSERVER_SOURCE_FILES
        ${CX_TEST_CATCH_ORG_CUSTUSX_WEBSERVER_MOC_SOURCE_FILES}
        cxtestWebServerPlugin.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ORG_CUSTUSX_WEBSERVER_MOC_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_WEBSERVER_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_webserver ${CX_TEST_CATCH_ORG_CUSTUSX_WEBSERVER_SOURCE_FILES} ${CX_TEST_CATCH_ORG_CUSTUSX_WEBSERVER_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_webserver)
    target_include_directories(cxtest_org_custusx_webserver
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_webserver PRIVATE org_custusx_webserver cxCatch)
    cx_add_tests_to_catch(cxtest_org_custusx_webserver)

endif(BUILD_TESTING)
