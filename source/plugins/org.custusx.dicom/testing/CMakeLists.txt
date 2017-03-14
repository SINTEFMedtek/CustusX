
if(BUILD_TESTING)
    set(CX_TEST_CATCH_ORG_CUSTUSX_DICOM_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_ORG_CUSTUSX_DICOM_SOURCE_FILES
        cxtestDicomConverter.cpp
        cxtestExportDummyClassForLinkingOnWindowsInLibWithoutExportedClass.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ORG_CUSTUSX_DICOM_MOC_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_DICOM_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_dicom ${CX_TEST_CATCH_ORG_CUSTUSX_DICOM_SOURCE_FILES} ${CX_TEST_CATCH_ORG_CUSTUSX_DICOM_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_dicom)
    target_include_directories(cxtest_org_custusx_dicom
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_dicom
        PRIVATE
        org_custusx_dicom
        cxLogicManager
        cxCatch
        cxtestUtilities)
    cx_add_tests_to_catch(cxtest_org_custusx_dicom)

endif(BUILD_TESTING)
