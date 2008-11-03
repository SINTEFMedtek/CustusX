cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ Library 
# =========================================================

configure_file(
	${PROJECT_SOURCE_DIR}/Test/sscConfig.h.in
#	${PROJECT_BINARY_DIR}/sscConfig.h)
	${PROJECT_SOURCE_DIR}/Test/sscConfig.h)

add_subdirectory(Library)
add_subdirectory(Test)



