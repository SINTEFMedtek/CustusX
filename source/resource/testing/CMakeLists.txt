cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_TESTING)
	include_directories(
		${CPPUNIT_INCLUDE_DIR}
		${CustusX3_SOURCE_DIR}/source/resource
		${CustusX3_SOURCE_DIR}/source/resource/settings
		${CustusX3_SOURCE_DIR}/source/resource/utilities
		${CustusX3_SOURCE_DIR}/source/resource/algorithms
        ${CustusX3_SOURCE_DIR}/source/logic
		)
      
	# List of all test units
	QT4_WRAP_CPP( RESOURCE_TEST_SOURCE_FILES
		)
  
	set(RESOURCE_TEST_SOURCE_FILES
		cxTest_ImageAlgorithms.h
		cxTest_ImageAlgorithms.cpp
		cxTestUSReconstructionFileIO.h
		cxTestUSReconstructionFileIO.cpp
		${RESOURCE_TEST_SOURCE_FILES}
		)
  
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/CMake)
	include(cxTestMacros)
	# define cppunit tests and ctest tests from same source:
	SSC_ADD_CTEST_CPPUNIT_TESTS(cxTestResource cxCppUnitMain.cpp "${RESOURCE_TEST_SOURCE_FILES}" cxResource)
	
	
	#Catch
	set(RESOURCE_TEST_CATCH_SOURCE_FILES
	    cxtestCatchFrameMetric.cpp
	    cxtestFrameMetricFixture.h
	    cxtestFrameMetricFixture.cpp
	)
	cx_catch_add_lib_and_exe(cxResource "${RESOURCE_TEST_CATCH_SOURCE_FILES}")
    
endif (BUILD_TESTING)
