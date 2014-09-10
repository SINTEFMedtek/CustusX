
if (BUILD_TESTING)
    include_directories(
    )


    set(CX_TEST_CATCH_LOGIC_MOC_SOURCE_FILES
    )
	set(CX_TEST_CATCH_LOGIC_SOURCE_FILES
		${CX_TEST_CATCH_LOGIC_MOC_SOURCE_FILES}
		cxtestPlugins.cpp
		cxtestStreamerService.cpp
		cxtestRegistrationServiceProxy.cpp
		)

    cx_catch_add_lib_and_exe(cxLogicManager 
        "${CX_TEST_CATCH_LOGIC_SOURCE_FILES}" 
        "${CX_TEST_CATCH_LOGIC_MOC_SOURCE_FILES}")

endif (BUILD_TESTING)
