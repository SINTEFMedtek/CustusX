#ifndef VTKFIXTURE_H
#define VTKFIXTURE_H

//OpenGL
#include <GL/glew.h>
#include <glut.h> //Framework on Mac

#include <vtkSmartPointer.h>

class vtkfixture
{
public:
	vtkfixture();

	void createVTKWindowWithCylinderSourceWith3DTexture();

protected:
	vtkSmartPointer<class vtkTextureObject> createTextureObject(unsigned int width, unsigned int height, unsigned int depth, int dataType, int numComps, void *data, vtkSmartPointer<class vtkOpenGLRenderWindow> opengl_renderwindow);
	vtkSmartPointer<class vtkOpenGLBufferObject> allocateAndUploadArrayBuffer(int my_numberOfTextureCoordinates, int numberOfComponentsPerTexture, const GLfloat *texture_data);

private:
	//OpenGL
	void printActiveVertexAndFragmentShader(vtkSmartPointer<class vtkOpenGLPolyDataMapper> mapper);
	void printOpenGLVersion();
	void printVtkOpenGLRenderWindowInfo(vtkSmartPointer<class vtkOpenGLRenderWindow> opengl_renderwindow);
};

#endif // VTKFIXTURE_H
