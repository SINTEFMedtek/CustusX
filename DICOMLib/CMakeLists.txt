add_subdirectory( tests )
add_subdirectory( glDicom )
#add_subdirectory( cyDICOM )

FIND_PACKAGE( Qt4 REQUIRED )
INCLUDE( ${QT_USE_FILE} )
#QT4_WRAP_CPP( MOC_HEADER_DICOMLIB file.h )

#find_package(VTK REQUIRED)
#include(${VTK_USE_FILE})

find_package(SSC REQUIRED)
include (${SSC_USE_FILE} )


INCLUDE_DIRECTORIES( 
    ${QT_INCLUDES} 
#    ../DataBus 
#    /usr/lib64/dbus-1.0/include/ 
#    /usr/include/dbus-1.0 
#    /usr/local/include 
 #   ${SSC_SOURCE_DIR}/Code/
 #   ${SSC_SOURCE_DIR}/Code/Utilities/
 #   ${SSC_SOURCE_DIR}/Code/Data/
 #   ${SSC_SOURCE_DIR}/Code/Math/
    )
LINK_DIRECTORIES( 
#    . 
#    ../DataBus 
    )

ADD_LIBRARY( DICOMLib SHARED 
    convert.c 
    DICOMLib.cpp 
    identify.c 
    DCMTK.cpp 
    export.cpp 
    vector3d.h 
    sscDICOMLibAPI.cpp
    )

SET_TARGET_PROPERTIES( DICOMLib PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "-Wextra ${SNW_COMPILE_FLAGS}" )

TARGET_LINK_LIBRARIES( DICOMLib 
#    DataBusLib 
    pthread 
    z 
    m 
    ofstd 
    stdc++ 
    oflog 
    dcmdata 
    ofstd 
    dcmimgle 
    dcmdata 
    ofstd 
    dcmimage
    dcmdata 
    dcmnet 
    dcmdata 
    dcmjpeg 
#    CharLS 
    /usr/local/lib/libcharls.so
    dcmjpls 
    ofstd 
    ijg12 
    ijg8 
    ijg16 
    dcmjpeg 
    ${SNW_LINK_LIBS}
    ${QT_LIBRARIES} 
    -lsqlite3 
    SSC
    )

INSTALL( TARGETS DICOMLib LIBRARY DESTINATION lib64 )
