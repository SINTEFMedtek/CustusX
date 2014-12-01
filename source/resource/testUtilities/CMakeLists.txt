# This file is part of CustusX, an Image Guided Therapy Application.
#
# Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
#
# CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
# code and binaries can only be used by SMT and those with explicit permission
# from SMT. CustusX shall not be distributed to anyone else.
#
# CustusX is a research tool. It is NOT intended for use or certified for use
# in a normal clinical setting. SMT does not take responsibility for its use
# in any way.
#
# See CustusX_License.txt for more information.

###########################################################
#               cxTestUtilities
#
# Utilities library for testing.
# Contains the catch unit test framework.
# Add all system-wide test utilities here.
###########################################################
	
include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    )

add_subdirectory(cisst_code)

if (BUILD_TESTING)
        # link statically on linux, see explanation in cx_catch__private_define_platform_specific_linker_options
#        cx_catch__private_define_platform_specific_linker_options()

        #cx_add_class_qt_moc( CX_TESTUTILITIES_SOURCE_FILES
        #    cxtestQueuedSignalListener
        #    cxtestDirectSignalListener
        #    )
        #cx_add_class(CX_TESTUTILITIES_SOURCE_FILES
        #    cxtestUtilities
        #    cxtestCatchImpl
        #    cxtestCatchExamples.cpp
        #    cxtestJenkinsMeasurement
        #	cxtestDummyDataManager
        #    )
	include_directories(
	    .
	    ${CustusX_SOURCE_DIR}/source/ThirdParty/catch
	    )

        set(CX_TESTUTILITIES_FILES_TO_MOC
            cxtestQueuedSignalListener.h
            cxtestDirectSignalListener.h
        )

        set(CX_TESTUTILITIES_SOURCE_FILES
            cxtestUtilities.h
            cxtestUtilities.cpp
            cxtestCatchImpl.h
            cxtestCatchImpl.cpp
            cxtestJenkinsMeasurement.h
            cxtestJenkinsMeasurement.cpp
            cxtestDummyDataManager.h
            cxtestDummyDataManager.cpp
            cxtestQueuedSignalListener.cpp
            cxtestDirectSignalListener.cpp
            cxtestSyntheticVolumeComparer.h
            cxtestSyntheticVolumeComparer.cpp
            cxtestSphereSyntheticVolume.h
            cxtestSphereSyntheticVolume.cpp
            cxtestSyntheticReconstructInput.h
            cxtestSyntheticReconstructInput.cpp
        )

        set(DEPENDENT_ON_LIBS
            Qt5::Test
            cxResource
        )

        cx_remove_duplicate_include_directories()
        cx_catch_add_lib_and_exe(""
                "${CX_TESTUTILITIES_SOURCE_FILES}"
                "${CX_TESTUTILITIES_FILES_TO_MOC}"
                "${DEPENDENT_ON_LIBS}")
endif (BUILD_TESTING)
