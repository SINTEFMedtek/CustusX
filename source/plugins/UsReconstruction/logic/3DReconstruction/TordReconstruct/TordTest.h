// This is a preliminary test written by Tord Øygard in order to familiarize himself with the CustusX codebase.
#ifndef TORD_TEST_H_
#define TORD_TEST_H_


#include "sscReconstructAlgorithm.h"
#include "Thunder/setup.h"

namespace cx
{
/**
 * \addtogroup sscUSReconstruction
 * \{
 */


/**
 * \brief Tords US test
 */

class TordTest : public ReconstructAlgorithm
{
public:
	TordTest();
	virtual ~TordTest();
	virtual QString getName() const
	{
		return "TordTest";
	}
	
	virtual std::vector<DataAdapterPtr> getSettings(QDomElement root);
	virtual bool reconstruct(ProcessedUSInputDataPtr input,
	                         vtkImageDataPtr outputData,
	                         QDomElement settings);

protected:
	
	virtual bool initCL(QString kernel_file);
	virtual bool doGPUReconstruct(ProcessedUSInputDataPtr input,
	                              vtkImageDataPtr outputData);
	/// OpenCL handles
	cl_kernel mClKernel;
	std::vector<cl_mem> mVClMemBscan;
	cl_mem mClMemOutput;
	ocl_context* moClContext;
	
};

}
		
#endif
