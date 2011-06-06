SET ( LIB 3DUSReconstruction)

SET ( ${LIB}_3DUSReconstruction
  cxImport2DUS.h
  cxImport2DUS.cpp
  cxInput2DUS.h
  cxInput2DUS.cpp
  cxInput2DUStemplate.h
  cxUsReconstruction.h
  cxUsReconstruction.cpp
)

ADD_LIBRARY ( ${LIB} ${${LIB}_3DUSReconstruction} )
target_link_libraries(${LIB} ITKCommon vtkImaging vtkIO)
