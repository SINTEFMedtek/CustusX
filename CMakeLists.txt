cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ Library 
# =========================================================

project( SSC )

#enable_testing()

include_directories( /usr/local/include )

add_subdirectory(Code)
#add_subdirectory(Examples)
#add_subdirectory(Testing)
