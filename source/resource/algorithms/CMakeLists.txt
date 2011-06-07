###########################################################
##
##               Algorithms Library
## 
##               Resource Layer
##
##  Various algorithms - basic stuff     
##
###########################################################

SET( CX_QT_MOC_HEADER_FILES
    cxTimedAlgorithm.h
)

SET ( cxAlgorithms_Res_SOURCE
    ItkVtkGlue/itkImageToVTKImageFilter.h
    ItkVtkGlue/itkImageToVTKImageFilter.txx
    ItkVtkGlue/itkVTKImageToImageFilter.h
    ItkVtkGlue/itkVTKImageToImageFilter.txx
    vesselReg/SeansVesselReg.cxx
    vesselReg/SeansVesselReg.hxx
    vesselReg/HackTPSTransform.hxx
    itkBinaryThinningImageFilter3D.h
    itkBinaryThinningImageFilter3D.txx
    sscImageAlgorithms.h
    sscImageAlgorithms.cpp
    cxTimedAlgorithm.h
    cxTimedAlgorithm.cpp
)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxAlgorithms_Res ${cxAlgorithms_Res_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxAlgorithms_Res SSC cxSettings cxUtilities ITKIO ITKCommon ITKBasicFilters) 
