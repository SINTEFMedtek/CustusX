
if(BUILD_TESTING)
    cx_add_class(CXTEST_SOURCES ${CXTEST_SOURCES}
        cxtestBranchHandling.cpp
        cxtestExportDummyClassForLinkingOnWindowsInLibWithoutExportedClass.cpp
    )
    set(CXTEST_SOURCES_TO_MOC
    )

    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtest_org_custusx_registration_method_bronchoscopy ${CXTEST_SOURCES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_registration_method_bronchoscopy)
    target_include_directories(cxtest_org_custusx_registration_method_bronchoscopy
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_registration_method_bronchoscopy
        PRIVATE
        cxCatch
        cxtestUtilities
        org_custusx_registration_method_bronchoscopy
    )
    cx_add_tests_to_catch(cxtest_org_custusx_registration_method_bronchoscopy)

endif(BUILD_TESTING)
