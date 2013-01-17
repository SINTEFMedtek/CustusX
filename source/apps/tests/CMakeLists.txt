cmake_minimum_required(VERSION 2.6)
 
IF(BUILD_TESTING)

	PROJECT(DepthSortPolyData)

	FIND_PACKAGE(VTK REQUIRED)
	INCLUDE(${VTK_USE_FILE})

	ADD_EXECUTABLE(CorrectlyRenderTranslucentGeometry CorrectlyRenderTranslucentGeometry.cxx)
	TARGET_LINK_LIBRARIES(CorrectlyRenderTranslucentGeometry vtkHybrid)

ENDIF(BUILD_TESTING)
