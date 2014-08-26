###########################################################
##               Resource UsReconstructionTypes library
###########################################################

cx_add_class_qt_moc(CX_RESOURCE_USRECONSTRUCTIONTYPES_FILES
	cxSavingVideoRecorder
	)

cx_add_class(CX_RESOURCE_USRECONSTRUCTIONTYPES_FILES
	cxUsReconstructionFileMaker
	cxUsReconstructionFileReader
	cxUSFrameData
	cxUSReconstructInputData
	cxUSReconstructInputDataAlgoritms
    )

unset(CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES)
set(CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CustusX_SOURCE_DIR}/source/resource/usReconstructionTypes
    CACHE INTERNAL "Include directories for resource US Reconstruction Types library")
include_directories(${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES} )

add_library( cxResourceUsReconstructionTypes ${CX_RESOURCE_USRECONSTRUCTIONTYPES_FILES})
target_link_libraries(cxResourceUsReconstructionTypes cxResource )

add_subdirectory(testing)
