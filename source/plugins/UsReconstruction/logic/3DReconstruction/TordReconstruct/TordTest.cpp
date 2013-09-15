#include "TordTest.h"
#include "sscMessageManager.h"
#include <vtkImageData.h>
#include <QDomElement>
#include <iostream>
namespace ssc
{

TordTest::TordTest()
{
}

std::vector<DataAdapterPtr>
TordTest::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;
	// Add settings here if needed
	return retval;
}

bool
TordTest::reconstruct(ProcessedUSInputDataPtr input,
                      vtkImageDataPtr outputData,
                      QDomElement settings)
{
	
	vtkImageDataPtr target = outputData;
	Eigen::Array3i targetDims(target->GetDimensions());
	ssc::Vector3D targetSpacing(target->GetSpacing());


	// Print dimensions
	QString info = QString("Target dims: %1 %2 %3").arg(targetDims[0]).arg(targetDims[1]).arg(targetDims[2]);
	messageManager()->sendInfo(info);


	// Iterate over outputData and fill volume with 255-s

	unsigned char *outputPointer  = static_cast<unsigned char*>(outputData->GetScalarPointer());
	for(unsigned int dim0 = 0; dim0 < targetDims[0]; dim0++)
	{
		for(unsigned int dim1 = 0; dim1 < targetDims[1]; dim1++)
		{
			for(unsigned int dim2 = 0; dim2 < targetDims[2]; dim2++)
			{
				unsigned int idx = dim0 + dim1*targetDims[0] + dim2*targetDims[0]*targetDims[1];
				outputPointer[idx] = 255;
			
			} // dim2
		} // dim1
	} // dim0

	return true;
}


}


