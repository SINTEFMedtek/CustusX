
if(BUILD_TESTING)
    set(CX_TEST_CATCH_ORG_CUSTUSX_LIVER_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_ORG_CUSTUSX_LIVER_SOURCE_FILES
        ${CX_TEST_CATCH_ORG_CUSTUSX_LIVER_MOC_SOURCE_FILES}
    )
    cx_add_class(CX_TEST_CATCH_ORG_CUSTUSX_LIVER_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_LIVER_SOURCE_FILES}
        cxtestLiverPlugin.cpp
        cxtestExportDummyClassForLinkingOnWindowsInLibWithoutExportedClass.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ORG_CUSTUSX_LIVER_MOC_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_LIVER_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_liver ${CX_TEST_CATCH_ORG_CUSTUSX_LIVER_SOURCE_FILES} ${CX_TEST_CATCH_ORG_CUSTUSX_LIVER_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_liver)
    target_include_directories(cxtest_org_custusx_liver
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_liver
      PRIVATE
      org_custusx_liver
      cxCatch
      cxtestUtilities
      cxtestResource
      cxResource
      cxResourceFilter
      cxResourceWidgets)
    cx_add_tests_to_catch(cxtest_org_custusx_liver)

endif(BUILD_TESTING)
