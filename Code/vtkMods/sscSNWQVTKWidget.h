#include <vtkConfigure.h>

#if VTK_MINOR_VERSION == 4
	#include "sscSNWQVTKWidget5_4.h"
#elif VTK_MINOR_VERSION == 5
	#include "sscSNWQVTKWidget5_5.h"
#elif VTK_MINOR_VERSION == 6
	#include "sscSNWQVTKWidget5_5.h"
#else
	#error "VTK Version not supported"
#endif
