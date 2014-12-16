
if(BUILD_TESTING)
    cx_add_class(CXTEST_SOURCES ${CXTEST_SOURCES}
        cxtestTSFParameters.cpp
        cxtestTSFFixture
        cxtestTSF.cpp
        cxtestTSFPresetsFixture
        cxtestTSFPresets.cpp
    )
    set(CXTEST_SOURCES_TO_MOC
    )

    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtest_org_custusx_filter_tubesegmentation ${CXTEST_SOURCES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_filter_tubesegmentation)
    target_include_directories(cxtest_org_custusx_filter_tubesegmentation
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_filter_tubesegmentation
        PRIVATE
        cxResource
        cxResourceFilter
        cxLogicManager
        cxGui
        tubeSegmentationLib
        SIPL
        org_custusx_filter_tubesegmentation
        org_custusx_core_patientmodel
        cxCatch
    )
    cx_add_tests_to_catch(cxtest_org_custusx_filter_tubesegmentation)

endif(BUILD_TESTING)
