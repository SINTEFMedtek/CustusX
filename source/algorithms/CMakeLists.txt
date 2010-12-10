SET (LIB cxAlgorithms)

SET( CX_QT_MOC_HEADER_FILES
)

SET ( ${LIB}_algorithms
    ItkVtkGlue/itkImageToVTKImageFilter.h
    ItkVtkGlue/itkImageToVTKImageFilter.txx
    ItkVtkGlue/itkVTKImageToImageFilter.h
    ItkVtkGlue/itkVTKImageToImageFilter.txx 
    cxSegmentation.h
    cxSegmentation.cpp
    vesselReg/SeansVesselReg.cxx
    vesselReg/SeansVesselReg.hxx
    vesselReg/HackTPSTransform.hxx
    itkBinaryThinningImageFilter3D.h
    itkBinaryThinningImageFilter3D.txx
    sscImageAlgorithms.h
    sscImageAlgorithms.cpp
    cxTrackingDataToVolume.h
    cxTrackingDataToVolume.cpp
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} ${${LIB}_algorithms} ${MOC_HEADER_FILES})
target_link_libraries(${LIB} dataManager ITKCommon ITKBasicFilters) 
