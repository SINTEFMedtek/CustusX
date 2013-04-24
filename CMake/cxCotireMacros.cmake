# This file is part of SSC,
# a C++ Library supporting Image Guided Therapy Applications.
#
# Copyright (C) 2008- SINTEF Medical Technology
# Copyright (C) 2008- Sonowand AS
#
# SSC is owned by SINTEF Medical Technology and Sonowand AS,
# hereafter named the owners. Each particular piece of code
# is owned by the part that added it to the library.
# SSC source code and binaries can only be used by the owners
# and those with explicit permission from the owners.
# SSC shall not be distributed to anyone else.
#
# SSC is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.
#
# See sscLicense.txt for more information.

#-----------------------------------------------------------------------------
# Cotire (COmpile TIme REducer)
# Cross-platform cmake module for reducing compile time by generating
# precomiled headers (pch) and single compilation unit builds
# Go to https://github.com/sakra/cotire for more information
#-----------------------------------------------------------------------------
macro(CX_COTIRE_INITIALIZE)
	option(COTIRE_ENABLE_PRECOMPILED_HEADERS "use cotire to autogenerate precompiled headers (pch)" OFF)
	option(COTIRE_ADD_UNITY_BUILDS "use cotire to autogenerate single comilation unit builds" OFF)

	if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
		include(cotire)
		# The properties COTIRE_ENABLE_PRECOMPILED_HEADER and COTIRE_ADD_UNITY_BUILD can also be set on targets.
		# A target inherits the property value from its enclosing directory.
		set_directory_properties(
					PROPERTIES
					COTIRE_ENABLE_PRECOMPILED_HEADER ${COTIRE_ENABLE_PRECOMPILED_HEADERS}
					COTIRE_ADD_UNITY_BUILD ${COTIRE_ADD_UNITY_BUILDS}
					)
	endif()
endmacro()

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
#
# Add the input library to the cotire build system. Also insert all the link libraries
# required by the library
# Input:
#   LIBRARY_NAME : The library to add
#   LINK_LIBRARIES : List of all link libraries. Remember to add it in hyphens when calling function ( "${LINK_LIBRARIES}" )
#
macro(CX_COTIRE_ADD_LIBRARY LIBRARY_NAME LINK_LIBRARIES )
	if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
		cotire(${LIBRARY_NAME})
		if(COTIRE_ADD_UNITY_BUILDS)
			target_link_libraries(${LIBRARY_NAME}_unity
					${LINK_LIBRARIES}
					)
		endif(COTIRE_ADD_UNITY_BUILDS)
	endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
endmacro()

