#ifndef GLDICOMVIEW_H
#define GLDICOMVIEW_H

#include <stdint.h>
#include <GL/glew.h>
#include <QGLWidget>

#define MAX_LAYERS 4

class glDicomTest : public QGLWidget
{
	Q_OBJECT

public:
	glDicomTest(QWidget *parent = 0);
	~glDicomTest();
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void initialize();

public slots:
	void tick();
	void changeLLR(int value);
	void changeActive(int value);
	void changeBrightness(int value);
	void changeContrast(int value);

private:
	QTimer *timer;
	GLfloat reject[MAX_LAYERS];
	int activeVolume;
};

#endif
