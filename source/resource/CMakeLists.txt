###########################################################
##               Resource Layer
###########################################################

option(SSC_BUILD_PURE_CPPUNIT_TESTS "build cppunit tests unconnected to the ctest framework. Do not use when doine code coverage" OFF)

option(SSC_PRINT_CALLER_INFO "Print extra information about messages on the commandline." OFF)
if(SSC_PRINT_CALLER_INFO)
        add_definitions(-DSSC_PRINT_CALLER_INFO)
endif()

add_subdirectory(core)
add_subdirectory(visualization)
add_subdirectory(OpenIGTLinkUtilities)
add_subdirectory(testUtilities)
add_subdirectory(widgets)
add_subdirectory(usReconstructionTypes)
add_subdirectory(videoServer)


