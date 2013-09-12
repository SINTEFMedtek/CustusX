#include "TordTest.h"
#include "sscMessageManager.h"

namespace ssc
{

TordTest::TordTest()
{
}

std::vector<dataAdapterPtr>
TordTest::getSettigns(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;
	// Add settings here if needed
	return retval;
}

bool
TordTest::reconstruct(ProcessedUsInputDataPtr input,
                      vtkImageDataPtr outputData,
                      QDomElement settings)
{
	
	vtkImageDataPtr target = outputData;
	Eigen::Array3i targetDims(target->GetDimensions());
	ssc::Vector3D targetSpacing(target->GetSpacing());


	// Print dimensions
	messageManager()->sendInfo("Target dims: "
	                           + targetDims[0]
	                           + " " + targetDims[1]
	                           + " " + targetDims[2]);

	// Iterate over outputData and fill volume with 255-s

	unsigned char *outputPointer  = static_cast<unsigned char*>(outputData->getScalarPointer());
	for(unsigned int dim0 = 0; dim0 < targetDims[0]; dim0++)
	{
		for(unsigned int dim1 = 0; dim1 < targetDims[1]; dim1++)
		{
			for(unsigned int dim2 = 0; dim2 < targetDims[2]; dim2++)
			{
				unsigned int idx = dim0 + dim1*outputDims[0] + dim2*outputDims[0]*outputDims[1];
				outputPointer[idx] = 255;
			}

			} // dim2
		} // dim1
	} // dim0

	return true;
}


}


