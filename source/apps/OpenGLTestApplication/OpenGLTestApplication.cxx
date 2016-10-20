#include "openglfixture.h"
#include "vtkfixture.h"

int main(int argc, char *argv[])
{
	//OpenGLFixture opengl(argc, argv);

	vtkfixture vtk;
	vtk.createVTKWindowWithCylinderSourceWith3DTexture(argc, argv);

	return 0;
}
