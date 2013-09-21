
if (BUILD_TESTING)
	QT4_WRAP_CPP(RESOURCE_TEST_SOURCE_FILES
	)
  
	set(RESOURCE_TEST_SOURCE_FILES
        sscTestSliceComputer.cpp
        sscTestUtilityClasses.cpp
        sscTestTransform3D.cpp
        sscTestBoundingBox3D.cpp
		cxTest_ImageAlgorithms.h
		cxTest_ImageAlgorithms.cpp
		cxTestUSReconstructionFileIO.h
		cxTestUSReconstructionFileIO.cpp
		${RESOURCE_TEST_SOURCE_FILES}
		)
 
	include(cxTestMacros)
	SSC_ADD_CTEST_CPPUNIT_TESTS(cxTestResource cxCppUnitMain.cpp "${RESOURCE_TEST_SOURCE_FILES}" cxResource)
	
	#Catch
        set(RESOURCE_TEST_CATCH_SOURCE_FILES
            cxtestCatchStringHelpers.cpp
            )
        cx_catch_add_lib_and_exe(cxResource "${RESOURCE_TEST_CATCH_SOURCE_FILES}")
    
endif (BUILD_TESTING)
