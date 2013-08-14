# This file is part of SSC,
# a C++ Library supporting Image Guided Therapy Applications.
#
# Copyright (C) 2008- SINTEF Medical Technology
# Copyright (C) 2008- Sonowand AS
#
# SSC is owned by SINTEF Medical Technology and Sonowand AS,
# hereafter named the owners. Each particular piece of code
# is owned by the part that added it to the library.
# SSC source code and binaries can only be used by the owners
# and those with explicit permission from the owners.
# SSC shall not be distributed to anyone else.
#
# SSC is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.
#
# See sscLicense.txt for more information.


# Macro for defining a cppunit suite as both a standalone executable and
# a ctest-compatible set of tests.
#
# Controlled by
#   SSC_BUILD_PURE_CPPUNIT_TESTS : build the standalone exe (dont do this if doing coverage testing)
#   BUILD_TESTING : Turn off all generation
#
# Input:
#   TEST_NAME : Name of resulting cppunit executable. The corresponding ctest version will have a postfix.
#   CPPUNIT_MAIN : Name of the main function file of the classical cppunit project.
#   SOURCE_FILES : List of all source files. Remember to ad it in hyphens when calling function ( "${SOURCE_FILES}" )
#   LIBRARIES : List of all libraries. Remember to ad it in hyphens when calling function ( "${LIBRARIES}" )
#
macro(SSC_ADD_CTEST_CPPUNIT_TESTS TEST_NAME CPPUNIT_MAIN SOURCE_FILES LIBRARIES )
#	message("Test name:" ${TEST_NAME})
#	message("CPPUNIT_MAIN:" ${CPPUNIT_MAIN})
#	message("SOURCE_FILES:" ${SOURCE_FILES})
#	message("LIBRARIES:" ${LIBRARIES})

	if(BUILD_TESTING)
		# build the standalone cppunit executable:
		if(SSC_BUILD_PURE_CPPUNIT_TESTS AND NOT SSC_USE_GCOV)
			add_executable(${TEST_NAME}
				${CPPUNIT_MAIN}
				${SOURCE_FILES}
				)
			target_link_libraries(${TEST_NAME} ${LIBRARIES} ${CPPUNIT_LIBRARIES})
			# sonowand stuff
			set_target_properties(${TEST_NAME} PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "${SNW_COMPILE_FLAGS}")
		endif()

		# --------------- -------------------------------
		# -- CppUnit - CTest integration. ---------------
		# --   Use CISST_ADD_TESTS() to convert each CppUnit test to a CTest test.

		# Initialize cisst routines
		set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/Testing/cisst_code)
		include(cisstMacros)
		#add_subdirectory(cisst_code)

		# link all CppUnit tests to the cisstTestsMain library
		set(MERGE_CTEST_NAME ${TEST_NAME}_CppUnit_CTest)
		add_executable(${MERGE_CTEST_NAME} ${SOURCE_FILES} )
		set_target_properties(${MERGE_CTEST_NAME} PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "${SNW_COMPILE_FLAGS}")
		target_link_libraries(${MERGE_CTEST_NAME} cisstTestsMain ${LIBRARIES} ${CPPUNIT_LIBRARIES})
		# Generate a CTest list of tests
		CISST_ADD_TESTS(${MERGE_CTEST_NAME})

		# -- finished CppUnit - CTest integration. ------
		# -----------------------------------------------

	endif(BUILD_TESTING)
endmacro(SSC_ADD_CTEST_CPPUNIT_TESTS)
