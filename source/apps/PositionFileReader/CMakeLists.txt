
# =========================================================
# app for reading sonowand recorder position files 
# =========================================================

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
cx_remove_duplicate_include_directories()
add_executable(sscPositionFileReader main.cpp)
target_link_libraries(sscPositionFileReader cxResource)

