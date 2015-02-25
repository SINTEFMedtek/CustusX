
if(BUILD_TESTING)

    set(CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_MOC_SOURCE_FILES
    )
    set(CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_SOURCE_FILES
        cxtestRegistrationPlugin.cpp
        cxtestRegistrationApplicator.cpp
        cxtestSeansVesselRegFixture.h
        cxtestSeansVesselRegFixture.cpp
        cxtestCatchSeansVesselReg.cpp
        cxtestRegistrationServiceProxy.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_MOC_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_registration ${CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_SOURCE_FILES} ${CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_registration)
    target_include_directories(cxtest_org_custusx_registration
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
	target_link_libraries(cxtest_org_custusx_registration
		PRIVATE
		org_custusx_registration
		cxLogicManager
		cxtestUtilities cxtestResource cxCatch)
    cx_add_tests_to_catch(cxtest_org_custusx_registration)

endif(BUILD_TESTING)
