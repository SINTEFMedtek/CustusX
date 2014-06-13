
if (BUILD_TESTING)
    include_directories(
        ${CX_RESOURCE_INCLUDE_DIRECTORIES}
        ${CustusX3_BINARY_DIR}/source/resource/core/utilities/
        ${CustusX3_SOURCE_DIR}/source/modules/UsReconstruction/testing
		${CustusX3_SOURCE_DIR}/source/modules/UsReconstruction/logic
		${CustusX3_SOURCE_DIR}/source/logic
		${CustusX3_SOURCE_DIR}/source/resource/pluginUtilities
		${CustusX3_SOURCE_DIR}/source/resource/testUtilities
		${CustusX3_SOURCE_DIR}/source/resource/core/dataAdapters
    )


    set(CX_TEST_CATCH_ORG_CUSTUSX_PNNRECONSTRUCTION_MOC_SOURCE_FILES
    )
	set(CX_TEST_CATCH_ORG_CUSTUSX_PNNRECONSTRUCTION_SOURCE_FILES
		${CX_TEST_CATCH_ORG_CUSTUSX_PNNRECONSTRUCTION_MOC_SOURCE_FILES}
		cxtestPNNPlugin.cpp
		)

    cx_catch_add_lib_and_exe(org_custusx_pnnreconstruction 
        "${CX_TEST_CATCH_ORG_CUSTUSX_PNNRECONSTRUCTION_SOURCE_FILES}" 
        "${CX_TEST_CATCH_ORG_CUSTUSX_PNNRECONSTRUCTION_MOC_SOURCE_FILES}"
        cxtestCatchcxPluginUSReconstruction #TODO remove...
        )

endif (BUILD_TESTING)
