#include "sscConfig.h"
#include <vtkConfigure.h>

#ifdef USE_GLX_SHARED_CONTEXT
	// typedef to ssc-special renderwindow

	#if ( VTK_MAJOR_VERSION==5 )&&( VTK_MINOR_VERSION < 8 )
		#include "sscSNWXOpenGLRenderWindow_5_4.cpp"
	#elif ( VTK_MAJOR_VERSION==5 )&&( VTK_MINOR_VERSION == 8 )
		#include "sscModified_vtkXOpenGLRenderWindow_5_8.cxx"
	#elif ( VTK_MAJOR_VERSION==5 )&&( VTK_MINOR_VERSION == 10 )&&( VTK_BUILD_VERSION == 1)
		#include "sscModified_vtkXOpenGLRenderWindow_5_10_1.cxx"
	#else
		#error "VTK Version not supported"
	#endif
#endif
