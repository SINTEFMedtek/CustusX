#define GL_GLEXT_PROTOTYPES 1

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <sys/time.h>

#include "glviews.h"

void GLViews::setThreshold( float inputReject )
{
	reject = inputReject;
}

void GLViews::setBrightness( float inputBrightness )
{
	brightness = inputBrightness;
}

void GLViews::setContrast( float inputContrast )
{
	contrast = inputContrast;
}

void GLViews::errmsg( const char *msg )
{
	QMessageBox::critical(this, tr("Oops..."), msg, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
}

GLViews::GLViews( QWidget *parent )
	: QGLWidget( parent )
{
	setFormat( QGLFormat( QGL::DoubleBuffer | QGL::DepthBuffer ) );
	enabled = false;
	rotationX = -21.0;
	rotationY = -57.0;
	rotationZ = 0.0;
	faceColors[0] = Qt::red;
	faceColors[1] = Qt::green;
	faceColors[2] = Qt::blue;
	faceColors[3] = Qt::yellow;
	theta = 0.01f;
	frames = 0;
	effect = 0;
	reject = 0.075;
	brightness = 1.0;
	contrast = 1.0;
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
				content = (char *)malloc( sizeof( char ) * ( count + 1 ) );
				count = fread( content, sizeof( char ), count, fp );
				content[count] = '\0';
			}

			fclose( fp );
		}
	}

	return content;
}

void GLViews::initializeGL()
{
	char *fs = textFileRead( "zeroAlpha.frag" );
	if ( !fs )
	{
		errmsg( "Failed to find shader file" );
		return;
	}

	shader = glCreateShader( GL_FRAGMENT_SHADER );
	if ( shader == 0 )
	{
		errmsg( "Failed to create OpenGL shader!" );
		return;
	}
	glShaderSource( shader, 1, ( const char ** )&fs, NULL );
	free( fs );

	glCompileShader( shader );
	program = glCreateProgram();
	if ( program == 0 )
	{
		errmsg( "Failed to create OpenGL shader program!" );
		return;
	}
	glAttachShader( program, shader );
	glLinkProgram( program );
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glColor3f( 1.0, 1.0, 1.0 );
}

void GLViews::resizeGL( int width, int height )
{
	float ratio;

	if ( height == 0 ) height = 1;
	ratio = 1.0 * width / height;

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glViewport( 0, 0, width, height );

	gluPerspective( 45, ratio, 0.1, 100 );
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

void GLViews::paintGL()
{
	GLfloat depth = theta;
	const GLfloat texCoords0[] = { 0.0, 0.0, depth, 1.0, 0.0, depth, 1.0, 1.0, depth, 0.0, 1.0, depth };
	const GLfloat texCoords1[] = { 0.0, depth, 0.0, 1.0, depth, 0.0, 1.0, depth, 1.0, 0.0, depth, 1.0 };
	const GLfloat texCoords2[] = { depth, 0.0, 0.0, depth, 1.0, 0.0, depth, 1.0, 1.0, depth, 0.0, 1.0 };
	struct timeval now;
	int i;
	const GLfloat slices = 75.0;
	GLint location;

	glUseProgram( program );
	location = glGetUniformLocation( program, "brightness" );
	glUniform1f( location, brightness );
	location = glGetUniformLocation( program, "contrast" );
	glUniform1f( location, contrast );
	location = glGetUniformLocation( program, "reject" );
	glUniform1f( location, 0.0f );

	glDisable( GL_BLEND );
	glClear( GL_COLOR_BUFFER_BIT );
	if ( !enabled )
	{
		return;
	}
	glMatrixMode( GL_PROJECTION );

	draw_cut( -0.5, -0.5, -1.0, texCoords0 );
	draw_cut( 0.5, 0.5, -1.0, texCoords1 );
	draw_cut( -0.5, 0.5, -1.0, texCoords2 );

	glEnable( GL_BLEND );
	location = glGetUniformLocation( program, "reject" );
	glUniform1f( location, reject );

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
}

void GLViews::mousePressEvent( QMouseEvent *event )
{
	lastPos = event->pos();
}

void GLViews::mouseMoveEvent( QMouseEvent *event )
{
}

void GLViews::mouseDoubleClickEvent( QMouseEvent *event )
{
}

void GLViews::enable()
{
	enabled = true;
}

void GLViews::disable()
{
	enabled = false;
}
