
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
		cxtestSpaceProviderMock.h
		cxtestSpaceProviderMock.cpp
		cxtestSpaceListenerMock.h
		cxtestSpaceListenerMock.cpp
		)
	if (CX_USE_OPENCL_UTILITY)
	   	set(RESOURCE_TEST_CATCH_SOURCE_FILES
	   	    ${RESOURCE_TEST_CATCH_SOURCE_FILES}
    		cxtestOpenCLPrinter.cpp
		) 

		set(OUL_RESOURCE_TEST_CATCH_SOURCE_FILES
            ${OpenCLUtilityLibrary_SOURCE_DIR}/testing/tests.cpp
            ${OpenCLUtilityLibrary_SOURCE_DIR}/testing/histogramPyramidTests.cpp
            )	
        cx_catch_add_lib_and_exe(OpenCLUtilityLibrary "${OUL_RESOURCE_TEST_CATCH_SOURCE_FILES}" "" "TestOpenCLUtilityLibrary")
	endif(CX_USE_OPENCL_UTILITY)

    cx_catch_add_lib_and_exe(cxResource "${RESOURCE_TEST_CATCH_SOURCE_FILES}" "${CXTEST_SOURCES_TO_MOC}")

    
endif (BUILD_TESTING)
