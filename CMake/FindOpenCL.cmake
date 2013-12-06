## Thanks to git://gitorious.org/findopencl/findopencl.git
## Used by SSC, but not created or owned by SSC.

# - Try to find OpenCL
# This module tries to find an OpenCL implementation on your system. It supports
# AMD / ATI, Apple and NVIDIA implementations, but should work, too.
#
# To set manually the paths, define these environment variables:
# OpenCL_INCPATH    - Include path (e.g. OpenCL_INCPATH=/opt/cuda/4.0/cuda/include)
# OpenCL_LIBPATH    - Library path (e.h. OpenCL_LIBPATH=/usr/lib64/nvidia)
#
# Once done this will define
#  OPENCL_FOUND        - system has OpenCL
#  OPENCL_INCLUDE_DIRS  - the OpenCL include directory
#  OPENCL_LIBRARIES    - link these to use OpenCL
#
# WIN32 should work, but is untested

FIND_PACKAGE(PackageHandleStandardArgs)

IF (APPLE)

	FIND_LIBRARY(OPENCL_LIBRARIES OpenCL DOC "OpenCL lib for OSX")
	FIND_PATH(OPENCL_INCLUDE_DIRS OpenCL/cl.h DOC "Include for OpenCL on OSX")
	FIND_PATH(_OPENCL_CPP_INCLUDE_DIRS OpenCL/cl.hpp DOC "Include for OpenCL CPP bindings on OSX")

ELSE (APPLE)

	IF (WIN32)

		FIND_PATH(OPENCL_INCLUDE_DIRS CL/cl.h)
		FIND_PATH(_OPENCL_CPP_INCLUDE_DIRS CL/cl.hpp)

		# The AMD SDK and Nvidias CUDA currently installs both x86 and x86_64 libraries
		# This is only a hack to find out architecture
		
		IF(CMAKE_SIZEOF_VOID_P EQUAL 8) #64bit
			IF(DEFINED ENV{ATISTREAMSDKROOT})
				SET(OPENCL_LIB_DIR "$ENV{ATISTREAMSDKROOT}/lib/x86_64") #amd
			ELSE(DEFINED ENV{ATISTREAMSDKROOT})
				SET(OPENCL_LIB_DIR "$ENV{CUDA_PATH}/lib/x64") #nvidia
			ENDIF(DEFINED ENV{ATISTREAMSDKROOT})
		ELSE (CMAKE_SIZEOF_VOID_P EQUAL 8) #32bit
			IF(DEFINED ENV{ATISTREAMSDKROOT})
				SET(OPENCL_LIB_DIR "$ENV{ATISTREAMSDKROOT}/lib/x86") #amd
			ELSE(DEFINED ENV{ATISTREAMSDKROOT})
				SET(OPENCL_LIB_DIR "$ENV{CUDA_PATH}/lib/Win32") #nvidia
			ENDIF(DEFINED ENV{ATISTREAMSDKROOT})
		ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)
		FIND_LIBRARY(OPENCL_LIBRARIES OpenCL.lib PATHS ${OPENCL_LIB_DIR} ENV OpenCL_LIBPATH)

		GET_FILENAME_COMPONENT(_OPENCL_INC_CAND ${OPENCL_LIB_DIR}/../../include ABSOLUTE)

		# On Win32 search relative to the library
		FIND_PATH(OPENCL_INCLUDE_DIRS CL/cl.h PATHS "${_OPENCL_INC_CAND}" ENV OpenCL_INCPATH)
		FIND_PATH(_OPENCL_CPP_INCLUDE_DIRS CL/cl.hpp PATHS "${_OPENCL_INC_CAND}" ENV OpenCL_INCPATH)

	ELSE (WIN32)

		# Unix style platforms
		FIND_LIBRARY(OPENCL_LIBRARIES OpenCL
			PATHS ENV LD_LIBRARY_PATH ENV OpenCL_LIBPATH
		)

		GET_FILENAME_COMPONENT(OPENCL_LIB_DIR ${OPENCL_LIBRARIES} PATH)
		GET_FILENAME_COMPONENT(_OPENCL_INC_CAND ${OPENCL_LIB_DIR}/../../include ABSOLUTE)

		# The AMD SDK currently does not place its headers
		# in /usr/include, therefore also search relative
		# to the library
		FIND_PATH(OPENCL_INCLUDE_DIRS CL/cl.h PATHS ${_OPENCL_INC_CAND} "/usr/local/cuda/include" "/opt/AMDAPP/include" ENV OpenCL_INCPATH)
		FIND_PATH(_OPENCL_CPP_INCLUDE_DIRS CL/cl.hpp PATHS ${_OPENCL_INC_CAND} "/usr/local/cuda/include" "/opt/AMDAPP/include" ENV OpenCL_INCPATH)

	ENDIF (WIN32)

