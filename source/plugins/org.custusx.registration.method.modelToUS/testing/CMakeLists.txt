
if (BUILD_TESTING)
    include_directories(
    )


    set(CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_MOC_SOURCE_FILES
    )
	set(CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_SOURCE_FILES
		${CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_MOC_SOURCE_FILES}
		cxtestRegistrationMethodModelToUS.cpp
		)

    cx_catch_add_lib_and_exe(org_custusx_registration_method_modelToUS
        "${CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_SOURCE_FILES}" 
        "${CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_MOC_SOURCE_FILES}")

endif (BUILD_TESTING)
