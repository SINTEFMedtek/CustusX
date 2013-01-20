cmake_minimum_required(VERSION 2.6)
 
IF(BUILD_TESTING)

	PROJECT(DepthSortPolyData)

	FIND_PACKAGE(VTK REQUIRED)
	INCLUDE(${VTK_USE_FILE})

	set(TARGET_LIBRARIES vtkHybrid)

# code coverage
#option(SSC_USE_GCOV "add gcov to enable coverage testing" OFF)
#if(SSC_USE_GCOV)
	message(STATUS "Building SSC with gcov code coverage support.")
	if(WIN32)
		message(ERROR "gcov not supported for WIN32")
	# needed on apple
	elseif(APPLE)
		set( TARGET_LIBRARIES ${TARGET_LIBRARIES} profile_rt )
	# needed on linux
	else(WIN32)
		set( TARGET_LIBRARIES ${TARGET_LIBRARIES} gcov)
	endif(WIN32)

#	add_definitions(--coverage)
#endif()


	ADD_EXECUTABLE(CorrectlyRenderTranslucentGeometry CorrectlyRenderTranslucentGeometry.cxx)
	TARGET_LINK_LIBRARIES(CorrectlyRenderTranslucentGeometry ${TARGET_LIBRARIES})

ENDIF(BUILD_TESTING)
