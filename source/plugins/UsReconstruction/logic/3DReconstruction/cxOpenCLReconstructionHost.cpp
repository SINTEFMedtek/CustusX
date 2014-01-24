#include <cxOpenCLReconstructionHost.h>

namespace cx
{

OpenCLReconstructionHost::OpenCLReconstructionHost(OpenCL::ocl* opencl, ProcessedUSInputDataPtr input, vtkImageDataPtr output) :
		mOpenCL(opencl), mInput(input), mOutput(output)
{
	mKernelPath = "./kernels/kernesl.cl";
}

bool OpenCLReconstructionHost::reconstruct()
{
	int numberOfPlanesToFind = 2;
	int radiusInMm = 10;
	this->findClosesInputPlanesForAllOutputVoxels(numberOfPlanesToFind, radiusInMm); //run a kernel

	this->fillOutputVoxelWithApropiateIntensity(); //run a kernel

	return true;
}

void OpenCLReconstructionHost::findClosesInputPlanesForAllOutputVoxels(int numberOfPlanesToFind, int searchRadiusInMm)
{
	//run a kernel
}

void OpenCLReconstructionHost::fillOutputVoxelWithApropiateIntensity()
{
	//run a kernel
}
} /* namespace cx */
