###########################################################
##               ThirdParty
###########################################################

if(BUILD_OPEN_IGTLINK_SERVER)
  if(CX_WIN32)
    add_subdirectory(ultrasonix)
  endif(CX_WIN32)
endif(BUILD_OPEN_IGTLINK_SERVER)

add_subdirectory(VTKMultiVolume)