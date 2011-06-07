###########################################################
##               ThirdParty
###########################################################

if(BUILD_GRABBER_SERVER)
  if(WIN32)
    add_subdirectory(ultrasonix)
  endif(WIN32)
endif(BUILD_GRABBER_SERVER)