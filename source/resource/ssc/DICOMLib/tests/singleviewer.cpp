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

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dctag.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcddirif.h"
#include "dcmtk/dcmdata/dcdirrec.h"
#include "dcmtk/dcmimage/diregist.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdicent.h"

#include "sscLogger.h"

#define WINX 640
#define WINY 640

static Display *dpy;
static GLXWindow gwin;

void *DICOM_image( const char *filename, int bits, int frame, int *x, int *y )
{
        const int planar = 0; /* interleave colour data, not separate; not that we use this for our monochrome data */
	DicomImage dicomimage( filename );
        EI_Status status = dicomimage.getStatus();
	void *buffer = NULL;
	int size;

        if ( status != EIS_Normal )
        {
		printf( "Error from DCMTK reading %s: %s\n", filename, dicomimage.getString( status ) );
                return NULL;
        }

        dicomimage.setPresentationLutShape( ESP_Default );
        dicomimage.hideAllOverlays();
	dicomimage.setNoVoiTransformation();

	*x = dicomimage.getWidth();
	*y = dicomimage.getHeight();
	size = dicomimage.getOutputDataSize( bits );
	buffer = malloc( size );
	if ( !dicomimage.getOutputData( buffer, size, bits, frame, planar ) )
	{
		printf( "FAILED TO GET DATA\n" );
		return NULL;
	}
	return buffer;
}

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

	return 0;
}

int show_image( const void *image, uint32_t x, uint32_t y )
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ); // test
	glClear( GL_COLOR_BUFFER_BIT );
	glDisable( GL_TEXTURE_2D );

	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_PACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_PACK_SKIP_ROWS, 0 );
	glPixelStorei( GL_PACK_SKIP_PIXELS, 0 );
	glDrawPixels( x, y, GL_LUMINANCE, GL_UNSIGNED_BYTE, image );
	glFinish();

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
	int x = 0, y = 0;
	void *buffer;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: singleviewer <path>\n" );
		exit( 1 );
	}

	if ( SSC_Logging_Init_Default( "com.sonowand.test" ) != 0 )
	{
		printf( "Failed SW_Init!\n" );
	}
	buffer = DICOM_image( argv[1], 8, 0, &x, &y );
	if ( !buffer )
	{
		printf( "Failed to get image!\n" );
		SSC_Logging_Done();
		exit( 1 );
	}
	setup_window( 0, 0, x, y );
	printf( "Displaying file: %s\n", argv[1] );
	show_image( buffer, x, y );
	sleep( 3 );
	close_window();
	SSC_Logging_Done();

	return 0;
}
