
if(BUILD_TESTING)
    set(CX_TEST_CATCH_ACQUISITION_MOC_SOURCE_FILES
        cxtestUSSavingRecorderFixture.h
        cxtestAcquisitionFixture.h
    )
    set(CX_TEST_CATCH_ACQUISITION_SOURCE_FILES
        cxtestUSSavingRecorder.cpp
        cxtestUSSavingRecorderFixture.cpp
        cxtestUSSavingRecorderFixture.h
        cxtestAcquisitionFixture.cpp
        cxtestAcquisitionFixture.h
        cxtestAcquisition.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ACQUISITION_MOC_SOURCE_FILES ${CX_TEST_CATCH_ACQUISITION_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_acquisition ${CX_TEST_CATCH_ACQUISITION_SOURCE_FILES} ${CX_TEST_CATCH_ACQUISITION_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_acquisition)
    target_include_directories(cxtest_org_custusx_acquisition
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
	target_link_libraries(cxtest_org_custusx_acquisition
		PRIVATE
		org_custusx_acquisition
		org_custusx_core_video cxCatch
		cxLogicManager)
    cx_add_tests_to_catch(cxtest_org_custusx_acquisition)

endif(BUILD_TESTING)
