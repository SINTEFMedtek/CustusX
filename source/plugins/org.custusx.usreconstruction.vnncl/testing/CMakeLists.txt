
if(BUILD_TESTING)

    set(CX_TEST_CATCH_ORG_CUSTUSX_VNNCLRECONSTRUCTION_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_ORG_CUSTUSX_VNNCLRECONSTRUCTION_SOURCE_FILES
        cxtestVNNclReconstructionService.cpp
        cxtestVNNclFixture.h
        cxtestVNNclFixture.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ORG_CUSTUSX_VNNCLRECONSTRUCTION_MOC_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_VNNCLRECONSTRUCTION_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_usreconstruction_vnncl ${CX_TEST_CATCH_ORG_CUSTUSX_VNNCLRECONSTRUCTION_SOURCE_FILES} ${CX_TEST_CATCH_ORG_CUSTUSX_VNNCLRECONSTRUCTION_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_usreconstruction_vnncl)
    target_include_directories(cxtest_org_custusx_usreconstruction_vnncl
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
	target_link_libraries(cxtest_org_custusx_usreconstruction_vnncl
		PRIVATE
		org_custusx_usreconstruction_vnncl
		cxtest_org_custusx_usreconstruction cxtestUtilities cxCatch
		cxLogicManager)
    cx_add_tests_to_catch(cxtest_org_custusx_usreconstruction_vnncl)

endif(BUILD_TESTING)
