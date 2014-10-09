
if(BUILD_TESTING)
  include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
  include_directories(
	../logic
	..
	${CustusX_SOURCE_DIR}/source/service/patient
	${CustusX_SOURCE_DIR}/source/service/tracking
	)

    set(CXTEST_SOURCES_TO_MOC
        cxElastixSingleThreadedRunner.h
    )
	set(CXTEST_PLUGINREGISTRATION_SOURCES ${CXTEST_PLUGINREGISTRATION_SOURCES}
		cxElastixSingleThreadedRunner.h
		cxElastixSingleThreadedRunner.cpp
		cxtestCatchElastix.cpp
	)

	cx_catch_add_lib_and_exe(org_custusx_registration_method_commandline "${CXTEST_PLUGINREGISTRATION_SOURCES}" "${CXTEST_SOURCES_TO_MOC}")

endif(BUILD_TESTING)
