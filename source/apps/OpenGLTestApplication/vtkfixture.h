#ifndef VTKFIXTURE_H
#define VTKFIXTURE_H

#include <vtkSmartPointer.h>

class vtkfixture
{
public:
	vtkfixture();

	void createVTKWindowWithCylinderSourceWith3DTexture();

private:
	//OpenGL
	void printActiveVertexAndFragmentShader(vtkSmartPointer<class vtkOpenGLPolyDataMapper> mapper);
	void printOpenGLVersion();
	void printVtkOpenGLRenderWindowInfo(vtkSmartPointer<class vtkOpenGLRenderWindow> opengl_renderwindow);
};

#endif // VTKFIXTURE_H
