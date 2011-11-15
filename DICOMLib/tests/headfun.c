// FIXME: It uses non-POT 3D textures, which are probably far slower than POT 3D textures.
// FIXME: Aspect ratios

#define GL_GLEXT_PROTOTYPES 1

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <sys/time.h>
#include <math.h>

#include "sscLogger.h"
#include "DICOMLib.h"

static GLfloat sizes[3];
static GLuint texnames[1];
static GLuint shader, program;
static GLfloat theta = 0.0f;
static struct timeval prev;
static int frames = 0;
static int effect = 0;
static GLint llr, scale;

static const char *shaderString = \
"uniform sampler3D tex;"\
"uniform float llr;"\
"uniform float scale;"\
"void main()"\
"{"\
"       vec4 col = texture3D(tex, gl_TexCoord[0].xyz);"\
"       if (col.r <= llr)"\
"       {"\
"		discard;"\
"       }"\
"	else"\
"	{"\
"		col.r = (col.r - llr) * scale;"\
"		col.g = (col.g - llr) * scale;"\
"		col.b = (col.b - llr) * scale;"\
"		gl_FragColor = col;"\
"        }"\
"}";

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
	if ( i != 0 )
	{
		abort();
	}
}

void printShaderInfoLog( GLuint shader )
{
	int infologLen = 0;

	glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infologLen );
	if ( infologLen > 1 )
	{
		int charsWritten = 0;
		GLchar *infoLog = malloc( infologLen );

		glGetShaderInfoLog( shader, infologLen, &charsWritten, infoLog );
		printf( "InfoLog: \n%s\n\n", infoLog );
		free( infoLog );
	}
}

static void setShaders( void )
{
	shader = glCreateShader( GL_FRAGMENT_SHADER );
	if ( shader == 0 )
	{
		printf( "Failed to create OpenGL shader!\n" );
		abort();
	}
	glShaderSource( shader, 1, ( const char ** )&shaderString, NULL );

	glCompileShader( shader );
	printShaderInfoLog( shader );
	program = glCreateProgram();
	if ( program == 0 )
	{
		printf( "Failed to create OpenGL shader program!\n" );
		abort();
	}
	glAttachShader( program, shader );
	glLinkProgram( program );
	glUseProgram( program );
	llr = glGetUniformLocation(program, "llr");
	scale = glGetUniformLocation(program, "scale");
	report_gl_error();
}

void upload_image( const char *image, uint32_t x, uint32_t y, uint32_t z, int bits )
{
	assert( image != NULL );

	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_PACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_PACK_SKIP_ROWS, 0 );
	glPixelStorei( GL_PACK_SKIP_PIXELS, 0 );

	glDisable( GL_TEXTURE_1D );
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_TEXTURE_3D );
	glClear( GL_COLOR_BUFFER_BIT );
	glEnable( GL_BLEND );

	glGenTextures( 1, texnames );
	glBindTexture( GL_TEXTURE_3D, texnames[0] );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	if ( bits == 8 )
	{
		glTexImage3D( GL_TEXTURE_3D, 0, GL_LUMINANCE, x, y, z, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, image );
	}
	else
	{
		glTexImage3D( GL_TEXTURE_3D, 0, GL_LUMINANCE16, x, y, z, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, image );
	}

	sizes[0] = x;
	sizes[1] = y;
	sizes[2] = z;

	report_gl_error();
}

static void draw_cut( GLfloat x, GLfloat y, GLfloat z, const GLfloat texCoords[] )
{
	const GLfloat vertices[] = { -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0 };

	glLoadIdentity();
	glTranslatef( x, y, z );
	glBegin( GL_QUADS );
	glTexCoord3fv( &texCoords[0] );
	glVertex2fv( &vertices[0] );
	glTexCoord3fv( &texCoords[3] );
	glVertex2fv( &vertices[2] );
	glTexCoord3fv( &texCoords[6] );
	glVertex2fv( &vertices[4] );
	glTexCoord3fv( &texCoords[9] );
	glVertex2fv( &vertices[6] );
	glEnd();
}

