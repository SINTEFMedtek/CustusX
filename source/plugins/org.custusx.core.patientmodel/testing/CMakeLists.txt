
if(BUILD_TESTING)

    set(CX_TEST_CATCH_org_custusx_core_patientmodel_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_org_custusx_core_patientmodel_SOURCE_FILES
        cxtestPatientModelPlugin.cpp
        cxtestCatchPatientService.cpp
        cxtestCatchFrameMetric.cpp
        cxtestCatchToolMetric.cpp
        cxtestCatchDistanceMetric.cpp
        cxtestMetricFixture.cpp
        cxtestPatientStorage.cpp
        cxtestSessionStorageTestFixture.h
        cxtestSessionStorageTestFixture.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_org_custusx_core_patientmodel_MOC_SOURCE_FILES ${CX_TEST_CATCH_org_custusx_core_patientmodel_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_core_patientmodel ${CX_TEST_CATCH_org_custusx_core_patientmodel_SOURCE_FILES} ${CX_TEST_CATCH_org_custusx_core_patientmodel_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_core_patientmodel)
    target_include_directories(cxtest_org_custusx_core_patientmodel
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_core_patientmodel
        PRIVATE
        org_custusx_core_patientmodel
        cxLogicManager
        cxtestResource
        cxtestUtilities
        cxGui
        cxCatch)
    cx_add_tests_to_catch(cxtest_org_custusx_core_patientmodel)

endif(BUILD_TESTING)
