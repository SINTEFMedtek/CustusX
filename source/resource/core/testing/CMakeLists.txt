
if (BUILD_TESTING)
	QT4_WRAP_CPP(RESOURCE_TEST_SOURCE_FILES
	)
  
	set(RESOURCE_TEST_SOURCE_FILES
		cxTest_ImageAlgorithms.h
		cxTest_ImageAlgorithms.cpp
		${RESOURCE_TEST_SOURCE_FILES}
		)
 
	include(cxTestMacros)
	SSC_ADD_CTEST_CPPUNIT_TESTS(cxTestResource cxCppUnitMain.cpp "${RESOURCE_TEST_SOURCE_FILES}" cxResource)
	
	#Catch
        set(RESOURCE_TEST_CATCH_SOURCE_FILES
            cxtestCatchStringHelpers.cpp
			cxtestCatchSliceComputer.cpp
			cxtestCatchBoundingBox3D.cpp
			cxtestCatchFrame.cpp
			cxtestCatchSharedMemory.cpp
			cxtestCatchTransform3D.cpp
			cxtestCatchVector3D.cpp
			)
        cx_catch_add_lib_and_exe(cxResource "${RESOURCE_TEST_CATCH_SOURCE_FILES}")
    
endif (BUILD_TESTING)
