cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_TESTING)
	include_directories(
		${CPPUNIT_INCLUDE_DIR}
		../logic
		../logic/3DReconstruction
		${CustusX3_SOURCE_DIR}/source/resource
		${CustusX3_SOURCE_DIR}/source/service/patient
		${CustusX3_SOURCE_DIR}/source/service/tracking
		${CustusX3_SOURCE_DIR}/source/resource/settings
		${CustusX3_SOURCE_DIR}/source/resource/utilities
		${CustusX3_SOURCE_DIR}/source/resource/algorithms
		)

	# Catch unit test framework stuff:

	QT4_WRAP_CPP(CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES
		)
	set(CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES ${CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES}
		cxtestReconstructManager.cpp
		)
	cx_catch_add_lib_and_exe(cxPluginUSReconstruction "${CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES}")

endif (BUILD_TESTING)
