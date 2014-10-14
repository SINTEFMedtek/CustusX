
if (BUILD_TESTING)
    include_directories(
    )


    set(CX_TEST_CATCH_org_custusx_core_visualization_MOC_SOURCE_FILES
    )
	set(CX_TEST_CATCH_org_custusx_core_visualization_SOURCE_FILES
		${CX_TEST_CATCH_org_custusx_core_visualization_MOC_SOURCE_FILES}
		cxtestVisualizationPlugin.cpp
		)

    cx_catch_add_lib_and_exe(org_custusx_core_visualization 
        "${CX_TEST_CATCH_org_custusx_core_visualization_SOURCE_FILES}" 
        "${CX_TEST_CATCH_org_custusx_core_visualization_MOC_SOURCE_FILES}")

endif (BUILD_TESTING)
