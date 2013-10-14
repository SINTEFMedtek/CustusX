
if (BUILD_TESTING)	

    set(CXTEST_SOURCES_TO_MOC
    )

    set(RESOURCE_TEST_CATCH_SOURCE_FILES
        cxtestCatchStringHelpers.cpp
		cxtestCatchSliceComputer.cpp
		cxtestCatchBoundingBox3D.cpp
		cxtestCatchFrame.cpp
		cxtestCatchSharedMemory.cpp
		cxtestCatchTransform3D.cpp
		cxtestCatchVector3D.cpp
		cxtestCatchImageAlgorithms.cpp
                cxtestImageParameters.cpp
		)

    cx_catch_add_lib_and_exe(cxResource "${RESOURCE_TEST_CATCH_SOURCE_FILES}" "${CXTEST_SOURCES_TO_MOC}")

    
endif (BUILD_TESTING)
