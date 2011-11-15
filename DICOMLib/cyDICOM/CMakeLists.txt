PROJECT( cyDICOM )

# ---------------------------------------------------------
# Define variables for paths and include files.
# ---------------------------------------------------------


set( INCLUDE_DIRS 
#	.
#	..
#	../../DataBus
#	/usr/local/dicom/include
#	/usr/lib64/dbus-1.0/include/
#	/usr/include/dbus-1.0
    ${CMAKE_CURRENT_SOURCE_DIR}/../../DICOMLib 
    ${QT_INCLUDES}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR} )

set( LIBRARY_DIRS
#	.
#	..
#	../../DataBus 
#	/usr/lib64/qt4/lib64 
	)

set( HEADER_FILES 
	mainwindow.h 	
	glviews.h 	
	studylist.h 
	settings.h 
	serieslist.h 
	searchPACS.h )

set( UI_FILES
	cyDICOM.ui 
	studylist.ui 
	settingsPACS.ui	
	serieslist.ui
	searchPACS.ui )

set( SOURCE_FILES 
	main.cpp 
	mainwindow.cpp 
	glviews.cpp 
	studylist.cpp 
	settings.cpp 
	serieslist.cpp 
	searchPACS.cpp )

set( LIBRARIES
	DICOMLib 
	QtOpenGL 
	#DataBusLib
	 )

set( RESOURCES
	cyDICOM.qrc )

# ---------------------------------------------------------
# Setup the Qt app based on the variables above.
# ---------------------------------------------------------

find_package( Qt4 REQUIRED )

include_directories( ${INCLUDE_DIRS} ${QT_INCLUDES} )
link_directories( ${LIBRARY_DIRS} )

QT4_WRAP_UI( UI_HEADER_FILES ${UI_FILES} )
QT4_WRAP_CPP( MOC_HEADER_FILES ${HEADER_FILES} )
QT4_ADD_RESOURCES( GENERATED_RESOURCES ${RESOURCES} )

message(STATUS ${CMAKE_PROJECT_NAME})
add_executable( cydicom ${UI_HEADER_FILES} ${MOC_HEADER_FILES} ${SOURCE_FILES} ${GENERATED_RESOURCES} )
target_link_libraries( cydicom ${QT_LIBRARIES} ${LIBRARIES} )

configure_file("${PROJECT_SOURCE_DIR}/zeroAlpha.frag" "${PROJECT_BINARY_DIR}/zeroAlpha.frag")
