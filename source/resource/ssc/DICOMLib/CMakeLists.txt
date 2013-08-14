###########################################################
##               DICOMLib
###########################################################

# openCV
#----------
if (SSC_USE_DCMTK)
    find_package( DCMTK REQUIRED)
else (SSC_USE_DCMTK)
    find_package( DCMTK QUIET)
endif (SSC_USE_DCMTK)

IF( DCMTK_FOUND )
    option(SSC_USE_DCMTK "use DCMTK for DICOM support" ON)
ELSE( DCMTK_FOUND )
    option(SSC_USE_DCMTK "use DCMTK for DICOM support" OFF)
ENDIF( DCMTK_FOUND )

if (SSC_USE_DCMTK)

    add_subdirectory( tests )
    
    FIND_PACKAGE( Qt4 REQUIRED )
    INCLUDE( ${QT_USE_FILE} )
    QT4_WRAP_CPP( MOC_HEADER_DICOMLIB 
        sscDICOMLibAPI.h
        )
    
    #find_package(VTK REQUIRED)
    #include(${VTK_USE_FILE})
    
    find_package(SSC REQUIRED)
    include (${SSC_USE_FILE} )
    
    find_package( DCMTK REQUIRED )
    include_directories( ${DCMTK_INCLUDE_DIR})
    
    find_package( TIFF REQUIRED )
    include_directories( ${TIFF_INCLUDE_DIR})
    
    INCLUDE_DIRECTORIES( 
        ${QT_INCLUDES} 
        .
        )
    LINK_DIRECTORIES( 
        )
    
    ADD_LIBRARY( DICOMLib SHARED 
        ${MOC_HEADER_DICOMLIB}
        convert.c 
        DICOMLib.cpp 
        identify.c 
        DCMTK.cpp 
        export.cpp 
        vector3d.h 
        sscDICOMLibAPI.cpp
	niftiexport.cpp
	nifti1.h
	znzlib.h
	znzlib.c
	nifti1_io.c
	nifti1_io.h
        )
    
    SET_TARGET_PROPERTIES( DICOMLib PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "-Wextra ${SNW_COMPILE_FLAGS}" )
    
    TARGET_LINK_LIBRARIES( DICOMLib 
        z m
        ${DCMTK_LIBRARIES}
        ${SNW_LINK_LIBS}
        ${QT_LIBRARIES} 
        ${TIFF_LIBRARIES} 
        sqlite3
        SSC
        )
    
    IF(BUILD_TESTING)
	add_subdirectory( unittest )
    ENDIF(BUILD_TESTING)
    
    INSTALL( TARGETS DICOMLib LIBRARY DESTINATION lib64 )

endif (SSC_USE_DCMTK)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/sscDICOMLibConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/sscDICOMLibConfig.h" )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
