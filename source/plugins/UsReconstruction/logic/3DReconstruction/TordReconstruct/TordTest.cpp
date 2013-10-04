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
}

TordTest::~TordTest()
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

	float *planeEqs = new float[nPlanes*4];
	float *planeCorners = new float[nPlanes*9];

	this->fillPlaneEqs(planeEqs, input);
	this->fillPlaneCorners(planeCorners, input);


	// Perform a sanity check on the plane corners - TODO delete this
	for(int i = 0; i < nPlanes; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			float dist = 0.0;
			for(int k = 0; k < 3; k++)
			{
				dist += planeEqs[i*4+k] * planeCorners[i*9+j*3 + k];
			}
			dist += planeEqs[i*4+3];
			if(fabs(dist) > 0.1f)
			{
				messageManager()->sendError(QString("Corner distance for plane %1 too long: %2!\n")
				                            .arg(i).arg(dist));
				return false;
			}
		}
	}
	
	messageManager()->sendInfo(QString("Allocating buffer for plane equations, %1 floats").arg(nPlanes*4));
	cl_mem clPlaneEqs = ocl_create_buffer(moClContext->context,
	                                      CL_MEM_READ_ONLY,
	                                      nPlanes*sizeof(float)*4,
	                                      planeEqs);
	messageManager()->sendInfo(QString("Allocating buffer for plane corners, %1 floats").arg(nPlanes*9));
	cl_mem clPlaneCorners = ocl_create_buffer(moClContext->context,
	                                          CL_MEM_READ_ONLY,
	                                          nPlanes*sizeof(float)*9,
	                                          planeCorners);

	delete [] planeEqs;
	delete [] planeCorners;
	

	float *planeMatrices = new float[16*nPlanes];

	this->fillPlaneMatrices(planeMatrices, input);

	cl_mem clPlaneMatrices = ocl_create_buffer(moClContext->context,
	                                           CL_MEM_READ_ONLY,
	                                           nPlanes*sizeof(float)*16,
	                                           planeMatrices);
	                                           

	
	// Set kernel args
	int arg = 0;
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_int), &outputDims[0]));
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_int), &outputDims[1]));
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_int), &outputDims[2]));
	double *out_spacing = outputData->GetSpacing();
	float spacings[3];
	spacings[0] = out_spacing[0];
	spacings[1] = out_spacing[1];
	spacings[2] = out_spacing[2];

	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &spacings[0]));
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &spacings[1]));
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &spacings[2]));
	double *out_origin = outputData->GetOrigin();
	spacings[0] = out_origin[0];
	spacings[1] = out_origin[1];
	spacings[2] = out_origin[2];

	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &spacings[0]));
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &spacings[1]));
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &spacings[2]));
	
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_int), &input->getDimensions()[0]));
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_int), &input->getDimensions()[1]));
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_int), &input->getDimensions()[2]));
	spacings[0] = input->getSpacing()[0];
	spacings[1] = input->getSpacing()[1];
	
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &spacings[0]));
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_float), &spacings[1]));
	
	// The input blocks
	for(int i = 0; i < numBlocks; i++)
	{
		ocl_check_error(clSetKernelArg(mClKernel, arg++,sizeof(cl_mem),&clBlocks[i]));
	}
	// The output volume
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_mem), &clOutputVolume));
	// Plane equations
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_mem), &clPlaneEqs));
	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_mem), &clPlaneCorners));

	ocl_check_error(clSetKernelArg(mClKernel, arg++, sizeof(cl_mem), &clPlaneMatrices));
		
	// FIXME: radius
	float radius = 2.0f;
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
TordTest::fillPlaneEqs(float *planeEqs,
                       ProcessedUSInputDataPtr input)
{
	std::vector<TimedPosition> vecPosition = input->getFrames();
	
	// Sanity check on the number of frames
	if(input->getDimensions()[2] != vecPosition.end() - vecPosition.begin())
	{
		messageManager()->sendError(QString("Number of frames %1 != %2 dimension 2 of US input")
		                            .arg(input->getDimensions()[2])
		                            .arg(vecPosition.end() - vecPosition.begin()));
		return;
	}
	int i = 0;
	for(std::vector<TimedPosition>::iterator it = vecPosition.begin();
	    it != vecPosition.end(); it++)
	{
		// FIXME: This should be a separate function.
		Transform3D pos = it->mPos;
		// Pos is a transformation matrix. This means that the Z component of its
		// rotational matrix is a normal unit vector to the plane.
		float a, b, c, d;
		a = pos(0,2);
		b = pos(1,2);
		c = pos(2,2);

		d = -(a*pos(0,3) + b*pos(1,3) + c*pos(2,3));

		planeEqs[i++] = a;
		planeEqs[i++] = b;
		planeEqs[i++] = c;
		planeEqs[i++] = d;
		
	}
}

