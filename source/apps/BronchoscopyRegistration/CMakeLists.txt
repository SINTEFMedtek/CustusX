cmake_minimum_required( VERSION 2.6 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# CX C++ Test App for Qt fun stuff 
# =========================================================

find_package(SSC REQUIRED)
include (${SSC_USE_FILE})

# library
SET( SOURCE_FILES
    #BronchoscopyRegistrationMain.h
    BronchoscopyRegistration.h
    BronchoscopyRegistration.cpp
    Branch.h
    Branch.cpp
    BranchList.h
    BranchList.cpp
    PositionData.cpp
    PositionData.h
    #RegistrationFunctions.cpp
    #RegistrationFunctions.h
    BronchoscopyRegistrationMain.cpp
	) 
	
include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR} )	
		
# =========================================================
# create the target
add_executable( BronchoscopyRegistration MACOSX_BUNDLE 
    ${SOURCE_FILES} )
target_link_libraries( BronchoscopyRegistration SSC )
# =========================================================


