#include <cxOpenCLReconstruction.h>

#include "cxOpenCLUtilities.h"
#include "cxOpenCLReconstructionHost.h"
#include "sscMessageManager.h"

namespace cx
{

QString OpenCLReconstruction::getName() const
{
	return "OpenCLReconstruction";
}

std::vector<DataAdapterPtr> OpenCLReconstruction::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;
	return retval;
}

bool OpenCLReconstruction::reconstruct(ProcessedUSInputDataPtr input, vtkImageDataPtr output, QDomElement settings)
{
	OpenCL::ocl* opencl = OpenCL::init(CL_DEVICE_TYPE_GPU);
	OpenCLReconstructionHost host(opencl, input, output);
	return host.reconstruct();
}


} /* namespace cx */
