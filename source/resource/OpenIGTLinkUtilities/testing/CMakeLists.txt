
if (BUILD_TESTING)
	include_directories("..")


		set(RESOURCE_OPENIGTLINKUTILITIES_TEST_CATCH_SOURCE_FILES
			cxtestCatchIGTLinkConversion.cpp
			)
		cx_catch_add_lib_and_exe(cxOpenIGTLinkUtilities "${RESOURCE_OPENIGTLINKUTILITIES_TEST_CATCH_SOURCE_FILES}" "")

endif (BUILD_TESTING)
