#ifndef SSCVIEWRENDERWINDOW_H_
#define SSCVIEWRENDERWINDOW_H_

/**\class ssc::ViewRenderWindow
 *
 *	This class defaults to the vtkRenderWindow,
 *	but if USE_GLX_SHARED_CONTEXT is set, a
 *	custom ssc RenderWindow is used instead,
 *	with shared gl contexts enabled.
 *
 */

#include "sscConfig.h"
#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include <vtkConfigure.h>


#ifdef USE_GLX_SHARED_CONTEXT
	// typedef to ssc-special renderwindow

	#if ( VTK_MAJOR_VERSION==5 )&&( VTK_MINOR_VERSION < 8 )
		#include "sscSNWXOpenGLRenderWindow_5_4.h"
		namespace ssc
		{
			typedef SNWXOpenGLRenderWindow ViewRenderWindow;
			typedef vtkSmartPointer<SNWXOpenGLRenderWindow> ViewRenderWindowPtr;
		}
	#elif ( VTK_MAJOR_VERSION==5 )&&( VTK_MINOR_VERSION == 8 )
		#include "sscModified_vtkXOpenGLRenderWindow_5_8.h"
		namespace ssc
		{
			typedef sscModified_vtkXOpenGLRenderWindow ViewRenderWindow;
			typedef vtkSmartPointer<sscModified_vtkXOpenGLRenderWindow> ViewRenderWindowPtr;
		}
	#else
		#error "VTK Version not supported"
	#endif
#else
	// typedef to default
	#include <vtkRenderWindow.h>
	namespace ssc
	{
		typedef vtkRenderWindow ViewRenderWindow;
		typedef vtkSmartPointer<ViewRenderWindow> ViewRenderWindowPtr;
	}
#endif



#endif /*SSCVIEWRENDERWINDOW_H_*/
