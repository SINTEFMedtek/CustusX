#ifndef VTKFIXTURE_H
#define VTKFIXTURE_H

#include <vtkSmartPointer.h>

class vtkfixture
{
public:
	vtkfixture();

	void createVTKWindowWithCylinderSource();

	void createVTKWindowWithCylinderSourceWith3DTexture(int argc, char *argv[]);

	static void opengl_upload3dTextures();

private:
	//VTK
	vtkSmartPointer<class vtkCylinderSource> createPolygonalCylinderModel();
	vtkSmartPointer<class vtkOpenGLPolyDataMapper> createOpenGLPolyDataMapper();
	vtkSmartPointer<class vtkActor> createActor(vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper);
	void startRender(vtkSmartPointer<class vtkActor> actor);

	//OpenGL
	void printActiveVertexAndFragmentShader(vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper);
	void setUniforms();
	void printOpenGLVersion();
	void printVtkOpenGLRenderWindowInfo(vtkSmartPointer<class vtkOpenGLRenderWindow> opengl_renderwindow);
};

#endif // VTKFIXTURE_H
