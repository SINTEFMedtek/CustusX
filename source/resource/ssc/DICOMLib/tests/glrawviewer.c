// FIXME: It uses non-POT 3D textures, which are probably far slower than POT 3D textures.
// FIXME: Aspect ratios

#include <assert.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <sys/time.h>

#include "sscLogger.h"

static GLfloat sizes[3];
static GLuint shader, program;
static GLfloat theta = 0.0f;
static struct timeval prev;
static int frames = 0;
static int effect = 0;
static GLint llr;

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
	if ( infologLen > 0 )
	{
		int charsWritten = 0;
		GLchar *infoLog = malloc( infologLen );

		glGetShaderInfoLog( shader, infologLen, &charsWritten, infoLog );
		printf( "InfoLog: \n%s\n\n", infoLog );
		free( infoLog );
	}
	else printf( "No log\n" );
}

char *textFileRead( const char *fn )
{
	FILE *fp;
	char *content = NULL;
	int count = 0;

	if ( fn != NULL )
	{
		fp = fopen( fn, "rt" );
		if ( fp != NULL )
		{
			fseek( fp, 0, SEEK_END );
			count = ftell( fp );
			rewind( fp );

			if ( count > 0 )
			{
				content = malloc( sizeof( char ) * ( count + 1 ) );
				count = fread( content, sizeof( char ), count, fp );
				content[count] = '\0';
			}

			fclose( fp );
		}
	}

	return content;
}

static void setShaders( void )
{
	char *fs = textFileRead( "zeroAlpha.frag" );

	shader = glCreateShader( GL_FRAGMENT_SHADER );
	if ( shader == 0 )
	{
		printf( "Failed to create OpenGL shader!\n" );
		abort();
	}
	glShaderSource( shader, 1, ( const char ** )&fs, NULL );
	free( fs );

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
	report_gl_error();
}

void upload_image( const char *image, const float *lut, int lutsize, int bits, uint32_t x, uint32_t y, uint32_t z )
{
	GLenum size, type;
	GLuint textures[2], lutBuffer;

	assert( image != NULL && lut != NULL);

	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_PACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_PACK_SKIP_ROWS, 0 );
	glPixelStorei( GL_PACK_SKIP_PIXELS, 0 );

	glEnable( GL_TEXTURE_1D );
	glEnable( GL_TEXTURE_3D );
	glEnable( GL_TEXTURE_2D );
	glClear( GL_COLOR_BUFFER_BIT );

	glGenTextures(2, textures);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, textures[0]);
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	switch (bits)
	{
		case 8: size = GL_UNSIGNED_BYTE; type = GL_LUMINANCE; break; //8UI_EXT; break;
		case 16: size = GL_UNSIGNED_SHORT; type = GL_LUMINANCE16; break; //16UI_EXT; break;
		default: size = GL_UNSIGNED_BYTE; type = GL_LUMINANCE;  printf("No such bit size: %d\n", bits); assert(false); break;
	}
	glTexImage3D(GL_TEXTURE_3D, 0, type, x, y, z, 0, GL_LUMINANCE, size, image);

	glGenBuffersARB(1, &lutBuffer);
	glBindBuffer(GL_TEXTURE_BUFFER_EXT, lutBuffer);
	glBufferDataARB(GL_TEXTURE_BUFFER_EXT, lutsize * sizeof(*lut) * 4, lut, GL_STATIC_DRAW);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER_EXT, textures[1]);
	glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, GL_RGBA32F_ARB, lutBuffer);

	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);	// texture unit 0
	glUniform1i(glGetUniformLocation(program, "lut"), 1);	// texture unit 1
	glUniform1i(glGetUniformLocation(program, "lutsize"), lutsize);
	llr = glGetUniformLocation(program, "llr");

	sizes[0] = x;
	sizes[1] = y;
	sizes[2] = z;

	report_gl_error();
}

static void draw_cut( GLfloat x, GLfloat y, GLfloat z, const GLfloat texCoords[] )
{
	const GLfloat vertices[] = { -0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, -0.5 };

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
	const GLfloat texCoords0[] = { 0.0, 0.0, depth, 1.0, 0.0, depth, 1.0, 1.0, depth, 0.0, 1.0, depth };
	const GLfloat texCoords1[] = { 0.0, depth, 0.0, 1.0, depth, 0.0, 1.0, depth, 1.0, 0.0, depth, 1.0 };
	const GLfloat texCoords2[] = { depth, 0.0, 0.0, depth, 1.0, 0.0, depth, 1.0, 1.0, depth, 0.0, 1.0 };
	struct timeval now;
	int i;
	const GLfloat slices = 75.0;

	glDisable( GL_BLEND );
	glClear( GL_COLOR_BUFFER_BIT );
	glMatrixMode( GL_PROJECTION );
	glUniform1f(llr, 0.0f);

	draw_cut( -0.5, -0.5, -1.0, texCoords0 );
	draw_cut( 0.5, 0.5, -1.0, texCoords1 );
	draw_cut( -0.5, 0.5, -1.0, texCoords2 );

	glEnable( GL_BLEND );
	glUniform1f(llr, 0.075f);

	for ( i = 1; i < slices; i++ )
	{
		GLfloat texCoords[] = { 0.0, 0.0, 0.5, 1.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.0, 1.0, 0.5 };

		if ( effect == 3 )
		{
			glBlendColorEXT( 1.f, 1.f, 1.f, 1.f / i );
		}
		texCoords[2] = ( float )i / slices;
		texCoords[5] = ( float )i / slices;
		texCoords[8] = ( float )i / slices;
		texCoords[11] = ( float )i / slices;

		draw_cut( 0.5, -0.5, -( ( float )i / slices ), texCoords );
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
		glDeleteProgram( program );
		glDeleteShader( shader );
		SSC_Logging_Done();
		exit( 0 );
	}
}

