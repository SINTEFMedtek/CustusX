
if (BUILD_TESTING)
    include_directories(
    )


    set(CX_TEST_CATCH_org_custusx_core_video_MOC_SOURCE_FILES
        cxtestTestVideoConnectionWidget.h
    )
	set(CX_TEST_CATCH_org_custusx_core_video_SOURCE_FILES
		${CX_TEST_CATCH_org_custusx_core_video_MOC_SOURCE_FILES}
		cxtestVideoPlugin.cpp
	    cxCatchVideo.cpp
        cxtestTestVideoConnectionWidget.cpp
        cxtestTestVideoConnectionWidget.h
		cxtestCatchStreamingWidgets.cpp
		)

    cx_catch_add_lib_and_exe(org_custusx_core_video 
        "${CX_TEST_CATCH_org_custusx_core_video_SOURCE_FILES}" 
        "${CX_TEST_CATCH_org_custusx_core_video_MOC_SOURCE_FILES}")

endif (BUILD_TESTING)
