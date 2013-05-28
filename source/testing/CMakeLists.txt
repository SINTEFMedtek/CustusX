cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Custus unit test folder.
#
# Contains examples of unit tests, using both ctest and cppunit and both
# =========================================================

if (BUILD_TESTING)

#
# Build a library with testing functionality
#
set( cxtest_moc_files
    cxtestSignalListener.h 
	cxtestUtilities.h
)

set (cxtestUtilities_source
	cxtestUtilities.h
	cxtestUtilities.cpp
    cxtestSignalListener.h
    cxtestSignalListener.cpp
)

include_directories(
    .
    )

QT4_WRAP_CPP( cxtest_moced_files ${cxtest_moc_files} )

add_library( cxtestUtilities ${cxtestUtilities_source} ${cxtest_moced_files} )
target_link_libraries(cxtestUtilities SSC)


####################################################################################
    # CATCH -start
    
    #------------------------------------------------------------------------------
    # Set variables
    #-----------------------------------------------------------------------------
    QT4_WRAP_CPP( CATCH_MOCED_SOURCES
            ../gui/testing/cxtestTestVideoConnectionWidget.h
        )
    set(CATCH_SOURCES 
        ${CATCH_MOCED_SOURCES}
        cxCatch.cpp
        ../plugins/Algorithm/testing/cxCatchAlgorithm.cpp
        ../service/video/testing/cxCatchVideo.cpp
        ../gui/testing/cxtestCatchStreamingWidgets.cpp
        ../gui/testing/cxtestTestVideoConnectionWidget.h
        ../gui/testing/cxtestTestVideoConnectionWidget.cpp
    )
    
    #------------------------------------------------------------------------------
    # Includes
    #-----------------------------------------------------------------------------
    include_directories(
		.
		${CustusX3_SOURCE_DIR}/source/gui/testing
		${CustusX3_SOURCE_DIR}/source/gui/utilities
		${CustusX3_SOURCE_DIR}/source/gui/streamingWidgets
		${CustusX3_SOURCE_DIR}/source/plugins/Algorithm/logic
		${CustusX3_SOURCE_DIR}/source/services/video
		${CustusX3_SOURCE_DIR}/source/resource/videoServer
		${CustusX3_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
        ${CustusX3_SOURCE_DIR}/source/resource/videoServer/testing
		)
    #------------------------------------------------------------------------------
    # Executable
    #-----------------------------------------------------------------------------
    if (APPLE)
    	add_executable(Catch MACOSX_BUNDLE ${CATCH_SOURCES} )
    else (APPLE)
    	if(CX_WINDOWS)
    		add_executable(Catch WIN32 ${CATCH_SOURCES} )
    	else(CX_WINDOWS)
    		add_executable(Catch ${CATCH_SOURCES} )
    	endif(CX_WINDOWS)
    endif (APPLE)
    
    #------------------------------------------------------------------------------
    # Linking
    #------------------------------------------------------------------------------
    set( CATCH_LIBS
        cxPluginAlgorithm
        cxGui
        cxtestUtilities
    )
    if (CX_USE_TSF)
        set( CATCH_LIBS
            ${CATCH_LIBS}
            ${Tube-Segmentation-Framework_LIBRARIES}
        )
    endif()
    
    target_link_libraries( Catch ${CATCH_LIBS} )
    		
	# CATCH -end
####################################################################################



include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource

    ${CustusX3_SOURCE_DIR}/source/service/patient     
    ${CustusX3_SOURCE_DIR}/source/service/tracking     
    ${CustusX3_SOURCE_DIR}/source/service/visualization
    ${CustusX3_SOURCE_DIR}/source/service/video
    ${CustusX3_SOURCE_DIR}/source/service/state

    ${CustusX3_SOURCE_DIR}/source/logic
    ${CustusX3_SOURCE_DIR}/source/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase
    )    
    # -----------------------------------------------
    # CppUnit test framework: Create executable testCX
    #
    include_directories( ${CPPUNIT_INCLUDE_DIR})
    
    # List of all test units
    QT4_WRAP_CPP( CX_TEST_SOURCE_FILES
        cxTestCustusXController.h 
        )
    set(CX_TEST_SOURCE_FILES
        cxTestSpeed.cpp
        cxTestSpeed.h
        cxTestCustusXController.cpp
        cxTestCustusXController.h
        ${CX_TEST_SOURCE_FILES}
        )
        
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SSC_SOURCE_DIR}/CMake)
	include(cxTestMacros)
	# define cppunit tests and ctest tests from same source:
	SSC_ADD_CTEST_CPPUNIT_TESTS(testCX cxCppUnitMain.cpp "${CX_TEST_SOURCE_FILES}" cxGui)
endif (BUILD_TESTING)
