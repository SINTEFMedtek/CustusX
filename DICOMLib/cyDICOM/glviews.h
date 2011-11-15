#ifndef GLVIEWS_H
#define GLVIEWS_H

#include <QtOpenGL/QtOpenGL>

class GLViews : public QGLWidget
{
	Q_OBJECT

	public:
	GLViews( QWidget *parent = 0 );
	void enable();
	void disable();
	void setThreshold( float inputReject );
	void setBrightness( float inputBrightness );
	void setContrast( float inputContrast );

	protected:
	void initializeGL();
	void resizeGL( int width, int height );
	void paintGL();
	void mousePressEvent( QMouseEvent *event );
	void mouseMoveEvent( QMouseEvent *event );
	void mouseDoubleClickEvent( QMouseEvent *event );

	private:
	float reject;
	float contrast;
	float brightness;
	bool enabled;
	void errmsg( const char *msg );
	int faceAtPosition( const QPoint &pos );
	GLfloat rotationX;
	GLfloat rotationY;
	GLfloat rotationZ;
	QColor faceColors[4];
	QPoint lastPos;
	GLfloat theta;
	GLuint texnames[1];
	GLuint shader, program;
	struct timeval prev;
	int frames;
	int effect;
};

#endif
