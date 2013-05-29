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
    cxTestVisualization.cpp
    cxTestVisualization.h
    cxTestRenderSpeed.h
    cxTestRenderSpeed.cpp
  ${SOURCE_FILES}
  ${CX_TEST_SOURCE_FILES}
  )

	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/CMake)
	include(cxTestMacros)
	# define cppunit tests and ctest tests from same source:
	SSC_ADD_CTEST_CPPUNIT_TESTS(cxTestVisualization cxCppUnitMain.cpp "${SOURCE_FILES}" cxVisualizationService)
endif (BUILD_TESTING)
