#ifndef VTKFIXTURE_H
#define VTKFIXTURE_H

#include <vtkSmartPointer.h>

class vtkfixture
{
public:
	vtkfixture();

	void createVTKWindowWithCylinderSourceWith3DTexture();

	vtkSmartPointer<class vtkTextureObject> createTextureObject(unsigned int depth, unsigned int width, int dataType, int numComps, unsigned int height, vtkSmartPointer<class vtkOpenGLRenderWindow> opengl_renderwindow, void *data);

private:
	//OpenGL
	void printActiveVertexAndFragmentShader(vtkSmartPointer<class vtkOpenGLPolyDataMapper> mapper);
	void printOpenGLVersion();
	void printVtkOpenGLRenderWindowInfo(vtkSmartPointer<class vtkOpenGLRenderWindow> opengl_renderwindow);
};

#endif // VTKFIXTURE_H
