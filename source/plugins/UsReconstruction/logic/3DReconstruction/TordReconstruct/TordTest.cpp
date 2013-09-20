#include "TordTest.h"
#include "sscMessageManager.h"
#include <vtkImageData.h>
#include <QDomElement>
#include <iostream>
#include <Thunder/utils.h>
#include <recConfig.h>
namespace cx
{

TordTest::TordTest()
{
	// FIXME: Be more clever when determining kernel path
	initCL(QString(TORD_KERNEL_PATH) + "/kernels.ocl");
}

TordTest::~TordTest()
{
	if(moClContext != NULL)
	{
		ocl_release(moClContext);
		moClContext = NULL;
	}
}

std::vector<DataAdapterPtr>
TordTest::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;
	// Add settings here if needed
	return retval;
}

bool
TordTest::initCL(QString kernel_path)
{
	// Reusing initialization code from Thunder
	moClContext = ocl_init("GPU");

	size_t source_len;

	messageManager()->sendInfo(QString("Kernel path: %1").arg(kernel_path));
	char* s_source = file2string(kernel_path.toLocal8Bit().data(),
	                             &source_len);

	
	cl_program clprogram = ocl_create_program(moClContext->context,
	                                    moClContext->device,
	                                    s_source, kernel_path);
	mClKernel = ocl_kernel_build(clprogram,
	                             moClContext->device, "voxel_methods");
	return true;
	
}



bool
TordTest::initializeFrameBlocks(unsigned char** framePointers,
                                int numBlocks,
                                ProcessedUSInputDataPtr input_frames)
{
	// TODO: Fill me
}

bool
TordTest::doGPUReconstruct(ProcessedUSInputDataPtr input,
                           vtkImageDataPtr outputData)
{
	// TODO: Fill me
	return true;
}


bool
TordTest::reconstruct(ProcessedUSInputDataPtr input,
                      vtkImageDataPtr outputData,
                      QDomElement settings)
{
	
	vtkImageDataPtr target = outputData;
	Eigen::Array3i targetDims(target->GetDimensions());
	Vector3D targetSpacing(target->GetSpacing());


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


