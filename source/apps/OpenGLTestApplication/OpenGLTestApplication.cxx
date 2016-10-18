#include "openglfixture.h"
#include "vtkfixture.h"

//TODO:
//Create OpenGL context
//Create a volume
//Upload volume to GPU
//Create a glut window
//Create VTK window
//Modify fragmentshader

int main(int argc, char *argv[])
{
	//unsigned char *texture = generateTexture(3,3,3,3);

	//OpenGLFixture opengl(argc, argv);

	vtkfixture vtk;
	vtk.createVTKWindowWithCylinderSourceWith3DTexture(argc, argv);

	return 0;
}
