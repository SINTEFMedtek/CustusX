
if (BUILD_TESTING)
    include_directories(
        ${CustusX_SOURCE_DIR}/source/logic
    )


    set(CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_MOC_SOURCE_FILES
    )
	set(CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_SOURCE_FILES
		${CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_MOC_SOURCE_FILES}
		cxtestRegistrationPlugin.cpp
		cxtestRegistrationApplicator.cpp
		cxtestSeansVesselRegFixture.h
		cxtestSeansVesselRegFixture.cpp
		cxtestCatchSeansVesselReg.cpp
		)

    cx_catch_add_lib_and_exe(org_custusx_registration
	"${CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_SOURCE_FILES}"
	"${CX_TEST_CATCH_ORG_CUSTUSX_REGISTRATION_MOC_SOURCE_FILES}")

endif (BUILD_TESTING)
