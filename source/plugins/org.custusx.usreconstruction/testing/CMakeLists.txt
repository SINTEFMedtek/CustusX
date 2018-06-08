
if(BUILD_TESTING)
    set(CXTEST_SOURCES_TO_MOC
    )
    set(CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES
        cxtestReconstructionManagerFixture.h
        cxtestReconstructionManagerFixture.cpp
        cxtestReconstructionManager.cpp
        cxtestReconstructionAlgorithmFixture.h
        cxtestReconstructionAlgorithmFixture.cpp
        cxtestReconstructRealData.h
        cxtestReconstructRealData.cpp
        cxtestPositionFilter.cpp
    )
    
    qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
    add_library(cxtest_org_custusx_usreconstruction ${CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES} ${CXTEST_SOURCES_TO_MOC})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_usreconstruction)
    target_include_directories(cxtest_org_custusx_usreconstruction
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
	target_link_libraries(cxtest_org_custusx_usreconstruction
		PRIVATE
		org_custusx_usreconstruction
		cxtestUtilities cxCatch cxLogicManager)
    cx_add_tests_to_catch(cxtest_org_custusx_usreconstruction)

endif(BUILD_TESTING)
