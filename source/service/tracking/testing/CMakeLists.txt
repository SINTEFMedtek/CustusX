
if (BUILD_TESTING)

include_directories(
    ..
    ${CustusX_SOURCE_DIR}/source/logic
)

set(CXTEST_SOURCES_TO_MOC
)

set(CXTEST_TRACKING_SOURCES
    cxtestToolFiles.cpp
    cxtestTestToolMesh.h
    cxtestTestToolMesh.cpp
    cxtestCatchTrackingService.cpp
    cxtestCatchProbe.cpp
    cxtestProbeFixture.h
    cxtestProbeFixture.cpp
)
	
cx_catch_add_lib_and_exe(cxTrackingService "${CXTEST_TRACKING_SOURCES}" "${CXTEST_SOURCES_TO_MOC}")
	
endif (BUILD_TESTING)
