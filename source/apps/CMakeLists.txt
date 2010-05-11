cmake_minimum_required(VERSION 2.6)

ADD_SUBDIRECTORY(UsReconstruction)

SET ( LINK_LIBRARIES
  ${BOOST_LIBRARIES}
  ${QT_LIBRARIES}
  vtkRendering
  vtkWidgets
  vtkGraphics
  vtkIO
  vtkCommon
  vtkImaging
  vtkFiltering
  vtkHybrid
  vtkVolumeRendering
  IGSTK
  vtkDataManagement
  vtkRenderingAddOn)

IF(APPLE)
    ADD_EXECUTABLE ( reconstruct cxReconstructMain.cpp )
    TARGET_LINK_LIBRARIES ( reconstruct
      3DUSReconstruction
      ${LINK_LIBRARIES}
      )
ENDIF(APPLE)
      