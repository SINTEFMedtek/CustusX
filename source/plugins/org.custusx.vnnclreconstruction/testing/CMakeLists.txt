
if (BUILD_TESTING)
    include_directories(
        ${CX_RESOURCE_INCLUDE_DIRECTORIES}
        ${CustusX_BINARY_DIR}/source/resource/core/utilities/
        ${CustusX_SOURCE_DIR}/source/modules/UsReconstruction/testing
		${CustusX_SOURCE_DIR}/source/modules/UsReconstruction/logic
		${CustusX_SOURCE_DIR}/source/logic
		${CustusX_SOURCE_DIR}/source/resource/pluginUtilities
		${CustusX_SOURCE_DIR}/source/resource/testUtilities
		${CustusX_SOURCE_DIR}/source/resource/core/dataAdapters
    )


    set(CX_TEST_CATCH_ORG_CUSTUSX_VNNCLRECONSTRUCTION_MOC_SOURCE_FILES
    )
	set(CX_TEST_CATCH_ORG_CUSTUSX_VNNCLRECONSTRUCTION_SOURCE_FILES
		${CX_TEST_CATCH_ORG_CUSTUSX_VNNCLRECONSTRUCTION_MOC_SOURCE_FILES}
		cxtestVNNclReconstructionService.cpp
		)

    cx_catch_add_lib_and_exe(org_custusx_vnnclreconstruction
        "${CX_TEST_CATCH_ORG_CUSTUSX_VNNCLRECONSTRUCTION_SOURCE_FILES}" 
        "${CX_TEST_CATCH_ORG_CUSTUSX_VNNCLRECONSTRUCTION_MOC_SOURCE_FILES}"        
        cxtestCatchcxPluginUSReconstruction #TODO remove...
        )

endif (BUILD_TESTING)
