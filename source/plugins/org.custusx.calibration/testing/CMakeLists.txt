if(BUILD_TESTING)

    set(CX_TEST_PLUGINCALIBRATION_SOURCE_FILES
        cxtestTemporalCalibration.cpp
        cxtestDummyCalibration.h
        cxtestDummyCalibration.cpp
        )

    #qt5_wrap_cpp(CX_TEST_MOC_SOURCE_FILES ${CX_TEST_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_calibration ${CX_TEST_PLUGINCALIBRATION_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_calibration)
    target_include_directories(cxtest_org_custusx_calibration
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_calibration PRIVATE org_custusx_calibration cxCatch)
    cx_add_tests_to_catch(cxtest_org_custusx_calibration)


endif(BUILD_TESTING)
