cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_TESTING)
	include_directories(
            ${CustusX3_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
            ${CustusX3_SOURCE_DIR}/source/resource/videoServer
	)

    cx_add_class_qt_moc( CX_TEST_SOURCE_FILES
        	cxTestSenderController.h
	)

    cx_add_class(CX_TEST_SOURCE_FILES
            cxTestGEInterface
            cxTestSenderController
            cxtestCatchGEInterface.cpp
        	${CX_TEST_SOURCE_FILES}
	)

    cx_catch_add_lib_and_exe(cxGrabber "${CX_TEST_SOURCE_FILES}")
   
if(CX_USE_ISB_GE AND NOT CX_WINDOWS)
    include(cxTestMacros)

	# Add the tests from the GEStreamer lib
    set(GEStreamer_TEST_SOURCE_FILES
		${GEStreamer_SOURCE_DIR}/Testing/TestGEInterface.h
		${GEStreamer_SOURCE_DIR}/Testing/TestGEInterface.cpp
		)
	SSC_ADD_CTEST_CPPUNIT_TESTS(TestGEInterface 
	    "${GEStreamer_SOURCE_DIR}/Testing/CppUnitMain.cpp"
	    "${GEStreamer_TEST_SOURCE_FILES}" 
	    "${GEStreamer_LIBRARIES}")
endif()

endif (BUILD_TESTING)
