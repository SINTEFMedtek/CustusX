#include <assert.h>
#include <stdlib.h>
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

static Display *dpy;
static GLXWindow gwin;

static int setup_window( int x, int y, int width, int height )
{
	short w, h;
	GLfloat fAspect;
	GLXContext cx;
	GLXFBConfig *fbc;
	XVisualInfo *vi;
	Colormap cmap;
	XSetWindowAttributes swa;
	Window win;
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

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ); // test
	glClear( GL_COLOR_BUFFER_BIT );
	glDisable( GL_TEXTURE_2D );
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_PACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_PACK_SKIP_ROWS, 0 );
	glPixelStorei( GL_PACK_SKIP_PIXELS, 0 );

	return 0;
}

int show_image( const void *image, uint32_t x, uint32_t y, int samples )
{
	switch ( samples )
	{
	case 1:	glDrawPixels( x, y, GL_LUMINANCE, GL_UNSIGNED_BYTE, image ); break;
	case 2:	glDrawPixels( x, y, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, image ); break;
	case 3:	glDrawPixels( x, y, GL_RGB, GL_UNSIGNED_BYTE, image ); break;
	case 4:	glDrawPixels( x, y, GL_RGBA, GL_UNSIGNED_BYTE, image ); break;
	}

	glXSwapBuffers( dpy, gwin );

	return 0;
}

static int close_window( void )
{
	// Whatever! Let the OS clean up after us.
	return 0;
}

int main ( int argc, const char * argv[] )
{
	struct study_t *study;
	struct series_t *series;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: gl2dviewer <path>\n" );
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
	if ( !series )
	{
		fprintf( stderr, "No series found!\n" );
		SSC_Logging_Done();
		exit( 1 );
	}

	printf( "Displaying study: %s\n", study->studyID );
	while ( series != NULL )
	{
		int frame;
		double c, w;

		DICOMLib_UseAutoVOI( series );
		DICOMLib_GetWindow( series, &c, &w );
		printf( "  Displaying series %s (%d frames, %.02f center %.02f width)\n", series->seriesID, series->frames, c, w );
		for (frame = 0; frame < series->VOI.numPresets; frame++)
		{
			printf( "\tPreset %d: %s center=%.02f width=%.02f\n", frame, series->VOI.preset[frame].explanation, series->VOI.preset[frame].window.center, series->VOI.preset[frame].window.width );
		}
		setup_window( 0, 0, series->columns, series->rows );

		/* Display image */
		for ( frame = 0; series->valid && frame < series->frames; frame++ )
		{
			int x = series->columns;
			int y = series->rows;
			void *buffer = DICOMLib_Image( series, &x, &y, 8, frame );

			assert( series->columns == x && series->rows == y );
			if ( !buffer )
			{
				close_window();
				abort();
			}

			show_image( buffer, x, y, series->samples_per_pixel );
			usleep( 30000 );
			free(buffer);
		}
		close_window();

		series = series->next_series;
	}
	DICOMLib_CloseStudies( study );

	DICOMLib_Done();

	SSC_Logging_Done();

	return 0;
}
