cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_TESTING)
        include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
        include_directories(
		../logic
		../logic/3DReconstruction
		${CustusX3_BINARY_DIR}/source/service/patient
		${CustusX3_SOURCE_DIR}/source/service/patient
		${CustusX3_SOURCE_DIR}/source/service/tracking
		)

	# Catch unit test framework stuff:

    set(CXTEST_SOURCES_TO_MOC
    )
	#QT4_WRAP_CPP(CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES
	#	)
	set(CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES ${CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES}
		cxtestReconstructionManagerFixture.h
		cxtestReconstructionManagerFixture.cpp
		cxtestReconstructionManager.cpp
		cxtestReconstructionAlgorithm.cpp
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
    
	cx_catch_add_lib_and_exe(cxPluginUSReconstruction "${CXTEST_PLUGIN_USRECONSTRUCTION_SOURCES}" "${CXTEST_SOURCES_TO_MOC}")

endif (BUILD_TESTING)
