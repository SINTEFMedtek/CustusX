#include "vtkfixture.h"

int main(int argc, char *argv[])
{
	vtkfixture vtk;
	vtk.createVTKWindowWithCylinderSourceWith3DTexture();

	return 0;
}
