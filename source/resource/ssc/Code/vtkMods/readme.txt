***********************************************************
vtk Modifications for the SSC Library
***********************************************************

This folder contains modifications of vtk files that are 
needed to use the same gl context in several vtkRenderWindows.

The files are prefixed with sscModified and postfixed with vtk version:

File: vtkXOpenGLRenderWindow.h    -> sscModified_vtkXOpenGLRenderWindow_X_Y.h
File: vtkXOpenGLRenderWindow.cpp  -> sscModified_vtkXOpenGLRenderWindow_X_Y.cpp
File: QVTKWidget.h                -> sscSNWQVTKWidget_X_Y.h (for vtk5.6 and below)
File: QVTKWidget.cpp              -> sscSNWQVTKWidget_X_Y.cpp (for vtk5.6 and below)

For each new release of vtk, these files must be generated anew!

The file sscViewRenderWindow.h/cpp and ssvViewQVTKWidget.h/cpp 
contains the preprocessor code to include the the correct 
file versions. These files (only) should be included into 
the user code.


***********************************************************
***********************************************************



***********************************************************
File: QVTKWidget.h                -> sscSNWQVTKWidget_X_Y.h (for vtk5.6 and below)
File: QVTKWidget.cpp              -> sscSNWQVTKWidget_X_Y.cpp (for vtk5.6 and below)

For vtk5.8 and up, this QVTKWidget is not modified, because it is 
difficult to extract from the library.

Modifications:

 *** Rename class QVTKWidget -> sscModified_QVTKWidget
 *** Replace 
	#include "vtkXOpenGLRenderWindow.h"
	 ->
	#include "sscModifiedvtkXOpenGLRenderWindow.h"
 *** Replace 
	vtkXOpenGLRenderWindow* ogl_win = vtkXOpenGLRenderWindow::SafeDownCast(mRenWin);
	 ->
	sscModifiedvtkXOpenGLRenderWindow* ogl_win = sscModifiedvtkXOpenGLRenderWindow::SafeDownCast(mRenWin); 

***********************************************************
File: vtkXOpenGLRenderWindow.h    -> sscModified_vtkXOpenGLRenderWindow.h
Modifications:

 *** Rename class vtkXOpenGLRenderWindow -> sscModified_vtkXOpenGLRenderWindow

***********************************************************
File: vtkXOpenGLRenderWindow.cpp  -> sscModified_vtkXOpenGLRenderWindow.cpp
Modifications:

 *** Rename class vtkXOpenGLRenderWindow -> sscModified_vtkXOpenGLRenderWindow

 *** Add to top of file:
// SSC modification: define a shared gl context for all render windows
static GLXContext sscSharedGLXContext = 0;

 *** Add to destructor:
  // SSC modification: clear the shared gl context
  sscSharedGLXContext = 0;
 
 *** Change in createAWindow(): 

  if (!this->Internal->ContextId)
    {
	  // SSC modification: glXCreateContext with share list 
	  if(!sscSharedGLXContext)
		{
			this->Internal->ContextId = glXCreateContext(this->DisplayId, v, 0, GL_TRUE);
			sscSharedGLXContext = this->Internal->ContextId;
		}
		else
		{
		    this->Internal->ContextId = glXCreateContext(this->DisplayId, v, sscSharedGLXContext, GL_TRUE);
		}
    }

 *** Change:
          int loaded = vtkgl::LoadExtension("GLX_VERSION_1_3", manager);
     to:
     	  // SSC modification: Should not be necessary, but we run into a linker problem here.
          int loaded = manager->LoadSupportedExtension("GLX_VERSION_1_3");


***********************************************************
