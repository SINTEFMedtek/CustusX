
if(BUILD_TESTING)
    set(CXTEST_SOURCES_TO_MOC
    cxtestFastPatientRegistrationWidgetFixture.h
    )
    set(CXTEST_PLUGINREGISTRATION_SOURCES ${CXTEST_PLUGINREGISTRATION_SOURCES}
        cxtestFastPatientRegistration.cpp
        cxtestFastPatientRegistrationWidgetFixture.cpp
        cxtestLandmarkPatientRegistration.cpp
    )

    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtest_org_custusx_registration_method_landmark ${CXTEST_PLUGINREGISTRATION_SOURCES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_registration_method_landmark)
    target_include_directories(cxtest_org_custusx_registration_method_landmark
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_registration_method_landmark
        PRIVATE
        org_custusx_registration_method_landmark
        org_custusx_core_view
        cxtest_org_custusx_core_view
        cxtestUtilities
        cxtestResource
        cxtestResourceVisualization
        cxCatch
        cxLogicManager
        cxtest_org_custusx_core_patientmodel
    )
    cx_add_tests_to_catch(cxtest_org_custusx_registration_method_landmark)

endif(BUILD_TESTING)
