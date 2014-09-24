###########################################################
##               Resource Layer
###########################################################

if(NOT CX_WINDOWS)
    option(SSC_BUILD_PURE_CPPUNIT_TESTS "build cppunit tests unconnected to the ctest framework. Do not use when doine code coverage" OFF)
endif()

option(SSC_PRINT_CALLER_INFO "Print extra information about messages on the commandline." OFF)
if(SSC_PRINT_CALLER_INFO)
        add_definitions(-DSSC_PRINT_CALLER_INFO)
endif()

#message(STATUS "***** PRINT INCLUDE_DIRECTORIES " ${CMAKE_CURRENT_SOURCE_DIR})
#get_directory_property(_inc_DIRS INCLUDE_DIRECTORIES)
#foreach(_current ${_inc_DIRS})
#	message(STATUS "  " ${_current})
#endforeach()
#message(FATAL_ERROR "stopping")


add_subdirectory(core)
add_subdirectory(visualization)
add_subdirectory(OpenIGTLinkUtilities)
add_subdirectory(testUtilities)
add_subdirectory(widgets)
add_subdirectory(usReconstructionTypes)
add_subdirectory(videoServer)
add_subdirectory(pluginUtilities)
add_subdirectory(filter)