int main ( int argc, char **argv )
{
	int result, lutsize;
	char *ptr = NULL;
	GLint tSize;
	float *lut = NULL;
	char path[PATH_MAX];
	FILE *fp;
	struct stat stats;
	const int x = 512;
	const int y = 544;
	const int z = 60;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: glvolumeviewer <path to base timestamp>\n" );
		exit( 1 );
	}

	SSC_Logging_Init_Default( "com.sonowand.test" );
	sstrcpy(path, argv[1]);
	sstrcat(path, ".raw");
	fp = fopen(path, "r");
	if (!fp)
	{
		SSC_ERROR("No such file \"%s\"", path);
		return -1;
	}
	fstat(fileno(fp), &stats);
	ptr = malloc(stats.st_size);
	result = fread(ptr, stats.st_size, 1, fp);
	if (result != stats.st_size)
	{
		SSC_ERROR("Error reading \"%s\"", path);
	}
	fclose(fp);

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( 1600, 1200 );
	glutCreateWindow( "DICOM Volume Viewer" );

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		exit(1);
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	glutDisplayFunc( show_image );
	glutIdleFunc( show_image );
	glutReshapeFunc( changeSize );
	glutKeyboardFunc( processNormalKeys );
	glutSpecialFunc( processSpecialKeys );

	printf( "OpenGL version: %s\n", glGetString( GL_VERSION ) );
	glGetIntegerv( GL_MAX_3D_TEXTURE_SIZE, &tSize );
	printf( "Maximum texture size is: %d\n", tSize );
	setShaders();

	lutsize = 256;
#if 1
	{
		int i;
		float *newlut, step, count;
		const int center = 32788;
		const int width = 80;
		int start = center - (width / 2);
		int stop = center + (width / 2);
		const int bits_per_sample = 16;

		lutsize = pow(2, bits_per_sample);
		step = 1.0f / (stop - start);
		newlut = malloc(lutsize * sizeof(*newlut) * 4);
		for (i = 0; i < start * 4; i += 4)
		{
			newlut[i + 0] = 0.0f;		// R
			newlut[i + 1] = 0.0f;		// G
			newlut[i + 2] = 0.0f;		// B
			newlut[i + 3] = 1.0f;		// A
		}
		count = 0.0f;
		for (i = start * 4; i < stop * 4; i += 4)
		{
			newlut[i + 0] = step * count;	// R
			newlut[i + 1] = step * count;	// G
			newlut[i + 2] = step * count;	// B
			newlut[i + 3] = 1.0f;		// A
			count += 1.0f;
		}
		for (i = stop * 4; i < lutsize * 4; i += 4)
		{
			newlut[i + 0] = 1.0f;		// R
			newlut[i + 1] = 1.0f;		// G
			newlut[i + 2] = 1.0f;		// B
			newlut[i + 3] = 1.0f;		// A
		}
		lut = newlut;
		//printf("start=%d stop=%d step=%f size=%d\n", start, stop, step, lutsize);
	}
#else
	{
		int i;
		float *newlut;
		char *ptr;

		sstrcpy(path, argv[1]);
		sstrcat(path, ".lut");
		fp = fopen(path, "r");
		if (!fp)
		{
			SSC_ERROR("No such file \"%s\"", path);
			return -1;
		}
		fstat(fileno(fp), &stats);
		ptr = malloc(stats.st_size);
		result = fread(ptr, stats.st_size, 1, fp);
		if (result != stats.st_size)
		{
			SSC_ERROR("Error reading \"%s\"", path);
		}
		fclose(fp);
		newlut = malloc(lutsize * sizeof(*newlut) * 4);
		for (i = 0; i < lutsize; i += 4)
		{
			newlut[i + 0] = (float)ptr[i] / 255.0f;		// R
			newlut[i + 1] = (float)ptr[i] / 255.0f;		// G
			newlut[i + 2] = (float)ptr[i] / 255.0f;		// B
			newlut[i + 3] = 1.0f;				// A
		}
		lut = newlut;
	}
#endif
	upload_image( ptr, lut, lutsize, 16, x, y, z );
	gettimeofday( &prev, NULL );
	glutMainLoop();

	free(lut);
	return 0; // shut up compiler
}
