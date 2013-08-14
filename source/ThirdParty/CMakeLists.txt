###########################################################
##               ThirdParty
###########################################################

if(BUILD_OPEN_IGTLINK_SERVER)
  if(CX_WIN32)
    add_subdirectory(ultrasonix)
  endif(CX_WIN32)
endif(BUILD_OPEN_IGTLINK_SERVER)


option(BUILD_VTKMULTIVOLUME_DEMO "Build the multi volume demo by Medhi" OFF)
if(BUILD_VTKMULTIVOLUME_DEMO)
	add_subdirectory(VTKMultiVolume)
endif(BUILD_VTKMULTIVOLUME_DEMO)