void
TordTest::fillPlaneCorners(float *planeCorners,
                           ProcessedUSInputDataPtr input)
{
	std::vector<TimedPosition> vecPosition = input->getFrames();
	
	// Sanity check on the number of frames
	if(input->getDimensions()[2] != vecPosition.end() - vecPosition.begin())
	{
		messageManager()->sendError(QString("Number of frames %1 != %2 dimension 2 of US input")
		                            .arg(input->getDimensions()[2])
		                            .arg(vecPosition.end() - vecPosition.begin()));
		return;
	}
	Eigen::Array3i dims = input->getDimensions();
	Vector3D spacings = input->getSpacing();
	int i = 0;
	// Corners in image space
	Vector3D iCorner_0_0, iCorner_x_0, iCorner_0_y;
	// Corners in volume space
	Vector3D vCorner_0_0, vCorner_x_0, vCorner_0_y;

	iCorner_0_0(0) = 0.0;
	iCorner_0_0(1) = 0.0;
	iCorner_0_0(2) = 0.0;
	
	iCorner_x_0(0) = dims[0]*spacings(0);
	iCorner_x_0(1) = 0.0;
	iCorner_x_0(2) = 0.0;

	iCorner_0_y(0) = 0.0;
	iCorner_0_y(1) = dims[1]*spacings(1);
	iCorner_0_y(2) = 0.0;
	
	for(std::vector<TimedPosition>::iterator it = vecPosition.begin();
	    it != vecPosition.end();
	    it++)
	{
		// Transform the image space corner positions into world volume space
		// TODO: Maybe the GPU should be doing this?
		Transform3D pos = it->mPos;
		vCorner_0_0 = pos * iCorner_0_0;
		vCorner_x_0 = pos * iCorner_x_0;
		vCorner_0_y = pos * iCorner_0_y;

		// Now store the result in the output
		planeCorners[i++] = vCorner_0_0(0);
		planeCorners[i++] = vCorner_0_0(1);
		planeCorners[i++] = vCorner_0_0(2);

		planeCorners[i++] = vCorner_x_0(0);
		planeCorners[i++] = vCorner_x_0(1);
		planeCorners[i++] = vCorner_x_0(2);

		planeCorners[i++] = vCorner_0_y(0);
		planeCorners[i++] = vCorner_0_y(1);
		planeCorners[i++] = vCorner_0_y(2);
		
	}
}

void
TordTest::fillPlaneMatrices(float *planeMatrices,
                           ProcessedUSInputDataPtr input)
{
	std::vector<TimedPosition> vecPosition = input->getFrames();
	
	// Sanity check on the number of frames
	if(input->getDimensions()[2] != vecPosition.end() - vecPosition.begin())
	{
		messageManager()->sendError(QString("Number of frames %1 != %2 dimension 2 of US input")
		                            .arg(input->getDimensions()[2])
		                            .arg(vecPosition.end() - vecPosition.begin()));
		return;
	}

	int i = 0;
	for(std::vector<TimedPosition>::iterator it = vecPosition.begin();
	    it != vecPosition.end();
	    it++)
	{
		Transform3D pos = it->mPos;
	
		// Now store the result in the output
		for(int j = 0; j < 16; j++)
		{
			planeMatrices[i++] = pos(j/4, j%4);
		}
	}
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

	initCL(QString(TORD_KERNEL_PATH) + "/kernels.ocl");
	bool ret = 	doGPUReconstruct(input, outputData);
	if(moClContext != NULL)
	{
		ocl_release(moClContext);
		moClContext = NULL;
	}
	return ret;

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


