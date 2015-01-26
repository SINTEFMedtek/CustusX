
# =========================================================
# app for reading sonowand recorder position files 
# =========================================================

add_executable(sscPositionFileReader main.cpp)
target_link_libraries(sscPositionFileReader cxResource ${SSC_GCOV_LIBRARY})

