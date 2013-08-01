#==============================================================================
# vtkMultiVolRen
#
# Description: vtkMultiVolRen demo
#
# Author: xXx <xXX>, (C) 2010
#
# Copyright: See COPYING file that comes with this distribution
#==============================================================================

project (vtkGPUMultiVolumeRenderDemo)


set(KIT vtkGPUMultiVolumeRenderDemo)

set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "4.2")

IF(POLICY CMP0017) 
  CMAKE_POLICY(SET CMP0017 NEW)
ENDIF(POLICY CMP0017)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

FIND_PACKAGE( VTK 5.8 REQUIRED)
INCLUDE(${VTK_USE_FILE})

#-----------------------------------------------------------------------------
# Configure OpenGL support (copied from VTK main CMakeLists.txt).
# At the moment CMake's FindOpenGL considers OpenGL should be found
# in the framework version on OSX. This is a reasonable assumption for
# few people are going to use X. The module warns that if X is to be
# used, one has to set the libs and include dir manually, which is
# exactly what we are going to do below.
IF(APPLE AND VTK_USE_X)
  FIND_PATH(OPENGL_INCLUDE_DIR GL/gl.h
    /usr/X11R6/include)
  FIND_PATH(OPENGL_xmesa_INCLUDE_DIR GL/xmesa.h
    /usr/X11R6/include)
  FIND_LIBRARY(OPENGL_gl_LIBRARY NAMES GL MesaGL
    PATHS /usr/lib /usr/local/lib /usr/X11R6/lib)
  FIND_LIBRARY(OPENGL_glu_LIBRARY NAMES GLU MesaGLU
    PATHS ${OPENGL_gl_LIBRARY} /usr/lib /usr/local/lib /usr/X11R6/lib)
ENDIF(APPLE AND VTK_USE_X)
FIND_PACKAGE(OpenGL)


INCLUDE_DIRECTORIES(
  ${OPENGL_INCLUDE_DIR}
)


SET(${KIT}_HDRS
  ${${KIT}_HDRS}
  vtkGPUMultiVolumeRayCastMapper.h
  vtkOpenGLGPUMultiVolumeRayCastMapper.h
)

SET(${KIT}_SRCS
  ${${KIT}_SRCS}
  vtkGPUMultiVolumeRayCastMapper.cxx
  vtkOpenGLGPUMultiVolumeRayCastMapper.cxx
  ${KIT}.cxx 
)

set(KIT_GLSL_SRCS
   vtkGPUMultiVolumeRayCastMapper_CompositeFS
   vtkGPUMultiVolumeRayCastMapper_OneComponentFS
   vtkGPUMultiVolumeRayCastMapper_FourComponentsFS
   vtkGPUMultiVolumeRayCastMapper_NoShadeFS
   vtkGPUMultiVolumeRayCastMapper_ShadeFS
)


# -----------------------------------------------------------------------------
# Create custom commands to encode each glsl file into a C string literal
# in a header file
# -----------------------------------------------------------------------------

# Needs vtkEncodeString, VTK_ENCODESTRING_EXE 
# should work ...  
# VTK_VOLUMERENDERING_EXPORT is the flag associated to the library, should be adapted
MESSAGE("PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}")
foreach(file ${KIT_GLSL_SRCS})
  ## TODO
  set(src  ${PROJECT_SOURCE_DIR}/GLSL/${file}.glsl)
  set(res  ${CMAKE_CURRENT_BINARY_DIR}/${file}.cxx)
  set(resh ${CMAKE_CURRENT_BINARY_DIR}/${file}.h)
  add_custom_command(
    OUTPUT ${res} ${resh}
    DEPENDS ${src} vtkEncodeString
    COMMAND ${VTK_ENCODESTRING_EXE}
    ARGS ${res} ${src} ${file} --build-header VTK_VOLUMERENDERING_EXPORT vtkSystemIncludes.h
    )
  set(Kit_EXTRA_SRCS ${Kit_EXTRA_SRCS} ${res})
endforeach(file)


IF(APPLE)
  SET(EXECUTABLE_FLAG MACOSX_BUNDLE)
ENDIF(APPLE)

ADD_EXECUTABLE(${KIT} ${EXECUTABLE_FLAG} 
  ${${KIT}_SRCS}  ${Kit_EXTRA_SRCS})

TARGET_LINK_LIBRARIES(${KIT} 
  vtkRendering 
  vtkIO 
  vtkVolumeRendering 
  vtkWidgets
  ${OPENGL_gl_LIBRARY}
 )

