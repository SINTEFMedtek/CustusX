#
# Source files
#

SET (Ultrasound_SRCS
  vtkSonixVideoSource.h
  vtkSonixVideoSource.cxx
  vtkUltrasoundWin32Header.h
)

#
# Libraries (starts empty)
#

SET (Ultrasound_LIBS
  )
 
#
# Look for SonixRP Ulterius SDK
#
SET (ULTERIUS_ROOT "C:/Ulterius")
FIND_PATH(ULTERIUS_INCLUDE_DIR "ulterius.h" "${ULTERIUS_ROOT}/inc")
FIND_LIBRARY(ULTERIUS_LIBRARY ulterius "${ULTERIUS_ROOT}/lib")
INCLUDE_DIRECTORIES(${ULTERIUS_INCLUDE_DIR})
SET (Ultrasound_LIBS ${Ultrasound_LIBS} ${ULTERIUS_LIBRARY})


# --------------------------------------------------------------------------
# You probably do not need to modify anything below this line

# Create the vtkOpenIGTUltrasound C++ library
ADD_LIBRARY (vtkOpenIGTUltrasound ${Ultrasound_SRCS})
TARGET_LINK_LIBRARIES(vtkOpenIGTUltrasound vtkCommon vtkHybrid vtkParallel
                      ${Ultrasound_LIBS})

# Create the vtkOpenIGTUltrasound Tcl library
IF (VTK_WRAP_TCL AND VTKOPENIGT_WRAP_TCL)
  VTK_WRAP_TCL3 (vtkOpenIGTUltrasoundTCL UltrasoundTCL_SRCS
                 "${Ultrasound_SRCS}" "")
  ADD_LIBRARY (vtkOpenIGTUltrasoundTCL ${UltrasoundTCL_SRCS})
  TARGET_LINK_LIBRARIES (vtkOpenIGTUltrasoundTCL vtkCommonTCL
                         vtkOpenIGTUltrasound vtkHybridTCL)
ENDIF (VTK_WRAP_TCL AND VTKOPENIGT_WRAP_TCL)

# Create the vtkOpenIGTUltrasound Python library.
IF (VTK_WRAP_PYTHON AND VTKOPENIGT_WRAP_PYTHON)
  VTK_WRAP_PYTHON3 (vtkOpenIGTUltrasoundPython UltrasoundPython_SRCS
                    "${Ultrasound_SRCS}")
  ADD_LIBRARY (vtkOpenIGTUltrasoundPythonD ${UltrasoundPython_SRCS})
  ADD_LIBRARY (vtkOpenIGTUltrasoundPython MODULE
               vtkOpenIGTUltrasoundPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkOpenIGTUltrasoundPythonD vtkOpenIGTUltrasound
                        vtkOpenIGTCommonPythonD vtkHybrid vtkHybridPythonD)
  TARGET_LINK_LIBRARIES(vtkOpenIGTUltrasoundPython vtkOpenIGTUltrasoundPythonD)
ENDIF (VTK_WRAP_PYTHON AND VTKOPENIGT_WRAP_PYTHON)

# Create the vtkOpenIGTUltrasound Java library.
IF (VTK_WRAP_JAVA AND VTKOPENIGT_WRAP_JAVA)
  VTK_WRAP_JAVA3 (vtkOpenIGTUltrasoundJava UltrasoundJava_SRCS
                  "${Ultrasound_SRCS}")
  ADD_LIBRARY (vtkOpenIGTUltrasoundJava SHARED ${UltrasoundJava_SRCS})
  TARGET_LINK_LIBRARIES (vtkOpenIGTUltrasoundJava vtkOpenIGTCommonJava
                         vtkOpenIGTUltrasound vtkHybridJava)
ENDIF (VTK_WRAP_JAVA AND VTKOPENIGT_WRAP_JAVA)
