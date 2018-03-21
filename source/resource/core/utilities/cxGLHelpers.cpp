/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxGLHelpers.h"

#ifndef WIN32
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>

void really_report_gl_errors( const char *file, int line, const char* text )
{
	GLenum error;
	int i = 0;

	while ( ( error = glGetError () ) != GL_NO_ERROR  && i < 20 )
	{
		printf( "GL error caught: Code: [%i] %s\n\tLocation:%s:%d\n\tDescription:%s\n", error, gluErrorString( error ), file, line, text );
		++i;
	}
}
#endif //WIN32
