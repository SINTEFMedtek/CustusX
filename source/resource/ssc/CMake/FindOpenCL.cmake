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

SET (OPENCL_VERSION_STRING "0.1.0")
SET (OPENCL_VERSION_MAJOR 0)
SET (OPENCL_VERSION_MINOR 1)
SET (OPENCL_VERSION_PATCH 0)

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
		IF( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64" )
			IF(DEFINED ENV{ATISTREAMSDKROOT})
				SET(OPENCL_LIB_DIR "$ENV{ATISTREAMSDKROOT}/lib/x86_64") #amd
			ELSE(DEFINED ENV{ATISTREAMSDKROOT})
				SET(OPENCL_LIB_DIR "$ENV{CUDA_PATH}/lib/x64") #nvidia
			ENDIF(DEFINED ENV{ATISTREAMSDKROOT})
		ELSE (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
			IF(DEFINED ENV{ATISTREAMSDKROOT})
				SET(OPENCL_LIB_DIR "$ENV{ATISTREAMSDKROOT}/lib/x86") #amd
			ELSE(DEFINED ENV{ATISTREAMSDKROOT})
				SET(OPENCL_LIB_DIR "$ENV{CUDA_PATH}/lib/Win32") #nvidia
			ENDIF(DEFINED ENV{ATISTREAMSDKROOT})
		ENDIF( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64" )
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

MARK_AS_ADVANCED(
  OPENCL_INCLUDE_DIRS
)
