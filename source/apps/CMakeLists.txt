cmake_minimum_required(VERSION 2.6)

ADD_SUBDIRECTORY(UsReconstruction)

IF(APPLE)
    ADD_EXECUTABLE ( reconstruct apps/cxReconstructMain.cpp )
    TARGET_LINK_LIBRARIES ( reconstruct
      3DUSReconstruction
      ${LINK_LIBRARIES}
      )
ENDIF(APPLE)
      