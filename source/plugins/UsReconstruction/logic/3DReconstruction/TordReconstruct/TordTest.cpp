#include "TordTest.h"
#include "sscMessageManager.h"
#include <vtkImageData.h>
#include <QDomElement>
#include <iostream>
#include <Thunder/utils.h>
#include <recConfig.h>
#include <sscUSFrameData.h>
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
TordTest::initCL(QString kernelPath)
{
	// Reusing initialization code from Thunder
	moClContext = ocl_init("GPU");

	size_t sourceLen;

	messageManager()->sendInfo(QString("Kernel path: %1").arg(kernelPath));
	char* sSource = file2string(kernelPath.toLocal8Bit().data(),
	                             &sourceLen);

	
	cl_program clprogram = ocl_create_program(moClContext->context,
	                                    moClContext->device,
	                                    sSource, kernelPath);
	mClKernel = ocl_kernel_build(clprogram,
	                             moClContext->device, "voxel_methods");
	return true;
	
}

bool
TordTest::initializeFrameBlocks(frameBlock_t* framePointers,
                                int numBlocks,
                                ProcessedUSInputDataPtr inputFrames)
{
	// Compute the size of each frame in bytes
	Eigen::Array3i dims = inputFrames->getDimensions();
	size_t frameSize = dims[0]*dims[1];
	size_t numFrames = dims[2];
	messageManager()->sendInfo(QString("Input dims: %1 %2 %3").arg(dims[0]).arg(dims[1]).arg(dims[2]));

	
	// Find out how many frames needs to be in each block
	size_t framesPerBlock = numFrames / numBlocks;
	messageManager()->sendInfo(QString("Frames: %1 Blocks: %2 Frames per block: %3").arg(numFrames).arg(numBlocks).arg(framesPerBlock));
	// Some blocks will need to contain one extra frame
	// (numFrames and numBlocks is probably not evenly divisible)
	size_t numBigBlocks = numFrames % numBlocks;
	messageManager()->sendInfo(QString("Allocating %1 big blocks").arg(numBigBlocks));
	// Allocate the big blocks
	for(unsigned int block = 0; block < numBigBlocks; block++)
	{
		framePointers[block].length = (1+framesPerBlock)*frameSize;
		framePointers[block].data = new unsigned char[framePointers[block].length];
	}
	messageManager()->sendInfo(QString("Allocating %1 small blocks").arg(numBlocks-numBigBlocks));
	// Then the small ones
	for(int block = numBigBlocks; block < numBlocks; block++)
	{
		framePointers[block].length = (framesPerBlock)*frameSize;
		framePointers[block].data = new unsigned char[framePointers[block].length];
	}
	
	// Now fill them
	unsigned int frame = 0;
	for(int block = 0; block < numBlocks; block++)
	{
		for(unsigned int frameInThisBlock = 0;
		    frameInThisBlock < framePointers[block].length/frameSize;
		    frameInThisBlock++) {

			memcpy(&(framePointers[block].data[frameInThisBlock*frameSize]),
			       inputFrames->getFrame(frame),
			       frameSize);
			frame++;
		}
	}	
	return true;
}

