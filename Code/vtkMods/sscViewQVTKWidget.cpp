#include "sscConfig.h"
#include <vtkConfigure.h>

#ifdef USE_GLX_SHARED_CONTEXT
	// typedef to ssc-special QVTKWidget
	#if ( VTK_MAJOR_VERSION==5 )
		#if VTK_MINOR_VERSION == 4
			#include "sscSNWQVTKWidget5_4.cpp"
		#elif VTK_MINOR_VERSION == 5
			#include "sscSNWQVTKWidget5_5.cpp"
		#elif VTK_MINOR_VERSION == 6
			#include "sscSNWQVTKWidget5_5.cpp"
		#endif
	#endif
#endif
