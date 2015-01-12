if(BUILD_TESTING)

    set(CX_TEST_PLUGINCALIBRATION_SOURCE_FILES
        cxtestTemporalCalibration.cpp
        cxtestDummyCalibration.h
        cxtestDummyCalibration.cpp
        )

    #qt5_wrap_cpp(CX_TEST_MOC_SOURCE_FILES ${CX_TEST_MOC_SOURCE_FILES})
    add_library(cxtestPluginCalibration ${CX_TEST_PLUGINCALIBRATION_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtestPluginCalibration)
    target_include_directories(cxtestPluginCalibration
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtestPluginCalibration PRIVATE cxPluginCalibration cxCatch)
    cx_add_tests_to_catch(cxtestPluginCalibration)


endif(BUILD_TESTING)
