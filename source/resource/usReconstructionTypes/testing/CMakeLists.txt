
if (BUILD_TESTING)  

	set(RESOURCE_TEST_SOURCE_FILES
		cxTestUSReconstructionFileIO.h
		cxTestUSReconstructionFileIO.cpp
		${RESOURCE_TEST_SOURCE_FILES}
		)
 
	include(cxTestMacros)
	SSC_ADD_CTEST_CPPUNIT_TESTS(cxTestResourceUSReconstructionTypes cxCppUnitMain.cpp "${RESOURCE_TEST_SOURCE_FILES}" cxResourceUsReconstructionTypes)
	    
endif (BUILD_TESTING)
