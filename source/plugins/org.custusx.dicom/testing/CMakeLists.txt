
if (BUILD_TESTING)
    include_directories(
    )


    set(CX_TEST_CATCH_ORG_CUSTUSX_DICOM_MOC_SOURCE_FILES
    )
	set(CX_TEST_CATCH_ORG_CUSTUSX_DICOM_SOURCE_FILES
		${CX_TEST_CATCH_ORG_CUSTUSX_DICOM_MOC_SOURCE_FILES}
		cxtestDicomConverter.cpp
		)

    cx_catch_add_lib_and_exe(org_custusx_dicom 
        "${CX_TEST_CATCH_ORG_CUSTUSX_DICOM_SOURCE_FILES}" 
        "${CX_TEST_CATCH_ORG_CUSTUSX_DICOM_MOC_SOURCE_FILES}")

endif (BUILD_TESTING)
