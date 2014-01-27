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
	int numberOfPlanesToFind = 2;
	int radiusInMm = 10;
	this->findClosesInputPlanesForAllOutputVoxels(numberOfPlanesToFind, radiusInMm); //run a kernel

	this->fillOutputVoxelWithApropiateIntensity(); //run a kernel

	return true;
}

void OpenCLReconstructionHost::findClosesInputPlanesForAllOutputVoxels(int numberOfPlanesToFind, int searchRadiusInMm)
{
	try
	{
		cl::Kernel kernel = this->getKernelWithName("test");

		cl::NDRange offset = 0;
		cl::NDRange global = mOpenCL->device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
		cl::NDRange local = cl::NullRange;
		mOpenCL->cmd_queue.enqueueNDRangeKernel(kernel, offset ,global, local, NULL, NULL);
		mOpenCL->cmd_queue.finish();
	}
	catch (cl::Error error)
	{
		messageManager()->sendError("Could not find closes planes. Reason: "+QString(error.what()));
		check_error(error.err());
	}

}

void OpenCLReconstructionHost::fillOutputVoxelWithApropiateIntensity()
{
	//TODO run a kernel
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
	catch (cl::Error error)
	{
		messageManager()->sendError("Could get the kernel "+name+". Reason: "+QString(error.what()));
		check_error(error.err());
	}
	return kernel;

}

} /* namespace cx */
