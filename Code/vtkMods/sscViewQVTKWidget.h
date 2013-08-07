#ifndef SSCVIEWQVTKWIDGET_H_
#define SSCVIEWQVTKWIDGET_H_

/**\class ssc::ViewParent
 *
 *	This class defaults to the QVTKWidget,
 *	but if USE_GLX_SHARED_CONTEXT is set, a
 *	custom ssc QVTKWidget is used instead.
 */

/* NOTE: the custom qvtkwidget is removed for vtk 5.8+ because it is
 * a real hassle to extract from QVTK.so. Rather use the default and
 * hope that the X11-hack inside QVTKWidget is unnecessary.
 */


#include "sscConfig.h"
#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include <vtkConfigure.h>

#ifdef USE_GLX_SHARED_CONTEXT
	// typedef to ssc-special QVTKWidget
	#if ( VTK_MAJOR_VERSION==5 )
		#if VTK_MINOR_VERSION == 4
			#include "sscSNWQVTKWidget5_4.h"
			namespace ssc
			{
				typedef SNWQVTKWidget ViewQVTKWidget;
			}
		#elif VTK_MINOR_VERSION == 5
			#include "sscSNWQVTKWidget5_5.h"
			namespace ssc
			{
				typedef SNWQVTKWidget ViewQVTKWidget;
			}
		#elif VTK_MINOR_VERSION == 6
			#include "sscSNWQVTKWidget5_5.h"
			namespace ssc
			{
				typedef SNWQVTKWidget ViewQVTKWidget;
			}
		#elif VTK_MINOR_VERSION >= 8
			// typedef to default
			#include "QVTKWidget.h"
//			#include "sscModified_QVTKWidget_5_8.h"
			namespace ssc
			{
				typedef QVTKWidget ViewQVTKWidget;
//				typedef sscModified_QVTKWidget ViewQVTKWidget;
			}
//		#else
//			#error "VTK Version not supported"
		#endif
//	#else
//		#error "VTK Version not supported"
	#endif
#else
	// typedef to default
	#include "QVTKWidget.h"
	namespace ssc
	{
		typedef QVTKWidget ViewQVTKWidget;
	}
#endif

#endif /*SSCVIEWQVTKWIDGET_H_*/
