
if(BUILD_TESTING)
    include_directories(
#        ${CX_RESOURCE_INCLUDE_DIRECTORIES}
 #       ${CustusX_BINARY_DIR}/source/resource/core/utilities/
#		${CustusX_SOURCE_DIR}/source/logic
#		${CustusX_SOURCE_DIR}/source/resource/pluginUtilities
#		${CustusX_SOURCE_DIR}/source/resource/testUtilities
#		${CustusX_SOURCE_DIR}/source/resource/core/dataAdapters

#	    ${CustusX_SOURCE_DIR}/source/logic/
#	    ${CustusX_SOURCE_DIR}/source/service/tracking/
	)

    cx_add_class(CXTEST_SOURCES ${CXTEST_SOURCES}
        cxCatchAlgorithm.cpp
        cxTestTubeSegmentationFramework
        cxtestCatchTubeSegmentationFramework.cpp
		cxtestTSFPresetsFixture
		cxtestCatchTSFPresets.cpp
    )
    set(CXTEST_SOURCES_TO_MOC
    )

	cx_catch_add_lib_and_exe(org_custusx_filter_tubesegmentation
        "${CXTEST_SOURCES}"
        "${CXTEST_SOURCES_TO_MOC}")

endif(BUILD_TESTING)
