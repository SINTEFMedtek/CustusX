SET(LIBNAME gui)

# Make VERSION_NUMBER available for c++ files in gui folder
ADD_DEFINITIONS(-DVERSION_NUMBER="${VERSION_NUMBER}")

SET(cxProject_MOC_HDRS
  cxMainWindow.h
  cxFileCopied.h
  cxPatientData.h
  cxCustomStatusBar.h
  cxContextDockWidget.h
  cxNavigationWidget.h
  cxBrowserWidget.h
  cxImageRegistrationWidget.h
  cxRegistrationWidget.h
  cxPatientRegistrationWidget.h
  cxTransferFunctionWidget.h
  cxTransferFunctionAlphaWidget.h
  cxTransferFunctionColorWidget.h
  cxPreferencesDialog.h
  cxShiftCorrectionWidget.h
  cxRegistrationHistoryWidget.h
  cxImagePropertiesWidget.h
  cxToolPropertiesWidget.h
  cxPointSamplingWidget.h
  cxDataInterface.h
  cxShadingParamsInterfaces.h
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
    ItkVtkGlue/itkImageToVTKImageFilter.h
    ItkVtkGlue/itkImageToVTKImageFilter.txx
    ItkVtkGlue/itkVTKImageToImageFilter.h
    ItkVtkGlue/itkVTKImageToImageFilter.txx 
    cxMainWindow.h
    cxMainWindow.cpp 
    cxFileCopied.cpp
    cxPatientData.cpp
    cxCustomStatusBar.h
    cxCustomStatusBar.cpp
    cxContextDockWidget.h
    cxContextDockWidget.cpp
    cxNavigationWidget.h
    cxNavigationWidget.cpp
    cxBrowserWidget.h
    cxBrowserWidget.cpp
    cxImageRegistrationWidget.h
    cxImageRegistrationWidget.cpp
    cxRegistrationWidget.h
    cxRegistrationWidget.cpp
    cxPatientRegistrationWidget.h
    cxPatientRegistrationWidget.cpp
    cxTransferFunctionWidget.h
    cxTransferFunctionWidget.cpp
    cxTransferFunctionAlphaWidget.h
    cxTransferFunctionAlphaWidget.cpp
    cxTransferFunctionColorWidget.h
    cxTransferFunctionColorWidget.cpp
    cxPreferencesDialog.h
    cxPreferencesDialog.cpp
    cxShiftCorrectionWidget.h
    cxShiftCorrectionWidget.cpp
    cxRegistrationHistoryWidget.h
    cxRegistrationHistoryWidget.cpp
    cxImagePropertiesWidget.h
    cxImagePropertiesWidget.cpp
    cxToolPropertiesWidget.h
    cxToolPropertiesWidget.cpp
    cxPointSamplingWidget.h
    cxPointSamplingWidget.cpp
    cxDataInterface.h
    cxDataInterface.cpp
    cxShadingParamsInterfaces.h
    cxShadingParamsInterfaces.cpp
    ${cxProject_RCC_SRCS} )

# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)


ADD_LIBRARY ( ${LIBNAME} STATIC ${${LIBNAME}_SRCS}  ${${LIBNAME}_MOC_SRCS} )

