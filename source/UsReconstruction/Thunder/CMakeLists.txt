cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(GLUT)
IF (APPLE)
  FIND_LIBRARY(OPENCL_LIBRARIES OpenCL DOC "OpenCL lib for OSX")
  FIND_PATH(OPENCL_INCLUDE_DIR OpenCL/cl.h DOC "Include for OpenCL on OSX")
ELSE (APPLE)
  FIND_PATH(OPENCL_INCLUDE_DIR CL/cl.h )
  FIND_LIBRARY(OPENCL_LIBRARIES opencl64 )
  IF( OPENCL_LIBRARIES )
      FIND_LIBRARY(OPENCL_LIBRARIES opencl32 )
  ENDIF( OPENCL_LIBRARIES )

  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIR} )

  #FIND_PACKAGE(OpenCL)
  #LINK_LIBRARIES("..\\source_tree\\lib\\OpenCL\\x64\\OpenCL.lib")
  #LINK_LIBRARIES("..\\source_tree\\lib\\glut\\glut64.lib")
ENDIF (APPLE)

  #INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIR} )

# library
SET( SOURCE_FILES
  holger_time.h
  holger_time.cpp
  setup.h
  setup.cpp
  utils.h
  utils.cpp
  reconstruct_vnn.h
  reconstruct_vnn.cpp
  kernels.ocl
  ) 

SET_SOURCE_FILES_PROPERTIES(kernels.ocl PROPERTIES HEADER_FILE_ONLY TRUE)

# create the target
add_library( thunder  ${SOURCE_FILES} )

TARGET_LINK_LIBRARIES(thunder
  ${OPENGL_LIBRARIES}
  ${OPENCL_LIBRARIES}
  ${GLUT_LIBRARIES}
)