static void show_image( void )
{
	GLfloat depth = theta;
	struct timeval now;
	int i;
	const GLfloat slices = 150.0;
	GLfloat xrot = depth * 360;
	GLfloat yrot = depth * 360;
	GLfloat zrot = depth * 360;

	glClear( GL_COLOR_BUFFER_BIT );
	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();
	glTranslatef( 0.5f, 0.5f, 0.5f );
	glRotatef( xrot, 1.0f, 0.0f, 0.0f );
	glRotatef( yrot, 0.0f, 1.0f, 0.0f );
	glRotatef( zrot, 0.0f, 0.0f, 1.0f );
	glMatrixMode( GL_PROJECTION );

	for ( i = 1; i < slices; i++ )
	{
		GLfloat texCoords[] = { -0.8, -0.8, 0.5, 0.8, -0.8, 0.5, 0.8, 0.8, 0.5, -0.8, 0.8, 0.5 };

		if ( effect == 3 )
		{
			glBlendColorEXT( 1.f, 1.f, 1.f, 1.f / i );
		}
		texCoords[2] = ( float )i / slices;
		texCoords[5] = ( float )i / slices;
		texCoords[8] = ( float )i / slices;
		texCoords[11] = ( float )i / slices;

		draw_cut( 0.0, 0.0, -( ( float )i / slices ), texCoords );
	}

	theta += 0.01f;
	if ( theta > 1.0f )
	{
		theta = 0.0f;
	}

	gettimeofday( &now, NULL );
	frames++;
	if ( now.tv_sec > prev.tv_sec )
	{
		prev = now;
		printf( "fps: %d\n", frames );
		frames = 0;
	}

	glutSwapBuffers();
}

static void changeSize( int w, int h )
{
	float ratio;

	if ( h == 0 ) h = 1;
	ratio = 1.0 * w / h;

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glViewport( 0, 0, w, h );

	gluPerspective( 45, ratio, 0.1, 100 );
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glColor3f( 1.0, 1.0, 1.0 );
}

static void processSpecialKeys( int key, int x, int y )
{
	if ( key >= 1 && key <= 12 )
	{
		effect = key;	// FXX - keys

		// Reset
		glBlendEquationEXT( GL_FUNC_ADD );
		glBlendFunc( GL_ONE, GL_ZERO );

		switch ( effect )
		{
		case 1:
			break;	// default
		case 2:
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			break;
		case 3:
			glBlendFunc( GL_CONSTANT_ALPHA_EXT, GL_ONE );	// also a condition in the loop
			break;
		case 4:
			glBlendFunc( GL_CONSTANT_ALPHA_EXT, GL_ONE );
			glBlendEquationEXT( GL_MAX_EXT );
			break;
		case 5:
			glBlendFunc( GL_ONE_MINUS_DST_COLOR, GL_DST_COLOR );
			break;
		case 6:
			glBlendFunc( GL_ONE_MINUS_SRC_COLOR, GL_SRC_COLOR );
			break;
		case 7:
			glBlendFunc( GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR );
			break;
		}
	}
}

static void processNormalKeys( unsigned char key, int x, int y )
{
	if ( key == 27 )
	{
		DICOMLib_Done();
		glDeleteProgram( program );
		glDeleteShader( shader );
		SSC_Logging_Done();
		exit( 0 );
	}
}

int main ( int argc, char **argv )
{
	struct study_t *study;
	const struct volume_t *volume;
	float reject, factor, step, size;

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( 1600, 1200 );
	glutCreateWindow( "DICOM Volume Stress Test" );

	glutDisplayFunc( show_image );
	glutIdleFunc( show_image );
	glutReshapeFunc( changeSize );
	glutKeyboardFunc( processNormalKeys );
	glutSpecialFunc( processSpecialKeys );

	setShaders();

	SSC_Logging_Init_Default( "com.sonowand.test" );
	DICOMLib_Init();
	study = DICOMLib_StudiesFromPath( "/testdata/SONOWAND-06-Eirik-Mo/", NULL, DICOMLIB_NO_CACHE );
	if ( !study || !study->first_series )
	{
		fprintf( stderr, "Nothing found!\n" );
		SSC_Logging_Done();
		exit( 1 );
	}
	DICOMLib_UseAutoVOI( study->first_series );
	volume = DICOMLib_GetVolume( study->first_series, NULL );
	if ( !volume )
	{
		fprintf( stderr, "No volume found (only checked first series in study)!\n" );
		exit( EXIT_FAILURE );
	}

	upload_image( volume->volume, volume->x, volume->y, volume->z, volume->bits_per_sample );

	// Calculate shader variables from window values
	size = pow(2, volume->bits_per_sample);		// size of LUT in OpenGL terms
	factor = 1.0f / size;				// size of each pixel value in the LUT in OpenGL terms
	reject = factor * (volume->voi.center + 1 - volume->voi.width / 4);	// the division by four is a hack because Kaiisa window is too wide
	step = size / volume->voi.width;		// find magnification value to increase brightness
	glUniform1f(llr, reject);
	glUniform1f(scale, step);
	//printf("LLR=%f SCALE=%f based on size=%f factor=%f center %f and width %f\n", reject, step, size, factor, volume->voi.center, volume->voi.width);
	gettimeofday( &prev, NULL );
	glutMainLoop();

	return 0; // shut up compiler
}
