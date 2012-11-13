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
   )
  
  set(SOURCE_FILES
  cxTest_ImageAlgorithms.h
  cxTest_ImageAlgorithms.cpp
  ${CX_TEST_SOURCE_FILES}
  )
  
  # A CppUnit executable included for debugging purposes.
  # Not part of ctest
  add_executable(cxTestImageAlgorithms cxCppUnitMain.cpp ${SOURCE_FILES} )
  target_link_libraries(cxTestImageAlgorithms
    cxResource
    ${CPPUNIT_LIBRARIES})
  
  # -----------------------------------------------
  # -- CppUnit - CTest integration. ---------------
  # Use CISST_ADD_TESTS() to convert each CppUnit test to a CTest test. 
  
  # Initialize cisst routines
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/Testing/cisst_code)
  include(cisstMacros)
  
  # Link all CppUnit tests to the cisstTestsMain library
  add_executable(testImgAlg_CppUnit_CTest ${SOURCE_FILES} )
  target_link_libraries(testImgAlg_CppUnit_CTest 
    cisstTestsMain
    cxResource
    ${CPPUNIT_LIBRARIES}
    ${BOOST_LIBRARIES}
)
  # Generate a CTest list of tests
  CISST_ADD_TESTS(testImgAlg_CppUnit_CTest)
  
  # -- finished CppUnit - CTest integration. ------
  # -----------------------------------------------
endif (BUILD_TESTING)
