# =========================================================================
# This file is part of CustusX, an Image Guided Therapy Application.
#
# Copyright (c) SINTEF Department of Medical Technology.
# All rights reserved.
#
# CustusX is released under a BSD 3-Clause license.
#
# See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
# =========================================================================

if(BUILD_TESTING)
	set(VTK_MODULES
		VTK::FiltersCore
		VTK::FiltersGeneral
	)

	cx_add_class(CXTEST_SOURCES ${CXTEST_SOURCES}
		cxtestClipMeshFilter.cpp
	)
	set(CXTEST_SOURCES_TO_MOC
	)

	qt5_wrap_cpp(CXTEST_SOURCES_TO_MOC ${CXTEST_SOURCES_TO_MOC})
	add_library(cxtest_org_custusx_filter_clipmesh ${CXTEST_SOURCES} ${CXTEST_SOURCES_TO_MOC})
	include(GenerateExportHeader)
	generate_export_header(cxtest_org_custusx_filter_clipmesh)
	target_include_directories(cxtest_org_custusx_filter_clipmesh
		PUBLIC
		.
		${CMAKE_CURRENT_BINARY_DIR}
	)
	target_link_libraries(cxtest_org_custusx_filter_clipmesh
		PRIVATE
		org_custusx_filter_clipmesh
		cxLogicManager
		cxtestUtilities
		cxCatch
		cxResource
		cxResourceFilter
		cxtestResource
		cxtest_org_custusx_core_patientmodel
		${VTK_MODULES}
	)

	vtk_module_autoinit(
		TARGETS cxtest_org_custusx_filter_clipmesh
		MODULES ${VTK_MODULES}
	)

	cx_add_tests_to_catch(cxtest_org_custusx_filter_clipmesh)

endif(BUILD_TESTING)
