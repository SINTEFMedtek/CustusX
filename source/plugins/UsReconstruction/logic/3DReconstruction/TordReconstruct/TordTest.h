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

	typedef struct __frameBlock_t
	{
		unsigned char* data;
		size_t length;
	} frameBlock_t;

protected:
	
	virtual bool initCL(QString kernelFile);
	virtual bool doGPUReconstruct(ProcessedUSInputDataPtr input,
	                              vtkImageDataPtr outputData);

	/**
	 * Split the US input into numBlock blocks of whole frames 
	 * and store them in framePointers.
	 * \param[out] framePointers Pre-allocated array of numBlocks unsigned char* pointers
	 *             Data will be returned here
	 * \param[in] numBlocks Number of blocks to split US input into
	 * \param[in] inputFrames The input US B-scans
	 * \return true on success, false otherwise
	 */
	virtual bool initializeFrameBlocks(frameBlock_t* framePointers,
	                                   int numBlocks,
	                                   ProcessedUSInputDataPtr inputFrames);
	virtual void freeFrameBlocks(frameBlock_t* framePointers,
	                             int numBlock);

	/// OpenCL handles
	cl_kernel mClKernel;
	std::vector<cl_mem> mVClMemBscan;
	cl_mem mClMemOutput;
	ocl_context* moClContext;
	
};


}
		
#endif
