#include "sscGLHelpers.h"
//#include "SonoWand.h"

#ifndef WIN32
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>

void fgl_really_report_gl_errors( const char *file, int line )
{
	GLenum error;
	int i = 0;

	while ( ( error = glGetError () ) != GL_NO_ERROR  && i < 20 )
	{
		printf( "Oops, GL error caught: %s %s:%d\n", gluErrorString( error ), file, line );
		++i;
	}
	if ( i != 0 )
	{
		abort();
	}
}
#endif //WIN32