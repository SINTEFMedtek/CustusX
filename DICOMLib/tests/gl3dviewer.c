#define GL_GLEXT_PROTOTYPES 1

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/Xlib.h>
#include <unistd.h>

#include "sscLogger.h"
#include "DICOMLib.h"

static int setup_window( int x, int y, int width, int height );

static Display *dpy;
static GLXWindow gwin;
static GLXContext cx;
static Window win;

#define report_gl_error() fgl_really_report_gl_errors(__FILE__, __LINE__)
void fgl_really_report_gl_errors( const char *file, int line )
{
	GLenum error;
	int i = 0;
	while ( ( error = glGetError () ) != GL_NO_ERROR  && i < 20 )
	{
		printf( "Oops, GL error caught: %s %s:%d\n", gluErrorString( error ), file, line );
		++i;
	}
	assert( i == 0 );
}

int show_2D_image( const char *image, uint32_t x, uint32_t y, int samples, int bits )
{
	GLenum size;

	switch ( bits )
	{
	case 8: size = GL_UNSIGNED_BYTE; break;
	case 16: size = GL_UNSIGNED_SHORT; break;
	default: size = GL_UNSIGNED_BYTE; printf("Bad bits %d\n", bits); assert(false); break;
	}

	switch ( samples )
	{
	case 1: glDrawPixels( x, y, GL_LUMINANCE, size, image ); break;
	case 2: glDrawPixels( x, y, GL_LUMINANCE_ALPHA, size, image ); break;
	case 3: glDrawPixels( x, y, GL_RGB, size, image ); break;
	case 4: glDrawPixels( x, y, GL_RGBA, size, image ); break;
	}

	glXSwapBuffers( dpy, gwin );
	report_gl_error();

	return 0;
}

int main ( int argc, const char * argv[] )
{
	int i;
	struct study_t *study;
	struct series_t *series;
	const struct volume_t *data = NULL;
	const char *ptr;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: gl3dviewer <path>\n" );
		exit( 1 );
	}

	SSC_Logging_Init_Default( "com.sonowand.test" );
	DICOMLib_Init();

	study = DICOMLib_StudiesFromPath( argv[1], NULL, DICOMLIB_NO_CACHE );
	if ( !study )
	{
		fprintf( stderr, "Nothing found!\n" );
		SSC_Logging_Done();
		exit( 1 );
	}

	series = DICOMLib_GetSeries( study, NULL );
	assert( series );

	while ( series && !data )
	{
		DICOMLib_UseAutoVOI( series );
		data = DICOMLib_GetVolume( series, NULL );
		if ( !data )
		{
			fprintf( stderr, "Skipping %s series %s with %d frames: %s\n", series->modality, series->seriesID, series->frames, series->series_info );
			series = series->next_series;
		}
	}
	if ( !series )
	{
		fprintf( stderr, "No useful series found.\n" );
		SSC_Logging_Done();
		exit( 1 );
	}

	setup_window( 0, 0, data->x, data->y );

	ptr = data->volume;
	printf("Size=%d,%d,%d samples=%d\n", data->x, data->y, data->z, data->samples_per_pixel);
	for ( i = 0; i < series->frames; i++ )
	{
		show_2D_image( ptr, data->x, data->y, data->samples_per_pixel, data->bits_per_sample );
		printf( "Showing image %d\n", i );
		usleep( 100000 );
		ptr += data->x * data->y * data->samples_per_pixel * (data->bits_per_sample / 8);
	}
	assert( i == series->frames );

	glXMakeCurrent( dpy, None, NULL );
	glXDestroyContext( dpy, cx );
	XDestroyWindow( dpy, win );
	DICOMLib_CloseStudies( study );
	DICOMLib_Done();
	SSC_Logging_Done();

	return 0;
}

static int setup_window( int x, int y, int width, int height )
{
	short w, h;
	GLfloat fAspect;
	GLXFBConfig *fbc;
	XVisualInfo *vi;
	Colormap cmap;
	XSetWindowAttributes swa;
	int nelements;

	dpy = XOpenDisplay( 0 );

	/* Find a FBConfig that uses RGBA.  Note that no attribute list is */
	/* needed since GLX_RGBA_BIT is a default attribute.               */
	fbc = glXChooseFBConfig( dpy, DefaultScreen( dpy ), 0, &nelements );
	vi = glXGetVisualFromFBConfig( dpy, fbc[0] );

	/* Create a GLX context using the first FBConfig in the list. */
	cx = glXCreateNewContext( dpy, fbc[0], GLX_RGBA_TYPE, 0, GL_FALSE );

	/* Create a colormap */
	cmap = XCreateColormap( dpy, RootWindow( dpy, vi->screen ), vi->visual, AllocNone );

	/* Create a window */
	swa.colormap = cmap;
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;
	win = XCreateWindow( dpy, RootWindow( dpy, vi->screen ), x, y, width, height, 0, vi->depth, InputOutput,
	                     vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa );
	XMapWindow( dpy, win );

	/* Create a GLX window using the same FBConfig that we used for the */
	/* the GLX context.                                                 */
	gwin = glXCreateWindow( dpy, fbc[0], win, 0 );

	/* Connect the context to the window for read and write */
	glXMakeContextCurrent( dpy, gwin, gwin, cx );

	/* Assume correct - too lazy to check on Linux! */
	w = width;
	h = height;

	// Prevent a divide by zero
	if ( h == 0 )
	{
		h = 1;
	}
	fAspect = ( GLfloat ) w / ( GLfloat ) h;

	// Set Viewport to window dimensions
	glViewport( 0, 0, ( GLsizei ) w, ( GLsizei ) h );
	glMatrixMode( GL_PROJECTION );

	// Reset coordinate system
	glLoadIdentity();

	// Setup perspective for viewing
	gluPerspective( 45.0, fAspect, 0.1, 1 );

	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_PACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_PACK_SKIP_ROWS, 0 );
	glPixelStorei( GL_PACK_SKIP_PIXELS, 0 );

	return 0;
}
