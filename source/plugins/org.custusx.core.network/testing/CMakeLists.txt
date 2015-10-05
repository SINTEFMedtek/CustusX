
if(BUILD_TESTING)
    set(CX_TEST_CATCH_ORG_CUSTUSX_CORE_NETWORK_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_ORG_CUSTUSX_CORE_NETWORK_SOURCE_FILES
        ${CX_TEST_CATCH_ORG_CUSTUSX_CORE_NETWORK_MOC_SOURCE_FILES}
        cxtestNetworkPlugin.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ORG_CUSTUSX_CORE_NETWORK_MOC_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_CORE_NETWORK_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_core_network ${CX_TEST_CATCH_ORG_CUSTUSX_CORE_NETWORK_SOURCE_FILES} ${CX_TEST_CATCH_ORG_CUSTUSX_CORE_NETWORK_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_core_network)
    target_include_directories(cxtest_org_custusx_core_network
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_core_network PRIVATE org_custusx_core_network cxCatch)
    cx_add_tests_to_catch(cxtest_org_custusx_core_network)

endif(BUILD_TESTING)
