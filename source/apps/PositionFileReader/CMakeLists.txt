
# =========================================================
# app for reading sonowand recorder position files 
# =========================================================

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
add_executable(sscPositionFileReader main.cpp)
target_link_libraries(sscPositionFileReader cxResource)

