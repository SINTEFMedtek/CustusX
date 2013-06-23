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
#               cxGui Tests
###########################################################

if (BUILD_TESTING)
	# Catch unit test framework stuff:

	QT4_WRAP_CPP(CXTEST_GUI_SOURCES
		cxTestCustusXController.h
		cxtestTestVideoConnectionWidget.h)
	set(CXTEST_GUI_SOURCES ${CXTEST_GUI_SOURCES}
		cxTestCustusXController.cpp
		cxtestSpeed.cpp
		cxtestCatchStreamingWidgets.cpp
	    cxtestTestVideoConnectionWidget.cpp)

	cx_catch_add_lib_and_exe(cxGui "${CXTEST_GUI_SOURCES}")

endif (BUILD_TESTING)
