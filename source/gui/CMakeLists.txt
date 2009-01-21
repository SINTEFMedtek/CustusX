SET(LIBNAME gui)

SET(cxProject_MOC_HDRS
  cxMainWindow.h
  cxMessageManager.h
)

# After this call, cxProject_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx ...
IF(WIN32)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${cxProject_MOC_HDRS} OPTIONS "-D_WINDOWS")
ELSE(WIN32)
  QT4_WRAP_CPP(${LIBNAME}_MOC_SRCS ${cxProject_MOC_HDRS} )
ENDIF(WIN32)

SET(cxProject_RCCS cxResources.qrc)

# generate rules for building source files from the resources
QT4_ADD_RESOURCES(cxProject_RCC_SRCS ${cxProject_RCCS})

SET(${LIBNAME}_SRCS 
    cxMainWindow.h
    cxMainWindow.cpp 
    cxMessageManager.h
    cxMessageManager.cpp
    ${cxProject_RCC_SRCS} )

# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)


ADD_LIBRARY ( ${LIBNAME} STATIC ${${LIBNAME}_SRCS}  ${${LIBNAME}_MOC_SRCS} )

