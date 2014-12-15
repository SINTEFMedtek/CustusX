 
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
        vtkCommonCore
        vtkCommonDataModel
        vtkCommonExecutionModel
        vtkCommonMath
        vtkCommonMisc
        vtkCommonSystem
        vtkCommonTransforms
        vtkIOCore
        vtkIOImage
        vtkIOGeometry
        vtkIOLegacy
        vtkIOVideo
        vtkImagingStatistics
        vtkImagingColor
        vtkImagingMath
        vtkImagingGeneral
        vtkImagingCore
        vtkFiltersCore
        vtkFiltersGeneral
        vtkFiltersSources
        vtkRenderingFreeTypeOpenGL
        vtkRenderingFreeType
        vtkRenderingOpenGL
        vtkRenderingCore
        vtkRenderingVolume
        vtkInteractionStyle
        cxResource
    )

endif(BUILD_TESTING)


# Add include directories needed to use VTK.
#include_directories(${VTK_INCLUDE_DIRS})

# Add link directories needed to use VTK.
#link_directories(${VTK_LIBRARY_DIRS})
