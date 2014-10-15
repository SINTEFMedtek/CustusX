
if (BUILD_TESTING)
    include_directories(
    )


    set(CX_TEST_CATCH_org_custusx_core_tracking_MOC_SOURCE_FILES
    )
	set(CX_TEST_CATCH_org_custusx_core_tracking_SOURCE_FILES
		${CX_TEST_CATCH_org_custusx_core_tracking_MOC_SOURCE_FILES}
		cxtestTrackingPlugin.cpp
		)

    cx_catch_add_lib_and_exe(org_custusx_core_tracking 
        "${CX_TEST_CATCH_org_custusx_core_tracking_SOURCE_FILES}" 
        "${CX_TEST_CATCH_org_custusx_core_tracking_MOC_SOURCE_FILES}")

endif (BUILD_TESTING)
