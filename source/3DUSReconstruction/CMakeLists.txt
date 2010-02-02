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

ADD_LIBRARY ( ${LIB} STATIC ${${LIB}_3DUSReconstruction} )
