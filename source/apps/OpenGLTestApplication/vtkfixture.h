#ifndef VTKFIXTURE_H
#define VTKFIXTURE_H

//OpenGL
#include <GL/glew.h>
#include <GL/glut.h> //Framework on Mac

#include <vtkSmartPointer.h>

class vtkfixture
{
public:
	vtkfixture();

	void createVTKWindowWithCylinderSourceWith3DTexture();

protected:
	vtkSmartPointer<class vtkTextureObject> createTextureObject(unsigned int depth, unsigned int width, int dataType, int numComps, unsigned int height, vtkSmartPointer<class vtkOpenGLRenderWindow> opengl_renderwindow, void *data);
	vtkSmartPointer<class vtkOpenGLBufferObject> allocateAndUploadArrayBuffer(int my_numberOfTextureCoordinates, int numberOfComponentsPerTexture, const GLfloat *texture_data);

private:
	//OpenGL
	void printActiveVertexAndFragmentShader(vtkSmartPointer<class vtkOpenGLPolyDataMapper> mapper);
	void printOpenGLVersion();
	void printVtkOpenGLRenderWindowInfo(vtkSmartPointer<class vtkOpenGLRenderWindow> opengl_renderwindow);
};

#endif // VTKFIXTURE_H
