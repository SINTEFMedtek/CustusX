
if (BUILD_TESTING)
        include_directories(
            ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    		../logic
    		../logic/3DReconstruction
    		${CustusX_BINARY_DIR}/source/service/patient
    		${CustusX_SOURCE_DIR}/source/service/patient
    		${CustusX_SOURCE_DIR}/source/service/tracking
		)

	# Catch unit test framework stuff:

    set(CXTEST_SOURCES_TO_MOC
    )
	#QT5_WRAP_CPP(CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES
	#	)
	set(CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES ${CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES}
		cxtestReconstructionManagerFixture.h
		cxtestReconstructionManagerFixture.cpp
		cxtestReconstructionManager.cpp
		cxtestReconstructionAlgorithmFixture.h
		cxtestReconstructionAlgorithmFixture.cpp
		cxtestSyntheticVolumeComparer.h
		cxtestSyntheticVolumeComparer.cpp
		cxtestSphereSyntheticVolume.h
		cxtestSphereSyntheticVolume.cpp
		cxtestReconstructRealData.h
		cxtestReconstructRealData.cpp
		cxtestSyntheticReconstructInput.h
		cxtestSyntheticReconstructInput.cpp
		)
    
    cx_catch_add_lib_and_exe(org_custusx_usreconstruction "${CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES}" "${CXTEST_SOURCES_TO_MOC}")

endif (BUILD_TESTING)
