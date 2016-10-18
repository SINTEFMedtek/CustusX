#ifndef VTKFIXTURE_H
#define VTKFIXTURE_H

#include <vtkSmartPointer.h>

class vtkfixture
{
public:
	vtkfixture();

	void createVTKWindowWithCylinderSource();

	void createVTKWindowWithCylinderSourceWith3DTexture(int argc, char *argv[]);

	static void upload3dTextures();

private:
	//VTK
	vtkSmartPointer<class vtkCylinderSource> createPolygonalCylinderModel();
	vtkSmartPointer<class vtkOpenGLPolyDataMapper> createOpenGLPolyDataMapper();
	vtkSmartPointer<class vtkActor> createActor(vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper);
	void startRender(vtkSmartPointer<class vtkActor> actor);

	//OpenGL
	std::string getCustomFragmentShader();
	void printActiveFragmentShader(vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper);
	void setUniforms();
};

#endif // VTKFIXTURE_H
