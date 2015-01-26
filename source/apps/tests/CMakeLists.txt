 
if(BUILD_TESTING)

    project(DepthSortPolyData)

    find_package(VTK REQUIRED)
    include(${VTK_USE_FILE})

    add_executable(CorrectlyRenderTranslucentGeometry CorrectlyRenderTranslucentGeometry.cxx)
    target_include_directories(CorrectlyRenderTranslucentGeometry
        PRIVATE
        ${VTK_INCLUDE_DIRS}
    )
    target_link_libraries(CorrectlyRenderTranslucentGeometry
        PRIVATE
        cxResource
        ${SSC_GCOV_LIBRARY}
    )

endif(BUILD_TESTING)
