#
# Source files
#
#------------------------------------------------------------------------------
# Qt - MOC
#------------------------------------------------------------------------------
set(${LIBNAME}_MOC_HDRS
    SonixHelper.h
)

# After this call, cxProject_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx ...
if(APPLE)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${${LIBNAME}_MOC_HDRS})
endif(APPLE)

if(CX_WINDOWS)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${${LIBNAME}_MOC_HDRS} OPTIONS "-D_WINDOWS")
endif(CX_WINDOWS)

SET (Ultrasound_SRCS
  vtkSonixVideoSource.h
  vtkSonixVideoSource.cxx
  vtkUltrasoundWin32Header.h
  cxFrame.h
  sonixHelper.h
  ${${LIBNAME}_MOC_HDRS}
  ${${LIBNAME}_MOC_SRCS}
)

#
# Libraries (starts empty)
#
SET (Ultrasound_LIBS
  )
 
INCLUDE_DIRECTORIES(${ULTERIUS_INCLUDE_DIR} ${CX_SSC_INCLUDE_DIRECTORIES})
SET (Ultrasound_LIBS ${Ultrasound_LIBS} ${ULTERIUS_LIBRARY})

# Create the vtkOpenIGTUltrasound C++ library
ADD_LIBRARY (vtkOpenIGTUltrasound ${Ultrasound_SRCS})
TARGET_LINK_LIBRARIES(vtkOpenIGTUltrasound vtkCommon vtkHybrid vtkParallel
                      ${Ultrasound_LIBS})
