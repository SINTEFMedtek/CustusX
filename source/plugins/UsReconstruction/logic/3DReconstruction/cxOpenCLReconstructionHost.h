#ifndef CXOPENCLRECONSTRUCTIONHOST_H_
#define CXOPENCLRECONSTRUCTIONHOST_H_

#include "sscReconstructAlgorithm.h"
#include "cxOpenCLUtilities.h"

namespace cx
{
/**
 * \brief 
 *
 * \date Jan 22, 2014
 * \author Janne Beate Bakeng, SINTEF
 */

class OpenCLReconstructionHost
{
public:
	OpenCLReconstructionHost(OpenCL::ocl* opencl, ProcessedUSInputDataPtr input, vtkImageDataPtr outputData);

	bool reconstruct();

private:
	void findClosesInputPlanesForAllOutputVoxels(int numberOfPlanesToFind, int searchRadiusInMm); //run a kernel
	void fillOutputVoxelWithApropiateIntensity(); //run a kernel

	OpenCL::ocl* mOpenCL;

	ProcessedUSInputDataPtr mInput;
	vtkImageDataPtr mOutput;

	QString mKernelPath;

};
} /* namespace cx */

#endif /* CXOPENCLRECONSTRUCTIONHOST_H_ */
