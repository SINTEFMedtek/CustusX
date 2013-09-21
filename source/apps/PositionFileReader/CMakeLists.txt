cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# app for reading sonowand recorder position files 
# =========================================================

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
add_executable(sscPositionFileReader main.cpp)
target_link_libraries(sscPositionFileReader cxResource)

