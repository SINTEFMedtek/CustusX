
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
        cxtestTSFParameters.cpp
        cxtestTSFFixture
        cxtestTSF.cpp
		cxtestTSFPresetsFixture
		cxtestTSFPresets.cpp
    )
    set(CXTEST_SOURCES_TO_MOC
    )

	cx_catch_add_lib_and_exe(org_custusx_filter_tubesegmentation
        "${CXTEST_SOURCES}"
        "${CXTEST_SOURCES_TO_MOC}" tubeSegmentationLib)
#        "LINK_PRIVATE tubeSegmentationLib")

#    LINK_PUBLIC
#    ${PLUGIN_target_libraries}   
#    cxPluginAlgorithm
#    LINK_PRIVATE tubeSegmentationLib

endif(BUILD_TESTING)
