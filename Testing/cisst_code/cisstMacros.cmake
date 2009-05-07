#
# $Id: cisstMacros.cmake,v 1.17 2007/04/26 19:33:56 anton Exp $
#
# Author(s):  Anton Deguet
# Created on: 2004-01-22
#
# (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# The macro adds a library to a CISST-related project by processing the 
# externally defined variables listed below:
#
# - LIBRARY is the name of the library, e.g. cisstVector
# - DEPENDENCIES is a list of dependencies, for cisstVector, set it to cisstCommon
# - SOURCE_FILES is a list of files, without any path (absolute or relative)
# - HEADER_FILES is a list of files, without any path (absolute or relative)
#
#
# Invoke this macro from within a library's CMakeLists.txt to add that library
# to a larger project.  The name of the project is given as a macro argument.
#

# The macro performs the following:
# -- create the source and header lists of files with the right path
# -- check the dependencies
# -- add the link options based on the dependencies
# -- add the library 
# -- create the install targets for the headers as well as the library

MACRO(CISST_ADD_LIBRARY_TO_PROJECT PROJECT_NAME)

# Make sure this lib should be compiled
IF(BUILD_LIBS_${LIBRARY} OR BUILD_${LIBRARY})

  # Build source list with full path
  FOREACH(file ${SOURCE_FILES})
    SET(SOURCES ${SOURCES} ${${PROJECT_NAME}_SOURCE_DIR}/code/${LIBRARY}/${file})
  ENDFOREACH(file)


  # Build header list with full path and generate a main header file for the library
  STRING(ASCII 35 CISST_STRING_POUND)
  SET(LIBRARY_MAIN_HEADER ${${PROJECT_NAME}_BINARY_DIR}/include/${LIBRARY}.h)
  SET(LIBRARY_MAIN_HEADER_TMP ${${PROJECT_NAME}_BINARY_DIR}/include/${LIBRARY}.h.tmp)

  SET(FILE_CONTENT "// This file is generated automatically by CMake, DO NOT EDIT\n")
  SET(FILE_CONTENT ${FILE_CONTENT} "// CMake: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}\n")
  SET(FILE_CONTENT ${FILE_CONTENT} "// System: ${CMAKE_SYSTEM}\n")
  SET(FILE_CONTENT ${FILE_CONTENT} "// Source: ${CMAKE_SOURCE_DIR}\n\n")
  SET(FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}ifndef _${LIBRARY}_h\n")
  SET(FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}define _${LIBRARY}_h\n\n")
  FOREACH(file ${HEADER_FILES})
    SET(HEADERS ${HEADERS} ${${PROJECT_NAME}_SOURCE_DIR}/include/${LIBRARY}/${file})
    SET(FILE_CONTENT ${FILE_CONTENT} "${CISST_STRING_POUND}include <${LIBRARY}/${file}>\n")
  ENDFOREACH(file)
  SET(FILE_CONTENT ${FILE_CONTENT} "\n${CISST_STRING_POUND}endif // _${LIBRARY}_h\n")
  FILE(WRITE ${LIBRARY_MAIN_HEADER_TMP} ${FILE_CONTENT})

  EXEC_PROGRAM(${CMAKE_COMMAND}
               ARGS -E copy_if_different
               \"${LIBRARY_MAIN_HEADER_TMP}\"
               \"${LIBRARY_MAIN_HEADER}\") 

  EXEC_PROGRAM(${CMAKE_COMMAND}
               ARGS -E remove
               \"${LIBRARY_MAIN_HEADER_TMP}\")

  # Add the library
  ADD_LIBRARY(${LIBRARY}
              ${IS_SHARED}
              ${SOURCES}
              ${HEADERS}
              )
  INSTALL_TARGETS(/lib ${LIBRARY})


  # Add dependencies for linking, also check BUILD_xxx for dependencies
  IF(DEPENDENCIES)
    # Check that dependencies are build
    FOREACH(dependency ${DEPENDENCIES})
      SET(BUILD_DEPENDENCIES ${BUILD_DEPENDENCIES} BUILD_LIBS_${dependency})
    ENDFOREACH(dependency)
    VARIABLE_REQUIRES(BUILD_LIBS_${LIBRARY} BUILD_LIBS_${LIBRARY} ${BUILD_DEPENDENCIES})
    # Set the link flags
    TARGET_LINK_LIBRARIES(${LIBRARY} ${DEPENDENCIES})
    # Keep a trace of dependencies for main CMake level
    SET(${LIBRARY}_DEPENDENCIES "${DEPENDENCIES}" CACHE STRING "Required libraries for ${LIBRARY}" FORCE)
    MARK_AS_ADVANCED(${LIBRARY}_DEPENDENCIES)
  ENDIF(DEPENDENCIES)


  # Install all header files
  INSTALL_FILES(/include/${LIBRARY}
                ".h"
                ${HEADERS})
  INSTALL_FILES(/include/
                ".h"
                ${LIBRARY_MAIN_HEADER})

ENDIF(BUILD_LIBS_${LIBRARY} OR BUILD_${LIBRARY})

ENDMACRO(CISST_ADD_LIBRARY_TO_PROJECT)




# Macro used to compare required libraries for a given target with
# libraries actually compiled.  This macro adds the required link
# options.
MACRO(CISST_REQUIRES WHO_REQUIRES REQUIRED_CISST_LIBRARIES)
   # First test that all libraries should have been compiled
   FOREACH(required ${REQUIRED_CISST_LIBRARIES})
     IF("${CISST_LIBRARIES}"  MATCHES ${required})
     ELSE("${CISST_LIBRARIES}"  MATCHES ${required})     
       MESSAGE("${WHO_REQUIRES} requires ${required} which doesn't exist or hasn't been compiled")
     ENDIF("${CISST_LIBRARIES}"  MATCHES ${required})
   ENDFOREACH(required)
   # Second, create a list of libraries in the right order
   FOREACH(existing ${CISST_LIBRARIES})
     IF("${REQUIRED_CISST_LIBRARIES}" MATCHES ${existing})
       SET(CISST_LIBRARIES_TO_USE ${CISST_LIBRARIES_TO_USE} ${existing})
     ENDIF("${REQUIRED_CISST_LIBRARIES}" MATCHES ${existing})
   ENDFOREACH(existing)
   # Link with the required libraries
   TARGET_LINK_LIBRARIES(${WHO_REQUIRES} ${CISST_LIBRARIES_TO_USE})
ENDMACRO(CISST_REQUIRES)


# Macro to add all the available tests
MACRO(CISST_ADD_TESTS testProgram)
  # Once the test program is compiled, run it to create a list of available tests
  ADD_CUSTOM_COMMAND(TARGET ${testProgram}
                     POST_BUILD
                     #COMMAND ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${testProgram}
                     COMMAND ${testProgram}
                     ARGS -d > DartTestfile-${testProgram}.txt
                     COMMENT "Generating DartTestfile-${testProgram}.txt")

  # Add the custom build list
  SET_DIRECTORY_PROPERTIES(PROPERTIES TEST_INCLUDE_FILE
                           "DartTestfile-${testProgram}.txt")
ENDMACRO(CISST_ADD_TESTS)


#
# $Log: cisstMacros.cmake,v $
# Revision 1.17  2007/04/26 19:33:56  anton
# All files in libraries: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.16  2006/11/20 20:33:19  anton
# Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
# cisstInteractive, cisstImage and cisstOSAbstraction.
#
# Revision 1.15  2006/05/13 19:33:01  anton
# CMake configuration: Preliminary support for directory names with spaces.
# See ticket #226
#
# Revision 1.14  2006/03/30 19:08:27  anton
# cisstMacros.cmake: Updated so that cisstXyz.h is not changed everytime
# CMake runs (i.e. at each compilation).
#
# Revision 1.13  2005/11/02 14:58:45  anton
# cisstMacros.cmake: Bug in dependency check.  Flags to compile libraries
# are now BUILD_LIBS_xyz (used to be BUILD_xyz).
#
# Revision 1.12  2005/09/06 02:08:14  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.11  2005/05/19 19:29:00  anton
# cisst libs: Added the license to cisstCommon and cisstVector
#
# Revision 1.10  2005/01/18 15:43:21  anton
# cisstMacros.cmake: Yet another attempt to circumvent a CMake internal error
# while creating the main header files.  This time, I use only one WRITE and
# avoid the APPEND.  It seems to work ... for now.
#
# Revision 1.9  2005/01/10 19:17:45  anton
# cisstMacros.cmake:  Use WRITE(FILE ...) and WRITE(APPEND ...) instead of
# FILE_WRITE which seems deprecated.  This will hopefully solve the write
# issues with .net.
#
# Revision 1.8  2004/10/27 14:40:35  anton
# CMake configuration: Added code to compile netlib and reorganized the way
# new header files are created/copied.
#
# Revision 1.7  2004/08/20 20:07:28  anton
# CMake Config: Update the config files to force the use of CMake 2.0.x or
# more (see #40) and cleaned-up the SWIG wrapping process thanks to the new
# CMake macros (see #55).
#
# Revision 1.6  2004/05/04 14:09:16  anton
# Save each library dependencies in the cache for future use
#
# Revision 1.5  2004/03/29 21:20:19  anton
# Added code for SWIG
#
# Revision 1.4  2004/01/30 20:11:56  ofri
# Fixing bug by replacing cisst with ${PROJECT_NAME}
#
# Revision 1.3  2004/01/30 18:59:57  ofri
# Fix syntax error in END of macro
#
# Revision 1.2  2004/01/30 18:53:19  ofri
# Rename the macro and pass a parameter specifying the name of the project to
# which the target is added.
# + Documentation update.
#
# Revision 1.1  2004/01/22 21:55:54  anton
# cmake/cisstMacros.cmake
#
#
