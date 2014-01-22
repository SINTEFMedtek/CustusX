
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
        cxtestImageParameters.cpp
		cxtestCatchImageAlgorithms.cpp
		cxtestCatchProcessWrapper.cpp
		cxtestProcessWrapperFixture.h
		cxtestProcessWrapperFixture.cpp
		cxtestCatchVLCRecorder.cpp
		cxtestVLCRecorderFixture.h
		cxtestVLCRecorderFixture.cpp
		cxtestProbeDefinition.cpp
		cxtestOpenCLPrinter.cpp
		cxtestOpenCLUtilities.cpp
		)

    cx_catch_add_lib_and_exe(cxResource "${RESOURCE_TEST_CATCH_SOURCE_FILES}" "${CXTEST_SOURCES_TO_MOC}")

    
endif (BUILD_TESTING)