ENDIF (APPLE)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenCL DEFAULT_MSG OPENCL_LIBRARIES OPENCL_INCLUDE_DIRS)

IF(_OPENCL_CPP_INCLUDE_DIRS)
	SET( OPENCL_HAS_CPP_BINDINGS TRUE )
	LIST( APPEND OPENCL_INCLUDE_DIRS ${_OPENCL_CPP_INCLUDE_DIRS} )
	# This is often the same, so clean up
	LIST( REMOVE_DUPLICATES OPENCL_INCLUDE_DIRS )
ENDIF(_OPENCL_CPP_INCLUDE_DIRS)

#================================================================================
IF (OPENCL_FOUND)
# Code for determining OpenCL version found:
# https://github.com/bkloppenborg/simtoi/blob/master/CMakeModules/FindOpenCL.cmake

  SET (_OPENCL_VERSION_TEST_SOURCE
"
#if __APPLE__
#include <OpenCL/cl.h>
#else /* !__APPLE__ */
#include <CL/cl.h>
#endif /* __APPLE__ */

#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *version;
    cl_int result;
    cl_platform_id id;
    size_t n;

    result = clGetPlatformIDs(1, &id, NULL);

    if (result == CL_SUCCESS) {
        result = clGetPlatformInfo(id, CL_PLATFORM_VERSION, 0, NULL, &n);

        if (result == CL_SUCCESS) {
            version = (char*)malloc(n * sizeof(char));

            result = clGetPlatformInfo(id, CL_PLATFORM_VERSION, n, version,
                NULL);

            if (result == CL_SUCCESS) {
                printf(\"%s\", version);
                fflush(stdout);
            }

            free(version);
        }
    }

    return result == CL_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}
")

  SET (_OPENCL_VERSION_SOURCE
    "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/openclversion.c")

  FILE (WRITE ${_OPENCL_VERSION_SOURCE} "${_OPENCL_VERSION_TEST_SOURCE}\n")

  TRY_RUN (_OPENCL_VERSION_RUN_RESULT _OPENCL_VERSION_COMPILE_RESULT
    ${CMAKE_BINARY_DIR} ${_OPENCL_VERSION_SOURCE}
    RUN_OUTPUT_VARIABLE _OPENCL_VERSION_STRING
    CMAKE_FLAGS "-DINCLUDE_DIRECTORIES:STRING=${OPENCL_INCLUDE_DIRS}"
                "-DLINK_LIBRARIES:STRING=${OPENCL_LIBRARIES}")

  IF (_OPENCL_VERSION_RUN_RESULT EQUAL 0)
    STRING (REGEX REPLACE "OpenCL[ \t]+([0-9]+)\\.[0-9]+.*" "\\1"
      OPENCL_VERSION_MAJOR "${_OPENCL_VERSION_STRING}")
    STRING (REGEX REPLACE "OpenCL[ \t]+[0-9]+\\.([0-9]+).*" "\\1"
      OPENCL_VERSION_MINOR "${_OPENCL_VERSION_STRING}")

    SET (OPENCL_VERSION_COMPONENTS 2)
    SET (OPENCL_VERSION "${OPENCL_VERSION_MAJOR}.${OPENCL_VERSION_MINOR}")
  ENDIF (_OPENCL_VERSION_RUN_RESULT EQUAL 0)

  IF ("${OPENCL_VERSION}" STREQUAL "")
    MESSAGE (WARNING "Cannot determine OpenCL's version")
  ENDIF ("${OPENCL_VERSION}" STREQUAL "")
ENDIF (OPENCL_FOUND)
#================================================================================

MARK_AS_ADVANCED(
  OPENCL_INCLUDE_DIRS
)
