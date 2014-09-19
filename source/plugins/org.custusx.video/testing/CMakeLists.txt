
if (BUILD_TESTING)
    include_directories(
    )


    set(CX_TEST_CATCH_ORG_CUSTUSX_VIDEO_MOC_SOURCE_FILES
    )
	set(CX_TEST_CATCH_ORG_CUSTUSX_VIDEO_SOURCE_FILES
		${CX_TEST_CATCH_ORG_CUSTUSX_VIDEO_MOC_SOURCE_FILES}
		cxtestVideoPlugin.cpp
		)

    cx_catch_add_lib_and_exe(org_custusx_video 
        "${CX_TEST_CATCH_ORG_CUSTUSX_VIDEO_SOURCE_FILES}" 
        "${CX_TEST_CATCH_ORG_CUSTUSX_VIDEO_MOC_SOURCE_FILES}")

endif (BUILD_TESTING)
