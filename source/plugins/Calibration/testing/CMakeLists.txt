###########################################################
##               Calibration Plugin Tests
##
##               Tests
##
###########################################################

cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_TESTING)    

	QT4_WRAP_CPP( CX_TEST_PLUGINCALIBRATION_SOURCE_FILES
		)
	set(CX_TEST_PLUGINCALIBRATION_SOURCE_FILES
		${CX_TEST_PLUGINCALIBRATION_SOURCE_FILES}
		cxtestTemporalCalibration.cpp
		)

	cx_catch_add_lib_and_exe(cxPluginCalibration "${CX_TEST_PLUGINCALIBRATION_SOURCE_FILES}")


endif (BUILD_TESTING)
