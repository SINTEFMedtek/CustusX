cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_TESTING)
	cx_initialize_cppunit()

	include_directories(
            ${CustusX_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
            ${CustusX_SOURCE_DIR}/source/resource/videoServer
	)

	set(CXTEST_SOURCES_TO_MOC
    )

    cx_add_class(CX_TEST_SOURCE_FILES
            cxtestSonixProbeFileReader.cpp
	)

    cx_catch_add_lib_and_exe(cxGrabber "${CX_TEST_SOURCE_FILES}" "${CXTEST_SOURCES_TO_MOC}")

endif (BUILD_TESTING)
