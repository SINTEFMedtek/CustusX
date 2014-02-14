#include <cxOpenCLReconstructionHost.h>

#include <recConfig.h>
#include "sscMessageManager.h"

namespace cx
{

OpenCLReconstructionHost::OpenCLReconstructionHost(OpenCL::ocl* opencl, ProcessedUSInputDataPtr input, vtkImageDataPtr output) :
		mOpenCL(opencl), mInput(input), mOutput(output)
{
	if(mOpenCL == NULL)
		messageManager()->sendError("OpenCL is NULL!");

	mKernelPath = QString(KERNEL_PATH)+"kernels.cl";
}

bool OpenCLReconstructionHost::reconstruct()
{
	bool success = false;

	int numberOfPlanesToFind = 2;
	int radiusInMm = 10;

//	this->fillPlaneEquations();
	this->findClosesInputPlanesForAllOutputVoxels(numberOfPlanesToFind, radiusInMm);

	this->fillOutputVoxelWithApropiateIntensity();

	return success;
}

bool OpenCLReconstructionHost::findClosesInputPlanesForAllOutputVoxels(int numberOfPlanesToFind, int searchRadiusInMm)
{
	bool success = true;
	try
	{
		cl::Kernel kernel = this->getKernelWithName("test");
//		cl::Kernel kernel = this->getKernelWithName("findClosesPlanes");
//		this->setArguments(kernel, "");

		cl::NDRange offset = 0;
		cl::NDRange global = mOpenCL->device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
		cl::NDRange local = cl::NullRange;
		mOpenCL->cmd_queue.enqueueNDRangeKernel(kernel, offset ,global, local, NULL, NULL);
		mOpenCL->cmd_queue.finish();
	}
	catch (cl::Error &error)
	{
		messageManager()->sendError("Could not find closes planes. Reason: "+QString(error.what()));
		check_error(error.err());
		success = false;
	}
	return success;

}

bool OpenCLReconstructionHost::fillOutputVoxelWithApropiateIntensity()
{
	//TODO run a kernel
	return true;
}


cl::Kernel OpenCLReconstructionHost::getKernelWithName(QString name)
{
	cl::Kernel kernel;
	try
	{
		size_t sourceLength;
		const char* source = OpenCLUtilities::file2string(mKernelPath.toLocal8Bit().data(), &sourceLength);
		cl::Context context = mOpenCL->context;
		cl::Program program = OpenCL::createProgram(context, source, sourceLength);
		OpenCL::build(program, "");
		kernel = OpenCL::createKernel(program, name.toStdString().c_str());
	}
	catch (cl::Error &error)
	{
		messageManager()->sendError("Could get the kernel "+name+". Reason: "+QString(error.what()));
		check_error(error.err());
	}
	return kernel;

}

} /* namespace cx */
