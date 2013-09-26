cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(BUILD_TESTING)
  include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
  include_directories(
	../logic
	..
	${CustusX3_SOURCE_DIR}/source/service/patient
	${CustusX3_SOURCE_DIR}/source/service/tracking
	)

	QT4_WRAP_CPP(CXTEST_PLUGINREGISTRATION_SOURCES
		cxElastixSingleThreadedRunner.h
		)
	set(CXTEST_PLUGINREGISTRATION_SOURCES ${CXTEST_PLUGINREGISTRATION_SOURCES}
		cxElastixSingleThreadedRunner.h
		cxElastixSingleThreadedRunner.cpp
		cxtestCatchElastix.cpp
		cxtestSeansVesselRegFixture.h
		cxtestSeansVesselRegFixture.cpp
		cxtestCatchSeansVesselReg.cpp
	)

	cx_catch_add_lib_and_exe(cxPluginRegistration "${CXTEST_PLUGINREGISTRATION_SOURCES}")

endif(BUILD_TESTING)