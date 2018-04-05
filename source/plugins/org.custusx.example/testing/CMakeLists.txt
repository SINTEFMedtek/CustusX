
if(BUILD_TESTING)
    set(CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_SOURCE_FILES
        ${CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_MOC_SOURCE_FILES}
        cxtestExamplePlugin.cpp
        cxtestExportDummyClassForLinkingOnWindowsInLibWithoutExportedClass.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_MOC_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_example ${CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_SOURCE_FILES} ${CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_example)
    target_include_directories(cxtest_org_custusx_example
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_example
      PRIVATE
      org_custusx_example
      cxCatch
      cxtestUtilities
      cxResource)
    cx_add_tests_to_catch(cxtest_org_custusx_example)

endif(BUILD_TESTING)
