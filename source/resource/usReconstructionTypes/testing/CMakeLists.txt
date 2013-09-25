
if(BUILD_TESTING)
		set(RESOURCE_USRECONSTRUCTIONTYPES_TEST_CATCH_SOURCE_FILES
			cxtestUSReconstructionFileFixture.h
			cxtestUSReconstructionFileFixture.cpp
			cxtestCatchUSReconstructionFile.cpp
			)
		cx_catch_add_lib_and_exe(cxResourceUsReconstructionTypes "${RESOURCE_USRECONSTRUCTIONTYPES_TEST_CATCH_SOURCE_FILES}")
endif()
