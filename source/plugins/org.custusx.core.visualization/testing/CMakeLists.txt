
if (BUILD_TESTING)
    include_directories(
        ${CustusX_SOURCE_DIR}/source/ThirdParty/VTKMultiVolume
		${CustusX_SOURCE_DIR}/source/resource/visualization/testing
		${CustusX_SOURCE_DIR}/source/resource/core/testing
		)

    set(CX_TEST_CATCH_org_custusx_core_visualization_MOC_SOURCE_FILES
    )
	set(CX_TEST_CATCH_org_custusx_core_visualization_SOURCE_FILES
		${CX_TEST_CATCH_org_custusx_core_visualization_MOC_SOURCE_FILES}
		cxtestVisualizationPlugin.cpp
        cxtestCatchMultiVolume3DRepProducer.cpp
        cxtestMultiVolume3DRepProducerFixture.h
        cxtestMultiVolume3DRepProducerFixture.cpp
        cxtestCatchViewRenderSpeed.cpp
        cxtestCatchVolumeReps.cpp
        cxtestCatchVtkOpenGLGPUMultiVolumeRayCastMapper.cpp
	    cxtestDataTypeSort.cpp
        cxtestRendering.cpp
		)

    cx_catch_add_lib_and_exe(org_custusx_core_visualization 
        "${CX_TEST_CATCH_org_custusx_core_visualization_SOURCE_FILES}" 
        "${CX_TEST_CATCH_org_custusx_core_visualization_MOC_SOURCE_FILES}")

endif (BUILD_TESTING)

