
if (BUILD_TESTING)
		include_directories(
		${CustusX_SOURCE_DIR}/source/logic/
		${CustusX_SOURCE_DIR}/source/service/state
		)


    set(CX_TEST_CATCH_ACQUISITION_MOC_SOURCE_FILES
		cxtestUSSavingRecorderFixture.h
		cxtestAcquisitionFixture.h
    )
	set(CX_TEST_CATCH_ACQUISITION_SOURCE_FILES
		${CX_TEST_CATCH_ACQUISITION_SOURCE_FILES}
		cxtestUSSavingRecorder.cpp
		cxtestUSSavingRecorderFixture.cpp
		cxtestUSSavingRecorderFixture.h
		cxtestAcquisitionFixture.cpp
		cxtestAcquisitionFixture.h
		cxtestAcquisition.cpp
		)

	cx_catch_add_lib_and_exe(cxPluginAcquisition "${CX_TEST_CATCH_ACQUISITION_SOURCE_FILES}" "${CX_TEST_CATCH_ACQUISITION_MOC_SOURCE_FILES}")

endif (BUILD_TESTING)
