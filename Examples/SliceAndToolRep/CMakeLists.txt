cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Example app
# =========================================================

find_package(SSC REQUIRED)
include (${SSC_USE_FILE} )
	
find_package( DCMTK REQUIRED )
include_directories( ${DCMTK_INCLUDE_DIR})

include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR}  )
include_directories( ${SSC_SOURCE_DIR}/Examples/SliceAndToolRep )
include_directories( ${SSC_SOURCE_DIR}/DICOMLib/ )
include_directories( ${CPPUNIT_INCLUDE_DIR})
include_directories( DICOMLib )
QT4_WRAP_CPP( SSC_EX_SOURCE_FILES 
#    sscDICOMWidget.h
    sscExWindow.h)
    
set(SSC_EX_SOURCE_FILES
#    cxDcmData.cpp
#    cxDICOMTest.cpp
    DICOMLib_CPP_Test.cpp
    sscTransformTester.cpp
    sscExWindow.cpp 
    main.cpp
#    sscDICOMWidget.h
#    sscDICOMWidget.cpp
    ${SSC_EX_SOURCE_FILES}
    )

add_executable(exSliceAndToolRep 
    ${SSC_EX_SOURCE_FILES}
    )
#message(STATUS libs: ${DCMTK_LIBRARIES} )
#message(STATUS libs: ${LIBTIFF_LIBS} )
     
target_link_libraries(exSliceAndToolRep 
    SSC 
    DICOMLib 
    ${DCMTK_LIBRARIES} 
#    /usr/local/lib/libdcmpstat.so
#    /usr/local/lib/libdcmimgle.so
#    /usr/local/lib/libdcmdsig.so
#    /usr/local/lib/libdcmwlm.so
#    /usr/local/lib/libdcmjpls.so
#    /usr/local/lib/libdcmsr.so
#    /usr/local/lib/libdcmdata.so
#    /usr/local/lib/libdcmqrdb.so
#    /usr/local/lib/libdcmjpeg.so
#    /usr/local/lib/libdcmimage.so
#    /usr/local/lib/libdcmtls.so
#    /usr/local/lib/libdcmnet.so
    /usr/lib64/libtiff.so
#    /usr/lib64/libjpeg.so
    /usr/local/lib/liboflog.so
    /usr/local/lib/libcharls.so
    
    )

set_target_properties(exSliceAndToolRep PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "${SNW_COMPILE_FLAGS}")

#add_subdirectory( DICOMLib )
