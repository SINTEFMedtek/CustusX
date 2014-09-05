###########################################################
##               Calibration Plugin Tests
##
##               Tests
##
###########################################################

if (BUILD_TESTING)    

	set(CX_TEST_PLUGINCALIBRATION_SOURCE_FILES
		${CX_TEST_PLUGINCALIBRATION_SOURCE_FILES}
		cxtestTemporalCalibration.cpp
		)

	cx_catch_add_lib_and_exe(cxPluginCalibration "${CX_TEST_PLUGINCALIBRATION_SOURCE_FILES}" "")


endif (BUILD_TESTING)
