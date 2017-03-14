
if(BUILD_TESTING)

    set(CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_METHOD_MANUAL_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_METHOD_MANUAL_SOURCE_FILES
        cxtestRegistrationMethodManual.cpp
        cxtestExportDummyClassForLinkingOnWindowsInLibWithoutExportedClass.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_METHOD_MANUAL_MOC_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_METHOD_MANUAL_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_registration_method_manual ${CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_METHOD_MANUAL_SOURCE_FILES} ${CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_METHOD_MANUAL_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_registration_method_manual)
    target_include_directories(cxtest_org_custusx_registration_method_manual
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_registration_method_manual
        PRIVATE
        cxLogicManager
        org_custusx_registration_method_manual
        cxCatch
        cxtestUtilities
        )
    cx_add_tests_to_catch(cxtest_org_custusx_registration_method_manual)

endif(BUILD_TESTING)