bool
TordTest::doGPUReconstruct(ProcessedUSInputDataPtr input,
                           vtkImageDataPtr outputData)
{
	int numBlocks = 10; // FIXME?
	// Split input US into blocks
	frameBlock_t* inputBlocks = new frameBlock_t[numBlocks];
	
	this->initializeFrameBlocks(inputBlocks, numBlocks, input);

	// Allocate CL memory for each frame block
	cl_mem *clBlocks = new cl_mem[numBlocks];
	messageManager()->sendInfo("Allocating CL input block buffers");
	for(int i = 0; i < numBlocks; i++)
	{
		clBlocks[i] = ocl_create_buffer(moClContext->context,
		                                CL_MEM_READ_ONLY,
		                                inputBlocks[i].length,
		                                inputBlocks[i].data);
		                                
	}
	// Free the local frameblock buffers
	this->freeFrameBlocks(inputBlocks, numBlocks);
	delete [] inputBlocks;
	inputBlocks = NULL;

	// Allocate output memory
	int *outputDims = outputData->GetDimensions();
	
	size_t outputVolumeSize =
		outputDims[0]*outputDims[1]*outputDims[2]*sizeof(unsigned char);
	messageManager()->sendInfo(QString("Allocating CL output buffer, size %1").arg(outputVolumeSize));
	cl_mem clOutputVolume = ocl_create_buffer(moClContext->context,
	                                          CL_MEM_WRITE_ONLY,
	                                          outputVolumeSize,
	                                          NULL);

	// FIXME: Fill plane eqs
	size_t nPlanes = input->getDimensions()[2];

	float *planeEqs = new float[nPlanes];
	float *planeCorners = new float[nPlanes];
	memset(planeEqs, 0, nPlanes*sizeof(float));
	memset(planeCorners, 0, nPlanes*sizeof(float));

	cl_mem clPlaneEqs = ocl_create_buffer(moClContext->context,
	                                      CL_MEM_READ_ONLY,
	                                      nPlanes*sizeof(float),
	                                      planeEqs);
	
	cl_mem clPlaneCorners = ocl_create_buffer(moClContext->context,
	                                      CL_MEM_READ_ONLY,
	                                      nPlanes*sizeof(float),
	                                      planeCorners);

	// Set kernel args
	int arg = 0;
	ocl_check_error(clSetKernelArg(mClKernel,arg++,sizeof(cl_int),&outputDims[0]));
	ocl_check_error(clSetKernelArg(mClKernel,arg++,sizeof(cl_int),&outputDims[1]));
	ocl_check_error(clSetKernelArg(mClKernel,arg++,sizeof(cl_int),&outputDims[2]));
	ocl_check_error(clSetKernelArg(mClKernel,arg++,sizeof(cl_int),&input->getDimensions()[0]));
	ocl_check_error(clSetKernelArg(mClKernel,arg++,sizeof(cl_int),&input->getDimensions()[1]));
	ocl_check_error(clSetKernelArg(mClKernel,arg++,sizeof(cl_int),&input->getDimensions()[2]));
	// The input blocks
	for(int i = 0; i < numBlocks; i++)
	{
		ocl_check_error(clSetKernelArg(mClKernel,arg++,sizeof(cl_mem),&clBlocks[i]));
	}
	// The output volume
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_mem), &clOutputVolume));
	// Plane equations
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_mem), &clPlaneEqs));
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_mem), &clPlaneCorners));

	// FIXME: radius
	float radius = 1.0f;
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &radius));
	// FIXME: method
	int method = 0;
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_int), &method));

	// Global work items:
	size_t global_work_size = outputDims[0]*outputDims[2];
	ocl_check_error(clEnqueueNDRangeKernel(moClContext->cmd_queue,
	                                       mClKernel,
	                                       1,
	                                       NULL,
	                                       &global_work_size,
	                                       NULL,
	                                       0,
	                                       NULL,
	                                       NULL));

	// Read back data
	try {
		ocl_check_error(clEnqueueReadBuffer(moClContext->cmd_queue,
		                                    clOutputVolume,
		                                    CL_TRUE,
		                                    0,
		                                    outputVolumeSize,
		                                    outputData->GetScalarPointer(),
		                                    0, 0, 0));
	}
	catch (std::string& except)
	{
		messageManager()->sendInfo(QString((std::string("Got exception: ") + except).c_str()));
	}
		
	
	return true;
}


void
TordTest::freeFrameBlocks(frameBlock_t *framePointers,
                          int numBlocks)
{
	for(int i = 0; i < numBlocks; i++)
	{
		delete [] framePointers[i].data;
	}
}
bool
TordTest::reconstruct(ProcessedUSInputDataPtr input,
                      vtkImageDataPtr outputData,
                      QDomElement settings)
{

	return 	doGPUReconstruct(input, outputData);

	/* vtkImageDataPtr target = outputData;
	Eigen::Array3i targetDims(target->GetDimensions());
	Vector3D targetSpacing(target->GetSpacing());

	// Print dimensions
	QString info = QString("Target dims: %1 %2 %3").arg(targetDims[0]).arg(targetDims[1]).arg(targetDims[2]);
	messageManager()->sendInfo(info);


	// Iterate over outputData and fill volume with 255-s

	unsigned char *outputPointer  = static_cast<unsigned char*>(outputData->GetScalarPointer());
	for(int dim0 = 0; dim0 < targetDims[0]; dim0++)
	{
		for(int dim1 = 0; dim1 < targetDims[1]; dim1++)
		{
			for(int dim2 = 0; dim2 < targetDims[2]; dim2++)
			{
				unsigned int idx = dim0 + dim1*targetDims[0] + dim2*targetDims[0]*targetDims[1];
				outputPointer[idx] = 255;
			
			} // dim2
		} // dim1
	} // dim0

	return true; */
}

}


