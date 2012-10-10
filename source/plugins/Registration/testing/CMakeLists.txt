cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_TESTING)
  # -----------------------------------------------
  # CppUnit test framework: Create executable testCX
  #
  include_directories( 
    ${CPPUNIT_INCLUDE_DIR}     
    ../logic
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms
    )
      
  # List of all test units
  QT4_WRAP_CPP( SOURCE_FILES 
  cxElastixSingleThreadedRunner.h
   )
  
  set(SOURCE_FILES
  ${SOURCE_FILES}
  cxTest_RegistrationV2V.h
  cxTest_RegistrationV2V.cpp
  cxTest_RegistrationElastix.h
  cxTest_RegistrationElastix.cpp
  cxElastixSingleThreadedRunner.h
  cxElastixSingleThreadedRunner.cpp
  ${CX_TEST_SOURCE_FILES}
  )
  
  # A CppUnit executable included for debugging purposes.
  # Not part of ctest
  add_executable(cxTestRegistration cxCppUnitMain.cpp ${SOURCE_FILES} )
  target_link_libraries(cxTestRegistration
    cxPluginRegistrationLogic 
    ${CPPUNIT_LIBRARIES})
  
  # -----------------------------------------------
  # -- CppUnit - CTest integration. ---------------
  # Use CISST_ADD_TESTS() to convert each CppUnit test to a CTest test. 
  
  # Initialize cisst routines
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/Testing/cisst_code)
  include(cisstMacros)
  
  # Link all CppUnit tests to the cisstTestsMain library
  add_executable(testRegistration_CppUnit_CTest ${SOURCE_FILES} )
  target_link_libraries(testRegistration_CppUnit_CTest 
    cisstTestsMain
    cxPluginRegistrationLogic
    ${CPPUNIT_LIBRARIES}
    ${BOOST_LIBRARIES}
)
  # Generate a CTest list of tests
  CISST_ADD_TESTS(testRegistration_CppUnit_CTest)
  
  # -- finished CppUnit - CTest integration. ------
  # -----------------------------------------------
endif (BUILD_TESTING)
