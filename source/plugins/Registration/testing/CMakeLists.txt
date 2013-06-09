cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(BUILD_TESTING)
  include_directories(
	../logic
	..
	${CustusX3_SOURCE_DIR}/source/resource
	${CustusX3_SOURCE_DIR}/source/service/patient
	${CustusX3_SOURCE_DIR}/source/service/tracking
	${CustusX3_SOURCE_DIR}/source/resource/settings
	${CustusX3_SOURCE_DIR}/source/resource/utilities
	${CustusX3_SOURCE_DIR}/source/resource/algorithms
	)
endif(BUILD_TESTING)

if(BUILD_TESTING)
	# Catch unit test framework stuff:

	QT4_WRAP_CPP(CXTEST_PLUGINREGISTRATION_SOURCES
		cxElastixSingleThreadedRunner.h
		)
	set(CXTEST_PLUGINREGISTRATION_SOURCES ${CXTEST_PLUGINREGISTRATION_SOURCES}
#		cxElastixSingleThreadedRunner.h
		cxElastixSingleThreadedRunner.cpp
		cxtestCatchElastix.cpp)

	cx_catch_add_lib_and_exe(cxPluginRegistration "${CXTEST_PLUGINREGISTRATION_SOURCES}")

endif(BUILD_TESTING)

if (BUILD_TESTING)
  # -----------------------------------------------
  # CppUnit test framework: Create executable testCX
  #
  include_directories( 
    ${CPPUNIT_INCLUDE_DIR}     
    )
      
  # List of all test units
  QT4_WRAP_CPP( CX_TEST_REGISTRATION_SOURCE_FILES
#  cxElastixSingleThreadedRunner.h
   )
  
  set(CX_TEST_REGISTRATION_SOURCE_FILES
  ${CX_TEST_REGISTRATION_SOURCE_FILES}
  cxTest_RegistrationV2V.h
  cxTest_RegistrationV2V.cpp
#  cxTest_RegistrationElastix.h
#  cxTest_RegistrationElastix.cpp
#  cxElastixSingleThreadedRunner.h
#  cxElastixSingleThreadedRunner.cpp
  ${CX_TEST_SOURCE_FILES}
  )
  
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/CMake)
	include(cxTestMacros)
	# define cppunit tests and ctest tests from same source:
	SSC_ADD_CTEST_CPPUNIT_TESTS(cxTestRegistration cxCppUnitMain.cpp "${CX_TEST_REGISTRATION_SOURCE_FILES}" cxPluginRegistration)

endif (BUILD_TESTING)
