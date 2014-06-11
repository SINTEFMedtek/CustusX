
if (BUILD_TESTING)
    include_directories(
    ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation
    )


    set(CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_MOC_SOURCE_FILES
    )
	set(CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_SOURCE_FILES
		${CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_MOC_SOURCE_FILES}
		cxtestUSSimulatorPlugin.cpp
		cxtestSimulatedImageStreamerFixture.h
		cxtestSimulatedImageStreamerFixture.cpp
		)

  set(CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_SOURCE_FILES
    ${CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_SOURCE_FILES}
    ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/ImageSimulator.h
    ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/ImageSimulator.cpp
    ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/ImageArray.h
    ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/ImageArray.cpp
    )

    cx_catch_add_lib_and_exe(org_custusx_ussimulator
        "${CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_SOURCE_FILES}" 
        "${CX_TEST_CATCH_ORG_CUSTUSX_EXAMPLE_MOC_SOURCE_FILES}")

endif (BUILD_TESTING)
