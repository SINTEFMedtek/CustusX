#include "sscSNWQVTKWidget.h"


#if VTK_MINOR_VERSION == 4
	#include "sscSNWQVTKWidget5_4.cpp"
#elif VTK_MINOR_VERSION == 5
	#include "sscSNWQVTKWidget5_5.cpp"
#else
	#error "VTK Version not supported"
#endif



