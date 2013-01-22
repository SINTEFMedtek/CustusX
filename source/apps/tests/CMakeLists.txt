cmake_minimum_required(VERSION 2.6)
 
IF(BUILD_TESTING)

	PROJECT(DepthSortPolyData)

	FIND_PACKAGE(VTK REQUIRED)
	INCLUDE(${VTK_USE_FILE})

	set(TARGET_LIBRARIES vtkHybrid SSC)

	ADD_EXECUTABLE(CorrectlyRenderTranslucentGeometry CorrectlyRenderTranslucentGeometry.cxx)
	TARGET_LINK_LIBRARIES(CorrectlyRenderTranslucentGeometry ${TARGET_LIBRARIES})

ENDIF(BUILD_TESTING)
